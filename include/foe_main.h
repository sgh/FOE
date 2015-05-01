#ifndef FOE_MAIN_H
#define FOE_MAIN_H

#include <memory>

#include <QMainWindow>
#include <QModelIndex>

namespace Ui {
class FOE_Main;
class FOE_Clan;
}

class FoeClan;
class FoeGoods;
class FoeDataManager;
class FoeOverviewModel;
class FoePersistence;
class PusherHandler;

class FOE_Main : public QMainWindow
{
	Q_OBJECT

	std::shared_ptr<FoeDataManager> _data;
	std::shared_ptr<FoePersistence> _persist;
	std::shared_ptr<PusherHandler>  _pusherHandler;

	FoeClan* currentClan();
	Ui::FOE_Clan* currentClanui();
	void readSettings();
	void writeSettings();
	void updateUserCount(Ui::FOE_Clan* clanui);
	void updatebuttons();

public:
	explicit FOE_Main(QWidget *parent = 0);
	~FOE_Main();

private slots:
	void userDoubleClicked(const QModelIndex &index);
	void on_addUserButton_clicked();
	void on_deleteUserButton_clicked();
	void on_actionOpen_triggered();
	void on_actionNew_triggered();
	void on_actionData_sharing_triggered();
	void userlistChanged();
	void fileChanged(const QString& filename);
	void clanAdded(FoeClan* clan);
	void clanRemoved(FoeClan* clan);
	void clanRenamed(FoeClan* clan);
	void on_addClanButton_clicked();
	void on_removeClanButton_clicked();
	void on_renameClanButton_clicked();
	void splitterMoved(int pos, int index);

private:
	void setupTitle(const QString& filename);

private:
	std::unique_ptr<Ui::FOE_Main> _ui;
	QMap<QWidget*, FoeClan*> _widget2clan;
	QMap<FoeClan*, QWidget*> _clan2widget;
	QMap<QWidget*, Ui::FOE_Clan*> _widget2clanui;
};

#endif // FOE_MAIN_H
