#include "foeuser.h"
#include "foedatamanger.h"
#include "foegoods.h"

FoeUser::FoeUser(FoeClan* clan, FoeDataManager *data, const QString& name, int userid) {
	_data = data;
	_my_clan = clan;
	_userid = userid;
	_username = name;
	_b_initialized = false;
}


void FoeUser::initialize() {
	bool b_changed = false;
	if (_b_initialized)
		return;
	_b_initialized = true;

	QMap<const FoeGoods*, int> factories    = _data->getUserHas(_userid);
	QMap<const FoeGoods*, BoostLevel> boost = _data->getUserHasBonus(_userid);

	if (factories != _factories) {
		_factories = factories;
		b_changed = true;
	}

	if (boost != _boost) {
		_boost = boost;
		b_changed = true;
	}

	if (b_changed)
		_my_clan->userUpdated();
}

void FoeUser::storeGoods(const FoeGoods* product, int factories, BoostLevel boost_level)
{
	if (factories==0 && boost_level == e_NO_BOOST) {
		 _data->removeUserHas(this, product);
		_factories.remove(product);
	} else
		_data->setUserHas(this, product, factories, boost_level);
}


void FoeUser::setBonus(BoostLevel boost_level, const FoeGoods *product) {
	initialize();

	if (boost_level == e_NO_BOOST)
		_boost.remove(product);
	else
		_boost[product] = boost_level;

	int factories = 0;

	if (_factories.contains(product))
		factories = _factories[product];

	storeGoods(product, factories, boost_level);

	_my_clan->userUpdated();
}


void FoeUser::setProduct(int factories, const FoeGoods* product) {
	initialize();

	BoostLevel bl = e_NO_BOOST;

	if (_boost.contains(product))
		bl = _boost[product];

	_factories[product] = factories;
	storeGoods(product, factories, bl);

	_my_clan->userUpdated();
}

int FoeUser::hasProduct(const FoeGoods *product) {
	initialize();
	if (!_factories.contains(product))
		return 0;

	return _factories[product];
}

BoostLevel FoeUser::hasBonus(const FoeGoods *product) {
	initialize();
	if (!_boost.contains(product))
		return e_NO_BOOST;

	return _boost[product];
}

const QMap<const FoeGoods *, BoostLevel> &FoeUser::allBonus()
{
	return _boost;
}

QSet<const FoeGoods *> FoeUser::getProducts()
{
	QSet<const FoeGoods*> productSet;
	const FoeGoods* product;
	foreach (product, FoeGoods::getGoods()) {
		if (_factories.contains(product) || _boost.contains(product))
			productSet.insert(product);
	}

	return productSet;
}


const QString&FoeUser::clanName()
{
	return _my_clan->name();
}


void FoeUser::reload()
{
	_b_initialized = false;
	initialize();
}

