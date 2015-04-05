#pragma once
#include <foeuser.h>

class FoeDataManager;
class FoeGoods;

namespace Actions {

	void setData(FoeDataManager* data);

	void setProduct(FoeUser* user,  int factories, const FoeGoods* product);
	void setBonus(FoeUser* user, BoostLevel boost_level, const FoeGoods* product);

}