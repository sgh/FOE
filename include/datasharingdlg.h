#ifndef DATASHARINGDLG_H
#define DATASHARINGDLG_H

#include <QDialog>

namespace Ui {
class DataSharingDlg;
}

class DataSharingDlg : public QDialog
{
	Q_OBJECT

public:
	explicit DataSharingDlg(QWidget *parent = 0);
	~DataSharingDlg();

private:
	Ui::DataSharingDlg *ui;
};

#endif // DATASHARINGDLG_H
