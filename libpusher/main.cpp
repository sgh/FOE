#include "pusher.h"

#include <QCoreApplication>
#include <unistd.h>

Pusher* p;

class L : public IPusherListener {
public:
	virtual void eventReceived(const std::string& event, const std::string& data) {
		printf("%s: %s\n", event.c_str(), data.c_str());
	}

	virtual void connectionEstablished() {
		printf("Connection established\n");
		p->join("private-testchannel");
	}

	void subscriptionSucceeded(const std::string &channel) {
		printf("Subscripted : %s\n", channel.c_str());

		p->send_message("private-testchannel", "client-eventname", "DATA");
	}

	void memberAdded(const std::string& user_id, const std::string& user_info) {
		printf("Member addedd: %s - %s\n", user_id.c_str(), user_info.c_str());
	}

	void memberRemoved(const std::string& user_id) {
		printf("Member removed: %s\n", user_id.c_str());
	}

};



int main(int argc, char** argv) {
	QCoreApplication app(argc, argv);
	L l;

	p = new Pusher("dbc237fb9eac15998d95", "43c649a9018e9b957169", "App", "1.2");
	p->addListener(&l);

//	p->join_presence("presence-testchannel");

	printf("Entering event loop\n");
//	usleep(1000000);
//	p->send_message("private-testchannel", "client-testevent", "TESTDATA");
//	while (1) {
//		usleep(5000000);
//	}
 	app.exec();

}