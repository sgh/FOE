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

	QString _db_username;
	QString _db_password;

	void readSettings();
	void writeSettings();
	bool doQuery(const QString &query);

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
	QMap<const FoeProduct*, int> getUserHas(int userid);
	bool loadusers();
	QMap<const FoeProduct *, BonusLevel> getUserHasBonus(int userid);
	QSet<FoeUser*> getUsersForProduct(const FoeProduct* product);
	const QString &getDbUsername();
	const QString &getDbPassword();
	bool getUserAdmin(int userid);

signals:
	void userAdded(FoeUser* user);
	void userRemoved(FoeUser* user);
};

#endif // FOEDATAMANGER_H
