#ifndef FOEOVERVIEWMODEL_H
#define FOEOVERVIEWMODEL_H

#include <QAbstractItemModel>
#include <QStandardItem>

#include "foedatamanger.h"


class FoeOverviewModel : public QStandardItemModel
{
	Q_OBJECT
	QMap<const FoeGoods*, QVector<QStandardItem*> > _product2item;
	QMap<QStandardItem*, FoeUser*> _item2user;
	QMap<FoeAge*, QStandardItem*> _age2item;

	FoeDataManager& _data;

	void populate_toplevel();
	void populate_product(const FoeGoods *product);

	void setupProductTooltip(const FoeGoods *product, QStandardItem* productItem);
	void setupUserTooltip(FoeUser *user, QStandardItem* userItem);

private slots:
	void rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );

public:
	FoeOverviewModel(FoeDataManager& data);

public slots:
	void userAdded(FoeUser*user);
	void userRemoved(FoeUser*user);
	void update();
};

#endif // FOEOVERVIEWMODEL_H
