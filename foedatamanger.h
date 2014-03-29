#ifndef FOEDATAMANGER_H
#define FOEDATAMANGER_H

#include <QStringListModel>
#include <QtSql/QtSql>
#include <QDebug>

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
	virtual void actionSuccess(FoeDataManager*, int, QSqlQuery*) {}
	virtual void actionFailed() {}
};


class FoeDataManager : public QThread
{
	Q_OBJECT
	QSqlDatabase _db;
	QStringListModel _userModel;
	QList<FoeUser*> _userList;
	QSemaphore _commandSemaphore;
	QMutex _commandLock;
	QQueue<SqlCommand*> _commandQ;
	volatile bool _threadRun;

	QString _db_name;
	QString _db_username;
	QString _db_password;
	bool _b_insertPrivileges;

	void readSettings();
	void writeSettings();
	FoeUser* FoeUserFactory(unsigned int userid);
	bool doQuery(const QString &query);
	void updateInsertPrivileges();

	void timerEvent(QTimerEvent *e);

public:
	FoeDataManager();
	~FoeDataManager();

	void postCommand(SqlCommand* cmd);

	void run();

	// Commands
	void addUser(QString name);
	void removeUser(FoeUser *user);

	// Callbacks from commands
	void removeUserFromList(FoeUser* user);
	void addUserToList(FoeUser* user);

	// Model
	QStringListModel *userModel();

	// Database connection getters and setters
	const QString &getDbUsername();
	void setDbUsername(const QString& username);
	const QString &getDbPassword();
	void setDbPassword(const QString& password);
	const QString &getDbName();
	void setDbName(const QString &db);
	bool hasInsertPrivileges();

	// FOE structure getterS
	QList<FoeUser*>& getFoeUsers() { return _userList; }
	FoeUser* getFoeUser(QString username);
	QMap<const FoeGoods*, int> getUserHas(int userid);
	QMap<const FoeGoods *, BonusLevel> getUserHasBonus(int userid);
	QSet<FoeUser*> getUsersForProduct(const FoeGoods* product);
	QString getUsername(int userid);

	// Init functions
	bool loadusers(bool complete_reload = false);
	void disconnect();
	bool connect();
	bool isConnected();
	void refresh();

signals:
	void userAdded(FoeUser* user);
	void userRemoved(FoeUser* user);
};





class RemoveUserHasCommand : public SqlCommand {
	int _useridID;
	int _productID;

public:
	RemoveUserHasCommand(int userID, int productID) {
		_useridID = userID;
		_productID = productID;
	}

	virtual QString query(int)
	{
		return QString("delete from products where id_user = %1 and product = %2;").arg(_useridID).arg(_productID);
	}
};


class SetUserHasCommand : public SqlCommand {
	int _userID;
	int _productID;
	int _factories;
	BonusLevel _bonus_level;

public:
	SetUserHasCommand(int userID, int productID, int factories, BonusLevel bonus_level) {
		_userID = userID;
		_productID = productID;
		_factories = factories;
		_bonus_level = bonus_level;
	}

	virtual QString query(int)
	{
		return QString("replace into products (id_user,product,factories,bonus) values(%1,%2,%3,%4);").arg(_userID).arg(_productID).arg(_factories).arg(_bonus_level);
	}
};


class AddUserCommand : public SqlCommand {
	QString _name;

public:
	AddUserCommand(const QString& name) {
		_name = name;
	}

	int nqueries() {
		return 2;
	}

	void actionSuccess(FoeDataManager* data, int n, QSqlQuery* result) override  {
		if (n != 1)
			return;
		int fieldNo = result->record().indexOf("id");
		result->next();
		FoeUser* user = new FoeUser(data, result->value(fieldNo).toInt());
		data->addUserToList(user);
	}

	QString query(int n) {
		switch (n) {
			case 0: return QString("insert into users (name) values (\"%1\");").arg(_name);
			case 1: return QString("select id from users where name = \"%1\";").arg(_name);
		}
		return "";
	}
};


class RemoveUserCommand : public SqlCommand {
	FoeUser* _user;
public:
	RemoveUserCommand(FoeUser* user) {
		_user = user;
	}

	int nqueries() {
		return 2;
	}

	QString query(int n) {
		switch (n) {
			case 0: return QString("delete from products where id_user = %1;").arg(_user->id());
			case 1: return QString("delete from users where id = \"%1\";").arg(_user->id());
		}
	}

	void actionSuccess(FoeDataManager* data, int n, QSqlQuery* result) override  {
		if (n==1)
			data->removeUserFromList(_user);
	}
};


#endif // FOEDATAMANGER_H
