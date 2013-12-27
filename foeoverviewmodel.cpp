#include <iostream>

#include <QPixmap>

#include "foeoverviewmodel.h"
#include "foeproduct.h"
#include "foeuser.h"


using namespace std;

FoeOverviewModel::FoeOverviewModel(FoeDataManager &data) :
	_data(data)
{
	populate_toplevel();
	update();
	connect(this, SIGNAL(rowsAboutToBeRemoved ( const QModelIndex &, int, int)), this, SLOT(rowsAboutToBeRemoved ( const QModelIndex &, int, int )));
}

void FoeOverviewModel::userAdded(FoeUser * user)
{
	connect (user, SIGNAL(updated()), SLOT(update()) );
	update();
}

void FoeOverviewModel::userRemoved(FoeUser *user)
{
	disconnect( user, SIGNAL(updated()));
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

		QList<const FoeProduct*> products = FoeProduct::getProductsForAge(age);
		const FoeProduct* product;

		foreach(product, products) {
			QStandardItem* productItem = new QStandardItem(product->name());
			productItem->setIcon(product->icon());
			QVector<QStandardItem*> v;
			v << productItem << new QStandardItem();
			_product2item[product] = v;
			ageItem->appendRow( v.toList() );
		}
	}
	setHorizontalHeaderItem(0, new QStandardItem("Vare"));
	setHorizontalHeaderItem(1, new QStandardItem("Status"));
}


void FoeOverviewModel::populate_product(const FoeProduct* product)
{
	FoeUser* user;
	QStandardItem* productItem       = _product2item[product][0];
	QStandardItem* productStatusItem = _product2item[product][1];

	setupProductTooltip(product, productItem);

	// Remove current old rows
	productItem->removeRows(0, productItem->rowCount());

	int count = 0;
	int bonus_count = 0;
	int almost_bonus_count = 0;
	QList<FoeUser*> users = _data.getFoeUsers();

	foreach (user, users) {
		int factories = user->hasProduct(product);
		if (user->hasBonus(product) != e_NO_BONUS || factories>0) {
			QList<QStandardItem*> lst2;

			QStandardItem* user_item = new QStandardItem(user->name());
			_item2user[user_item] = user;
			setupUserTooltip(user, user_item);

			count+=factories;
			if (user->hasBonus(product) == e_BONUS) {
				bonus_count++;
				QFont f = user_item->font();
				user_item->setForeground(Qt::green);
				f.setBold(true);
				user_item->setFont(f);
			}

			if (user->hasBonus(product) == e_NO_BONUS) {
				bonus_count++;
				QFont f = user_item->font();
				user_item->setForeground(Qt::red);
				f.setBold(true);
				user_item->setFont(f);
			}

			QStandardItem* fabitem;
			if (factories > 0)
				fabitem = new QStandardItem(QString("%1").arg(factories));
			else
				fabitem = new QStandardItem();

			BonusLevel bl = user->hasBonus(product);
			if (bl == e_NOT_CONQUERED || bl == e_NEEDS_RESEARCH) {
				almost_bonus_count++;
				QFont f = user_item->font();
				user_item->setForeground(Qt::blue);
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

	if (bonus_count > 0)
		str +=  QString("%1%2 bonus").arg(str.isEmpty()?"":" - ").arg(bonus_count);

	if (almost_bonus_count > 0)
		str += QString("%1%2 snart bonus").arg(str.isEmpty()?"":" - ").arg(almost_bonus_count);

	productStatusItem->setData(str, Qt::DisplayRole);
	QFont f = productStatusItem->font();
	f.setBold(true);
	productStatusItem->setFont(f);
}


void FoeOverviewModel::setupProductTooltip(const FoeProduct *product, QStandardItem *productItem)
{
	FoeUser* user;

	// Tooltip
	QSet<FoeUser*> userSet = _data.getUsersForProduct(product);
	QSet<const FoeProduct*> productSet;
	foreach (user, userSet) {
		productSet += user->getProducts();
	}

	QString text = "<table><tr><td></td>";
	foreach (product, productSet) {
		text += QString("<td><center><img src='%1'><br>&nbsp;%2&nbsp;</center></td>").arg(product->iconFile()).arg(product->name());
	}
	text += "</tr>";

	foreach (user, userSet) {
		text += QString("<tr><td>%1</td>").arg(user->name());
		foreach (product, productSet) {
			BonusLevel bl = user->hasBonus(product);
			int factories = user->hasProduct(product);
			QString factories_text;
			if (bl > e_NO_BONUS)
				factories_text = "-";
			if (factories>0)
				factories_text = QString("%1").arg(factories);
			text += QString("<td><font color='%1'><center>%2</center></font></td>").arg(FoeProduct::bonusColorHTML(bl)).arg(factories_text);
		}
		text += "</tr>";
	}
	text += "</table>";

	if (!productSet.empty() && !userSet.empty())
		productItem->setToolTip(text);
	else
		productItem->setToolTip("Ingen har bonus på, eller producerer denne vare.");
}


void FoeOverviewModel::setupUserTooltip(FoeUser *user, QStandardItem *userItem)
{
	QString text = "<table>";
	const QMap<const FoeProduct *, BonusLevel>& allBonus  = user->allBonus();
	const QList<const FoeProduct *> &products = FoeProduct::getProducts();
	const FoeProduct* product;
	foreach (product, products) {

		BonusLevel bl = e_NO_BONUS;
		if (allBonus.contains(product))
			bl = allBonus[product];
		int factories = user->hasProduct(product);
		if (factories>0 || bl > e_NO_BONUS) {

			text += "<tr>";
			text += QString("<td>%1</td>").arg(product->name());
			text += "<td>";
			if (factories>0)
				text += QString("%1").arg(factories);
			text += "</td>";
			text += QString("<td><font color='%1'>%2</font></td>").arg(FoeProduct::bonusColorHTML(bl)).arg(FoeProduct::bonusText(bl));
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
	QList<const FoeProduct *> productList = FoeProduct::getProducts();
	const FoeProduct* product;
	foreach (product, productList) {
		populate_product(product);
	}

	FoeUser* user;
	QList<FoeUser*> users = _data.getFoeUsers();

	QMap<const FoeProduct*, int> m;

	productList =  FoeProduct::getProducts();
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

		productList = FoeProduct::getProductsForAge(age);
//		int factories = 0;
		QString str;
		foreach (product, productList) {
			str += QString(product->name() + ":%1   ").arg( m[product] );
		}
		_age2item[age]->setText(str);
	}

}
