#include <map>

#include <QApplication>
#include <QString>

#include "foeproduct.h"

using namespace std;

static QList<const FoeGoods*> _mAll;
static QList<const FoeGoods*> _mEmpty;

static QList<const FoeGoods*> _mBronzeAge;
static QList<const FoeGoods*> _mIronAge;
static QList<const FoeGoods*> _mEarlyMiddleAges;
static QList<const FoeGoods*> _mHighMiddleAges;
static QList<const FoeGoods*> _mLateMiddleAges;
static QList<const FoeGoods*> _mColonialAge;
static QList<const FoeGoods*> _mIndustrialAge;
static QList<const FoeGoods*> _mProgressiveEra;
static QList<const FoeGoods*> _mModernMaterials;
static QList<const FoeGoods*> _mPostmodernEra;


bool FoeGoods::_b_initialized = false;
QStringList FoeGoods::_bonusTexts;

void FoeGoods::initialize() {
	if (_b_initialized)
		return;
	_b_initialized = true;

	_bonusTexts.insert(e_NO_BONUS,        tr("No boost"      ) );
	_bonusTexts.insert(e_NEEDS_RESEARCH,  tr("Need research" ) );
	_bonusTexts.insert(e_NOT_CONQUERED,   tr("Not conquered" ) );
	_bonusTexts.insert(e_BONUS,           tr("Boost"         ) );

	_mBronzeAge << new FoeGoods( e_STONE,   QT_TR_NOOP("Stone"   ) );
	_mBronzeAge << new FoeGoods( e_LUMBER,  QT_TR_NOOP("Lumber"  ) );
	_mBronzeAge << new FoeGoods( e_MARBLE,  QT_TR_NOOP("Marble"  ) );
	_mBronzeAge << new FoeGoods( e_COLOR,   QT_TR_NOOP("Dye"     ) );
	_mBronzeAge << new FoeGoods( e_WINE,    QT_TR_NOOP("Wine"    ) );
	_mAll << _mBronzeAge;

	_mIronAge << new FoeGoods( e_LIMESTONE, QT_TR_NOOP("Limestone" ) );
	_mIronAge << new FoeGoods( e_EBONY,     QT_TR_NOOP("Ebony"     ) );
	_mIronAge << new FoeGoods( e_IRON,      QT_TR_NOOP("Iron"      ) );
	_mIronAge << new FoeGoods( e_FABRIC,    QT_TR_NOOP("Cloth"     ) );
	_mIronAge << new FoeGoods( e_JEWELRY,   QT_TR_NOOP("Jewelry"   ) );
	_mAll << _mIronAge;

	_mEarlyMiddleAges << new FoeGoods( e_COPPER,    QT_TR_NOOP("Copper"    ) );
	_mEarlyMiddleAges << new FoeGoods( e_GRANIT,    QT_TR_NOOP("Granite"   ) );
	_mEarlyMiddleAges << new FoeGoods( e_ALABASTER, QT_TR_NOOP("Alabaster" ) );
	_mEarlyMiddleAges << new FoeGoods( e_GOLD,      QT_TR_NOOP("Gold"      ) );
	_mEarlyMiddleAges << new FoeGoods( e_HONEY,     QT_TR_NOOP("Honney"    ) );
	_mAll << _mEarlyMiddleAges;

	_mHighMiddleAges << new FoeGoods( e_BRICK,  QT_TR_NOOP("Brick"        ) );
	_mHighMiddleAges << new FoeGoods( e_ROPE,   QT_TR_NOOP("Rope"         ) );
	_mHighMiddleAges << new FoeGoods( e_SALT,   QT_TR_NOOP("Salt"         ) );
	_mHighMiddleAges << new FoeGoods( e_HERBS,  QT_TR_NOOP("Dried Herbs"  ) );
	_mHighMiddleAges << new FoeGoods( e_GLASS,  QT_TR_NOOP("Glass"        ) );
	_mAll << _mHighMiddleAges;

	_mLateMiddleAges << new FoeGoods( e_BASALT,     QT_TR_NOOP("Basalt"       ) );
	_mLateMiddleAges << new FoeGoods( e_BRASS,      QT_TR_NOOP("Brass"        ) );
	_mLateMiddleAges << new FoeGoods( e_SILK,       QT_TR_NOOP("Silk"         ) );
	_mLateMiddleAges << new FoeGoods( e_TALC,       QT_TR_NOOP("Talc Powder"  ) );
	_mLateMiddleAges << new FoeGoods( e_GUNPOWDER,  QT_TR_NOOP("Gunpowder"    ) );
	_mAll << _mLateMiddleAges;

	_mColonialAge << new FoeGoods( e_PAPER,      QT_TR_NOOP("Paper"     ) );
	_mColonialAge << new FoeGoods( e_COFFEE,     QT_TR_NOOP("Coffee"    ) );
	_mColonialAge << new FoeGoods( e_WIRE,       QT_TR_NOOP("Wire"      ) );
	_mColonialAge << new FoeGoods( e_PORCELAIN,  QT_TR_NOOP("Porcelain" ) );
	_mColonialAge << new FoeGoods( e_TAR,        QT_TR_NOOP("Tar"       ) );
	_mAll << _mColonialAge;

	_mIndustrialAge << new FoeGoods( e_RUBBER,      QT_TR_NOOP("Rubber"     ) );
	_mIndustrialAge << new FoeGoods( e_COKE,        QT_TR_NOOP("Coke"       ) );
	_mIndustrialAge << new FoeGoods( e_TEXTILE,     QT_TR_NOOP("Textile"    ) );
	_mIndustrialAge << new FoeGoods( e_WHALEOLIE,   QT_TR_NOOP("Whale Oil"  ) );
	_mIndustrialAge << new FoeGoods( e_FERTILIZER,  QT_TR_NOOP("Fertilizer" ) );
	_mAll << _mIndustrialAge;

	_mProgressiveEra << new FoeGoods( e_ASBESTOS,      QT_TR_NOOP("Asbestos"      ) );
	_mProgressiveEra << new FoeGoods( e_GASOLINE,      QT_TR_NOOP("Gasoline"      ) );
	_mProgressiveEra << new FoeGoods( e_MACHINEPARTS,  QT_TR_NOOP("Machine Parts" ) );
	_mProgressiveEra << new FoeGoods( e_TIN,           QT_TR_NOOP("Tinplate"      ) );
	_mProgressiveEra << new FoeGoods( e_EXPLOSIVES,    QT_TR_NOOP("Explosives"    ) );
	_mAll << _mProgressiveEra;

	_mModernMaterials << new FoeGoods( e_FERROCONCRETE,    QT_TR_NOOP("Ferroconcrete"     ) );
	_mModernMaterials << new FoeGoods( e_CONVENIENCEFOOD,  QT_TR_NOOP("Convenience Food"  ) );
	_mModernMaterials << new FoeGoods( e_FLAVORANTS,       QT_TR_NOOP("Flavorants"        ) );
	_mModernMaterials << new FoeGoods( e_PACKAGES,         QT_TR_NOOP("Packages"          ) );
	_mModernMaterials << new FoeGoods( e_LUXYRYMATERIALS,  QT_TR_NOOP("Luxury Materials"  ) );
	_mAll << _mModernMaterials;

	_mAll << _mPostmodernEra;
}

