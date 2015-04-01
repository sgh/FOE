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


FoeClan* FoeDataManager::constructClan(unsigned int clanid)
{
	FoeClan* clan;
	foreach (clan, _clanList) {
		if (clan->id() == clanid) {
			return clan;
		}
	}

	QSqlQuery query;
	if (_persist.doQuery(QString("select * from users where clanid=%1;").arg(clanid), query)) {
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
	_persist.db() =  QSqlDatabase::addDatabase("QSQLITE");

	if (!_persist.db().isValid())
		qDebug() << "Driver could not be added";

	readSettings();
}


void FoeDataManager::addClan(const QString& clanname)
{
	if (_persist.addClan(clanname)) {
		constructClan( _persist.getClanID(clanname));
	}
}


void FoeDataManager::removeClan(FoeClan* clan)
{
	emit clanAboutToBeRemoved(clan);
	FoeUser* user;
	foreach (user, clan->getFoeUsers()) {
		removeUser(clan, user);
	}

	if (_persist.removeClan(clan))
		removeClanFromList(clan);
}


bool FoeDataManager::renameClan(FoeClan* clan, const QString& new_name)
{
	FoeClan* c;
	foreach (c, _clanList) {
		if(c->name() == new_name)
			return false;
	}

	if (_persist.renameClan(clan, new_name)) {
		clan->setName(new_name);
		emit clanRenamed(clan);
		return true;
	}

	return false;
}


bool FoeDataManager::removeUserHas(FoeUser* user, const FoeGoods* product)
{
	return _persist.removeUserHas(user, product);
}


bool FoeDataManager::setUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level)
{
	return _persist.setUserHas(user, product, factories, boost_level);
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

	// Check for initial schema existence
	if (!_persist.doQuery("select * from users,products;"))
		foreach (q, initial_schema) { _persist.doQuery(q); }

	// First get schemaversion if any.
	q = QString("select val from options where name='schemaversion';");
	QSqlQuery query(_persist.db());
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
			foreach (q, v1_schema) { _persist.doQuery(q); }

		q = QString("replace into options (name,val) values ('schemaversion', '%1');").arg(valid_schema_version);
		_persist.doQuery(q);
	}

}


void FoeDataManager::addUser(FoeClan* clan, QString name)
{
	if (!_persist.addUser(clan, name))
		QMessageBox::warning(NULL, tr("Add  user."), QString("Unable to add user.").arg(name), QMessageBox::Ok);
}


void FoeDataManager::removeUser(FoeClan* clan, FoeUser* user)
{
	QString name = user->name();
	if (!_persist.removeUser(clan, user))
		QMessageBox::warning(NULL, tr("Remove user."), QString("Unable to delete user.").arg(user->name()), QMessageBox::Ok);
}


QString FoeDataManager::getClanname(int clanid)
{
	return _persist.getClanName(clanid);
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
	QVector<int> ids = _persist.getClanIDs();
	int id;
	foreach (id, ids) {
		constructClan(id);
	}
}


QMap<const FoeGoods*, int> FoeDataManager::getUserHas(int userid) {
	return _persist.getUserHas(userid);
}


QMap<const FoeGoods*, BoostLevel> FoeDataManager::getUserHasBonus(int userid)
{
	return _persist.getUserHasBonus(userid);
}


FoeDataManager::~FoeDataManager()
{
	writeSettings();
	closeFile();
}

bool FoeDataManager::loadFile(const QString& dbfile, bool overwrite)
{
	if (dbfile.isEmpty())
		return false;
	closeFile();
	_filename = dbfile;
	if (overwrite) {
		QDir d;
		d.remove(_filename);
	}
	_persist.db().setDatabaseName(_filename);

	if (!_persist.db().open()) {
		qDebug() << "Failed opening DB";
		return false;
	}

	_persist.doQuery("pragma synchronous = off;");
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
	_persist.db().close();
	emit fileClosed();
}


void FoeDataManager::handleRemoteEvent(const QString& event, const QString& data) {
	if (event == "client-adduser") {
		_persist.addUser(_clanList[0], data);
	}

	if (event == "client-removeuser") {
		_persist.removeUser(_clanList[0], _clanList[0]->getUser(data));
	}
}


bool FoeDataManager::isValid() {
	return _persist.db().isOpen() && _persist.db().database().isValid();
}


void FoeDataManager::removeClanFromList(FoeClan* clan) {
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
