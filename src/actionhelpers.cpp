#include "actionhelpers.h"
#include "foedatamanger.h"
#include "remote/foepusher.h"

using namespace std;
shared_ptr<PusherHandler> _pusher;

void Actions::setPusher(shared_ptr<PusherHandler> pusher) {
	_pusher = pusher;
}

void Actions::setProduct(FoeUser* user, int factories, const FoeGoods* product) {
	shared_ptr<PusherHandler> pusher = _pusher;
	user->setProduct(factories, product);
	pusher->notifyUserHas(user, product, factories, user->hasBonus(product));
}


void Actions::setBonus(FoeUser* user, BoostLevel boost_level, const FoeGoods* product) {
	shared_ptr<PusherHandler> pusher = _pusher;
	user->setBonus(boost_level, product);
	pusher->notifyUserHas(user, product, user->hasProduct(product), boost_level);
}


void Actions::removeUser(FoeDataManager* data, FoeClan* clan, FoeUser* user) {
	shared_ptr<PusherHandler> pusher = _pusher;
	QString name = user->name();
	if (data->removeUser(clan, user))
		pusher->notifyUserRemove(name);
}


void Actions::addUser(FoeDataManager* data, FoeClan* clan, const QString& name) {
	shared_ptr<PusherHandler> pusher = _pusher;
	auto user = data->addUser(clan, name);
	if (user)
		pusher->notifyUserAdd(name);
}
