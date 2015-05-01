#include "foedatamanger.h"
#include "foeclan.h"
#include "foeuser.h"
#include "foeoverviewmodel.h"

using namespace std;

struct FoeClan::Private {
	Private(FoeDataManager& data)
		: data(data)
	{
		model = NULL;
		id = 0;
	}

	FoeOverviewModel* model;
	QStringListModel userModel;
	unsigned int id;
	QString name;
	FoeDataManager& data;
	QVector<shared_ptr<FoeUser>> userList;
};


FoeClan::FoeClan(FoeDataManager& data, unsigned id)
{
	_d.reset(new Private(data));
	_d->id = id;
	_d->name = _d->data.getClanname(id);
	_d->model = new FoeOverviewModel(this);
}


FoeClan::~FoeClan()
{
	_d->userList.clear();
	emit userRemoved();
	delete _d->model;
}


unsigned int FoeClan::id() {
	return _d->id;
}


const QString&FoeClan::name() {
	return _d->name;
}


void FoeClan::setName(const QString& new_name)
{
	_d->name = new_name;
}


shared_ptr<FoeUser> FoeClan::getFoeUser(QString username)
{
	foreach (auto user, _d->userList) {
		if (user->name() == username)
			return user;
	}

	return NULL;
}


QStringListModel* FoeClan::userModel() {
	return &_d->userModel;
}


const QVector<shared_ptr<FoeUser>>& FoeClan::getFoeUsers() {
	return _d->userList;
}


shared_ptr<FoeUser> FoeClan::getUser(const QString& name) {
	foreach (auto user, _d->userList) {
		if (user->name() == name)
			return user;
	}
	return NULL;
}


FoeOverviewModel*FoeClan::getOverviewModel() {
	return _d->model;
}


void FoeClan::userUpdated() {
	_d->model->updateOverview();
}


QVector<std::shared_ptr<FoeUser> > FoeClan::getUsersForProduct(const FoeGoods *product)
{
	QVector<shared_ptr<FoeUser>> userVec;
	foreach (shared_ptr<FoeUser> user, _d->userList) {
		if (user->hasBonus(product) != e_NO_BOOST || user->hasProduct(product)) {
			userVec.push_back(user);
		}
	}
	return userVec;
}


void FoeClan::removeUser(FoeUser* userToRemove)
{
	for (int idx=0; idx<_d->userList.size(); idx++) {
		shared_ptr<FoeUser> user = _d->userList[idx];
		if (user.get() == userToRemove) {
			_d->userList.remove(idx);
			refreshUserModel();
			emit userRemoved();
			break;
		}
	}
}


void FoeClan::addUser(shared_ptr<FoeUser> user) {
	_d->userList << user;
	refreshUserModel();
	user->setClan(this);
	user->setData(_d->data);
	emit userAdded(user.get());
}


void FoeClan::refreshUserModel()
{
	QStringList lst;
	foreach (auto user, _d->userList) {
		lst.append(user->name());
	}
	lst.sort();
	_d->userModel.setStringList(lst);
}
