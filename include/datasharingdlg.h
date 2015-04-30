#ifndef DATASHARINGDLG_H
#define DATASHARINGDLG_H

#include <memory>
#include <QDialog>

namespace Ui {
class DataSharingDlg;
}

class PusherHandler;
class FoePersistence;

class DataSharingDlg : public QDialog
{
	Q_OBJECT

public:
	explicit DataSharingDlg(PusherHandler* pusherHandler, FoePersistence* persist, QWidget *parent = 0);
	~DataSharingDlg();

private slots:
	void save();

private:
	std::unique_ptr<Ui::DataSharingDlg> ui;
	PusherHandler*  _pusherHandler;
	FoePersistence* _persist;
};

#endif // DATASHARINGDLG_H
