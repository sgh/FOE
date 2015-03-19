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
	QVector<FoeClan*> _clanList;
	QSemaphore _commandSemaphore;
	QMutex _commandLock;
	QQueue<SqlCommand*> _commandQ;
	volatile bool _threadRun;

	void readSettings();
	void writeSettings();
	bool doQuery(const QString &query_string, QSqlQuery* ret = NULL);
	bool doQuery(const QString &query_string, QSqlQuery& ret);
	void updateInsertPrivileges();
	void migrateDatabase();
	void loadclans();
	void run();

public:
	FoeDataManager();
	~FoeDataManager();

	void postCommand(SqlCommand* cmd);


	// Commands
	void addUser(FoeClan* clan, QString name);
	void removeUser(FoeClan* clan, FoeUser* user);
	void addClan(const QString& clanname);
	void removeClan(FoeClan* clan);
	bool renameClan(FoeClan* clan, const QString& new_name);
	bool removeUserHas(FoeUser* user, const FoeGoods* product);
	bool setUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level);

	// Callbacks from commands
	void removeClanFromList(FoeClan* clan);
	void clanRenameCallback(FoeClan* clan);
	FoeClan* FoeClanFactory(unsigned int clanid);

	// FOE structure getterS
	QMap<const FoeGoods*, int> getUserHas(int userid);
	QMap<const FoeGoods *, BoostLevel> getUserHasBonus(int userid);
	QString getClanname(int clanid);
	FoeClan* getClan(const QString clanname);

	// Init functions
	void dbdisconnect();
	bool dbconnect(const QString& dbfile);
	bool isConnected();

signals:
	void clanAdded(FoeClan* clan);
	void clanAboutToBeRemoved(FoeClan* clan);
	void clanRenamed(FoeClan* clan);
};


#endif // FOEDATAMANGER_H
