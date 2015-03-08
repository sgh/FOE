#ifndef FOEDATAMANGER_H
#define FOEDATAMANGER_H

#include <QStringListModel>
#include <QtSql/QtSql>
#include <QDebug>

#include "foeclan.h"
#include "foeuser.h"
#include "foegoods.h"


/**
 * @brief Generic sql command
 */
class SqlCommand {
public:
	/**
	 * @brief Returns the actual query
	 * @return
	 */
	virtual QString query(int n = 0) = 0;
	virtual int nqueries() { return 1; }
	virtual ~SqlCommand() {}
	virtual void actionSuccess(int, QSqlQuery*) {}
	virtual void actionFailed() {}
};


class FoeDataManager : public QThread
{
	Q_OBJECT
	QSqlDatabase _db;
	QList<FoeClan*> _clanList;
	QSemaphore _commandSemaphore;
	QMutex _commandLock;
	QQueue<SqlCommand*> _commandQ;
	volatile bool _threadRun;

	QString _db_name;
	QString _db_server;
	QString _db_username;
	QString _db_password;
	bool _b_insertPrivileges;
	QString _user_table;

	void readSettings();
	void writeSettings();
	bool doQuery(const QString &query);
	void updateInsertPrivileges();
	void migrateDatabase();

	void timerEvent(QTimerEvent *e);

public:
	FoeDataManager();
	~FoeDataManager();

	void postCommand(SqlCommand* cmd);

	void run();

	// Commands
	void addUser(FoeClan* clan, QString name);
	void removeUser(FoeClan* clan, FoeUser* user);
	void addClan(const QString& clanname);
	void removeClan(FoeClan* clan);
	bool renameClan(FoeClan* clan, const QString& new_name);

	// Callbacks from commands
	void removeClanFromList(FoeClan* clan);
	void clanRenameCallback(FoeClan* clan);
	FoeClan* FoeClanFactory(unsigned int clanid);

	// Database connection getters and setters
	const QString &getDbUsername();
	void setDbUsername(const QString& username);
	const QString &getDbPassword();
	void setDbPassword(const QString& password);
	const QString &getDbName();
	const QString& getServerName();
	void setDbName(const QString &db);
	void setServerName(const QString& servername);
	bool hasInsertPrivileges();

	// FOE structure getterS
	QMap<const FoeGoods*, int> getUserHas(int userid);
	QMap<const FoeGoods *, BoostLevel> getUserHasBonus(int userid);
	QString getUsername(int userid);
	QString getClanname(int clanid);
	FoeClan* getClan(const QString clanname);

	// Init functions
	void loadclans();
	void dbdisconnect();
	bool dbconnect();
	bool isConnected();

signals:
	void clanAdded(FoeClan* clan);
	void clanAboutToBeRemoved(FoeClan* clan);
	void clanRenamed(FoeClan* clan);
};





class RemoveUserHasCommand : public SqlCommand {
	int _useridID;
	int _productID;

public:
	RemoveUserHasCommand(int userID, int productID) {
		_useridID = userID;
		_productID = productID;
	}

	virtual QString query(int) override {
		return QString("delete from products where id_user = %1 and product = %2;").arg(_useridID).arg(_productID);
	}
};



class LoadUsersCommand : public SqlCommand {
	FoeClan* _clan;
	bool _complete_reload;

public:
	LoadUsersCommand(FoeClan* clan, bool complete_reload)
		: _clan(clan)
		, _complete_reload(complete_reload) {
	}

	QString query(int n) {
		Q_UNUSED(n);
		QString _user_table = "users";
		return QString("select * from %1 where clanid=%2;").arg(_user_table).arg(_clan->id());
	}

	void actionSuccess(int, QSqlQuery* query) {
		FoeUser* user;
		QSet<FoeUser*> userSet;

		if (_complete_reload) {
			foreach (user, _clan->getFoeUsers()) {
				_clan->removeUser(user);
				delete user;
			}
		}

		QStringList lst;
		int fieldNo = query->record().indexOf("name");
		int fieldNoUserId = query->record().indexOf("id");
		while (query->next()) {
			lst.append(query->value(fieldNo).toString());
			FoeUser* user = _clan->FoeUserFactory(query->value(fieldNoUserId).toInt());
			userSet << user;
		}

		// Substract the two sets to find which users where removed
		userSet = _clan->getFoeUsers().toSet() - userSet;
		foreach (user, userSet) {
			_clan->removeUser(user);
			delete user;
		}

		lst.sort();
		if (lst != _clan->userModel()->stringList())
			_clan->userModel()->setStringList(lst);
	}

};

class LoadClansCommand : public SqlCommand {
	FoeDataManager* _data;

public:
	LoadClansCommand(FoeDataManager* data) : _data(data) {
	}

	virtual QString query(int) override {
		return "select * from clans;";
	}

	virtual void actionSuccess(int, QSqlQuery* query) override {
		int fieldNoId = query->record().indexOf("id");
		_data->FoeClanFactory(query->value(fieldNoId).toUInt());
	}
};

