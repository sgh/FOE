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

	void readSettings();
	void writeSettings();
	FoeClan* FoeClanFactory(unsigned int clanid);
	bool doQuery(const QString &query);
	void updateInsertPrivileges();

	void timerEvent(QTimerEvent *e);

public:
	FoeDataManager();
	~FoeDataManager();

	void postCommand(SqlCommand* cmd);

	void run();

	// Commands
	void addUser(FoeClan* clan, QString name);
	void removeUser(FoeClan* clan, FoeUser *user);

	// Callbacks from commands
	void removeUserFromList(FoeUser* user);

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

	// Init functions
	bool loadusers(FoeClan* clan, bool complete_reload = false);
	void loadclans();
	void disconnect();
	bool connect();
	bool isConnected();
	void refresh();

signals:
	void userAdded(FoeUser* user);
	void userRemoved();
	void clanAdded(FoeClan* clan);
	void clanRemoved(FoeClan* clan);
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
			case 1: return QString("select id from users where name = \"%1\";").arg(_name);
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


#endif // FOEDATAMANGER_H
