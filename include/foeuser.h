#ifndef FOEUSER_H
#define FOEUSER_H

#include <QObject>
#include <QStringListModel>

#include "foegoods.h"

class FoeDataManager;
class FoeClan;

class FoeUser : public QObject
{
	Q_OBJECT

	bool _b_initialized;
	int _userid;
	QString _username;
	FoeDataManager* _data;
	FoeClan* _my_clan;
	int64_t _timestamp;

	QMap<const FoeGoods*, int> _factories;
	QMap<const FoeGoods*, BoostLevel> _boost;

	void initialize();
	void storeGoods(const FoeGoods* goods, int factories, BoostLevel boost_level);

public:
	~FoeUser() {}

	FoeUser(const QString& name, int userid);

	void setProduct(int factories, const FoeGoods* product);
	void setBonus(BoostLevel boost_level, const FoeGoods* product);
	void setTimestamp(int64_t timestamp);

	int hasProduct(const FoeGoods* product);
	BoostLevel hasBonus(const FoeGoods* product);

	const QMap<const FoeGoods*, BoostLevel>& allBonus();

	QSet<const FoeGoods*> getProducts();
	const QString& name() const { return _username; }
	const QString& clanName();
	unsigned int id() const     { return _userid;   }
	int64_t timestamp()         { return _timestamp; }
	QString serialize();
	void deserialize(QJsonObject json);
	QString hash();

	void setData(FoeDataManager& data);
	void setClan(FoeClan* clan);
};


#endif // FOEUSER_H
