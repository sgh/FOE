#include <map>

#include <QString>

#include "foeproduct.h"

using namespace std;

static QList<const FoeProduct*> _mAll;

static QList<const FoeProduct*> _mBronze;
static QList<const FoeProduct*> _mIron;
static QList<const FoeProduct*> _mEarlyMiddleAge;
static QList<const FoeProduct*> _mHighMiddleAge;
static QList<const FoeProduct*> _mLateMiddleAge;
static QList<const FoeProduct*> _mColonialAge;
static QList<const FoeProduct*> _mIndustrialAge;
static QList<const FoeProduct*> _mProgressiveEra;
static QList<e_FoeAges>         _mAges;


bool FoeProduct::_b_initialized = false;
QStringList FoeProduct::_bonusTexts;

void FoeProduct::initialize() {
	if (_b_initialized)
		return;
	_b_initialized = true;

	_mBronze << new FoeProduct( e_STONE,  "Sten"     , "stone"    );
	_mBronze << new FoeProduct( e_LUMBER, "Tømmer"   , "lumber" );
	_mBronze << new FoeProduct( e_MARBLE, "Marmor"   , "marble" );
	_mBronze << new FoeProduct( e_COLOR,  "Farvestof", "color"  );
	_mBronze << new FoeProduct( e_WINE,   "Vin"      , "wine"   );
	_mAll << _mBronze;

	_mIron << new FoeProduct( e_LIMESTONE, "Kalksten"   , "limestone" );
	_mIron << new FoeProduct( e_EBONY,     "Ibenholt"   , "ebony"     );
	_mIron << new FoeProduct( e_IRON,      "Jern"       , "iron"      );
	_mIron << new FoeProduct( e_FABRIC,    "Stof"       , "fabric"     );
	_mIron << new FoeProduct( e_JEWELRY,   "Smykker"    , "jewelry"   );
	_mAll << _mIron;

	_mEarlyMiddleAge << new FoeProduct( e_COPPER,    "Kobber"  , "copper"    );
	_mEarlyMiddleAge << new FoeProduct( e_GRANIT,    "Granit"  , "granite"   );
	_mEarlyMiddleAge << new FoeProduct( e_ALABASTER, "Alabast" , "alabaster" );
	_mEarlyMiddleAge << new FoeProduct( e_GOLD,      "Guld"    , "gold"      );
	_mEarlyMiddleAge << new FoeProduct( e_HONEY,     "Honning" , "honney"    );
	_mAll << _mEarlyMiddleAge;

	_mHighMiddleAge << new FoeProduct( e_BRICK, "Teglsten" , "brick" );
	_mHighMiddleAge << new FoeProduct( e_ROPE,  "Tov"      , "rope"  );
	_mHighMiddleAge << new FoeProduct( e_SALT,  "Salt"     , "salt"  );
	_mHighMiddleAge << new FoeProduct( e_HERBS, "Urter"    , "herbs" );
	_mHighMiddleAge << new FoeProduct( e_GLASS, "Glas"     , "glass" );
	_mAll << _mHighMiddleAge;

	_mLateMiddleAge << new FoeProduct( e_BASALT,    "Basalt"  , "basalt"    );
	_mLateMiddleAge << new FoeProduct( e_BRASS,     "Messing" , "brass"     );
	_mLateMiddleAge << new FoeProduct( e_SILK,      "Silke"   , "silk"      );
	_mLateMiddleAge << new FoeProduct( e_TALC,      "Talkum"  , "talc"      );
	_mLateMiddleAge << new FoeProduct( e_GUNPOWDER, "Krudt"   , "gunpowder" );
	_mAll << _mLateMiddleAge;

	_mColonialAge << new FoeProduct( e_PAPER,     "Papir"     , "paper"     );
	_mColonialAge << new FoeProduct( e_COFFEE,    "Kaffe"     , "coffee"    );
	_mColonialAge << new FoeProduct( e_WIRE,      "Metaltråd" , "wire"      );
	_mColonialAge << new FoeProduct( e_PORCELAIN, "Porcelæn"  , "porcelain" );
	_mColonialAge << new FoeProduct( e_TAR,       "Tjære"     , "tar"       );
	_mAll << _mColonialAge;

	_mIndustrialAge << new FoeProduct( e_RUBBER,     "Gummi"     , "rubber" );
	_mIndustrialAge << new FoeProduct( e_COKE,       "Koks"      , "coke"   );
	_mIndustrialAge << new FoeProduct( e_TEXTILE,    "Tekstil"   , "textile" );
	_mIndustrialAge << new FoeProduct( e_WHALEOLIE,  "Hvalolie"  , "whaleoil" );
	_mIndustrialAge << new FoeProduct( e_FERTILIZER, "Gødning"   , "fertilizer" );
	_mAll << _mIndustrialAge;

	_mProgressiveEra << new FoeProduct( e_ASBESTOS,     "Asbest"        , "asbestos"     );
	_mProgressiveEra << new FoeProduct( e_GASOLINE,     "Benzin"        , "gasoline"     );
	_mProgressiveEra << new FoeProduct( e_MACHINEPARTS, "Maskinedele"   , "machineparts" );
	_mProgressiveEra << new FoeProduct( e_TIN,          "Blik"          , "tin"          );
	_mProgressiveEra << new FoeProduct( e_EXPLOSIVES,   "Sprængstoffer" , "explosives"   );
	_mAll << _mProgressiveEra;

	_bonusTexts.insert(e_NO_BONUS,       "Ikke bonus");
	_bonusTexts.insert(e_NEEDS_RESEARCH, "Mangler forskning");
	_bonusTexts.insert(e_NOT_CONQUERED,  "Ikke erobret");
	_bonusTexts.insert(e_BONUS,          "Bonus");

	_mAges.push_back( e_Bronze         );
	_mAges.push_back( e_Iron           );
	_mAges.push_back( e_EarlyMiddleAge );
	_mAges.push_back( e_HighMiddleAge  );
	_mAges.push_back( e_LateMiddleAge  );
	_mAges.push_back( e_ColonialAge    );
	_mAges.push_back( e_IndustrialAge  );
	_mAges.push_back( e_ProgressiveEra );
}

