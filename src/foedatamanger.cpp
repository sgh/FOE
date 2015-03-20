#include <iostream>
#include <QMessageBox>

#include "foedatamanger.h"
#include "foegoods.h"

using namespace std;

void FoeDataManager::readSettings()
{
}

void FoeDataManager::writeSettings()
{
}


FoeClan* FoeDataManager::FoeClanFactory(unsigned int clanid)
{
	FoeClan* clan;
	foreach (clan, _clanList) {
		if (clan->id() == clanid) {
			return clan;
		}
	}

	QSqlQuery query;
	if (doQuery(QString("select * from users where clanid=%1;").arg(clanid), query)) {
		clan = new FoeClan(this, clanid);

		// Load users
		int fieldNoName = query.record().indexOf("name");
		int fieldNoId   = query.record().indexOf("id");
		while (query.next()) {
			clan->FoeUserFactory(query.value(fieldNoName).toString(), query.value(fieldNoId).toInt());
		}

		_clanList << clan;
		emit clanAdded(clan);
		return clan;
	}

	return NULL;
}


FoeDataManager::FoeDataManager()
{
	_db =  QSqlDatabase::addDatabase("QSQLITE");

	if (!_db.isValid())
		qDebug() << "Driver could not be added";

	readSettings();
}


void FoeDataManager::addClan(const QString& clanname)
{
	QSqlQuery query(_db);

	doQuery(QString("insert into clans (name) values (\"%1\")").arg(clanname), query);
	doQuery(QString("select id from clans where name = \"%1\";").arg(clanname), query);

	query.next();
	int fieldNoId = query.record().indexOf("id");
	int clanID = query.value(fieldNoId).toUInt();
	FoeClanFactory(clanID);
}


void FoeDataManager::removeClan(FoeClan* clan)
{
	emit clanAboutToBeRemoved(clan);
	FoeUser* user;
	foreach (user, clan->getFoeUsers()) {
		removeUser(clan, user);
	}

	QSqlQuery query;
	if (doQuery(QString("delete from clans where id = %1;").arg(clan->id()), query))
		removeClanFromList(clan);
}

bool FoeDataManager::renameClan(FoeClan* clan, const QString& new_name)
{
	FoeClan* c;
	foreach (c, _clanList) {
		if(c->name() == new_name)
			return false;
	}

	QSqlQuery query;
	if (doQuery(QString("update clans set name=\"%1\" where name=\"%2\";").arg(new_name).arg(clan->name()), query)) {
		clan->setName(new_name);
		clanRenameCallback(clan);
		return true;
	}

	return false;
}


bool FoeDataManager::removeUserHas(FoeUser* user, const FoeGoods* product)
{
	QSqlQuery query;
	return doQuery(QString("delete from products where id_user = %1 and product = %2;").arg(user->id()).arg(product->id()), query);
}


bool FoeDataManager::setUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level)
{
	QSqlQuery query;
	return doQuery(QString("replace into products (id_user,product,factories,bonus) values(%1,%2,%3,%4);").arg(user->id()).arg(product->id()).arg(factories).arg(boost_level), query);
}


bool FoeDataManager::doQuery(const QString& q, QSqlQuery* ret) {
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Query failed: " << q;
		return false;
	}

	if (ret)
		*ret = query;
	return true;
}

bool FoeDataManager::doQuery(const QString& query_string, QSqlQuery& ret) {
	return doQuery(query_string, &ret);
}


void FoeDataManager::migrateDatabase()
{
	QString q;
	const int valid_schema_version = 1;

	QStringList initial_schema;
	initial_schema << "create table users    ( id integer PRIMARY KEY AUTOINCREMENT, name varchar(30) unique, clanid int not null );";
	initial_schema << "create table products ( id_user int, product int, factories int, bonus int, primary key (id_user,product) );";

	QStringList v1_schema;
	v1_schema << "create table options ( name varchar(30) unique primary key, val varchar(30) );";
	v1_schema << "create table clans ( id integer unique primary key AUTOINCREMENT, name varchar(64) );";
	v1_schema << "insert into clans values (0, 'Klan1');";

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


void FoeDataManager::addUser(FoeClan* clan, QString name)
{
	QSqlQuery query;
	bool ok = doQuery(QString("insert into users (name, clanid) values (\"%1\", %2);").arg(name).arg(clan->id()), query);

	if (ok)
		ok = doQuery(QString("select id from users where name = \"%1\" and clanid=%2;").arg(name).arg(clan->id()), query);

	if (ok) {
		query.next();
		int fieldNoId = query.record().indexOf("id");
		clan->FoeUserFactory(name, query.value(fieldNoId).toInt());
	}

	if (!ok)
		QMessageBox::warning(NULL, tr("Add  user."), QString("Unable to add user.").arg(name), QMessageBox::Ok);
}

void FoeDataManager::removeUser(FoeClan* clan, FoeUser* user)
{
	QSqlQuery query;
	bool ok = doQuery(QString("delete from products where id_user = %1;").arg(user->id()), query);

	if (ok)
		ok = doQuery(QString("delete from users where id = \"%1\";").arg(user->id()), query);

	if (ok)
		clan->removeUser(user);

	if (!ok)
		QMessageBox::warning(NULL, tr("Remove user."), QString("Unable to delete user.").arg(user->name()), QMessageBox::Ok);
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


QString FoeDataManager::currentFile() {
	return _filename;
}


void FoeDataManager::loadclans()
{
	QSqlQuery query;
	doQuery("select * from clans;", query);
	while(query.next()) {
		int fieldNoId = query.record().indexOf("id");
		int clanID = query.value(fieldNoId).toUInt();
		FoeClanFactory(clanID);
	}
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
	_db.close(); // close connection
}

bool FoeDataManager::loadFile(const QString& dbfile)
{
	if (dbfile.isEmpty())
		return false;
	closeFile();
	_filename = dbfile;
	_db.setDatabaseName(_filename);

	if (!_db.open()) {
		qDebug() << "Failed opening DB";
		return false;
	}

	doQuery("pragma synchronous = off;");
	migrateDatabase();
	loadclans();
	emit fileChanged(_filename);
	return true;
}

void FoeDataManager::closeFile() {
	FoeClan* clan;
	foreach (clan, _clanList) {
		emit clanAboutToBeRemoved(clan);
		delete clan;
	}
	_clanList.clear();
	_db.close();
	emit fileClosed();
}

bool FoeDataManager::isValid()
{
	return _db.isOpen() && _db.database().isValid();
}


void FoeDataManager::removeClanFromList(FoeClan* clan)
{
	QVector<FoeClan*>::iterator it = _clanList.begin();
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
