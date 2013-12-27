#include <iostream>
//#include <QStringList>

#include "foedatamanger.h"
#include "foeproduct.h"

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

FoeDataManager::FoeDataManager()
{
//	_db =  QSqlDatabase::addDatabase("QSQLITE");
//	_db.setDatabaseName("../foe.db3");
    _db =  QSqlDatabase::addDatabase("QMYSQL");

	if (!_db.isValid())
		qDebug() << "Driver could not be added";

	readSettings();

}


bool FoeDataManager::loadusers() {
	_userList.clear();
	QSqlQuery query(_db);
	if (!query.exec("select * from users;")) {
		qDebug() << "Query failed";
		return false;
	}

	QStringList lst;
	int fieldNo = query.record().indexOf("name");
	int fieldNoUserId = query.record().indexOf("id");
	while (query.next()) {
		lst.append(query.value(fieldNo).toString());
		FoeUser* user = new FoeUser(this, query.value(fieldNoUserId).toInt());
		_userList << user;
		emit userAdded(user);
	}
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

bool FoeDataManager::doQuery(const QString& q) {
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Failed removing user";
		return false;
	}
	return true;
}

void FoeDataManager::addUser(QString name)
{
	QString q = QString("insert into users (name) values (\"%1\");").arg(name);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Failed adding user";

	q = QString("select id from users where name = \"%1\";").arg(name);
	if (!query.exec(q))
		qDebug() << "Failed getting userid";
	int fieldNo = query.record().indexOf("id");
	query.next();
	FoeUser* user = new FoeUser(this, query.value(fieldNo).toInt());
	_userList << user;

	emit userAdded(user);
	refresh();
}

void FoeDataManager::removeUser(FoeUser* user)
{
	QString q;

	q = QString("delete from products where id_user = %1;").arg(user->id());
	if (doQuery(q)) {
		q = QString("delete from users where id = \"%1\";").arg(user->id());
		if (doQuery(q)) {
			_userList.removeOne(user);
			emit userRemoved(user);
			delete user;
			refresh();
		}
	}
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
		qDebug() << "Query failed";

	int fieldNo = query.record().indexOf("name");
	query.next();
	return query.value(fieldNo).toString();
}


bool FoeDataManager::setUserHas(int userid, int productId, int factories, BonusLevel bonus_level)
{
	QString q = QString("replace into products (id_user,product,factories,bonus) values(%1,%2,%3,%4);").arg(userid).arg(productId).arg(factories).arg(bonus_level);
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Query failed: " << q;
		return false;
	}
	return true;
}


QMap<const FoeProduct*, int> FoeDataManager::getUserHas(int userid)
{
	QString q = QString("select factories,product,bonus from products where id_user = %1;").arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed";

	int factories_fieldNo = query.record().indexOf("factories");
	int product_fieldNo   = query.record().indexOf("product");

	QMap<const FoeProduct*, int> products;
	while (query.next()) {
		enum e_Products id = (enum e_Products)query.value(product_fieldNo).toInt();
		const FoeProduct* product = FoeProduct::fromId(id);
		int factories = query.value(factories_fieldNo).toInt();
		if (product && factories>0)
			products[product] = factories;
	}
	return products;
}


QMap<const FoeProduct*, BonusLevel> FoeDataManager::getUserHasBonus(int userid)
{
	QString q = QString("select factories,product,bonus from products where id_user = %1;").arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed";

	int bonus_fieldNo = query.record().indexOf("bonus");
	int product_fieldNo   = query.record().indexOf("product");

	QMap<const FoeProduct*, BonusLevel> products;
	while (query.next()) {
		enum e_Products id = (enum e_Products)query.value(product_fieldNo).toInt();
		const FoeProduct* product = FoeProduct::fromId(id);
		BonusLevel bl = (BonusLevel)query.value(bonus_fieldNo).toInt();
		if (product && bl > e_NO_BONUS && bl < e_NUM_BONUSLEVELS)
			products[product] = bl;
	}
	return products;
}


bool FoeDataManager::getUserAdmin(int userid)
{
	QString q = QString("select admin from users where id = %1;").arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed";

	int fieldNo = query.record().indexOf("admin");
	query.next();

	return query.value(fieldNo).toBool();
}


QSet<FoeUser *> FoeDataManager::getUsersForProduct(const FoeProduct *product)
{
	QSet<FoeUser*> userSet;
	FoeUser* user;
	foreach (user, _userList) {
		if (user->hasBonus(product) != e_NO_BONUS || user->hasProduct(product))
			userSet.insert(user);
	}
	return userSet;
}


bool FoeDataManager::removeUserHas(int userid, int productId)
{
	QString q = QString("delete from products where id_user = %1 and product = %2;").arg(userid).arg(productId);
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Query failed";
		return false;
	}
	return true;
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

	if (!_db.open()) {
		qDebug() << "Failed opening DB";
		return false;
	}

	loadusers();
	return true;
}

void FoeDataManager::disconnect() {
	_db.close();
}

bool FoeDataManager::isConnected()
{
	return _db.isOpen();
}
