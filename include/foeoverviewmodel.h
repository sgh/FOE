#ifndef FOEOVERVIEWMODEL_H
#define FOEOVERVIEWMODEL_H

#include <memory>

#include <QAbstractItemModel>
#include <QStandardItem>

class FoeGoods;
class FoeClan;
class FoeAge;
class FoeUser;

class FoeOverviewModel : public QStandardItemModel
{
	Q_OBJECT
	QMap<const FoeGoods*, QVector<QStandardItem*> > _product2item;
	QMap<QStandardItem*, std::shared_ptr<FoeUser>> _item2user;
	QMap<std::shared_ptr<FoeAge>, QStandardItem*> _age2item;

	FoeClan* _clan;

	void populate_toplevel();
	void populate_product(const FoeGoods *product);

	void setupProductTooltip(const FoeGoods *product, QStandardItem* productItem);
	void setupUserTooltip(FoeUser *user, QStandardItem* userItem);

private slots:
	void rowsAboutToBeRemoved ( const QModelIndex & parent, int start, int end );

public:
	FoeOverviewModel(FoeClan* clan);

public slots:
	void updateOverview();
};

#endif // FOEOVERVIEWMODEL_H
