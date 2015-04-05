#include "foeuser.h"
#include "foedatamanger.h"
#include "foegoods.h"

#include <QCryptographicHash>

FoeUser::FoeUser(const QString& name, int userid) {
	_data = NULL;
	_my_clan = NULL;
	_userid = userid;
	_username = name;
	_b_initialized = false;
}


void FoeUser::initialize() {
	bool b_changed = false;
	if (_b_initialized || _data==NULL)
		return;
	_b_initialized = true;

	QMap<const FoeGoods*, int> factories    = _data->getUserHas(_userid);
	QMap<const FoeGoods*, BoostLevel> boost = _data->getUserHasBonus(_userid);
	_timestamp = _data->getUserTimestamp(this);

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

void FoeUser::setTimestamp(int64_t timestamp) {
	_timestamp = timestamp;
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

QString FoeUser::serialize() {
	QJsonObject o;

	QVector<const FoeGoods*> goods = FoeGoods::getGoods();

	QJsonArray productarray;
	for (int idx=0; idx<goods.size(); idx++) {
		QJsonObject product_obj = QJsonObject();

		const FoeGoods* product = goods[idx];

		// Add factory count if it exists.
		if (_factories.contains(product))
			product_obj.insert("factories", _factories[product]);

		// Add boostlevel if it exists,
		if (_boost.contains(product))
			product_obj.insert("boostlevel", (int)_boost[product]);

		if (!product_obj.isEmpty()) {
			product_obj.insert("good", product->id());
			productarray.append(product_obj);
		}
	}
	o.insert("goods", productarray);
	o.insert("timestamp", QJsonValue((qint64)_timestamp));
	o.insert("user", _username);
	QJsonDocument doc;
	doc.setObject(o);
	return QString(doc.toJson(QJsonDocument::Compact));
}


void FoeUser::deserialize(QJsonObject json) {
	QList<const FoeGoods*> old_factories = _factories.keys();
	const	FoeGoods* good;

	QJsonArray product_obj = json.value("goods").toArray();
	for (int idx=0; idx<product_obj.size(); idx++) {
		QJsonObject o = product_obj[idx].toObject();
		good = FoeGoods::fromId( (e_Goods)o.value("good").toInt() );
		int factories = o.value("factories").toInt(0);
		BoostLevel bl = (BoostLevel)o.value("boostlevel").toInt((int)e_NO_BOOST);
		setProduct(factories, good);
		setBonus(bl, good);
		old_factories.removeAll(good);
	}

	// Now wipe away all the stuff not mensioned in the json
	foreach (good, old_factories) {
		setProduct(0, good);
	}

	setTimestamp( json.value("timestamp").toInt());
}


QString FoeUser::hash() {
	return QCryptographicHash::hash(serialize().toUtf8(), QCryptographicHash::Md5).toBase64();
}


void FoeUser::setData(FoeDataManager* data) {
	_data = data;
}


void FoeUser::setClan(FoeClan* clan) {
	_my_clan = clan;
}

