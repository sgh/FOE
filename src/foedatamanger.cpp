#include <iostream>
//#include <QStringList>

#include "foedatamanger.h"
#include "foegoods.h"

using namespace std;

void FoeDataManager::readSettings()
{
	QSettings settings;
	settings.beginGroup("ServerSettings");
	_db_name     = settings.value("database").toString();
	_db_server   = settings.value("server").toString();
	_db_username = settings.value("username").toString();
	_db_password = settings.value("password").toString();
}

void FoeDataManager::writeSettings()
{
	QSettings settings;
	settings.beginGroup("ServerSettings");
	settings.setValue("database", _db_name);
	settings.setValue("server",   _db_server);
	settings.setValue("username", _db_username);
	settings.setValue("password", _db_password);
	settings.endGroup();
}


FoeClan* FoeDataManager::FoeClanFactory(unsigned int clanid)
{
	FoeClan* clan;
	foreach (clan, _clanList) {
		if (clan->id() == clanid) {
			return clan;
		}
	}

	// Skip this clan if we have no insert orivileges and the clan does not match login name.
	if (!_b_insertPrivileges && getClanname(clanid).toUpper()!=_db_username.toUpper())
		return NULL;

	clan = new FoeClan(this, clanid);
	_clanList << clan;
	emit clanAdded(clan);
	return clan;
}


FoeDataManager::FoeDataManager()
{
	_b_insertPrivileges = false;
	_db =  QSqlDatabase::addDatabase("QMYSQL");

	if (!_db.isValid())
		qDebug() << "Driver could not be added";

	_user_table = "users";

	readSettings();
	startTimer(30000);
	_threadRun = true;
	start();
}


void FoeDataManager::postCommand(SqlCommand *cmd)
{
	_commandLock.lock();
	_commandQ.enqueue(cmd);
	_commandLock.unlock();
	_commandSemaphore.release();
}

void FoeDataManager::run()
{
	while (_threadRun || _commandSemaphore.available()) {
		if (_commandSemaphore.tryAcquire(1, 1000)) {
			_commandLock.lock();
			SqlCommand* cmd = _commandQ.dequeue();
			_commandLock.unlock();
			for (int idx=0; idx<cmd->nqueries() ; idx++) {
				QString q = cmd->query(idx);
				QSqlQuery query(_db);
				if (!query.exec(q)) {
					cmd->actionFailed();
					qDebug() << "Query failed: " << q;
				} else {
					while (query.next()) {
						cmd->actionSuccess(idx, &query);
					}
				}
			}
			delete cmd;
		}
	}
}


void FoeDataManager::addClan(const QString& clanname)
{
	postCommand(new AddClanCommand(this, clanname));
}


void FoeDataManager::removeClan(FoeClan* clan)
{
	emit clanAboutToBeRemoved(clan);
	FoeUser* user;
	foreach (user, clan->getFoeUsers()) {
		removeUser(clan, user);
	}
	postCommand(new RemoveClanCommand(this, clan));
}

bool FoeDataManager::renameClan(FoeClan* clan, const QString& new_name)
{
	FoeClan* c;
	foreach (c, _clanList) {
		if(c->name() == new_name)
			return false;
	}
	postCommand(new RenameClanCommand(this, clan, new_name));
	return true;
}


bool FoeDataManager::doQuery(const QString& q) {
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Query failed: " << q;
		return false;
	}

	return true;
}

void FoeDataManager::updateInsertPrivileges()
{
	bool b_insertPrivileges = false;
	QString q = QString("show grants;");
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Query failed: " << q;
	}

	while (query.next()) {
		QString s = query.value(0).toString();
		if (s.contains(_db_name)) {
			if (s.contains("ALL PRIVILEGES"))
				b_insertPrivileges = true;
		}
	}

	if (!b_insertPrivileges)
		_user_table = QString("users_%1").arg(_db_username.toUpper());
	else
		_user_table = "users";

	_b_insertPrivileges = b_insertPrivileges;
}


void FoeDataManager::migrateDatabase()
{
	QString q;
	const int valid_schema_version = 1;

	QStringList initial_schema;
	initial_schema << "create table users    ( id integer PRIMARY KEY AUTO_INCREMENT, name varchar(30) unique );";
	initial_schema << "create table products ( id_user int, product int, factories int, bonus int, primary key (id_user,product) );";

	QStringList v1_schema;
	v1_schema << "create table options ( name varchar(30) unique primary key, val varchar(30) );";
	v1_schema << "alter table users drop index  name;";
	v1_schema << "alter table users add clanid int not null;";
	v1_schema << "create table clans ( id integer unique auto_increment primary key, name varchar(64) );";
	v1_schema << "insert into clans values (0, 'Klan1');";

	// Return if we do not have insert privileges
	if (!_b_insertPrivileges)
		return;

	// Check for initial schema existence
	if (!doQuery("select * from users,products;"))
		foreach (q, initial_schema) { doQuery(q); }

	// First get schemaversion if any.
	q = QString("select val from options where name='schemaversion';");
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Query failed: " << q;
	}

	query.next();
	bool b_ok;
	int schemaversion = query.value(0).toInt(&b_ok);

	if (!b_ok)
		schemaversion = 0;

	// Run upgrade if current version is smaller than latest version
	if (schemaversion < valid_schema_version) {
		if (schemaversion < 1)
			foreach (q, v1_schema) { doQuery(q); }

		q = QString("replace into options (name,val) values ('schemaversion', '%1');").arg(valid_schema_version);
		doQuery(q);
	}

}


