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

	unsigned int id();
	const QString& name();
	void setName(const QString& new_name);
	FoeUser* getFoeUser(QString username);
	FoeUser* FoeUserFactory(unsigned int userid);
	QSet<FoeUser*> getUsersForProduct(const FoeGoods* product);
	bool loadusers(bool complete_reload = false);
	void removeUser(FoeUser* user);
	void refreshUserModel();
	QStringListModel* userModel();

	QVector<FoeUser*>& getFoeUsers();
	FoeOverviewModel* getOverviewModel();

public slots:
	void userUpdated();

signals:
	void userAdded(FoeUser* user);
	void userRemoved();

private:
	struct Private;
	struct Private* _d;
};
