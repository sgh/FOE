#include "foeages.h"


bool FoeAge::_b_initialized = false;

static QVector<FoeAge*> _ageList;

void FoeAge::initialize()
{
	if (_b_initialized)
		return;

	_ageList.clear();

	_ageList.append( new FoeAge( "Bronzealder",         e_BronzeAge,         QColor( 0xB3, 0x88, 0x12 ) ));
	_ageList.append( new FoeAge( "Jernalder",           e_IronAge,           QColor( 0x98, 0x45, 0x20 ) ));
	_ageList.append( new FoeAge( "Tidlig middelalder",  e_EarlyMiddleAges, QColor( 0x50, 0x81, 0x44 ) ));
	_ageList.append( new FoeAge( "Højmiddelalder",      e_HighMiddleAges,  QColor( 0x2F, 0x8E, 0x8E ) ));
	_ageList.append( new FoeAge( "Senmiddelalder",      e_LateMiddleAges,  QColor( 0x7E, 0x41, 0x86 ) ));
	_ageList.append( new FoeAge( "Kolonitiden",         e_ColonialAge,    QColor( 0xCD, 0x66, 0x03 ) ));
	_ageList.append( new FoeAge( "Industritiden",       e_IndustrialAge,  QColor( 0xAA, 0x27, 0x25 ) ));
	_ageList.append( new FoeAge( "Den progressive æra", e_ProgressiveEra, QColor( 0xB8, 0x9A, 0x62 ) ));

	_b_initialized = true;
}

FoeAge::FoeAge( const QString& name, e_FoeAges age, const QColor& color ) {
	_name  = name;
	_age   = age;
	_color = color;
}

const QVector<FoeAge*> & FoeAge::getAges()
{
	initialize();
	return _ageList;
}

QColor FoeAge::color() {
	return _color;
}

const QString& FoeAge::name() {
	return _name;
}
