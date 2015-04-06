#include "pusher.h"
#include "pusher_p.h"

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

// Cryptography
#include <QCryptographicHash>

// Threads
#include <QThread>

//
#include <QTcpSocket>

#include <QJsonDocument>
#include <QJsonObject>


using namespace std;


Pusher::Private::Private() {
//	connect(&thread, SIGNAL(started()), this, SLOT(setup()));
//	moveToThread(&thread);
//	thread.start();
	setup();
}


Pusher::Private::~Private() {
	delete tcp;
}


void Pusher::Private::parse_pusher_connection_established(const QString& data) {
	QJsonDocument jsondoc = QJsonDocument::fromJson(data.toUtf8());
	QJsonObject json = jsondoc.object();

	if (json.find("socket_id") == json.end()) {
// 		printf("Socket ID not found!\n");
		return;
	}

	socket_id =  json["socket_id"].toString();
// 	printf("Socket ID is : %s\n", qPrintable(socket_id));
}


void Pusher::Private::parse_pusher_error(const QString& data) {
	QJsonDocument jsondoc = QJsonDocument::fromJson(data.toUtf8());
	QJsonObject json = jsondoc.object();

	if (json.find("message") == json.end()) {
		printf("Error message not found!\n");
		return;
	}

	printf("Pusher error: %s\n", qPrintable(json["message"].toString()));
}

void Pusher::Private::network_connected() {
// 	printf("Network connected\n");
	stringstream ss;
	ss << "GET /app/" << api_key << "?client=" << clientname << "&version=" << clientversion << "&protocol=5 HTTP/1.1\r\n";
	ss << "Connection: Upgrade\r\n";
	ss << "Upgrade: websocket\r\n";
	ss << "\r\n";

	string connection_string = ss.str();

	tcp->write(connection_string.c_str(), connection_string.size());
}

void Pusher::Private::setup() {
	tcp = new QTcpSocket(this);
	connect(tcp, SIGNAL(connected()), this, SLOT(network_connected()));
	connect(tcp, SIGNAL(readyRead()), this, SLOT(network_data()));
	tcp->connectToHost("ws.pusherapp.com", 80);
}

void Pusher::Private::network_data() {
	QByteArray buf;
	buf = tcp->readAll();
	if (buf.length() > 0) {
		parse_websocket(buf);
	}
}


void Pusher::Private::parse_websocket( const QByteArray& data ) {
	accumulator.append(data);
	int start = accumulator.indexOf('\x00', 0);
	int end = accumulator.indexOf('\xff', 0);
// 	printf("%d %d \n", start, end);
	if (start != -1 && end != -1) {
		start++;

		accumulator.data()[end] = 0;
 		printf("Processing frame: %s\n", &accumulator.data()[start]);

		QJsonDocument jsondoc = QJsonDocument::fromJson(&accumulator.data()[start]);
		if (jsondoc.isObject()) {
			QJsonObject json = jsondoc.object();
			QString channel  = json["channel"].toString();
			QString event    = json["event"].toString();
			QString data     = json["data"].toString();

			if (!event.isEmpty() && !data.isEmpty()) {
// 				printf("Channel: %s\n", qPrintable(channel));
// 				printf("Event: %s\n", qPrintable(event));
// 				printf("Data: %s\n", qPrintable(data));
				if (event == "pusher:connection_established") {
					parse_pusher_connection_established( data );
					for (size_t idx=0; idx<listeners.size(); idx++) {
						listeners[idx]->connectionEstablished();
					}
				} else if (event ==  "pusher:ping")
					send("pusher:pong", "");
				else if (event ==  "pusher:error")
					parse_pusher_error( data );
				else if (event == "pusher_internal:subscription_succeeded") {
					for (size_t idx=0; idx<listeners.size(); idx++) {
						listeners[idx]->subscriptionSucceeded(channel);
					}
				} else if (event == "pusher_internal:member_added") {
					QJsonDocument datajson = QJsonDocument::fromJson(data.toStdString().c_str());
					QString user_id  = datajson.object()["user_id"].toString();
					for (size_t idx=0; idx<listeners.size(); idx++) {
						listeners[idx]->memberAdded(user_id, data);
					}
				} else if (event == "pusher_internal:member_removed") {
					QJsonDocument datajson = QJsonDocument::fromJson(data.toStdString().c_str());
					QString user_id  = datajson.object()["user_id"].toString();
					for (size_t idx=0; idx<listeners.size(); idx++) {
						listeners[idx]->memberRemoved(user_id);
					}

				} else {
					for (size_t idx=0; idx<listeners.size(); idx++) {
						listeners[idx]->eventReceived(event, data);
					}
				}
			}
		}
		accumulator.remove(0, end+1);
	}

	if (accumulator.length() > 0) {
		printf("Combined data not parsed: %d bytes\n", accumulator.length());
		printf("%s\n", accumulator.data());
	}
}


static string escape(const string& str) {
	stringstream ss;
	for (size_t idx=0; idx<str.size(); idx++) {
		if (str[idx] == '"')
			ss << "\\";
		ss << str[idx];
	}
	return ss.str();
}


