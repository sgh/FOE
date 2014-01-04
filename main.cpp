#include "foe_main.h"
#include <QApplication>
#include <QLocale>
#include <QTextEncoder>
#include <QTranslator>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
//	QLocale::setDefault(QLocale(QLocale::Danish, QLocale::Denmark));
	QApplication a(argc, argv);

	// First install translation of QT-strings
	QTranslator qtTranslator;
	qtTranslator.load( "qt_" + QLocale::system().name(), QLibraryInfo::location(QLibraryInfo::TranslationsPath));
	a.installTranslator(&qtTranslator);

	// Then install translation of FOE strings
	QTranslator myappTranslator;
	QString xlator_file = "foe_" + QLocale::system().name();

	myappTranslator.load(xlator_file);
	a.installTranslator(&myappTranslator);


#if QT_VERSION < 0x050000
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));
#endif
	FOE_Main w;
	w.show();

	return a.exec();
}
