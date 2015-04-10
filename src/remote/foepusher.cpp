#include "foepusher.h"
#include "foepersistence.h"

#include "pusher.h"

#include <QJsonDocument>

using namespace std;

struct PusherHandler::Private {
	Private(FoePersistence& persist, FoeDataManager& data)
		: pusher("","", "App", "1.2")
		, persist(persist)
		, data(data) {
	}


	void sendUserHash(FoeUser* user) {
			QString hash = QString("%1:%2:%3").arg(user->name()).arg(user->timestamp()).arg(user->hash());
			pusher.send_message("private-testchannel", "client-userhash", qPrintable(hash));
	}


	void sendAllUserHash() {
		QVector<FoeUser*> users = data.currentClan()->getFoeUsers();
		FoeUser* user;
		foreach (user, users) {
			sendUserHash(user);
		}
	}


	Pusher pusher;
	FoePersistence& persist;
	FoeDataManager& data;
};



PusherHandler::PusherHandler(FoePersistence& persist, FoeDataManager& data) {
	_d = new Private(persist, data);
	_d->pusher.addListener(this);

	setup();
}


PusherHandler::~PusherHandler() {
	delete _d;
}

void PusherHandler::eventReceived(const QString& event, const QString& data) {
	FoeClan* clan = _d->data.currentClan();

	if (event == "client-adduser") {
		FoeUser* user = _d->persist.addUser( clan, data);
		clan->addUser(user);

	}

	if (event == "client-removeuser") {
		FoeUser* user = clan->getUser(data);
		if (_d->persist.removeUser(user))
			clan->removeUser(user);
	}

	if (event == "client-setuserhas") {
		QStringList split = data.split(":");
		FoeUser* user = clan->getFoeUser(split[0]);
		const FoeGoods* product = FoeGoods::fromId((e_Goods)split[1].toInt());
		int factories = split[2].toInt();
		BoostLevel boostlevel = (BoostLevel)split[3].toInt();

		user->setProduct(factories, product);
		user->setBonus(boostlevel, product);
	}

	if (event == "client-userhash") {
		QStringList split = data.split(":");
		FoeUser* user = clan->getFoeUser(split[0]);
		QString hash = split[1];
		int64_t timestamp = split[1].toLongLong();

		// Only do stuff if the hashes are different
		if (hash != user->hash()) {
			if (user->timestamp() > timestamp) {
				// If we receive an older timestamp - send our user data
				QString data = user->serialize();
				_d->pusher.send_message_unescaped("private-testchannel", "client-userdata", qPrintable(data));
			}

			if (user->timestamp() < timestamp) {
				// If we receive a newer timestampr - send user hash and timestamp
				_d->sendUserHash(user);
			}
		}
	}

	if (event == "client-userdata") {
		QJsonDocument doc = QJsonDocument::fromJson(data.toUtf8());
		FoeUser* user = clan->getFoeUser(doc.object().value("user").toString());
		if (user)
			user->deserialize(doc.object());
	}

}


void PusherHandler::connectionEstablished() {
	_d->pusher.join("private-testchannel");
}


void PusherHandler::subscriptionSucceeded(const QString& channel) {
	if (channel == "private-testchannel")
		_d->sendAllUserHash();
}


void PusherHandler::memberAdded(const QString& user_id, const QString& user_info) {
}


void PusherHandler::memberRemoved(const QString& user_id) {
}


void PusherHandler::notifyUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level) {
	QString data = QString("%1:%2:%3:%4").arg(user->name()).arg(product->id()).arg(factories).arg(boost_level);
	_d->pusher.send_message("private-testchannel", "client-setuserhas", qPrintable(data));
}


void PusherHandler::notifyUserRemove(const QString& name) {
	_d->pusher.send_message("private-testchannel", "client-removeuser", name);
}


void PusherHandler::notifyUserAdd(const QString& name) {
	_d->pusher.send_message("private-testchannel", "client-adduser", name);
}

void PusherHandler::setup() {
	if (_d->persist.getBoolOption("pusher.enabled", false)) {
		_d->pusher.set_apikey(_d->persist.getStrOption("pusher.apikey", ""));
		_d->pusher.set_secret(_d->persist.getStrOption("pusher.secret", ""));
		_d->pusher.connectPusher();
	} else
		_d->pusher.disconnectPusher();

//	dbc237fb9eac15998d95", "43c649a9018e9b957169
}
