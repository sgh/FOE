#include "foepersistence.h"
#include "foeclan.h"

#include <QDateTime>
#include <QSqlRecord>

FoePersistence::FoePersistence()
{
}

QSqlDatabase&FoePersistence::db()
{
	return _db;
}


bool FoePersistence::doQuery(const QString& q, QSqlQuery* ret) {
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Query failed: " << q;
		return false;
	}

	if (ret)
		*ret = query;
	return true;
}


bool FoePersistence::doQuery(const QString& query_string, QSqlQuery& ret) {
	return doQuery(query_string, &ret);
}


int64_t FoePersistence::getUserTimestamp(FoeUser* user) {
	QSqlQuery query;
	doQuery(QString("select timestamp from users where id=%1").arg(user->id()), query);
	query.next();
	int idx = query.record().indexOf(("timestamp"));
	return query.value(idx).toLongLong();
}


void FoePersistence::setUserTimestamp(FoeUser* user, int64_t now) {
	QSqlQuery query;
	if (now == 0)
		now = QDateTime::currentMSecsSinceEpoch();
	if (doQuery(QString("update users set timestamp=%2 where id=%1").arg(user->id()).arg(now), query))
		user->setTimestamp(now);
}


FoeUser* FoePersistence::addUser(FoeClan* clan, QString name) {
	QSqlQuery query;
	FoeUser* user = NULL;
	qint64 now = QDateTime::currentMSecsSinceEpoch();
	bool ok = doQuery(QString("insert into users (name, clanid, timestamp) values (\"%1\", %2, %3);").arg(name).arg(clan->id()).arg(now), query);

	if (ok)
		ok = doQuery(QString("select id from users where name = \"%1\" and clanid=%2;").arg(name).arg(clan->id()), query);

	if (ok) {
		query.next();
		int fieldNoId = query.record().indexOf("id");
		user = new FoeUser(name, query.value(fieldNoId).toInt());
		user->setClan(clan);
	}

	return user;
}


bool FoePersistence::removeUser(FoeUser* user) {
	if (user == NULL)
		return false;

	QSqlQuery query;
	bool ok = doQuery(QString("delete from products where id_user = %1;").arg(user->id()), query);

	if (ok)
		ok = doQuery(QString("delete from users where id = \"%1\";").arg(user->id()), query);

	return ok;
}


bool FoePersistence::addClan(const QString& clanname) {
	QSqlQuery query(_db);

	return doQuery(QString("insert into clans (name) values (\"%1\")").arg(clanname), query);
}


int FoePersistence::getClanID(const QString& clanname) {
	QSqlQuery query(_db);

	if (!doQuery(QString("select id from clans where name = \"%1\";").arg(clanname), query))
		return -1;

	query.next();
	int fieldNoId = query.record().indexOf("id");
	int clanID = query.value(fieldNoId).toUInt();
	return clanID;
}

QVector<int> FoePersistence::getClanIDs() {
	QVector<int> ids;
	QSqlQuery query;
	doQuery("select * from clans;", query);
	while(query.next()) {
		int fieldNoId = query.record().indexOf("id");
		int clanID = query.value(fieldNoId).toUInt();
		ids << clanID;
	}
	return ids;
}


QString FoePersistence::getClanName(int clanid) {
	QString q = QString("select * from clans where id = %1;").arg(clanid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed" << q;

	int fieldNo = query.record().indexOf("name");
	query.next();
	return query.value(fieldNo).toString();
}


bool FoePersistence::removeClan(FoeClan* clan) {
	QSqlQuery query;
	return doQuery(QString("delete from clans where id = %1;").arg(clan->id()), query);
}


bool FoePersistence::renameClan(FoeClan* clan, const QString& new_name) {
	QSqlQuery query;
	return doQuery(QString("update clans set name=\"%1\" where name=\"%2\";").arg(new_name).arg(clan->name()), query);
}


bool FoePersistence::removeUserHas(FoeUser* user, const FoeGoods* product) {
	QSqlQuery query;
	setUserTimestamp(user);
	return doQuery(QString("delete from products where id_user = %1 and product = %2;").arg(user->id()).arg(product->id()), query);
}


bool FoePersistence::setUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level) {
	QSqlQuery query;
	setUserTimestamp(user);
	return doQuery(QString("replace into products (id_user,product,factories,bonus) values(%1,%2,%3,%4);").arg(user->id()).arg(product->id()).arg(factories).arg(boost_level), query);
}


int FoePersistence::getIntOption(const QString& optionname, int defaultvalue){
	QString q = QString("select val from options where name='%1';").arg(optionname);
	QSqlQuery query(_db);
	if (!query.exec(q)) {
		qDebug() << "Query failed: " << q;
	}

	query.next();
	bool b_ok;
	int value = query.value(0).toInt(&b_ok);

	if (!b_ok)
		return defaultvalue;

	return value;
}


bool FoePersistence::setIntOption(const QString& optionname, int value){
	QSqlQuery query(_db);
	return doQuery(QString("replace into options (name,val) values ('%1', '%2');").arg(optionname).arg(value), query);
}


QMap<const FoeGoods*, int> FoePersistence::getUserHas(int userid) {
	QString q = QString("select factories,product,bonus from products where id_user = %1;").arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed: " << q;

	int factories_fieldNo = query.record().indexOf("factories");
	int product_fieldNo   = query.record().indexOf("product");

	QMap<const FoeGoods*, int> products;
	while (query.next()) {
		enum e_Goods id = (enum e_Goods)query.value(product_fieldNo).toInt();
		const FoeGoods* product = FoeGoods::fromId(id);
		int factories = query.value(factories_fieldNo).toInt();
		if (product && factories>0)
			products[product] = factories;
	}
	return products;
}


QMap<const FoeGoods*, BoostLevel> FoePersistence::getUserHasBonus(int userid) {
	QString q = QString("select factories,product,bonus from products where id_user = %1;").arg(userid);
	QSqlQuery query(_db);
	if (!query.exec(q))
		qDebug() << "Query failed: " << q;

	int boost_fieldNo = query.record().indexOf("bonus");
	int product_fieldNo   = query.record().indexOf("product");

	QMap<const FoeGoods*, BoostLevel> products;
	while (query.next()) {
		enum e_Goods id = (enum e_Goods)query.value(product_fieldNo).toInt();
		const FoeGoods* product = FoeGoods::fromId(id);
		BoostLevel bl = (BoostLevel)query.value(boost_fieldNo).toInt();
		if (product && bl > e_NO_BOOST && bl < e_NUM_BOOSTLEVELS)
			products[product] = bl;
	}
	return products;
}
