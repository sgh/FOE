#pragma once

#include <QString>

#include <string>
#include <vector>

class IPusherListener {
public:
	virtual void eventReceived(const std::string& event, const std::string& data)  = 0;
	virtual void connectionEstablished() = 0;
	virtual void subscriptionSucceeded(const std::string &channel) = 0;
	virtual void memberAdded(const std::string& user_id, const std::string& user_info) = 0;
	virtual void memberRemoved(const std::string& user_id) = 0;
};



class Pusher {
public:
	explicit Pusher(const QString& api_key, const QString& secret, const QString& clientname, const QString& clientversion);
	virtual ~Pusher();

	void join(std::string channel);
	void join_presence(std::string channel);

	void send_message(const std::string& channel, const std::string& event, const std::string& data);
	void send_message_unescaped(const std::string& channel, const std::string& event, const std::string& data);

	void addListener(IPusherListener* listener);

private:
	class Private;
	Private* _d;
};
