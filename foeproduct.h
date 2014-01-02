#pragma once

#include <QMap>
#include <QIcon>
#include <QStringList>

#include "foeages.h"


enum BonusLevel {
	e_NO_BONUS        = 0,
	e_NEEDS_RESEARCH  = 1,
	e_NOT_CONQUERED   = 2,
	e_BONUS           = 3,
	e_NUM_BONUSLEVELS = 4,
};

enum e_Products {
	// Bronze-age
	e_STONE,
	e_LUMBER,
	e_MARBLE,
	e_COLOR,
	e_WINE,

	// Iron-age
	e_LIMESTONE,
	e_EBONY,
	e_IRON,
	e_FABRIC,
	e_JEWELRY,

	// Early middleage
	e_COPPER,
	e_GRANIT,
	e_ALABASTER,
	e_GOLD,
	e_HONEY,

	// Highmiddleage
	e_BRICK,
	e_ROPE,
	e_SALT,
	e_HERBS,
	e_GLASS,

	// Late middleage
	e_BASALT,
	e_BRASS,
	e_SILK,
	e_TALC,
	e_GUNPOWDER,

	// Colonialage
	e_PAPER,
	e_COFFEE,
	e_WIRE,
	e_PORCELAIN,
	e_TAR,

	// Industrial age
	e_RUBBER,
	e_COKE,
	e_TEXTILE,
	e_WHALEOLIE,
	e_FERTILIZER,

	// The progressive era
	e_ASBESTOS,
	e_GASOLINE,
	e_MACHINEPARTS,
	e_TIN,
	e_EXPLOSIVES,
};


class FoeGoods
{
	static bool _b_initialized;
	enum e_Products _id;
	QString _name;
	QString _iconFile;
	QPixmap _pixmap;
	QIcon _icon;
	static QStringList _bonusTexts;
	FoeGoods(enum e_Products id, QString name, QString base_english_name);

public:

	const QString name() const { return _name;     }
	const QIcon &icon() const  { return _icon;     }
	const QString& iconFile() const  { return _iconFile; }
	enum e_Products id() const { return _id;       }

	static const FoeGoods* fromId(enum e_Products id);
	static void initialize();

	static const QList<const FoeGoods *> &getProducts();

	static const QList<const FoeGoods *> &getProductsForAge(FoeAge* age);

	const QString& bonusText(BonusLevel bl) const;
	QString bonusColorHTML(BonusLevel bl) const;
	QColor bonusColor(BonusLevel bl) const;
	static const QStringList& bonusTexts();
	const QPixmap &pixmap() const;
};