const QList<const FoeGoods *> &FoeGoods::getProducts()
{
	initialize();
	return _mAll;
}


const QList<const FoeGoods *> &FoeGoods::getProductsForAge(FoeAge *age)
{
	initialize();
	switch (age->id()) {
		case e_BronzeAge:         return _mBronzeAge;
		case e_IronAge:           return _mIronAge;
		case e_EarlyMiddleAges:   return _mEarlyMiddleAges;
		case e_HighMiddleAges:    return _mHighMiddleAges;
		case e_LateMiddleAges:    return _mLateMiddleAges;
		case e_ColonialAge:       return _mColonialAge;
		case e_IndustrialAge:     return _mIndustrialAge;
		case e_ProgressiveEra:    return _mProgressiveEra;
		case e_ModernMaterials:   return _mModernMaterials;
		case e_PostModernEra:     return _mPostmodernEra;
		case e_NumAges:
			break;
	}
	return _mEmpty;
}


const QString &FoeGoods::bonusText(BonusLevel bl) const
{
	initialize();
	return _bonusTexts[bl];
}


QString FoeGoods::bonusColorHTML(BonusLevel bl) const
{

	switch(bl) {
		case e_BONUS:
			return "#00df00";
		case e_NOT_CONQUERED:
		case e_NEEDS_RESEARCH:
			return "#0000df";
		case e_NO_BONUS:
			return "#df0000";
		default:
			break;
	}

	return "";
}

QColor FoeGoods::bonusColor(BonusLevel bl) const
{
	switch(bl) {
		case e_BONUS:
			return QColor(0x0, 0xdf, 0);
		case e_NOT_CONQUERED:
		case e_NEEDS_RESEARCH:
			return QColor(0x0, 0x0, 0xdf);
		case e_NO_BONUS:
			return QColor(0xdf, 0x0, 0x0);
		default:
			break;
	}

	return QColor();
}


const QStringList &FoeGoods::bonusTexts()
{
	return _bonusTexts;
}


FoeGoods::FoeGoods(e_Products id, const char *name)
{
	_id = id;
	_name =  qApp->translate("FoeGoods", name);
	_iconFile = ":/images/" + QString(name).toLower() + ".png";
	_pixmap = QPixmap::fromImage(QImage(_iconFile));
	_icon = QIcon(_pixmap);
}


const QPixmap &FoeGoods::pixmap() const {
	return _pixmap;
}

const FoeGoods *FoeGoods::fromId(e_Products id) {
	const FoeGoods* p;
	foreach (p, _mAll) {
		if (p->id() == id)
			return p;
	}

	return NULL;
}