const QList<const FoeProduct *> &FoeProduct::getProducts()
{
	initialize();
	return _mAll;
}


const QList<const FoeProduct *> &FoeProduct::getProductsForAge(FoeAge *age)
{
	initialize();
	switch (age->id()) {
		case e_Bronze:         return _mBronze;
		case e_Iron:           return _mIron;
		case e_EarlyMiddleAge: return _mEarlyMiddleAge;
		case e_HighMiddleAge:  return _mHighMiddleAge;
		case e_LateMiddleAge:  return _mLateMiddleAge;
		case e_ColonialAge:    return _mColonialAge;
		case e_IndustrialAge:  return _mIndustrialAge;
		case e_ProgressiveEra: return _mProgressiveEra;
		case e_NumAges:
			break;
	}
	return _mAll;
}


const QList<const FoeProduct*> &FoeProduct::getBronzeAgeProducts()
{
	initialize();
	return _mBronze;
}

const QString &FoeProduct::bonusText(BonusLevel bl)
{
	initialize();
	return _bonusTexts[bl];
}

QString FoeProduct::bonusColorHTML(BonusLevel bl)
{

	switch(bl) {
		case e_BONUS:
			return "#00ff00";
		case e_NOT_CONQUERED:
		case e_NEEDS_RESEARCH:
			return "#0000ff";
		default:
			break;
	}

	return "";
}

const QStringList &FoeProduct::bonusTexts()
{
	return _bonusTexts;
}


const QList<const FoeProduct*> &FoeProduct::getIronAgeProducts()
{
	initialize();
	return _mIron;
}


const QList<const FoeProduct*> &FoeProduct::getEarlyMiddleAgeProducts()
{
	initialize();
	return _mEarlyMiddleAge;
}


const QList<const FoeProduct*> &FoeProduct::getHighMiddleAgeProducts()
{
	initialize();
	return _mHighMiddleAge;
}


const QList<const FoeProduct*> &FoeProduct::getLateMiddleAgeProducts()
{
	initialize();
	return _mLateMiddleAge;
}


const QList<const FoeProduct*> &FoeProduct::getColonialAgeProducts()
{
	initialize();
	return _mColonialAge;
}


const QList<const FoeProduct*> &FoeProduct::getIndustrialAgeProducts()
{
	initialize();
	return _mIndustrialAge;
}

const QList<const FoeProduct*> &FoeProduct::getProgressiveEraProducts()
{
	initialize();
	return _mProgressiveEra;
}






FoeProduct::FoeProduct(e_Products id, QString name, QString base_english_name)
{
	_id = id;
	_name = name;
	_iconFile = ":/images/" + base_english_name + ".png";
	_pixmap = QPixmap::fromImage(QImage(_iconFile));
	_icon = QIcon(_pixmap);
}


const QPixmap &FoeProduct::pixmap() const {
	return _pixmap;
}

const FoeProduct *FoeProduct::fromId(e_Products id) {
	const FoeProduct* p;
	foreach (p, _mBronze)         {   if (p->id() == id) return p;   }
	foreach (p, _mIron)           {   if (p->id() == id) return p;   }
	foreach (p, _mEarlyMiddleAge) {   if (p->id() == id) return p;   }
	foreach (p, _mHighMiddleAge)  {   if (p->id() == id) return p;   }
	foreach (p, _mLateMiddleAge)  {   if (p->id() == id) return p;   }
	foreach (p, _mColonialAge)    {   if (p->id() == id) return p;   }
	foreach (p, _mIndustrialAge)  {   if (p->id() == id) return p;   }
	foreach (p, _mProgressiveEra) {   if (p->id() == id) return p;   }

	return NULL;
}
