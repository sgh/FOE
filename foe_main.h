#ifndef FOE_MAIN_H
#define FOE_MAIN_H

#include <QMainWindow>
#include <QModelIndex>

namespace Ui {
class FOE_Main;
}

class FoeProduct;
class FoeDataManager;
class FoeOverviewModel;

class FOE_Main : public QMainWindow
{
	Q_OBJECT
	FoeOverviewModel* _model;
	FoeDataManager* _data;
	bool _b_connected;

	void readSettings();
	void writeSettings();

public:
	explicit FOE_Main(QWidget *parent = 0);
	~FOE_Main();

private slots:
	void on_listView_doubleClicked(const QModelIndex &index);
	void on_addUserButton_clicked();
	void on_deleteUserButton_clicked();
	void on_actionForbindelse_triggered();

private:
	Ui::FOE_Main *ui;

protected:
	virtual void timerEvent(QTimerEvent *);
};

#endif // FOE_MAIN_H
