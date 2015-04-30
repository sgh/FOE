#ifndef FOEAGES_H
#define FOEAGES_H

#include <memory>

#include <QColor>
#include <QString>
#include <QVector>

enum e_FoeAges {
	e_BronzeAge,
	e_IronAge,
	e_EarlyMiddleAges,
	e_HighMiddleAges,
	e_LateMiddleAges,
	e_ColonialAge,
	e_IndustrialAge,
	e_ProgressiveEra,
	e_ModernMaterials,
	e_PostModernEra,
	e_ContemporaryEra,
	e_Tomorrow,
	e_Future,
	e_NumAges,
};


class FoeAge : public QObject
{
	Q_OBJECT

	QString   _name;
	e_FoeAges _age;
	QColor    _color;

	FoeAge( const QString& name, e_FoeAges age, const QColor& color );

public:
	static void initialize();
	static const QVector<std::shared_ptr<FoeAge> >& getAges();

	e_FoeAges id() { return _age; }
	const QString &name();
	QColor color();
};

#endif // FOEAGES_H
