#include "foedatamanger.h"
#include "foeclan.h"
#include "foeuser.h"
#include "foeoverviewmodel.h"

struct FoeClan::Private {
	FoeOverviewModel* model;
	QStringListModel userModel;
	unsigned int id;
	QString name;
	FoeDataManager* data;
	QVector<FoeUser*> userList;
};


FoeClan::FoeClan(FoeDataManager* data, unsigned id)
{
	_d = new Private;
	_d->data = data;
	_d->id = id;
	_d->name = _d->data->getClanname(id);
	_d->model = new FoeOverviewModel(this);
}


FoeClan::~FoeClan()
{
	delete _d->model;
	delete _d;
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


FoeUser* FoeClan::getFoeUser(QString username)
{
	FoeUser* user;
	foreach (user, _d->userList) {
		if (user->name() == username)
			return user;
	}

	return NULL;
}


QStringListModel* FoeClan::userModel() {
	return &_d->userModel;
}


QVector<FoeUser*>&FoeClan::getFoeUsers() {
	return _d->userList;
}


FoeOverviewModel*FoeClan::getOverviewModel() {
	return _d->model;
}


void FoeClan::userUpdated() {
	_d->model->updateOverview();
}


FoeUser *FoeClan::FoeUserFactory(const QString& name, unsigned int userid)
{
	FoeUser* user;
	foreach (user, _d->userList) {
		if (user->id() == userid) {
			user->reload();
			return user;
		}
	}

	user = new FoeUser(this, _d->data, name, userid);
	_d->userList << user;
	refreshUserModel();
	emit userAdded(user);
	return user;
}


QSet<FoeUser *> FoeClan::getUsersForProduct(const FoeGoods *product)
{
	QSet<FoeUser*> userSet;
	FoeUser* user;
	foreach (user, _d->userList) {
		if (user->hasBonus(product) != e_NO_BOOST || user->hasProduct(product))
			userSet.insert(user);
	}
	return userSet;
}


void FoeClan::removeUser(FoeUser* userToRemove)
{
	for (int idx=0; idx<_d->userList.size(); idx++) {
		FoeUser* user = _d->userList[idx];
		if (user == userToRemove) {
			_d->userList.remove(idx);
			refreshUserModel();
			emit userRemoved();
			break;
		}
	}
}


void FoeClan::refreshUserModel()
{
	QStringList lst;
	FoeUser* user;
	foreach (user, _d->userList) {
		lst.append(user->name());
	}
	lst.sort();
	_d->userModel.setStringList(lst);
}