void FoeDataManager::timerEvent(QTimerEvent *)
{
	FoeClan* c;
	foreach (c, _clanList) {
		c->loadusers();
	}
}

void FoeDataManager::addUser(FoeClan* clan, QString name)
{
	postCommand(new AddUserCommand(clan, name));
}

void FoeDataManager::removeUser(FoeClan* clan, FoeUser* user)
{
	postCommand(new RemoveUserCommand(clan, user));
}

void FoeDataManager::setDbUsername(const QString &username) {
	_db_username = username;
}

void FoeDataManager::setDbPassword(const QString &password) {
	_db_password = password;
}


void FoeDataManager::setDbName(const QString &db) {
	_db_name = db;
}

void FoeDataManager::setServerName(const QString& servername)
{
	_db_server = servername;
}

bool FoeDataManager::hasInsertPrivileges()
{
	return _b_insertPrivileges;
}


const QString& FoeDataManager::getDbUsername() {
	return _db_username;
}


const QString &FoeDataManager::getDbName() {
	return _db_name;
}

const QString&FoeDataManager::getServerName()
{
	return _db_server;
}


const QString &FoeDataManager::getDbPassword() {
	return _db_password;
}



QString FoeDataManager::getUsername(int userid)
{
	QString q = QString("select * from %1 where id = %2;").arg(_user_table).arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed" << q;

	int fieldNo = query.record().indexOf("name");
	query.next();
	return query.value(fieldNo).toString();
}


QString FoeDataManager::getClanname(int clanid)
{
	QString q = QString("select * from clans where id = %1;").arg(clanid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed" << q;

	int fieldNo = query.record().indexOf("name");
	query.next();
	return query.value(fieldNo).toString();
}

FoeClan*FoeDataManager::getClan(const QString clanname)
{
	FoeClan* clan;
	foreach (clan, _clanList) {
		if (clan->name() == clanname)
			return clan;
	}
	return NULL;
}


void FoeDataManager::loadclans()
{
	postCommand(new LoadClansCommand(this));
}


QMap<const FoeGoods*, int> FoeDataManager::getUserHas(int userid)
{
	QString q = QString("select factories,product,bonus from products where id_user = %1;").arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed: " << q;

	int factories_fieldNo = query.record().indexOf("factories");
	int product_fieldNo   = query.record().indexOf("product");

	QMap<const FoeGoods*, int> products;
	while (query.next()) {
		enum e_Goods id = (enum e_Goods)query.value(product_fieldNo).toInt();
		const FoeGoods* product = FoeGoods::fromId(id);
		int factories = query.value(factories_fieldNo).toInt();
		if (product && factories>0)
			products[product] = factories;
	}
	return products;
}


QMap<const FoeGoods*, BoostLevel> FoeDataManager::getUserHasBonus(int userid)
{
	QString q = QString("select factories,product,bonus from products where id_user = %1;").arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed: " << q;

	int boost_fieldNo = query.record().indexOf("bonus");
	int product_fieldNo   = query.record().indexOf("product");

	QMap<const FoeGoods*, BoostLevel> products;
	while (query.next()) {
		enum e_Goods id = (enum e_Goods)query.value(product_fieldNo).toInt();
		const FoeGoods* product = FoeGoods::fromId(id);
		BoostLevel bl = (BoostLevel)query.value(boost_fieldNo).toInt();
		if (product && bl > e_NO_BOOST && bl < e_NUM_BOOSTLEVELS)
			products[product] = bl;
	}
	return products;
}


FoeDataManager::~FoeDataManager()
{
	writeSettings();
	_threadRun = false;
	wait();
	_db.close(); // close connection
}

bool FoeDataManager::dbconnect()
{
	_db.close();
	_db.setHostName(_db_server);
	_db.setDatabaseName(_db_name);
	_db.setUserName(_db_username);
	_db.setPassword(_db_password);
	_db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

	if (!_db.open()) {
		qDebug() << "Failed opening DB";
		return false;
	}

	updateInsertPrivileges();
	migrateDatabase();
	loadclans();
	return true;
}

void FoeDataManager::dbdisconnect() {
	_db.close();
}

bool FoeDataManager::isConnected()
{
	return _db.isOpen() && _db.database().isValid();
}


void FoeDataManager::removeClanFromList(FoeClan* clan)
{
	QList<FoeClan*>::iterator it = _clanList.begin();
	while (it != _clanList.end()) {
		if ((*it) == clan) {
			_clanList.erase(it);
			break;
		}
		++it;
	}

	delete clan;
}

void FoeDataManager::clanRenameCallback(FoeClan* clan)
{
	emit clanRenamed(clan);
}