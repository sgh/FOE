#ifndef FOEUSER_H
#define FOEUSER_H

#include <QObject>
#include <QList>
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

	QMap<const FoeGoods*, int> _factories;
	QMap<const FoeGoods*, BoostLevel> _boost;

	void initialize();
	void storeGoods(const FoeGoods* goods, int factories, BoostLevel boost_level);

public:
	~FoeUser() {}

	FoeUser(FoeClan* clan, FoeDataManager* data, int userid);

	void setProduct(int factories, const FoeGoods* product);
	void setBonus(BoostLevel boost_level, const FoeGoods* product);

	int hasProduct(const FoeGoods* product);
	BoostLevel hasBonus(const FoeGoods* product);

	const QMap<const FoeGoods*, BoostLevel>& allBonus();

	QSet<const FoeGoods*> getProducts();
	const QString& name() const { return _username; }
	const QString& clanName();
	unsigned int id() const     { return _userid;   }

	void reload();
};


#endif // FOEUSER_H
