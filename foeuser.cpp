#include "foeuser.h"
#include "foedatamanger.h"
#include "foeproduct.h"

FoeUser::FoeUser(FoeDataManager *data, int userid) {
	_data = data;
	_userid = userid;
	_username = _data->getUsername(userid);
	_b_initialized = false;
}


void FoeUser::initialize() {
	bool b_changed = false;
	if (_b_initialized)
		return;
	_b_initialized = true;

	QMap<const FoeGoods*, int> factories    = _data->getUserHas(_userid);
	QMap<const FoeGoods*, BonusLevel> bonus = _data->getUserHasBonus(_userid);

	if (factories != _factories) {
		_factories = factories;
		b_changed = true;
	}

	if (bonus != _bonus) {
		_bonus = bonus;
		b_changed = true;
	}

	if (b_changed)
		emit updated();
}

void FoeUser::setBonus(BonusLevel bonus_level, const FoeGoods *product) {
	initialize();

	if (bonus_level == e_NO_BONUS)
		_bonus.remove(product);
	else
		_bonus[product] = bonus_level;

	int factories = 0;

	if (_factories.contains(product))
		factories = _factories[product];

	if (!_data->setUserHas(_userid, product->id(), factories, bonus_level))
		reload();

	emit updated();
}


void FoeUser::setProduct(int factories, const FoeGoods* product) {
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

int FoeUser::hasProduct(const FoeGoods *product) {
	initialize();
	if (!_factories.contains(product))
		return 0;

	return _factories[product];
}

BonusLevel FoeUser::hasBonus(const FoeGoods *product) {
	initialize();
	if (!_bonus.contains(product))
		return e_NO_BONUS;

	return _bonus[product];
}

const QMap<const FoeGoods *, BonusLevel> &FoeUser::allBonus()
{
	return _bonus;
}

QSet<const FoeGoods *> FoeUser::getProducts()
{
	QSet<const FoeGoods*> productSet;
	const FoeGoods* product;
	foreach (product, FoeGoods::getProducts()) {
		if (_factories.contains(product) || _bonus.contains(product))
			productSet.insert(product);
	}

	return productSet;
}

void FoeUser::reload()
{
	_b_initialized = false;
	initialize();
}

