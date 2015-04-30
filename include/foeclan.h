#pragma once
#include <memory>
#include <QString>
#include <QStringListModel>

class FoeGoods;
class FoeUser;
class FoeDataManager;
class FoeOverviewModel;

class FoeClan : public QObject {
	Q_OBJECT

public:
	FoeClan(FoeDataManager& data, unsigned id);
	~FoeClan();

	unsigned int id();
	const QString& name();
	void setName(const QString& new_name);
	std::shared_ptr<FoeUser> getFoeUser(QString username);
	QVector<std::shared_ptr<FoeUser> > getUsersForProduct(const FoeGoods* product);
	void removeUser(FoeUser* user);
	void addUser(std::shared_ptr<FoeUser> user);
	void refreshUserModel();
	QStringListModel* userModel();

	const QVector<std::shared_ptr<FoeUser> >& getFoeUsers();
	std::shared_ptr<FoeUser> getUser(const QString& name);
	FoeOverviewModel* getOverviewModel();

public slots:
	void userUpdated();

signals:
	void userAdded(FoeUser* user);
	void userRemoved();

private:
	struct Private;
	std::shared_ptr<struct Private> _d;
};
