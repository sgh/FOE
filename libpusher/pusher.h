#pragma once

#include <QString>

#include <string>
#include <vector>

class IPusherListener {
public:
	virtual void eventReceived(const QString& event, const QString& data)  = 0;
	virtual void connectionEstablished() = 0;
	virtual void subscriptionSucceeded(const QString &channel) = 0;
	virtual void memberAdded(const QString& user_id, const QString& user_info) = 0;
	virtual void memberRemoved(const QString& user_id) = 0;
};



class Pusher {
public:
	explicit Pusher(const QString& api_key, const QString& secret, const QString& clientname, const QString& clientversion);
	virtual ~Pusher();

	void join(const QString& channel);
	void join_presence(const QString& channel);

	void send_message(const QString& channel, const QString& event, const QString& data);
	void send_message_unescaped(const QString& channel, const QString& event, const QString& data);

	void addListener(IPusherListener* listener);

private:
	class Private;
	Private* _d;
};