void Pusher::Private::send(string event, string data) {
	stringstream ss;
	ss << "{\"event\":\"" << event << "\", \"data\": " << data << " }";
	string str = ss.str();
	tcp->write("\x00", 1);
	tcp->write(str.c_str(), str.size());
 	printf("Sent frame: %s\n", str.c_str());
	tcp->write("\xff", 1);
	tcp->flush();
}

void Pusher::Private::send_channel(QString channel, QString event, QString data, bool escaped) {
	stringstream ss;
	ss << "{\"event\":\"" << event.toStdString() << "\", \"channel\":\"" << channel.toStdString() << "\"" << ",\"data\":";
	if (escaped)
		ss << "\"" ;
	ss << data.toStdString();
	if (escaped)
		ss << "\"";
	ss << "}";
	string str = ss.str();
	tcp->write("\x00", 1);
	tcp->write(str.c_str(), str.size());
	tcp->write("\xff", 1);
	printf("Sent frame %zd bytes: %s\n", str.size(), str.c_str());
	tcp->flush();
}


static QByteArray hmacSha256(QByteArray key, const QByteArray& message) {
	const int blocksize = 64;
	if (key.length() > blocksize)
		QCryptographicHash::hash(key, QCryptographicHash::Sha256);

	while (key.length() < blocksize)
		key.append('\0');

	QByteArray o_key_pad('\x5c', blocksize);
	o_key_pad.fill('\x5c', blocksize);

	QByteArray i_key_pad;
	i_key_pad.fill('\x36', blocksize);

	for (int i=0; i<blocksize; i++) {
		o_key_pad[i] = o_key_pad[i] ^ key[i];
		i_key_pad[i] = i_key_pad[i] ^ key[i];
	}

	return QCryptographicHash::hash(o_key_pad + QCryptographicHash::hash(i_key_pad + message, QCryptographicHash::Sha256), QCryptographicHash::Sha256);
}


string Pusher::Private::get_authentication(const QString& socket_id, const QString& channel_name) {
	static const char a2h[] = {'0','1','2','3','4','5','6','7','8','9','a','b','c','d','e','f'};

// 	printf("Hashing : %s:%s\n", socket_id.c_str(), channel_name.c_str());

	QString message = socket_id;
	message += ":";
	message += channel_name;
	QByteArray digest = hmacSha256( QByteArray(secret.c_str(), secret.size()), message.toUtf8());

	stringstream ss;
	for (int i=0; i<digest.length(); i++) {
		unsigned char val2 = (unsigned char)digest.data()[i];
		ss << a2h[val2 >> 4];
		ss << a2h[val2 & 0x0f];
	}

	return ss.str();
}





Pusher::Pusher(const QString& api_key, const QString& secret, const QString& clientname, const QString& clientversion) {
	_d = new Private;
	_d->clientname = clientname.toStdString();
	_d->clientversion = clientversion.toStdString();
	_d->api_key = api_key.toStdString();
	_d->secret = secret.toStdString();
}

Pusher::~Pusher() {
	delete _d;
}

void Pusher::join(const QString& channel) {
	_d->lock.lock();
	while (_d->socket_id.isEmpty()) {
		_d->lock.unlock();
		usleep(100000);
		_d->lock.lock();
	}
	string auth = _d->get_authentication(_d->socket_id, channel);
	stringstream ss;
	ss <<  "{\"channel\": \"" << channel.toStdString() << "\", \"auth\" : \"" << _d->api_key << ":" << auth << "\"}";
	_d->send("pusher:subscribe", ss.str());;
	_d->lock.unlock();
}


void Pusher::addListener(IPusherListener* listener) {
	_d->lock.lock();
	_d->listeners.push_back(listener);
	_d->lock.unlock();
}


void Pusher::join_presence(const QString& channel) {
	_d->lock.lock();
	while (_d->socket_id.isEmpty()) {
		_d->lock.unlock();
		usleep(100000);
		_d->lock.lock();
	}

	stringstream user_data;
	user_data << "{\"user_id\" : \"" << _d->socket_id.toStdString() << "\"";
	user_data << ", \"user_info\" :{ ";
	user_data << "\"name\": \"Phil Leggetter\"";
	user_data << "}";
	user_data << "}";

	string auth = _d->get_authentication(_d->socket_id, channel + ":" + user_data.str().c_str());

	stringstream ss;
	ss <<  "{\"channel\": \"" << channel.toStdString() << "\", \"auth\" : \"" << _d->api_key << ":" << auth << "\"";
	ss << ", \"channel_data\" : \"";
	ss << escape(user_data.str());
	ss << "\"}";

// 	printf("Data: %s\n", ss.str().c_str());
	_d->send("pusher:subscribe", ss.str());
	_d->lock.unlock();
}


void Pusher::send_message(const QString& channel, const QString& event, const QString& data) {
	_d->lock.lock();
	_d->send_channel(channel, event, data, true);
	_d->lock.unlock();
}

void Pusher::send_message_unescaped(const QString &channel, const QString &event, const QString &data) {
	_d->lock.lock();
	_d->send_channel(channel, event, data, false);
	_d->lock.unlock();

}

