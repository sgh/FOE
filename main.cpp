#include "foe_main.h"
#include <QApplication>
#include <QLocale>
#include <QTextEncoder>

int main(int argc, char *argv[])
{
//	QLocale::setDefault(QLocale(QLocale::Danish, QLocale::Denmark));
	QApplication a(argc, argv);
#if QT_VERSION < 0x050000
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
	FOE_Main w;
	w.show();

	return a.exec();
}
