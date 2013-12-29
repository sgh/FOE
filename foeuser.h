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

	QMap<const FoeProduct*, int> _factories;
	QMap<const FoeProduct*, BonusLevel> _bonus;

	void initialize();

public:
	~FoeUser() {}

	FoeUser(FoeDataManager* data, int userid);

	void setProduct(int factories, const FoeProduct* product);
	void setBonus(BonusLevel bonus_level, const FoeProduct* product);

	int hasProduct(const FoeProduct* product);
	BonusLevel hasBonus(const FoeProduct* product);

	const QMap<const FoeProduct*, BonusLevel>& allBonus();

	QSet<const FoeProduct*> getProducts();
	const QString& name() const { return _username; }
	unsigned int id() const     { return _userid;   }

	void reload();

signals:
	void updated();
};


#endif // FOEUSER_H
