#include "pusher.h"
#include "foedatamanger.h"

#include <string>

class PusherHandler : public IPusherListener {
public:
	explicit PusherHandler(FoePersistence& persist, FoeDataManager& data);
	~PusherHandler();

	void eventReceived(const QString& event, const QString& data);

	void connectionEstablished();

	void subscriptionSucceeded(const QString& channel);

	void memberAdded(const QString& user_id, const QString& user_info);

	void memberRemoved(const QString& user_id);

	void notifyUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level);
	void notifyUserRemove(const QString& name);
	void notifyUserAdd(const QString& name);

private:
	struct Private;
	Private* _d;
};

