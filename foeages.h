#ifndef FOEAGES_H
#define FOEAGES_H

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
	e_NumAges,
};


class FoeAge : public QObject
{
	Q_OBJECT
	static bool _b_initialized;
	static void initialize();

	QString   _name;
	e_FoeAges _age;
	QColor    _color;

	FoeAge( const QString& name, e_FoeAges age, const QColor& color );

public:

	e_FoeAges id() { return _age; }
	static const QVector<FoeAge*>& getAges();
	const QString &name();
	QColor color();
};

#endif // FOEAGES_H
