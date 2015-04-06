#include "datasharingdlg.h"
#include "ui_datasharingdlg.h"

DataSharingDlg::DataSharingDlg(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DataSharingDlg)
{
	ui->setupUi(this);
}

DataSharingDlg::~DataSharingDlg()
{
	delete ui;
}
