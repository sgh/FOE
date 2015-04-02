#pragma once

#include <QMap>
#include <QIcon>
#include <QStringList>

#include "foeages.h"


enum BoostLevel {
	e_NO_BOOST        = 0,
	e_NEEDS_RESEARCH  = 1,
	e_NOT_CONQUERED   = 2,
	e_BOOST           = 3,
	e_NUM_BOOSTLEVELS = 4,
};

enum e_Goods {
	// Bronze age
	e_STONE,
	e_LUMBER,
	e_MARBLE,
	e_COLOR,
	e_WINE,

	// Iron age
	e_LIMESTONE,
	e_EBONY,
	e_IRON,
	e_FABRIC,
	e_JEWELRY,

	// Early middle ages
	e_COPPER,
	e_GRANIT,
	e_ALABASTER,
	e_GOLD,
	e_HONEY,

	// High middle ages
	e_BRICK,
	e_ROPE,
	e_SALT,
	e_HERBS,
	e_GLASS,

	// Late middle ages
	e_BASALT,
	e_BRASS,
	e_SILK,
	e_TALC,
	e_GUNPOWDER,

	// Colonial age
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

	// Modern Materials
	e_FERROCONCRETE,
	e_CONVENIENCEFOOD,
	e_FLAVORANTS,
	e_PACKAGES,
	e_LUXYRYMATERIALS,

	// Postmoder Era
	e_RENEWABLE_RESOURCES,
	e_STEEL,
	e_SEMICONDUCTORS,
	e_INDUSTRIALFILTERS,
	e_GENOMEDATA,

	// Contemporary Era
	e_ELECTROMAGNETS,
	e_GAS,
	e_PLASTICS,
	e_ROBOTS,
	e_BIONICSDATA,

	//	Tomorrow
	e_TRANSLUCENTCONCRETE,
	e_SMARTMATERIALS,
	e_PAPERCRETE,
	e_PRESERVATIVES,
	e_NUTRITIONRESEARCH,
};


class FoeGoods : QObject
{
	Q_OBJECT
	enum e_Goods _id;
	QString _name;
	QString _iconFile;
	QPixmap _pixmap;
	QIcon _icon;
	static QStringList _boostTexts;
	FoeGoods(enum e_Goods id, const char* name);

public:

	const QString name() const { return _name;     }
	const QIcon &icon() const  { return _icon;     }
	const QString& iconFile() const  { return _iconFile; }
	enum e_Goods id() const { return _id;       }

	static const FoeGoods* fromId(enum e_Goods id);
	static void initialize();
	static void deinitialize();

	static const QVector<const FoeGoods *> &getGoods();

	static const QVector<const FoeGoods*>& getGoodsForAge(FoeAge* age);

	const QString& boostText(BoostLevel bl) const;
	QString boostColorHTML(BoostLevel bl) const;
	QColor boostColor(BoostLevel bl) const;
	static const QStringList& boostTexts();
	const QPixmap &pixmap() const;
};

