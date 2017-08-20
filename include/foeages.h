#pragma once

#include <QColor>
#include <vector>

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


struct FoeAge {
	const char*  name;
	e_FoeAges    id;
	QColor       color;
};

const std::vector<FoeAge>& getAges();
