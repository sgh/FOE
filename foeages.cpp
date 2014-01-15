#include "foeages.h"


bool FoeAge::_b_initialized = false;

static QVector<FoeAge*> _ageList;

void FoeAge::initialize()
{
	if (_b_initialized)
		return;

	_ageList.clear();

	_ageList.append( new FoeAge( tr("Bronze age"),         e_BronzeAge,       QColor( 0xB3, 0x88, 0x12 ) ));
	_ageList.append( new FoeAge( tr("Iron Age"),           e_IronAge,         QColor( 0x98, 0x45, 0x20 ) ));
	_ageList.append( new FoeAge( tr("Early Moddle Ages"),  e_EarlyMiddleAges, QColor( 0x50, 0x81, 0x44 ) ));
	_ageList.append( new FoeAge( tr("High Middle Ages"),   e_HighMiddleAges,  QColor( 0x2F, 0x8E, 0x8E ) ));
	_ageList.append( new FoeAge( tr("Late Middle Ages"),   e_LateMiddleAges,  QColor( 0x7E, 0x41, 0x86 ) ));
	_ageList.append( new FoeAge( tr("Colonial Age"),       e_ColonialAge,     QColor( 0xCD, 0x66, 0x03 ) ));
	_ageList.append( new FoeAge( tr("Industrial Age"),     e_IndustrialAge,   QColor( 0xAA, 0x27, 0x25 ) ));
	_ageList.append( new FoeAge( tr("Progressive Era"),    e_ProgressiveEra,  QColor( 0xB8, 0x9A, 0x62 ) ));
	_ageList.append( new FoeAge( tr("Modern Materials"),   e_ModernMaterials, QColor( 0x5E, 0xA3, 0xE9 ) ));
	_ageList.append( new FoeAge( tr("Postmodern Era"),     e_PostModernEra,   QColor( 0xA8, 0xB4, 0xA9 ) ));

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
