#include "foeuser.h"
#include "foedatamanger.h"
#include "foeproduct.h"

FoeUser::FoeUser(FoeDataManager *data, int userid) {
	_data = data;
	_userid = userid;
	_username = _data->getUsername(userid);
	_b_admin = false;
	_b_initialized = false;
}

void FoeUser::initialize() {
	if (_b_initialized)
		return;
	_b_initialized = true;

	_factories     = _data->getUserHas(_userid);
	_bonus         = _data->getUserHasBonus(_userid);
	_b_admin       = _data->getUserAdmin(_userid);
}

void FoeUser::setBonus(BonusLevel bonus_level, const FoeProduct *product) {
	initialize();

	if (bonus_level == e_NO_BONUS)
		_bonus.remove(product);
	else
		_bonus[product] = bonus_level;

	int factories = 0;

	if (_factories.contains(product))
		factories = _factories[product];

	if (!_data->setUserHas(_userid, product->id(), factories, bonus_level))
		invalidate();

	emit updated();
}


void FoeUser::setProduct(int factories, const FoeProduct* product) {
	initialize();

	BonusLevel bl = e_NO_BONUS;

	if (_bonus.contains(product))
		bl = _bonus[product];


	if (factories > 0) {
		if (_data->setUserHas(_userid, product->id(), factories, bl))
			_factories[product] = factories;
	} else {
		if (_data->removeUserHas(_userid, product->id()))
			_factories.remove(product);
	}

	emit updated();
}

int FoeUser::hasProduct(const FoeProduct *product) {
	initialize();
	if (!_factories.contains(product))
		return 0;

	return _factories[product];
}

BonusLevel FoeUser::hasBonus(const FoeProduct *product) {
	initialize();
	if (!_bonus.contains(product))
		return e_NO_BONUS;

	return _bonus[product];
}

const QMap<const FoeProduct *, BonusLevel> &FoeUser::allBonus()
{
	return _bonus;
}

QSet<const FoeProduct *> FoeUser::getProducts()
{
	QSet<const FoeProduct*> productSet;
	const FoeProduct* product;
	foreach (product, FoeProduct::getProducts()) {
		if (_factories.contains(product) || _bonus.contains(product))
			productSet.insert(product);
	}

	return productSet;
}

void FoeUser::invalidate()
{
	_b_initialized = false;
}

