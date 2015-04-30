#include "foeages.h"
#include <memory>
using namespace std;


static QVector<shared_ptr<FoeAge>> _ageList;

void FoeAge::initialize()
{
	if (_ageList.empty())
	{
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Bronze age"),         e_BronzeAge,       QColor( 0xB3, 0x88, 0x12 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Iron Age"),           e_IronAge,         QColor( 0x98, 0x45, 0x20 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Early Moddle Ages"),  e_EarlyMiddleAges, QColor( 0x50, 0x81, 0x44 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("High Middle Ages"),   e_HighMiddleAges,  QColor( 0x2F, 0x8E, 0x8E ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Late Middle Ages"),   e_LateMiddleAges,  QColor( 0x7E, 0x41, 0x86 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Colonial Age"),       e_ColonialAge,     QColor( 0xCD, 0x66, 0x03 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Industrial Age"),     e_IndustrialAge,   QColor( 0xAA, 0x27, 0x25 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Progressive Era"),    e_ProgressiveEra,  QColor( 0xB8, 0x9A, 0x62 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Modern Materials"),   e_ModernMaterials, QColor( 0x5E, 0xA3, 0xE9 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Postmodern Era"),     e_PostModernEra,   QColor( 0xA8, 0xB4, 0xA9 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Contemporary Era"),   e_ContemporaryEra, QColor( 0xDE, 0x6C, 0x4B ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Tomorrow"),           e_Tomorrow,        QColor( 0x29, 0x2C, 0x35 ) )));
		_ageList.append( shared_ptr<FoeAge>(new FoeAge(tr("Future"),             e_Future,          QColor( 0x8F, 0xC0, 0x5F ) )));
	}
}


FoeAge::FoeAge( const QString& name, e_FoeAges age, const QColor& color ) {
	_name  = name;
	_age   = age;
	_color = color;
}

const QVector<shared_ptr<FoeAge>> & FoeAge::getAges()
{
	return _ageList;
}

QColor FoeAge::color() {
	return _color;
}

const QString& FoeAge::name() {
	return _name;
}
