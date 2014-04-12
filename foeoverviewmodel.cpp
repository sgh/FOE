#include <iostream>

#include <QPixmap>

#include "foeoverviewmodel.h"
#include "foegoods.h"
#include "foeuser.h"


class FoeLessThan
{
public:
	FoeLessThan() {}

	bool operator()(const FoeUser *left, const FoeUser *right ) const {
		return left->name() < right->name();
	}

	bool operator()(const FoeGoods *left, const FoeGoods *right ) const {
		return left->id() < right->id();
	}
};


using namespace std;

FoeOverviewModel::FoeOverviewModel(FoeDataManager &data) :
	_data(data)
{
	populate_toplevel();
	update();
}

void FoeOverviewModel::userAdded(FoeUser * user)
{
	connect (user, &FoeUser::updated, this, &FoeOverviewModel::update );
	update();
}

void FoeOverviewModel::userRemoved()
{
	update();
}


void FoeOverviewModel::populate_toplevel()
{
	_product2item.clear();

	const QVector<FoeAge*>& ages = FoeAge::getAges();

	FoeAge* age;

	foreach (age, ages) {
		QStandardItem* ageItem   = new QStandardItem( age->name() );
		QStandardItem* blankItem = new QStandardItem();
		QColor c = age->color();
		ageItem->setBackground( c );
		blankItem->setBackground( c );
		ageItem->setForeground(Qt::white);
		QFont f = ageItem->font();
		f.setBold(true);
		ageItem->setFont(f);
		QVector<QStandardItem*> v;
		_age2item[age] = blankItem;
		v << ageItem << blankItem;
		appendRow( v.toList() );

		QList<const FoeGoods*> products = FoeGoods::getGoodsForAge(age);
		const FoeGoods* product;

		foreach(product, products) {
			QStandardItem* productItem = new QStandardItem(product->name());
			productItem->setIcon(product->icon());
			QVector<QStandardItem*> v;
			v << productItem << new QStandardItem();
			_product2item[product] = v;
			ageItem->appendRow( v.toList() );
		}
	}
}


void FoeOverviewModel::populate_product(const FoeGoods* product)
{
	FoeUser* user;
	QStandardItem* productItem       = _product2item[product][0];
	QStandardItem* productStatusItem = _product2item[product][1];

	setupProductTooltip(product, productItem);

	// Remove current old rows
	productItem->removeRows(0, productItem->rowCount());

	int count = 0;
	int boost_count = 0;
	int almost_boost_count = 0;
	QList<FoeUser*> users = _data.getFoeUsers();

	qSort(users.begin(), users.end(), FoeLessThan());

	foreach (user, users) {
		int factories = user->hasProduct(product);
		if (user->hasBonus(product) != e_NO_BOOST || factories>0) {
			QList<QStandardItem*> lst2;

			QStandardItem* user_item = new QStandardItem(user->name());
			_item2user[user_item] = user;
			setupUserTooltip(user, user_item);

			count+=factories;
			if (user->hasBonus(product) == e_BOOST) {
				boost_count++;
				QFont f = user_item->font();
				user_item->setForeground(product->boostColor(e_BOOST));
				f.setBold(true);
				user_item->setFont(f);
			}

			if (user->hasBonus(product) == e_NO_BOOST) {
				QFont f = user_item->font();
				user_item->setForeground(product->boostColor(e_NO_BOOST));
				f.setBold(true);
				user_item->setFont(f);
			}

			QStandardItem* fabitem;
			if (factories > 0)
				fabitem = new QStandardItem(QString("%1").arg(factories));
			else
				fabitem = new QStandardItem();

			BoostLevel bl = user->hasBonus(product);
			if (bl == e_NOT_CONQUERED || bl == e_NEEDS_RESEARCH) {
				almost_boost_count++;
				QFont f = user_item->font();
				user_item->setForeground(product->boostColor(e_NOT_CONQUERED));
				f.setBold(true);
				user_item->setFont(f);
			}
			lst2 << user_item;
			lst2 << fabitem;
			productItem->appendRow( lst2 );
		}
	}

	QString str;
	if (count > 0)
		str += QString("%1 fabrikker").arg(count);

	if (boost_count > 0)
		str +=  QString("%1%2 bonus").arg(str.isEmpty()?"":" - ").arg(boost_count);

	if (almost_boost_count > 0)
		str += QString("%1%2 snart bonus").arg(str.isEmpty()?"":" - ").arg(almost_boost_count);

	productStatusItem->setData(str, Qt::DisplayRole);
	QFont f = productStatusItem->font();
	f.setBold(true);
	productStatusItem->setFont(f);
}


