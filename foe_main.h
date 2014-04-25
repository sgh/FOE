#ifndef FOE_MAIN_H
#define FOE_MAIN_H

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

class FOE_Main : public QMainWindow
{
	Q_OBJECT

	FoeDataManager* _data;
	bool _b_connected;
	bool _b_try_connect;

	FoeClan* currentClan();
	Ui::FOE_Clan* currentClanui();
	void readSettings();
	void writeSettings();
	void updateUserCount(Ui::FOE_Clan* clanui);

public:
	explicit FOE_Main(QWidget *parent = 0);
	~FOE_Main();

private slots:
	void userDoubleClicked(const QModelIndex &index);
	void on_addUserButton_clicked();
	void on_deleteUserButton_clicked();
	void on_actionForbindelse_triggered();
	void userlistChanged();
	void clanAdded(FoeClan* clan);
	void clanRemoved(FoeClan* clan);
	void clanRenamed(FoeClan* clan);
	void on_addClanButton_clicked();
	void on_removeClanButton_clicked();

	void on_renameClanButton_clicked();

private:
	Ui::FOE_Main *_ui;
	QMap<QWidget*, FoeClan*> _widget2clan;
	QMap<FoeClan*, QWidget*> _clan2widget;
	QMap<QWidget*, Ui::FOE_Clan*> _widget2clanui;

protected:
	virtual void timerEvent(QTimerEvent *);
};

#endif // FOE_MAIN_H
