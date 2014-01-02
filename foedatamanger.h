#ifndef FOEDATAMANGER_H
#define FOEDATAMANGER_H

#include <QStringListModel>
#include <QtSql/QtSql>

#include "foeuser.h"
#include "foeproduct.h"

class FoeDataManager : public QObject
{
	Q_OBJECT
	QSqlDatabase _db;
	QStringListModel _userModel;
	QList<FoeUser*> _userList;

	QString _db_name;
	QString _db_username;
	QString _db_password;

	void readSettings();
	void writeSettings();
	FoeUser* FoeUserFactory(unsigned int userid);
	bool doQuery(const QString &query);

	void timerEvent(QTimerEvent *e);

public:
	FoeDataManager();
	~FoeDataManager();

	bool connect();
	bool isConnected();

	void refresh();

	QStringListModel *userModel();
	void addUser(QString name);
	void removeUser(FoeUser *user);

	void setDbUsername(const QString& username);
	void setDbPassword(const QString& password);

	QList<FoeUser*>& getFoeUsers() { return _userList; }
	FoeUser* getFoeUser(QString username);

	QString getUsername(int userid);
	bool setUserHas(int userid, int productId, int factories, BonusLevel bonus_level);
	bool removeUserHas(int userid, int productId);
	QMap<const FoeGoods*, int> getUserHas(int userid);
	bool loadusers();
	QMap<const FoeGoods *, BonusLevel> getUserHasBonus(int userid);
	QSet<FoeUser*> getUsersForProduct(const FoeGoods* product);
	const QString &getDbUsername();
	const QString &getDbPassword();
	const QString &getDbName();
	void setDbName(const QString &db);

	void disconnect();
signals:
	void userAdded(FoeUser* user);
	void userRemoved(FoeUser* user);
};

#endif // FOEDATAMANGER_H
