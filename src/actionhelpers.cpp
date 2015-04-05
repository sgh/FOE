#include "actionhelpers.h"
#include "foedatamanger.h"

static FoeDataManager* _data;

void Actions::setData(FoeDataManager* data) {
	_data = data;
}

void Actions::setProduct(FoeUser* user, int factories, const FoeGoods* product) {
	user->setProduct(factories, product);
	_data->notifyUserHas(user, product, factories, user->hasBonus(product));
}


void Actions::setBonus(FoeUser* user, BoostLevel boost_level, const FoeGoods* product) {
	user->setBonus(boost_level, product);
	_data->notifyUserHas(user, product, user->hasProduct(product), boost_level);
}
