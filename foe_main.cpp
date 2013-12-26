#include <iostream>

#include <QMessageBox>
#include <QInputDialog>
#include <QToolTip>
#include <QHelpEvent>
#include <QHelpEvent>

#include "foeconnectiondetails.h"
#include "foeoverviewmodel.h"
#include "foedatamanger.h"
#include "foe_main.h"
#include "foeusereditdlg.h"
#include "foeuser.h"
#include "foeproduct.h"
#include "ui_foe_main.h"

using namespace std;

void FOE_Main::readSettings()
{
	bool ok;
	QSettings settings;
	settings.beginGroup("MainWindow");
	double userlistpercentage = settings.value("splitter").toDouble(&ok);
	if (!ok)
		userlistpercentage = 0.30;

	QList<int> sizes;
	sizes << userlistpercentage*1000 << 1000;
	ui->splitter->setSizes(sizes);

	QVariant var = settings.value("geometry");
	if (var.isValid()) {
		QRect geometry = var.toRect();
		setGeometry(geometry);
	}
}

void FOE_Main::writeSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("splitter", ui->splitter->sizes().at(0) / (double)ui->splitter->sizes().at(1));
	settings.setValue("geometry", geometry());
	settings.endGroup();
}


FOE_Main::FOE_Main(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::FOE_Main)
{
	ui->setupUi(this);

	QCoreApplication::setOrganizationName("SGH Software");
	QCoreApplication::setOrganizationDomain("sgh.com");
	QCoreApplication::setApplicationName("FOE ClanManager");

	// Setup data
	_data  = new FoeDataManager();

	_model = new FoeOverviewModel(*_data);
	connect( _data, SIGNAL(userAdded(FoeUser*)), _model, SLOT(userAdded(FoeUser*)));

	 _b_connected = false;
	 _data->connect();

	// Setup UI
	ui->listView->setModel(_data->userModel());
	ui->overview->setModel(_model);
	ui->overview->setColumnWidth(0, 300);

	ui->statusBar->hide();
	ui->mainToolBar->hide();

	readSettings();

	startTimer(1000);
}


FOE_Main::~FOE_Main() {
	writeSettings();
	delete _data;
	delete ui;
}


void FOE_Main::on_listView_doubleClicked(const QModelIndex &index)
{
	QString username = ui->listView->model()->itemData(index)[0].toString();
	FoeUser* user = _data->getFoeUser(username);
	FoeUserEditDlg* dlg = new FoeUserEditDlg(user);

	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->show();
	dlg->raise();
	dlg->activateWindow();
}


void FOE_Main::on_addUserButton_clicked()
{
	QString new_username = QInputDialog::getText(this, "Indtast brugernavn","Brugernavn").trimmed();

	if (!_data->getFoeUser(new_username)) {
		_data->addUser(new_username);
	} else {
		QMessageBox::warning(this, "Opret bruger.", QString("Brugeren %1 eksisterer allerede.").arg(new_username), QMessageBox::Ok);
	}
}


void FOE_Main::on_deleteUserButton_clicked()
{
	QString username = ui->listView->model()->itemData(ui->listView->currentIndex())[0].toString();
	if (QMessageBox::Yes == QMessageBox::question(this, "Slet bruger.", QString("Vil du slette %1?").arg(username), QMessageBox::Yes, QMessageBox::No)) {
		FoeUser* user = _data->getFoeUser(username);
		if (user)
			_data->removeUser(user);

	}
}

void FOE_Main::on_actionForbindelse_triggered()
{
	if (FoeConnectionDetails(_data,this).exec() ==QDialog::Accepted) {
		_data->connect();
	}
}


void FOE_Main::timerEvent(QTimerEvent *)
{
	if (_b_connected != _data->isConnected()) {
		_b_connected = !_b_connected;
		ui->overview->setEnabled( _b_connected );
		ui->listView->setEnabled( _b_connected );
		ui->deleteUserButton->setEnabled( _b_connected );
		ui->addUserButton->setEnabled( _b_connected );
//		if (_b_connected)
//			_model->update();
	}
}

