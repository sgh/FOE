#include "foeconnectiondetails.h"
#include "ui_foeconnectiondetails.h"

#include <foedatamanger.h>

FoeConnectionDetails::FoeConnectionDetails(FoeDataManager* data, QWidget *parent)
	: QDialog(parent)
    , _data(data)
	, ui(new Ui::FoeConnectionDetails)
{
	ui->setupUi(this);
	ui->username->setText(   _data->getDbUsername() );
	ui->password->setText(   _data->getDbPassword() );
	ui->database->setText(   _data->getDbName()     );
	ui->servername->setText( _data->getServerName() );
}

FoeConnectionDetails::~FoeConnectionDetails()
{
	delete ui;
}

void FoeConnectionDetails::on_buttonBox_accepted()
{
	_data->setDbUsername( ui->username->text()   );
	_data->setDbPassword( ui->password->text()   );
	_data->setDbName(     ui->database->text()   );
	_data->setServerName( ui->servername->text() );
}
