#include <map>

#include <QApplication>
#include <QString>
#include <vector>

#include "foegoods.h"
#include <memory>

using namespace std;

static QVector<const FoeGoods*> _mAll;
static QVector<const FoeGoods*> _mEmpty;

static QVector<const FoeGoods*> _mBronzeAge;
static QVector<const FoeGoods*> _mIronAge;
static QVector<const FoeGoods*> _mEarlyMiddleAges;
static QVector<const FoeGoods*> _mHighMiddleAges;
static QVector<const FoeGoods*> _mLateMiddleAges;
static QVector<const FoeGoods*> _mColonialAge;
static QVector<const FoeGoods*> _mIndustrialAge;
static QVector<const FoeGoods*> _mProgressiveEra;
static QVector<const FoeGoods*> _mModernMaterials;
static QVector<const FoeGoods*> _mPostmodernEra;
static QVector<const FoeGoods*> _mContemporaryEra;
static QVector<const FoeGoods*> _mTomorrow;
static QVector<const FoeGoods*> _mFuture;

static vector<shared_ptr<const FoeGoods>> _deleter;

QStringList FoeGoods::_boostTexts;

void FoeGoods::initialize() {
	if (!_mAll.isEmpty())
		return;

	_boostTexts.insert(e_NO_BOOST,        tr("No boost"      ) );
	_boostTexts.insert(e_NEEDS_RESEARCH,  tr("Need research" ) );
	_boostTexts.insert(e_NOT_CONQUERED,   tr("Not conquered" ) );
	_boostTexts.insert(e_BOOST,           tr("Boost"         ) );

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

	_mPostmodernEra << new FoeGoods( e_RENEWABLE_RESOURCES, QT_TR_NOOP("Renewable Resources") );
	_mPostmodernEra << new FoeGoods( e_STEEL,               QT_TR_NOOP("Steel"              ) );
	_mPostmodernEra << new FoeGoods( e_SEMICONDUCTORS,      QT_TR_NOOP("Semiconductors"     ) );
	_mPostmodernEra << new FoeGoods( e_INDUSTRIALFILTERS,   QT_TR_NOOP("Industrial Filters" ) );
	_mPostmodernEra << new FoeGoods( e_GENOMEDATA,          QT_TR_NOOP("Genome Data"        ) );
	_mAll << _mPostmodernEra;


	// Contemporary Era
	_mContemporaryEra << new FoeGoods( e_ELECTROMAGNETS,  QT_TR_NOOP("Electromagnets" ) );
	_mContemporaryEra<< new FoeGoods( e_GAS,             QT_TR_NOOP("Gas"            ) );
	_mContemporaryEra << new FoeGoods( e_PLASTICS,        QT_TR_NOOP("Plastics"       ) );
	_mContemporaryEra << new FoeGoods( e_ROBOTS,          QT_TR_NOOP("Robots"         ) );
	_mContemporaryEra << new FoeGoods( e_BIONICSDATA,     QT_TR_NOOP("Bionics Data"   ) );
	_mAll << _mContemporaryEra;

	// Tomorrow
	_mTomorrow << new FoeGoods( e_TRANSLUCENTCONCRETE , QT_TR_NOOP("Translucent Concrete" ) );
	_mTomorrow << new FoeGoods( e_SMARTMATERIALS,       QT_TR_NOOP("Smart Materials"      ) );
	_mTomorrow << new FoeGoods( e_PAPERCRETE,           QT_TR_NOOP("Papercrete"           ) );
	_mTomorrow << new FoeGoods( e_PRESERVATIVES,        QT_TR_NOOP("Preservatives"        ) );
	_mTomorrow << new FoeGoods( e_NUTRITIONRESEARCH,    QT_TR_NOOP("Nutrition research"   ) );
	_mAll << _mTomorrow;

	_mAll << _mFuture;


	// Put all of them in a list just for cleaup during shutdown
	foreach (const FoeGoods* g, _mAll) {
		_deleter.push_back( unique_ptr<const FoeGoods>(g) );
	}
}

const QVector<const FoeGoods *> &FoeGoods::getGoods()
{
	initialize();
	return _mAll;
}


const QVector<const FoeGoods *> &FoeGoods::getGoodsForAge(const FoeAge *age) {
	initialize();
	switch (age->id) {
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
		case e_ContemporaryEra:   return _mContemporaryEra;
		case e_Tomorrow:          return _mTomorrow;
		case e_Future:            return _mFuture;
		case e_NumAges:
			break;
	}
	return _mEmpty;
}


const QString &FoeGoods::boostText(BoostLevel bl) const {
	initialize();
	return _boostTexts[bl];
}


QString FoeGoods::boostColorHTML(BoostLevel bl) const
{
	initialize();
	switch(bl) {
		case e_BOOST:
			return "#00df00";
		case e_NOT_CONQUERED:
		case e_NEEDS_RESEARCH:
			return "#0000df";
		case e_NO_BOOST:
			return "#df0000";
		default:
			break;
	}

	return "";
}


QColor FoeGoods::boostColor(BoostLevel bl) const
{
	initialize();
	switch(bl) {
		case e_BOOST:
			return QColor(0x0, 0xdf, 0);
		case e_NOT_CONQUERED:
		case e_NEEDS_RESEARCH:
			return QColor(0x0, 0x0, 0xdf);
		case e_NO_BOOST:
			return QColor(0xdf, 0x0, 0x0);
		default:
			break;
	}

	return QColor();
}


const QStringList &FoeGoods::boostTexts()
{
	initialize();
	return _boostTexts;
}


FoeGoods::FoeGoods(e_Goods id, const char *name)
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


const FoeGoods *FoeGoods::fromId(e_Goods id) {
	initialize();
	const FoeGoods* p;
	foreach (p, _mAll) {
		if (p->id() == id)
			return p;
	}

	return NULL;
}