void FoeOverviewModel::setupProductTooltip(const FoeGoods *product, QStandardItem *productItem)
{
	FoeUser* user;

	// Tooltip
	QList<FoeUser*> userList = _data.getUsersForProduct(product).toList();
	qSort( userList.begin(),    userList.end(),    FoeLessThan());

	QSet<const FoeGoods*> productSet;
	foreach (user, userList) {
		productSet += user->getProducts();
	}

	// Convert productSet to list and sort it
	QList<const FoeGoods*> productList = productSet.toList();
	qSort( productList.begin(), productList.end(), FoeLessThan());

	QString text = "<table><tr><td></td>";
	foreach (product, productList) {
		text += QString("<td><center><img src='%1'><br>&nbsp;%2&nbsp;</center></td>").arg(product->iconFile()).arg(product->name());
	}
	text += "</tr>";

	foreach (user, userList) {
		text += QString("<tr><td>%1</td>").arg(user->name());
		foreach (product, productList) {
			BoostLevel bl = user->hasBonus(product);
			int factories = user->hasProduct(product);
			QString factories_text;
			if (bl > e_NO_BOOST)
				factories_text = "-";
			if (factories>0)
				factories_text = QString("%1").arg(factories);
			text += QString("<td><font color='%1'><center>%2</center></font></td>").arg(product->boostColorHTML(bl)).arg(factories_text);
		}
		text += "</tr>";
	}
	text += "</table>";

	if (!productSet.empty() && !userList.empty())
		productItem->setToolTip(text);
	else
		productItem->setToolTip("Ingen har bonus på, eller producerer denne vare.");
}


void FoeOverviewModel::setupUserTooltip(FoeUser *user, QStandardItem *userItem)
{
	QString text = "<table>";
	const QMap<const FoeGoods *, BoostLevel>& allBonus  = user->allBonus();
	const QList<const FoeGoods *> &products = FoeGoods::getGoods();
	const FoeGoods* product;
	foreach (product, products) {

		BoostLevel bl = e_NO_BOOST;
		if (allBonus.contains(product))
			bl = allBonus[product];
		int factories = user->hasProduct(product);
		if (factories>0 || bl > e_NO_BOOST) {

			text += "<tr>";
			text += QString("<td>%1</td>").arg(product->name());
			text += "<td>";
			if (factories>0)
				text += QString("%1").arg(factories);
			text += "</td>";
			text += QString("<td><font color='%1'>%2</font></td>").arg(product->boostColorHTML(bl)).arg(product->boostText(bl));
		}
		text += "</tr>";
	}
	text += "</table>";
	userItem->setToolTip(text);
}


void FoeOverviewModel::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
	for (int i = start; i<=end; i++) {
		QStandardItem* item = itemFromIndex(parent.child(i,0)) ;
//		if (item && _item2user.contains(item))
//			cout << "rowsAboutToBeRemoved" << endl;
		_item2user.remove( item );
	}
}



void FoeOverviewModel::update () {
	QList<const FoeGoods *> productList = FoeGoods::getGoods();
	const FoeGoods* product;
	foreach (product, productList) {
		populate_product(product);
	}

	FoeUser* user;
	QList<FoeUser*> users = _data.getFoeUsers();

	QMap<const FoeGoods*, int> m;

	productList =  FoeGoods::getGoods();
	foreach (product, productList) {
		int factories;
		factories = 0;
		foreach (user, users) {
			factories += user->hasProduct(product);
		}
		m[product] = factories;
	}


	FoeAge* age;
	const QVector<FoeAge*>& ageList = FoeAge::getAges();
	foreach (age, ageList) {

		productList = FoeGoods::getGoodsForAge(age);
//		int factories = 0;
		QString str;
		foreach (product, productList) {
			str += QString(product->name() + ":%1   ").arg( m[product] );
		}
		_age2item[age]->setText(str);
	}

}
