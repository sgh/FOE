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

#include <memory>

// Cryptography
#include <QCryptographicHash>

// Locks and threads
#include <QMutex>

#include <QJsonDocument>
#include <QJsonObject>

#include <QTimer>
#include <QTcpSocket>


class Pusher::Private : public QObject {
	Q_OBJECT

public:
	Private();
	~Private();
	QString secret;
	QString api_key;
	QString clientname;
	QString clientversion;
	std::vector<IPusherListener*> listeners;
	QString socket_id;
	QByteArray accumulator;

	std::string get_authentication(const QString& socket_id, const QString& channel_name);

	void parse_websocket(const QByteArray& data);
	void send(std::string event, std::string data);
	void send_channel(QString channel, QString event, QString data, bool escaped);

	void parse_pusher_connection_established(const QString& data);
	void parse_pusher_error(const QString& data);
	QMutex lock;
	bool _connect;

private:
	QTcpSocket* tcp;
	QTimer _reconnectTimer;

private slots:
	void network_connected();
	void network_disconnected();
	void network_state_changed(QAbstractSocket::SocketState state);
	void network_data();
	void setup();

public slots:
	void connectToHost();
	void disconnectFromHost();

};
