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
#include "foegoods.h"
#include "ui_foe_main.h"
#include "ui_foe_clan.h"

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
	clanui->splitter->setSizes(sizes);

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
	settings.setValue("splitter", clanui->splitter->sizes().at(0) / (double)clanui->splitter->sizes().at(1));
	settings.setValue("geometry", geometry());
	settings.endGroup();
}


FOE_Main::FOE_Main(QWidget *parent)
	: QMainWindow(parent)
	, ui(new Ui::FOE_Main)
	, clanui(new Ui::FOE_Clan)
{
	ui->setupUi(this);
	QCoreApplication::setOrganizationName("SGH Software");
	QCoreApplication::setOrganizationDomain("sgh.dk");
	QCoreApplication::setApplicationName("FOE ClanManager");

	// Setup data
	_data  = new FoeDataManager();

	_model = new FoeOverviewModel(*_data);
	connect( _data, &FoeDataManager::userAdded,   _model, &FoeOverviewModel::userAdded);
	connect( _data, &FoeDataManager::userRemoved, _model, &FoeOverviewModel::userRemoved, Qt::QueuedConnection);

	_b_connected = false;
	_b_try_connect = true;

	// Setup UI
	ui->tabWidget->addTab(new QWidget(),"TEST");
	clanui->setupUi(ui->tabWidget->currentWidget());
	connect( clanui->listView, &QListView::doubleClicked, this, &FOE_Main::userDoubleClicked);

	clanui->listView->setModel(_data->userModel());
	clanui->overview->setModel(_model);
	clanui->overview->setColumnWidth(0, 300);

	// Watch model changes
	connect(_data->userModel(), &QStringListModel::modelReset, this, &FOE_Main::userlistChanged);

	ui->statusBar->hide();
	ui->mainToolBar->hide();
	clanui->listView->setEnabled(false);
	ui->addUserButton->setEnabled(false);
	ui->deleteUserButton->setEnabled(false);
	ui->addUserButton->setVisible( false );
	ui->deleteUserButton->setVisible( false );

	readSettings();

	startTimer(1000);
}


FOE_Main::~FOE_Main() {
	writeSettings();
	delete _data;
	delete ui;
}


void FOE_Main::userDoubleClicked(const QModelIndex &index)
{
	if (!_data->hasInsertPrivileges())
		return;

	QString username = clanui->listView->model()->itemData(index)[0].toString();
	FoeUser* user = _data->getFoeUser(username);
	FoeUserEditDlg* dlg = new FoeUserEditDlg(user);

	dlg->setAttribute(Qt::WA_DeleteOnClose);
	dlg->show();
	dlg->raise();
	dlg->activateWindow();
}


void FOE_Main::on_addUserButton_clicked()
{
	bool ok;
	QString title = tr("Create user.");
	QString new_username = QInputDialog::getText(this, "Enter user name", "User name", QLineEdit::Normal, "", &ok).trimmed();

	if (!ok)
		return;

	if (new_username.isEmpty()) {
		QMessageBox::critical(this, title, QString("Invalid user name."), QMessageBox::Ok);
	}

	if (!_data->getFoeUser(new_username)) {
		_data->addUser(new_username);
	} else {
		QMessageBox::warning(this, title, QString("The user %1 already exists.").arg(new_username), QMessageBox::Ok);
	}
}


void FOE_Main::on_deleteUserButton_clicked()
{
	QString username = clanui->listView->model()->itemData(clanui->listView->currentIndex())[0].toString();
	if (QMessageBox::Yes == QMessageBox::question(this, tr("Delete user"), QString(tr("Do you want to delete %1?")).arg(username), QMessageBox::Yes, QMessageBox::No)) {
		FoeUser* user = _data->getFoeUser(username);
		if (user)
			_data->removeUser(user);

	}
}

void FOE_Main::on_actionForbindelse_triggered()
{
	if (FoeConnectionDetails(_data,this).exec() ==QDialog::Accepted) {
		_b_try_connect = true;
		_data->disconnect();
	}
}

void FOE_Main::userlistChanged()
{
	clanui->usercount->setText(QString("(%1)").arg(clanui->listView->model()->rowCount()));
}


void FOE_Main::timerEvent(QTimerEvent *)
{
	if (!_b_connected && _b_try_connect) {
		_b_try_connect = false;
		_data->connect();
	}

	if (_b_connected != _data->isConnected()) {
		_b_connected = !_b_connected;
		clanui->overview->setEnabled( _b_connected );
		clanui->listView->setEnabled( _b_connected );
		ui->deleteUserButton->setEnabled( _b_connected );
		ui->addUserButton->setEnabled( _b_connected );
		clanui->listView->setEnabled( _b_connected );
		ui->addUserButton->setEnabled( _b_connected );
		ui->deleteUserButton->setEnabled( _b_connected );

		if (_data->hasInsertPrivileges()) {
			ui->addUserButton->setVisible( true );
			ui->deleteUserButton->setVisible( true );
		} else {
			ui->addUserButton->setVisible( false );
			ui->deleteUserButton->setVisible( false );
		}
	}
}

