#include "pusher.h"
#include "foedatamanger.h"

#include <string>
#include <memory>

#include <QLabel>

class PusherHandler : public QObject, public IPusherListener {
public:
	explicit PusherHandler(std::shared_ptr<FoePersistence> persist, std::shared_ptr<FoeDataManager> data);
	virtual ~PusherHandler();

	// IPusherListener interface
	void eventReceived(const QString& event, const QString& data) override;
	void connectionEstablished() override;
	void disconnected() override;
	void subscriptionSucceeded(const QString& channel) override;
	void memberAdded(const QString& user_id, const QString& user_info) override;
	void memberRemoved(const QString& user_id) override;

	void notifyUserHas(FoeUser* user, const FoeGoods* product, int factories, BoostLevel boost_level);
	void notifyUserRemove(const QString& name);
	void notifyUserAdd(const QString& name);

	QWidget* getStatusWidget();

	void setup();
private:
	struct Private;
	std::unique_ptr<Private> _d;
	QLabel* _statuslabel;
};

