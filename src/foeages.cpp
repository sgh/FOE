#include "foeages.h"
#include <QColor>
using namespace std;

#define tr(s) s

static std::vector<FoeAge> allAges {
	{tr("Bronze age"),         e_BronzeAge,       QColor( 0xB3, 0x88, 0x12 ) },
	{tr("Iron Age"),           e_IronAge,         QColor( 0x98, 0x45, 0x20 ) },
	{tr("Early Moddle Ages"),  e_EarlyMiddleAges, QColor( 0x50, 0x81, 0x44 ) },
	{tr("High Middle Ages"),   e_HighMiddleAges,  QColor( 0x2F, 0x8E, 0x8E ) },
	{tr("Late Middle Ages"),   e_LateMiddleAges,  QColor( 0x7E, 0x41, 0x86 ) },
	{tr("Colonial Age"),       e_ColonialAge,     QColor( 0xCD, 0x66, 0x03 ) },
	{tr("Industrial Age"),     e_IndustrialAge,   QColor( 0xAA, 0x27, 0x25 ) },
	{tr("Progressive Era"),    e_ProgressiveEra,  QColor( 0xB8, 0x9A, 0x62 ) },
	{tr("Modern Materials"),   e_ModernMaterials, QColor( 0x5E, 0xA3, 0xE9 ) },
	{tr("Postmodern Era"),     e_PostModernEra,   QColor( 0xA8, 0xB4, 0xA9 ) },
	{tr("Contemporary Era"),   e_ContemporaryEra, QColor( 0xDE, 0x6C, 0x4B ) },
	{tr("Tomorrow"),           e_Tomorrow,        QColor( 0x29, 0x2C, 0x35 ) },
	{tr("Future"),             e_Future,          QColor( 0x8F, 0xC0, 0x5F ) },
};

const std::vector<FoeAge>& getAges() {
	return allAges;
}
