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
			FoeUser* user = new FoeUser(query.value(fieldNoName).toString(), query.value(fieldNoId).toInt());
			clan->addUser(user);
		}

		_clanList << clan;
		emit clanAdded(clan);
		return clan;
	}

	return NULL;
}


class PusherHandler : public IPusherListener {
public:
	Pusher* pusher;
	FoeDataManager* data;

	void eventReceived(const string& event, const string& data) {
		this->data->handleRemoteEvent(event.c_str(), data.c_str());
	}

	void connectionEstablished() {
		pusher->join("private-testchannel");
	}

	void subscriptionSucceeded(const string& channel) {
		if (channel == "private-testchannel")
			this->data->sendAllUserHash();
	}

	void memberAdded(const string& user_id, const string& user_info) {
	}

	void memberRemoved(const string& user_id) {
	}

};


FoeDataManager::FoeDataManager()
	: pusher("dbc237fb9eac15998d95", "43c649a9018e9b957169", "App", "1.2")
{
	_persist.db() =  QSqlDatabase::addDatabase("QSQLITE");

	if (!_persist.db().isValid())
		qDebug() << "Driver could not be added";

	readSettings();

	PusherHandler* pusherHandler = new PusherHandler();
	pusherHandler->pusher = &pusher;
	pusherHandler->data = this;
	pusher.addListener(pusherHandler);

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
	bool ok = _persist.removeUserHas(user, product);
	if (ok) {
		QString data = QString("%1:%2").arg(user->name()).arg(product->id());
		pusher.send_message("private-testchannel", "client-removeuserhas", qPrintable(data));
	}

	return ok;
}


bool FoeDataManager::setUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level) {
	return _persist.setUserHas(user, product, factories, boost_level);
}


void FoeDataManager::notifyUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level) {
	QString data = QString("%1:%2:%3:%4").arg(user->name()).arg(product->id()).arg(factories).arg(boost_level);
	pusher.send_message("private-testchannel", "client-setuserhas", qPrintable(data));
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
	if (!_persist.doQuery("select * from users,products;"))
		foreach (q, initial_schema) { _persist.doQuery(q); }

	// First get schemaversion if any.
	int schemaversion = _persist.getIntOption("schemaversion", 0);

	// Run upgrade if current version is smaller than latest version
	if (schemaversion < valid_schema_version) {
		// V1
		if (schemaversion < 1)
			foreach (q, v1_schema) { _persist.doQuery(q); }

		// V2
		if (schemaversion < 2)
			foreach (q, v2_schema) { _persist.doQuery(q); }

		_persist.setIntOption("schemaversion", valid_schema_version);
	}
}


void FoeDataManager::addUser(FoeClan* clan, QString name)
{
	FoeUser* user = _persist.addUser(clan, name);
	if (user == NULL)
		QMessageBox::warning(NULL, tr("Add  user."), QString("Unable to add user.").arg(name), QMessageBox::Ok);
	else {
		clan->addUser(user);
		pusher.send_message("private-testchannel", "client-adduser", user->name().toStdString());
	}
}


void FoeDataManager::removeUser(FoeClan* clan, FoeUser* user)
{
	QString name = user->name();
	if (!_persist.removeUser(user))
		QMessageBox::warning(NULL, tr("Remove user."), QString("Unable to delete user.").arg(user->name()), QMessageBox::Ok);
	else {
		pusher.send_message("private-testchannel", "client-removeuser", user->name().toStdString());
		clan->removeUser(user);
	}
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


int64_t FoeDataManager::getUserTimestamp(FoeUser* user) {
	return _persist.getUserTimestamp(user);
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
		FoeUser* user = _persist.addUser(_clanList[0], data);
		_clanList[0]->addUser(user);

	}

	if (event == "client-removeuser") {
		FoeUser* user = _clanList[0]->getUser(data);
		if (_persist.removeUser(user))
			_clanList[0]->removeUser(user);
	}

	if (event == "client-userchanged") {
		printf("User changed !!!\n");
		fflush(0);
	}

	if (event == "client-setuserhas") {
		QStringList split = data.split(":");
		FoeUser* user = _clanList[0]->getFoeUser(split[0]);
		const FoeGoods* product = FoeGoods::fromId((e_Goods)split[1].toInt());
		int factories = split[2].toInt();
		BoostLevel boostlevel = (BoostLevel)split[3].toInt();

		user->setProduct(factories, product);
		user->setBonus(boostlevel, product);
	}

	if (event == "client-userhash") {
		QStringList split = data.split(":");
		FoeUser* user = _clanList[0]->getFoeUser(split[0]);
		QString hash = split[1];
		int64_t timestamp = split[1].toLongLong();

		// Only do stuff if the hashes are different
		if (hash != user->hash()) {
			if (user->timestamp() > timestamp) {
				// If we receive an older timestamp - send our user data
				QString data = user->serialize();
				pusher.send_message_unescaped("private-testchannel", "client-userdata", qPrintable(data));
			}

			if (user->timestamp() < timestamp) {
				// If we receive a newer timestampr - send user hash and timestamp
				sendUserHash(user);
			}
		}
	}

	if (event == "client-userdata") {
		QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
		FoeUser* user = _clanList[0]->getFoeUser(doc.object().value("user").toString());
		if (user)
			user->deserialize(doc.object());
	}
}


void FoeDataManager::sendUserHash(FoeUser* user) {
		QString hash = QString("%1:%2:%3").arg(user->name()).arg(user->timestamp()).arg(user->hash());
		pusher.send_message("private-testchannel", "client-userhash", qPrintable(hash));
}


void FoeDataManager::sendAllUserHash() {
	QVector<FoeUser*> users = _clanList[0]->getFoeUsers();
	FoeUser* user;
	foreach (user, users) {
		sendUserHash(user);
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
