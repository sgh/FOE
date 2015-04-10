#include "pusher.h"

#include <QCoreApplication>
#include <unistd.h>

Pusher* p;

class L : public IPusherListener {
public:
	virtual void eventReceived(const QString& event, const QString& data) {
		printf("%s: %s\n", qPrintable(event), qPrintable(data));
	}

	virtual void connectionEstablished() {
		printf("Connection established\n");
		p->join("private-testchannel");
	}

	void subscriptionSucceeded(const QString &channel) {
		printf("Subscripted : %s\n", qPrintable(channel));

		p->send_message("private-testchannel", "client-eventname", "DATA");
	}

	void memberAdded(const QString& user_id, const QString& user_info) {
		printf("Member addedd: %s - %s\n", qPrintable(user_id), qPrintable(user_info));
	}

	void memberRemoved(const QString& user_id) {
		printf("Member removed: %s\n", qPrintable(user_id));
	}

};



int main(int argc, char** argv) {
	QCoreApplication app(argc, argv);
	L l;

	p = new Pusher("dbc237fb9eac15998d95", "43c649a9018e9b957169", "App", "1.2");
	p->addListener(&l);
	p->connectPusher();

//	p->join_presence("presence-testchannel");

	printf("Entering event loop\n");
//	usleep(1000000);
//	p->send_message("private-testchannel", "client-testevent", "TESTDATA");
//	while (1) {
//		usleep(5000000);
//	}
 	app.exec();

}