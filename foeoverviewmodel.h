#ifndef FOEOVERVIEWMODEL_H
#define FOEOVERVIEWMODEL_H

#include <QAbstractItemModel>
#include <QStandardItem>

#include "foedatamanger.h"


class FoeOverviewModel : public QStandardItemModel
{
	Q_OBJECT
	QMap<const FoeProduct*, QVector<QStandardItem*> > _product2item;
	QMap<QStandardItem*, FoeUser*> _item2user;
	QMap<FoeAge*, QStandardItem*> _age2item;

	FoeDataManager& _data;

	void populate_toplevel();
	void populate_product(const FoeProduct *product);

	void setupProductTooltip(const FoeProduct *product, QStandardItem* productItem);
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
