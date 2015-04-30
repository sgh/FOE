#include "datasharingdlg.h"
#include "ui_datasharingdlg.h"
#include "foepersistence.h"
#include "remote/foepusher.h"

DataSharingDlg::DataSharingDlg(PusherHandler* pusherHandler, FoePersistence* persist, QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::DataSharingDlg)
	, _pusherHandler(pusherHandler)
	, _persist(persist)
{
	ui->setupUi(this);

	ui->pusher_enabled->setChecked( _persist->getBoolOption("pusher.enabled", false));
	ui->apikey_edit->setText( _persist->getStrOption("pusher.apikey", ""));
	ui->secret_edit->setText( _persist->getStrOption("pusher.secret", ""));
	connect(ui->buttonBox, &QDialogButtonBox::accepted, this, &DataSharingDlg::save);
}


DataSharingDlg::~DataSharingDlg()
{
}


void DataSharingDlg::save()
{
	_persist->setBoolOption("pusher.enabled", ui->pusher_enabled->isChecked());
	_persist->setStrOption("pusher.apikey", ui->apikey_edit->text());
	_persist->setStrOption("pusher.secret", ui->secret_edit->text());

	_pusherHandler->setup();
}
