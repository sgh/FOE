#include "foedatamanger.h"
#include "foeclan.h"
#include "foeuser.h"
#include "foeoverviewmodel.h"

FoeClan::FoeClan(FoeDataManager* data, unsigned id)
{
	_data = data;
	_id = id;
	_name = _data->getClanname(id);
	_model = new FoeOverviewModel(this);
	loadusers(true);
}

FoeClan::~FoeClan()
{
	delete _model;
}

void FoeClan::setName(const QString& new_name)
{
	_name = new_name;
}

FoeUser* FoeClan::getFoeUser(QString username)
{
	FoeUser* user;
	foreach (user, _userList) {
		if (user->name() == username)
			return user;
	}

	return NULL;
}


QStringListModel* FoeClan::userModel() {
	return &_userModel;
}


FoeUser *FoeClan::FoeUserFactory(unsigned int userid)
{
	FoeUser* user;
	foreach (user, _userList) {
		if (user->id() == userid) {
			user->reload();
			return user;
		}
	}

	user = new FoeUser(_data, userid);
	QObject::connect (user, &FoeUser::updated, _model, &FoeOverviewModel::update );
	_userList << user;
	refreshUserModel();
	return user;
}


QSet<FoeUser *> FoeClan::getUsersForProduct(const FoeGoods *product)
{
	QSet<FoeUser*> userSet;
	FoeUser* user;
	foreach (user, _userList) {
		if (user->hasBonus(product) != e_NO_BOOST || user->hasProduct(product))
			userSet.insert(user);
	}
	return userSet;
}


bool FoeClan::loadusers(bool complete_reload) {
	return _data->loadusers(this, complete_reload);
}

void FoeClan::removeUser(FoeUser* userToRemove)
{
	if (_userList.removeOne(userToRemove))
		refreshUserModel();
}


void FoeClan::refreshUserModel()
{
	QStringList lst;
	FoeUser* user;
	foreach (user, _userList) {
		lst.append(user->name());
	}
	lst.sort();
	_userModel.setStringList(lst);
}
