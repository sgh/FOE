#pragma once
#include <foeuser.h>

class PusherHandler;
class FoeGoods;

namespace Actions {

	void setPusher(PusherHandler* pusher);

	void setProduct(FoeUser* user,  int factories, const FoeGoods* product);
	void setBonus(FoeUser* user, BoostLevel boost_level, const FoeGoods* product);

	void removeUser(FoeDataManager* data, FoeClan* clan, FoeUser* user);
	void addUser(FoeDataManager* data, FoeClan*, const QString& name);
}