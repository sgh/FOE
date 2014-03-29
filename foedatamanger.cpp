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
	_db_username = settings.value("username").toString();
	_db_password = settings.value("password").toString();
}

void FoeDataManager::writeSettings()
{
	QSettings settings;
	settings.beginGroup("ServerSettings");
	settings.setValue("database", _db_name);
	settings.setValue("username", _db_username);
	settings.setValue("password", _db_password);
	settings.endGroup();
}

FoeUser *FoeDataManager::FoeUserFactory(unsigned int userid)
{
	FoeUser* user;
	foreach (user, _userList) {
		if (user->id() == userid) {
			user->reload();
			return user;
		}
	}

	user = new FoeUser(this, userid);
	_userList << user;
	return user;
}


FoeDataManager::FoeDataManager()
{
	_b_insertPrivileges = false;
//	_db =  QSqlDatabase::addDatabase("QSQLITE");
//	_db.setDatabaseName("../foe.db3");
	_db =  QSqlDatabase::addDatabase("QMYSQL");

	if (!_db.isValid())
		qDebug() << "Driver could not be added";

	readSettings();
	startTimer(30000);
}


bool FoeDataManager::loadusers(bool complete_reload) {
	FoeUser* user;
	QSet<FoeUser*> userSet;
	QString q = "select * from users;";
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Query failed: " << q;
		return false;
	}

	if (complete_reload) {
		foreach (user, _userList) {
			_userList.removeOne(user);
			emit userRemoved(user);
			delete user;
		}
	}

	QStringList lst;
	int fieldNo = query.record().indexOf("name");
	int fieldNoUserId = query.record().indexOf("id");
	while (query.next()) {
		lst.append(query.value(fieldNo).toString());
		FoeUser* user = FoeUserFactory(query.value(fieldNoUserId).toInt());
		emit userAdded(user);
		userSet << user;
	}

	// Substract the two sets to find which users where removed
	userSet = _userList.toSet() - userSet;
	foreach (user, userSet) {
		_userList.removeOne(user);
		emit userRemoved(user);
		delete user;
	}

	lst.sort();
	if (lst != _userModel.stringList())
		_userModel.setStringList(lst);
	return true;
}


void FoeDataManager::refresh() {
	QStringList lst;
	FoeUser* user;
	foreach (user, _userList) {
		lst.append(user->name());
	}
	lst.sort();
	_userModel.setStringList(lst);
}

QStringListModel* FoeDataManager::userModel() {
	return &_userModel;
}


void FoeDataManager::postCommand(SqlCommand *cmd)
{
	for (int idx=0; idx<cmd->nqueries() ; idx++) {
		QString q = cmd->query(idx);
		QSqlQuery query(_db);
		if (!query.exec(q)) {
			cmd->actionFailed();
			qDebug() << "Query failed: " << q;
		} else {
			cmd->actionSuccess(this, idx, &query);
			qDebug() << "Q: : " << q;
		}
	}
	delete cmd;
}

bool FoeDataManager::doQuery(const QString& q) {
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Failed removing user";
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

	_b_insertPrivileges = b_insertPrivileges;
}

void FoeDataManager::timerEvent(QTimerEvent *)
{
	loadusers();
}

void FoeDataManager::addUser(QString name)
{
	postCommand(new AddUserCommand(name));
}

void FoeDataManager::removeUser(FoeUser* user)
{
	postCommand(new RemoveUserCommand(user));
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


const QString &FoeDataManager::getDbPassword() {
	return _db_password;
}


FoeUser* FoeDataManager::getFoeUser(QString username)
{
	FoeUser* user;
	foreach (user, _userList) {
		if (user->name() == username)
			return user;
	}

	return NULL;
}

QString FoeDataManager::getUsername(int userid)
{
	QString q = QString("select * from users where id = %1;").arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed" << q;

	int fieldNo = query.record().indexOf("name");
	query.next();
	return query.value(fieldNo).toString();
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


QMap<const FoeGoods*, BonusLevel> FoeDataManager::getUserHasBonus(int userid)
{
	QString q = QString("select factories,product,bonus from products where id_user = %1;").arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed: " << q;

	int bonus_fieldNo = query.record().indexOf("bonus");
	int product_fieldNo   = query.record().indexOf("product");

	QMap<const FoeGoods*, BonusLevel> products;
	while (query.next()) {
		enum e_Goods id = (enum e_Goods)query.value(product_fieldNo).toInt();
		const FoeGoods* product = FoeGoods::fromId(id);
		BonusLevel bl = (BonusLevel)query.value(bonus_fieldNo).toInt();
		if (product && bl > e_NO_BONUS && bl < e_NUM_BONUSLEVELS)
			products[product] = bl;
	}
	return products;
}


QSet<FoeUser *> FoeDataManager::getUsersForProduct(const FoeGoods *product)
{
	QSet<FoeUser*> userSet;
	FoeUser* user;
	foreach (user, _userList) {
		if (user->hasBonus(product) != e_NO_BONUS || user->hasProduct(product))
			userSet.insert(user);
	}
	return userSet;
}


FoeDataManager::~FoeDataManager()
{
	writeSettings();
	_db.close(); // close connection
}

bool FoeDataManager::connect()
{
	_db.close();
	_db.setHostName("sgh.dk");
	_db.setDatabaseName(_db_name);
	_db.setUserName(_db_username);
	_db.setPassword(_db_password);
	_db.setConnectOptions("MYSQL_OPT_RECONNECT=1");

	if (!_db.open()) {
		qDebug() << "Failed opening DB";
		return false;
	}

	updateInsertPrivileges();
	loadusers(true);
	return true;
}

void FoeDataManager::disconnect() {
	_db.close();
}

bool FoeDataManager::isConnected()
{
	return _db.isOpen() && _db.database().isValid();
}

void FoeDataManager::removeUserFromList(FoeUser *user)
{
	_userList.removeOne(user);
	emit userRemoved(user);
	delete user;
	refresh();
}

void FoeDataManager::addUserToList(FoeUser *user)
{
	_userList << user;
	emit userAdded(user);
	refresh();
}
