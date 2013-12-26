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


class FoeProduct
{
	static bool _b_initialized;
	enum e_Products _id;
	QString _name;
	QPixmap _pixmap;
	QIcon _icon;
	static QStringList _bonusTexts;
	FoeProduct(enum e_Products id, QString name, QString base_english_name);

public:

	const QString name() const { return _name; }
	const QIcon &icon() const;
	enum e_Products id() const { return _id; }

	static const FoeProduct* fromId(enum e_Products id);
	static void initialize();

	static const QList<const FoeProduct *> &getProducts();

	static const QList<const FoeProduct *> &getProductsForAge(FoeAge* age);

	static const QList<const FoeProduct *> &getProgressiveEraProducts();
	static const QList<const FoeProduct *> &getIndustrialAgeProducts();
	static const QList<const FoeProduct *> &getColonialAgeProducts();
	static const QList<const FoeProduct *> &getLateMiddleAgeProducts();
	static const QList<const FoeProduct *> &getHighMiddleAgeProducts();
	static const QList<const FoeProduct *> &getEarlyMiddleAgeProducts();
	static const QList<const FoeProduct *> &getIronAgeProducts();
	static const QList<const FoeProduct *> &getBronzeAgeProducts();

	static const QString& bonusText(BonusLevel bl);
	static QString bonusColorHTML(BonusLevel bl);
	static const QStringList& bonusTexts();
	const QPixmap &pixmap() const;
};

