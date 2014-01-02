#ifndef FOEUSER_H
#define FOEUSER_H

#include <QObject>
#include <QList>
#include <QStringListModel>

#include "foeproduct.h"

class FoeDataManager;

class FoeUser : public QObject
{
	Q_OBJECT

	bool _b_initialized;
	int _userid;
	QString _username;
	FoeDataManager* _data;

	QMap<const FoeGoods*, int> _factories;
	QMap<const FoeGoods*, BonusLevel> _bonus;

	void initialize();

public:
	~FoeUser() {}

	FoeUser(FoeDataManager* data, int userid);

	void setProduct(int factories, const FoeGoods* product);
	void setBonus(BonusLevel bonus_level, const FoeGoods* product);

	int hasProduct(const FoeGoods* product);
	BonusLevel hasBonus(const FoeGoods* product);

	const QMap<const FoeGoods*, BonusLevel>& allBonus();

	QSet<const FoeGoods*> getProducts();
	const QString& name() const { return _username; }
	unsigned int id() const     { return _userid;   }

	void reload();

signals:
	void updated();
};


#endif // FOEUSER_H