class SetUserHasCommand : public SqlCommand {
	int _userID;
	int _productID;
	int _factories;
	BoostLevel _boost_level;

public:
	SetUserHasCommand(int userID, int productID, int factories, BoostLevel boost_level) {
		_userID = userID;
		_productID = productID;
		_factories = factories;
		_boost_level = boost_level;
	}

	virtual QString query(int) override {
		return QString("replace into products (id_user,product,factories,bonus) values(%1,%2,%3,%4);").arg(_userID).arg(_productID).arg(_factories).arg(_boost_level);
	}
};


class AddUserCommand : public SqlCommand {
	QString _name;
	FoeClan* _clan;

public:
	AddUserCommand(FoeClan* clan, const QString& name) {
		_name = name;
		_clan = clan;
	}

	int nqueries() override {
		return 2;
	}

	void actionSuccess(int n, QSqlQuery* result) override  {
		if (n != 1)
			return;
		int fieldNo = result->record().indexOf("id");
		result->next();
		_clan->FoeUserFactory(result->value(fieldNo).toInt());
	}

	QString query(int n) override {
		switch (n) {
			case 0: return QString("insert into users (name, clanid) values (\"%1\", %2);").arg(_name).arg(_clan->id());
			case 1: return QString("select id from users where name = \"%1\" and clanid=%2;").arg(_name).arg(_clan->id());
		}
		return "";
	}
};


class AddClanCommand : public SqlCommand {
	QString _name;
	FoeDataManager* _data;
	QString _add_view_query;

public:
	AddClanCommand(FoeDataManager* data, const QString& name) {
		_name = name;
		_data = data;
	}

	int nqueries() override {
		return 3;
	}

	void actionSuccess(int n, QSqlQuery* result) override  {
		if (n != 1)
			return;
		int fieldNo = result->record().indexOf("id");
		result->next();
		FoeClan* clan = _data->FoeClanFactory(result->value(fieldNo).toInt());
		_add_view_query = QString("create view users_%1 as select * from users where clanid=%2;").arg(clan->name().toUpper()).arg(clan->id());
	}

	QString query(int n) override {
		switch (n) {
			case 0: return QString("insert into clans (name) values (\"%1\")").arg(_name);
			case 1: return QString("select id from clans where name = \"%1\";").arg(_name);
			case 2: return _add_view_query;
		}
		return "";
	}
};


class RemoveUserCommand : public SqlCommand {
	FoeUser* _user;
	FoeClan* _clan;

public:
	RemoveUserCommand(FoeClan* clan, FoeUser* user) {
		_user = user;
		_clan = clan;
	}

	int nqueries() override {
		return 2;
	}

	QString query(int n) override {
		switch (n) {
			case 0: return QString("delete from products where id_user = %1;").arg(_user->id());
			case 1: return QString("delete from users where id = \"%1\";").arg(_user->id());
		}
		return "";
	}

	void actionSuccess(int n, QSqlQuery*) override  {
		if (n==1)
			_clan->removeUser(_user);
	}
};


class RemoveClanCommand : public SqlCommand {
	FoeClan* _clan;
	FoeDataManager* _data;
	QString _drop_view_query;

public:
	RemoveClanCommand(FoeDataManager* data, FoeClan* clan) {
		_clan = clan;
		_data = data;
		_drop_view_query = QString("drop view users_%1;").arg(_clan->name().toUpper());
	}

	int nqueries() override {
		return 2;
	}

	QString query(int n) override {
		switch (n) {
			case 0: return QString("delete from clans where id = %1;").arg(_clan->id());
			case 1: return _drop_view_query;
		}
		return "";
	}

	void actionSuccess(int n, QSqlQuery*) override  {
		if (n==0)
			_data->removeClanFromList(_clan);
	}
};



class RenameClanCommand : public SqlCommand {
	FoeClan* _clan;
	QString _new_name;
	QString _drop_view_query;
	QString _create_view_query;
	FoeDataManager* _data;

public:
	RenameClanCommand(FoeDataManager* data, FoeClan* clan, const QString& new_name) {
		_clan = clan;
		_new_name = new_name;
		_data = data;
		_drop_view_query   = QString("drop view users_%1;").arg(_clan->name().toUpper());
		_create_view_query = QString("create view users_%1 as select * from users where clanid=%2;").arg(_new_name.toUpper()).arg(_clan->id());
	}

	int nqueries() override {
		return 3;
	}

	QString query(int n) override {
		switch (n) {
			case 0: return QString("update clans set name=\"%1\" where name=\"%2\";").arg(_new_name).arg(_clan->name());
			case 1: return _drop_view_query;
			case 2: return _create_view_query;
		}
		return "";
	}

	void actionSuccess(int n, QSqlQuery*) override  {
		if (n==0) {
			_clan->setName(_new_name);
			_data->clanRenameCallback(_clan);
		}
	}
};

#endif // FOEDATAMANGER_H