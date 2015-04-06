#include "actionhelpers.h"
#include "foedatamanger.h"
#include "remote/foepusher.h"

static PusherHandler* _pusher;

void Actions::setPusher(PusherHandler* pusher) {
	_pusher = pusher;
}

void Actions::setProduct(FoeUser* user, int factories, const FoeGoods* product) {
	user->setProduct(factories, product);
	_pusher->notifyUserHas(user, product, factories, user->hasBonus(product));
}


void Actions::setBonus(FoeUser* user, BoostLevel boost_level, const FoeGoods* product) {
	user->setBonus(boost_level, product);
	_pusher->notifyUserHas(user, product, user->hasProduct(product), boost_level);
}


void Actions::removeUser(FoeDataManager* data, FoeClan* clan, FoeUser* user) {
	QString name = user->name();
	if (data->removeUser(clan, user))
		_pusher->notifyUserRemove(name);
}


void Actions::addUser(FoeDataManager* data, FoeClan* clan, const QString& name) {
	FoeUser* user = data->addUser(clan, name);
	if (user != NULL)
		_pusher->notifyUserAdd(name);
}
