#pragma once
#include "foeuser.h"

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QDebug>

class FoePersistence {
	QSqlDatabase _db;

public:
	FoePersistence();

	QSqlDatabase& db();


	bool doQuery(const QString& q, QSqlQuery* ret = NULL);
	bool doQuery(const QString& query_string, QSqlQuery& ret);

	int64_t getUserTimestamp(FoeUser* user);
	void setUserTimestamp(FoeUser* user, int64_t now = 0);

	FoeUser* addUser(FoeClan* clan, QString name);
	bool removeUser(FoeUser* user);
	bool addClan(const QString& clanname);
	bool removeClan(FoeClan* clan);
	bool renameClan(FoeClan* clan, const QString& new_name);
	bool removeUserHas(FoeUser* user, const FoeGoods* product);
	bool setUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level);

	int getIntOption(const QString& optionname, int defaultvalue);
	bool setIntOption(const QString& optionname, int value);

	QMap<const FoeGoods*, int> getUserHas(int userid);
	QMap<const FoeGoods *, BoostLevel> getUserHasBonus(int userid);
	int getClanID(const QString& clanname);
	QVector<int> getClanIDs();
	QString getClanName(int clanid);

};
