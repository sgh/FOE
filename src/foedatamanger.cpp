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


void FoeDataManager::constructClan(unsigned int clanid)
{
	foreach (auto clan, _clanList) {
		if (clan->id() == clanid) {
			return;
		}
	}

	QSqlQuery query;
	if (_persist->doQuery(QString("select * from users where clanid=%1;").arg(clanid), query)) {
		shared_ptr<FoeClan> clan = make_shared<FoeClan>(*this, clanid);

		// Load users
		int fieldNoName = query.record().indexOf("name");
		int fieldNoId   = query.record().indexOf("id");
		while (query.next()) {
			shared_ptr<FoeUser> user = make_shared<FoeUser>(query.value(fieldNoName).toString(), query.value(fieldNoId).toInt());
			clan->addUser(user);
		}

		_clanList << clan;
		emit clanAdded(clan.get());
	}
}


FoeDataManager::FoeDataManager(shared_ptr<FoePersistence> persist)
	: _persist(persist) {

	readSettings();
}


void FoeDataManager::addClan(const QString& clanname)
{
	if (_persist->addClan(clanname)) {
		constructClan( _persist->getClanID(clanname));
	}
}


void FoeDataManager::removeClan(FoeClan* clan)
{
	emit clanAboutToBeRemoved(clan);
	foreach (auto user, clan->getFoeUsers()) {
		removeUser(clan, user.get());
	}

	if (_persist->removeClan(clan))
		removeClanFromList(clan);
}


bool FoeDataManager::renameClan(FoeClan* clan, const QString& new_name)
{
	foreach (auto c, _clanList) {
		if(c->name() == new_name)
			return false;
	}

	if (_persist->renameClan(clan, new_name)) {
		clan->setName(new_name);
		emit clanRenamed(clan);
		return true;
	}

	return false;
}


bool FoeDataManager::removeUserHas(FoeUser* user, const FoeGoods* product) {
	return _persist->removeUserHas(user, product);
}


bool FoeDataManager::setUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level) {
	return _persist->setUserHas(user, product, factories, boost_level);
}


void FoeDataManager::setUserTimestamp(FoeUser* user, int64_t timestamo) {
	_persist->setUserTimestamp(user, timestamo);
}


void FoeDataManager::migrateDatabase()
{
	QString q;
	const int valid_schema_version = 2;

	// Initial schema
	QStringList initial_schema;
	initial_schema << "create table users    ( id integer PRIMARY KEY AUTOINCREMENT, name varchar(30) unique, clanid int not null );";
	initial_schema << "create table products ( id_user int, product int, factories int, bonus int, primary key (id_user,product) );";

	// v1 - options and clans
	QStringList v1_schema;
	v1_schema << "create table options ( name varchar(30) unique primary key, val varchar(30) );";
	v1_schema << "create table clans ( id integer unique primary key AUTOINCREMENT, name varchar(64) );";

	// Timestamps.
	QStringList v2_schema;
	v2_schema << "alter table users add timestamp integer(8) not null default 0;";


	// Check for initial schema existence
	if (!_persist->doQuery("select * from users,products;"))
		foreach (q, initial_schema) { _persist->doQuery(q); }

	// First get schemaversion if any.
	int schemaversion = _persist->getIntOption("schemaversion", 0);

	// Run upgrade if current version is smaller than latest version
	if (schemaversion < valid_schema_version) {
		// V1
		if (schemaversion < 1)
			foreach (q, v1_schema) { _persist->doQuery(q); }

		// V2
		if (schemaversion < 2)
			foreach (q, v2_schema) { _persist->doQuery(q); }

		_persist->setIntOption("schemaversion", valid_schema_version);
	}
}


shared_ptr<FoeUser> FoeDataManager::addUser(FoeClan* clan, QString name) {
	shared_ptr<FoeUser> user = _persist->addUser(clan, name);
	if (user == NULL)
		QMessageBox::warning(NULL, tr("Add  user."), QString("Unable to add user.").arg(name), QMessageBox::Ok);
	else {
		clan->addUser(user);
		return user;
	}
	return NULL;
}


bool FoeDataManager::removeUser(FoeClan* clan, FoeUser* user)
{
	QString name = user->name();
	if (!_persist->removeUser(user))
		QMessageBox::warning(NULL, tr("Remove user."), QString("Unable to delete user.").arg(user->name()), QMessageBox::Ok);
	else {
		clan->removeUser(user);
		return true;
	}
	return false;
}


QString FoeDataManager::getClanname(int clanid)
{
	return _persist->getClanName(clanid);
}

bool FoeDataManager::clanExists(const QString clanname)
{
	foreach (auto clan, _clanList) {
		if (clan->name() == clanname)
			return true;
	}
	return false;
}


int64_t FoeDataManager::getUserTimestamp(FoeUser* user) {
	return _persist->getUserTimestamp(user);
}


FoeClan* FoeDataManager::currentClan() {
	return _clanList[0].get();
}


QString FoeDataManager::currentFile() {
	return _filename;
}


void FoeDataManager::loadclans()
{
	QVector<int> ids = _persist->getClanIDs();
	int id;
	foreach (id, ids) {
		constructClan(id);
	}
}


QMap<const FoeGoods*, int> FoeDataManager::getUserHas(int userid) {
	return _persist->getUserHas(userid);
}


QMap<const FoeGoods*, BoostLevel> FoeDataManager::getUserHasBonus(int userid)
{
	return _persist->getUserHasBonus(userid);
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
	_persist->db().setDatabaseName(_filename);

	if (!_persist->db().open()) {
		qDebug() << "Failed opening DB";
		return false;
	}

	_persist->doQuery("pragma synchronous = off;");
	migrateDatabase();
	loadclans();
	if (_clanList.empty())
		addClan("Default");
	emit fileChanged(_filename);
	return true;
}

void FoeDataManager::closeFile() {
	foreach (auto clan, _clanList) {
		emit clanAboutToBeRemoved(clan.get());
	}
	_clanList.clear();
	_persist->db().close();
	emit fileClosed();
}


bool FoeDataManager::isValid() {
	return _persist->db().isOpen() && _persist->db().database().isValid();
}


void FoeDataManager::removeClanFromList(FoeClan* clan) {
	for (auto it = _clanList.begin(); it != _clanList.end(); ++it) {
		if ((*it).get() == clan) {
			_clanList.erase(it);
			break;
		}
	}
}
