#ifndef FOEDATAMANGER_H
#define FOEDATAMANGER_H

#include <memory>

#include <QStringListModel>
#include <QtSql/QtSql>
#include <QDebug>

#include "foeclan.h"
#include "foeuser.h"
#include "foegoods.h"
#include "foepersistence.h"

#include "pusher.h"


class FoeDataManager : public QObject
{
	Q_OBJECT
	std::shared_ptr<FoePersistence> _persist;
	QString _filename;
	QVector<std::shared_ptr<FoeClan>> _clanList;

	void readSettings();
	void writeSettings();
	void updateInsertPrivileges();
	void migrateDatabase();
	void loadclans();

public:
	FoeDataManager(std::shared_ptr<FoePersistence> persist);
	~FoeDataManager();

	// Commands
	std::shared_ptr<FoeUser> addUser(FoeClan* clan, QString name);
	bool removeUser(FoeClan* clan, FoeUser* user);
	void addClan(const QString& clanname);
	void removeClan(FoeClan* clan);
	bool renameClan(FoeClan* clan, const QString& new_name);
	bool removeUserHas(FoeUser* user, const FoeGoods* product);
	bool setUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level);
	void setUserTimestamp(FoeUser* user, int64_t timestamo);

	void removeClanFromList(FoeClan* clan);
	void constructClan(unsigned int clanid);

	// FOE structure getterS
	QMap<const FoeGoods*, int> getUserHas(int userid);
	QMap<const FoeGoods *, BoostLevel> getUserHasBonus(int userid);
	QString getClanname(int clanid);
	bool clanExists(const QString clanname);
	int64_t getUserTimestamp(FoeUser* user);
	FoeClan* currentClan();

	// File loading funcions
	QString currentFile();
	bool loadFile(const QString& dbfile, bool overwrite);
	bool isValid();
	void closeFile();

signals:
	void fileChanged(const QString& name);
	void fileClosed();
	void clanAdded(FoeClan* clan);
	void clanAboutToBeRemoved(FoeClan* clan);
	void clanRenamed(FoeClan* clan);
};


#endif // FOEDATAMANGER_H
