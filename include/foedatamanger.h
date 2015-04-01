#ifndef FOEDATAMANGER_H
#define FOEDATAMANGER_H

#include <QStringListModel>
#include <QtSql/QtSql>
#include <QDebug>

#include "foeclan.h"
#include "foeuser.h"
#include "foegoods.h"
#include "foepersistence.h"


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


class FoeDataManager : public QObject
{
	Q_OBJECT
	FoePersistence _persist;
	QString _filename;
	QVector<FoeClan*> _clanList;

	void readSettings();
	void writeSettings();
	void updateInsertPrivileges();
	void migrateDatabase();
	void loadclans();

public:
	FoeDataManager();
	~FoeDataManager();

	// Commands
	void addUser(FoeClan* clan, QString name);
	void removeUser(FoeClan* clan, FoeUser* user);
	void addClan(const QString& clanname);
	void removeClan(FoeClan* clan);
	bool renameClan(FoeClan* clan, const QString& new_name);
	bool removeUserHas(FoeUser* user, const FoeGoods* product);
	bool setUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level);

	void removeClanFromList(FoeClan* clan);
	FoeClan* constructClan(unsigned int clanid);

	// FOE structure getterS
	QMap<const FoeGoods*, int> getUserHas(int userid);
	QMap<const FoeGoods *, BoostLevel> getUserHasBonus(int userid);
	QString getClanname(int clanid);
	FoeClan* getClan(const QString clanname);

	// File loading funcions
	QString currentFile();
	bool loadFile(const QString& dbfile, bool overwrite);
	bool isValid();
	void closeFile();

public slots:
	void handleRemoteEvent(const QString& event, const QString& data);

signals:
	void fileChanged(const QString& name);
	void fileClosed();
	void clanAdded(FoeClan* clan);
	void clanAboutToBeRemoved(FoeClan* clan);
	void clanRenamed(FoeClan* clan);
};


#endif // FOEDATAMANGER_H
