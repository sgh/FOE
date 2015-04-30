#include "pusher.h"
#include "foedatamanger.h"

#include <string>
#include <memory>

class PusherHandler : public IPusherListener {
public:
	explicit PusherHandler(std::shared_ptr<FoePersistence> persist, std::shared_ptr<FoeDataManager> data);
	virtual ~PusherHandler();

	void eventReceived(const QString& event, const QString& data);

	void connectionEstablished();

	void subscriptionSucceeded(const QString& channel);

	void memberAdded(const QString& user_id, const QString& user_info);

	void memberRemoved(const QString& user_id);

	void notifyUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level);
	void notifyUserRemove(const QString& name);
	void notifyUserAdd(const QString& name);

	void setup();
private:
	struct Private;
	std::unique_ptr<Private> _d;
};

