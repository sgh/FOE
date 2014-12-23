#pragma once
#include <QString>
#include <QStringListModel>

class FoeGoods;
class FoeUser;
class FoeDataManager;
class FoeOverviewModel;

class FoeClan : public QObject {
	Q_OBJECT

public:
	FoeClan(FoeDataManager* data, unsigned id);
	~FoeClan();

	unsigned int id() { return _id; }
	const QString& name() { return _name; }
	void setName(const QString& new_name);
	FoeUser* getFoeUser(QString username);
	FoeUser* FoeUserFactory(unsigned int userid);
	QSet<FoeUser*> getUsersForProduct(const FoeGoods* product);
	bool loadusers(bool complete_reload = false);
	void removeUser(FoeUser* user);
	void refreshUserModel();
	QStringListModel* userModel();

	QList<FoeUser*>& getFoeUsers() { return _userList; }
	FoeOverviewModel* getOverviewModel() { return _model; }

public slots:
	void userUpdated();

signals:
	void userAdded(FoeUser* user);
	void userRemoved();

private:
	FoeOverviewModel* _model;
	QStringListModel _userModel;
	unsigned int _id;
	QString _name;
	FoeDataManager* _data;
	QList<FoeUser*> _userList;
};
