#include <map>

#include <QString>

#include "foeproduct.h"

using namespace std;

static QList<const FoeGoods*> _mAll;

static QList<const FoeGoods*> _mBronzeAge;
static QList<const FoeGoods*> _mIronAge;
static QList<const FoeGoods*> _mEarlyMiddleAges;
static QList<const FoeGoods*> _mHighMiddleAges;
static QList<const FoeGoods*> _mLateMiddleAges;
static QList<const FoeGoods*> _mColonialAge;
static QList<const FoeGoods*> _mIndustrialAge;
static QList<const FoeGoods*> _mProgressiveEra;
static QList<e_FoeAges>         _mAges;


bool FoeGoods::_b_initialized = false;
QStringList FoeGoods::_bonusTexts;

void FoeGoods::initialize() {
	if (_b_initialized)
		return;
	_b_initialized = true;

	_mBronzeAge << new FoeGoods( e_STONE,  "Sten"     , "stone"    );
	_mBronzeAge << new FoeGoods( e_LUMBER, "Tømmer"   , "lumber" );
	_mBronzeAge << new FoeGoods( e_MARBLE, "Marmor"   , "marble" );
	_mBronzeAge << new FoeGoods( e_COLOR,  "Farvestof", "dye"    );
	_mBronzeAge << new FoeGoods( e_WINE,   "Vin"      , "wine"   );
	_mAll << _mBronzeAge;

	_mIronAge << new FoeGoods( e_LIMESTONE, "Kalksten"   , "limestone" );
	_mIronAge << new FoeGoods( e_EBONY,     "Ibenholt"   , "ebony"     );
	_mIronAge << new FoeGoods( e_IRON,      "Jern"       , "iron"      );
	_mIronAge << new FoeGoods( e_FABRIC,    "Stof"       , "cloth"     );
	_mIronAge << new FoeGoods( e_JEWELRY,   "Smykker"    , "jewelry"   );
	_mAll << _mIronAge;

	_mEarlyMiddleAges << new FoeGoods( e_COPPER,    "Kobber"  , "copper"    );
	_mEarlyMiddleAges << new FoeGoods( e_GRANIT,    "Granit"  , "granite"   );
	_mEarlyMiddleAges << new FoeGoods( e_ALABASTER, "Alabast" , "alabaster" );
	_mEarlyMiddleAges << new FoeGoods( e_GOLD,      "Guld"    , "gold"      );
	_mEarlyMiddleAges << new FoeGoods( e_HONEY,     "Honning" , "honney"    );
	_mAll << _mEarlyMiddleAges;

	_mHighMiddleAges << new FoeGoods( e_BRICK, "Teglsten" , "brick" );
	_mHighMiddleAges << new FoeGoods( e_ROPE,  "Tov"      , "rope"  );
	_mHighMiddleAges << new FoeGoods( e_SALT,  "Salt"     , "salt"  );
	_mHighMiddleAges << new FoeGoods( e_HERBS, "Urter"    , "dried herbs" );
	_mHighMiddleAges << new FoeGoods( e_GLASS, "Glas"     , "glass" );
	_mAll << _mHighMiddleAges;

	_mLateMiddleAges << new FoeGoods( e_BASALT,    "Basalt"  , "basalt"    );
	_mLateMiddleAges << new FoeGoods( e_BRASS,     "Messing" , "brass"     );
	_mLateMiddleAges << new FoeGoods( e_SILK,      "Silke"   , "silk"      );
	_mLateMiddleAges << new FoeGoods( e_TALC,      "Talkum"  , "talc powder"      );
	_mLateMiddleAges << new FoeGoods( e_GUNPOWDER, "Krudt"   , "gunpowder" );
	_mAll << _mLateMiddleAges;

	_mColonialAge << new FoeGoods( e_PAPER,     "Papir"     , "paper"     );
	_mColonialAge << new FoeGoods( e_COFFEE,    "Kaffe"     , "coffee"    );
	_mColonialAge << new FoeGoods( e_WIRE,      "Metaltråd" , "wire"      );
	_mColonialAge << new FoeGoods( e_PORCELAIN, "Porcelæn"  , "porcelain" );
	_mColonialAge << new FoeGoods( e_TAR,       "Tjære"     , "tar"       );
	_mAll << _mColonialAge;

	_mIndustrialAge << new FoeGoods( e_RUBBER,     "Gummi"     , "rubber" );
	_mIndustrialAge << new FoeGoods( e_COKE,       "Koks"      , "coke"   );
	_mIndustrialAge << new FoeGoods( e_TEXTILE,    "Tekstil"   , "textile" );
	_mIndustrialAge << new FoeGoods( e_WHALEOLIE,  "Hvalolie"  , "whale oil" );
	_mIndustrialAge << new FoeGoods( e_FERTILIZER, "Gødning"   , "fertilizer" );
	_mAll << _mIndustrialAge;

	_mProgressiveEra << new FoeGoods( e_ASBESTOS,     "Asbest"        , "asbestos"     );
	_mProgressiveEra << new FoeGoods( e_GASOLINE,     "Benzin"        , "gasoline"     );
	_mProgressiveEra << new FoeGoods( e_MACHINEPARTS, "Maskinedele"   , "machine parts" );
	_mProgressiveEra << new FoeGoods( e_TIN,          "Blik"          , "tinplate"     );
	_mProgressiveEra << new FoeGoods( e_EXPLOSIVES,   "Sprængstoffer" , "explosives"   );
	_mAll << _mProgressiveEra;

	_bonusTexts.insert(e_NO_BONUS,       "Ikke bonus");
	_bonusTexts.insert(e_NEEDS_RESEARCH, "Mangler forskning");
	_bonusTexts.insert(e_NOT_CONQUERED,  "Ikke erobret");
	_bonusTexts.insert(e_BONUS,          "Bonus");

	_mAges.push_back( e_BronzeAge       );
	_mAges.push_back( e_IronAge         );
	_mAges.push_back( e_EarlyMiddleAges );
	_mAges.push_back( e_HighMiddleAges  );
	_mAges.push_back( e_LateMiddleAges  );
	_mAges.push_back( e_ColonialAge     );
	_mAges.push_back( e_IndustrialAge   );
	_mAges.push_back( e_ProgressiveEra  );
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
		case e_EarlyMiddleAges: return _mEarlyMiddleAges;
		case e_HighMiddleAges:  return _mHighMiddleAges;
		case e_LateMiddleAges:  return _mLateMiddleAges;
		case e_ColonialAge:    return _mColonialAge;
		case e_IndustrialAge:  return _mIndustrialAge;
		case e_ProgressiveEra: return _mProgressiveEra;
		case e_NumAges:
			break;
	}
	return _mAll;
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


FoeGoods::FoeGoods(e_Products id, QString name, QString base_english_name)
{
	_id = id;
	_name = name;
	_iconFile = ":/images/" + base_english_name + ".png";
	_pixmap = QPixmap::fromImage(QImage(_iconFile));
	_icon = QIcon(_pixmap);
}


const QPixmap &FoeGoods::pixmap() const {
	return _pixmap;
}

const FoeGoods *FoeGoods::fromId(e_Products id) {
	const FoeGoods* p;
	foreach (p, _mBronzeAge)       {   if (p->id() == id) return p;   }
	foreach (p, _mIronAge)         {   if (p->id() == id) return p;   }
	foreach (p, _mEarlyMiddleAges) {   if (p->id() == id) return p;   }
	foreach (p, _mHighMiddleAges)  {   if (p->id() == id) return p;   }
	foreach (p, _mLateMiddleAges)  {   if (p->id() == id) return p;   }
	foreach (p, _mColonialAge)     {   if (p->id() == id) return p;   }
	foreach (p, _mIndustrialAge)   {   if (p->id() == id) return p;   }
	foreach (p, _mProgressiveEra)  {   if (p->id() == id) return p;   }

	return NULL;
}
