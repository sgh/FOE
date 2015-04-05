#pragma once
#include "pusher.h"

#include <stdio.h>
#include <unistd.h>

#include <sys/types.h>
#include <errno.h>
#include <sys/types.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/time.h>
#include <stdlib.h>

#include <iostream>
#include <sstream>
#include <map>

// Cryptography
#include <QCryptographicHash>

// Locks and threads
#include <QMutex>
#include <QThread>

#include <QJsonDocument>
#include <QJsonObject>

#include <QTimerEvent>
#include <QTcpSocket>


class Pusher::Private : public QObject {
	Q_OBJECT
public:
	Private();
	~Private();
	std::string secret;
	std::string api_key;
	std::string clientname;
	std::string clientversion;
	std::vector<IPusherListener*> listeners;
	QString socket_id;
	QByteArray accumulator;

	std::string get_authentication(const std::string& socket_id, const std::string& channel_name);

	void parse_websocket(const QByteArray& data);
	void send(std::string event, std::string data);
	void send_channel(std::string channel, std::string event, std::string data, bool escaped);

	void parse_pusher_connection_established(const QString& data);
	void parse_pusher_error(const QString& data);
	QMutex lock;

private:

	QThread thread;
	QTcpSocket* tcp;

private slots:
	void network_connected();
	void network_data();
	void setup();

};
