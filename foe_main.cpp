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

FoeClan*FOE_Main::currentClan()
{
	return  _widget2clan[_ui->tabWidget->currentWidget()];
}

Ui::FOE_Clan* FOE_Main::currentClanui()
{
	return  _widget2clanui[_ui->tabWidget->currentWidget()];
}


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
	if (currentClanui())
		currentClanui()->splitter->setSizes(sizes);

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
	if (currentClanui())
		settings.setValue("splitter", currentClanui()->splitter->sizes().at(0) / (double)currentClanui()->splitter->sizes().at(1));
	settings.setValue("geometry", geometry());
	settings.endGroup();
}


FOE_Main::FOE_Main(QWidget *parent)
	: QMainWindow(parent)
	, _ui(new Ui::FOE_Main)
{
	_ui->setupUi(this);
	QCoreApplication::setOrganizationName("SGH Software");
	QCoreApplication::setOrganizationDomain("sgh.dk");
	QCoreApplication::setApplicationName("FOE ClanManager");

	// Setup data
	_data  = new FoeDataManager();

	connect( _data, &FoeDataManager::clanAdded,   this, &FOE_Main::clanAdded);
	connect( _data, &FoeDataManager::clanRemoved, this, &FOE_Main::clanRemoved);
	connect( _data, &FoeDataManager::clanRenamed, this, &FOE_Main::clanRenamed);

	_b_connected = false;
	_b_try_connect = true;

	_ui->statusBar->hide();
	_ui->mainToolBar->hide();
//	clanui->listView->setEnabled(false);
	_ui->addUserButton->setEnabled(false);
	_ui->deleteUserButton->setEnabled(false);
	_ui->addUserButton->setVisible( false );
	_ui->deleteUserButton->setVisible( false );

	readSettings();

	startTimer(1000);
}


FOE_Main::~FOE_Main() {
	writeSettings();
	delete _data;
	delete _ui;
}


void FOE_Main::userDoubleClicked(const QModelIndex &index)
{
	if (!_data->hasInsertPrivileges())
		return;

	QString username = currentClanui()->listView->model()->itemData(index)[0].toString();
	FoeUser* user = currentClan()->getFoeUser(username);
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

	if (!currentClan()->getFoeUser(new_username)) {
		_data->addUser(currentClan(), new_username);
	} else {
		QMessageBox::warning(this, title, QString("The user %1 already exists.").arg(new_username), QMessageBox::Ok);
	}
}


void FOE_Main::on_deleteUserButton_clicked()
{
	QString username = currentClanui()->listView->model()->itemData(currentClanui()->listView->currentIndex())[0].toString();
	if (QMessageBox::Yes == QMessageBox::question(this, tr("Delete user"), QString(tr("Do you want to delete %1?")).arg(username), QMessageBox::Yes, QMessageBox::No)) {
		FoeUser* user = currentClan()->getFoeUser(username);
		if (user)
			_data->removeUser(currentClan(), user);

	}
}

void FOE_Main::on_actionForbindelse_triggered()
{
	if (FoeConnectionDetails(_data,this).exec() ==QDialog::Accepted) {
		_b_try_connect = true;
		_data->disconnect();
	}
}

void FOE_Main::updateUserCount(Ui::FOE_Clan *clanui) {
	clanui->usercount->setText(QString("(%1)").arg(clanui->listView->model()->rowCount()));
}


void FOE_Main::userlistChanged()
{
	updateUserCount(currentClanui());
}


void FOE_Main::clanAdded(FoeClan* clan)
{
	cerr << "Clan \"" << clan->name().toStdString() <<  "\" added." << endl;
	QWidget* w = new QWidget();
	_ui->tabWidget->addTab(w, clan->name());

	Ui::FOE_Clan* clanui = new Ui::FOE_Clan;
	clanui->setupUi(w);
	connect( clanui->listView, &QListView::doubleClicked, this, &FOE_Main::userDoubleClicked);

	connect( _data, &FoeDataManager::userAdded,   clan->getOverviewModel(), &FoeOverviewModel::userAdded);
	connect( _data, &FoeDataManager::userRemoved, clan->getOverviewModel(), &FoeOverviewModel::userRemoved, Qt::QueuedConnection);

	// Watch model changes
	connect(clan->userModel(), &QStringListModel::modelReset, this, &FOE_Main::userlistChanged);

	clanui->listView->setModel(clan->userModel());
	clanui->overview->setModel(clan->getOverviewModel());
	clanui->overview->setColumnWidth(0, 300);
	_widget2clan[w] = clan;
	_clan2widget[clan] = w;
	_widget2clanui[w] = clanui;
	updateUserCount(clanui);
}


void FOE_Main::clanRemoved(FoeClan* clan)
{
	QWidget* w = _ui->tabWidget->currentWidget();
	_widget2clan.remove(w);
	_widget2clanui.remove(w);
	_clan2widget.remove(clan);
	int index = _ui->tabWidget->indexOf(w);
	_ui->tabWidget->removeTab(index);
	cerr << "Clan \"" << clan->name().toStdString() <<  "\" removed." << endl;
}


void FOE_Main::clanRenamed(FoeClan* clan)
{
	QWidget* w = _clan2widget[clan];
	int index = _ui->tabWidget->indexOf(w);
	_ui->tabWidget->setTabText(index, clan->name());
}


void FOE_Main::timerEvent(QTimerEvent *)
{
	if (!_b_connected && _b_try_connect) {
		_b_try_connect = false;
		_data->connect();
	}

	if (_b_connected != _data->isConnected()) {
		_b_connected = !_b_connected;
		if (currentClanui()) {
			currentClanui()->overview->setEnabled( _b_connected );
			currentClanui()->listView->setEnabled( _b_connected );
			currentClanui()->listView->setEnabled( _b_connected );
		}
		_ui->deleteUserButton->setEnabled( _b_connected );
		_ui->addUserButton->setEnabled( _b_connected );
		_ui->addUserButton->setEnabled( _b_connected );
		_ui->deleteUserButton->setEnabled( _b_connected );

		if (_data->hasInsertPrivileges()) {
			_ui->addUserButton->setVisible( true );
			_ui->deleteUserButton->setVisible( true );
		} else {
			_ui->addUserButton->setVisible( false );
			_ui->deleteUserButton->setVisible( false );
		}
	}
}


void FOE_Main::on_addClanButton_clicked()
{
	bool ok;
	QString title = tr("Add clan.");
	QString new_clanname = QInputDialog::getText(this, "Enter clan name", "Clan name", QLineEdit::Normal, "", &ok).trimmed();

	if (!ok)
		return;

	if (new_clanname.isEmpty()) {
		QMessageBox::critical(this, title, QString("Invalid clan name."), QMessageBox::Ok);
	}

	if (!_data->getClan(new_clanname)) {
		_data->addClan(new_clanname);
	} else {
		QMessageBox::warning(this, title, QString("The clan %1 already exists.").arg(new_clanname), QMessageBox::Ok);
	}
}

void FOE_Main::on_removeClanButton_clicked()
{
	QString clanname = currentClan()->name();
	if (QMessageBox::Yes == QMessageBox::question(this, tr("Delete clan"), QString(tr("Do you want to delete the clan %1?")).arg(clanname), QMessageBox::Yes, QMessageBox::No)) {
		_data->removeClan(currentClan());
	}
}


void FOE_Main::on_renameClanButton_clicked()
{
	bool ok;
	QString title = tr("Rename clan.");
	QString new_clanname = QInputDialog::getText(this, "Enter new clan name", "New clan name", QLineEdit::Normal, currentClan()->name(), &ok).trimmed();

	if (!ok)
		return;

	if (new_clanname.isEmpty()) {
		QMessageBox::critical(this, title, QString("Invalid new clan name."), QMessageBox::Ok);
	}

	if (new_clanname == currentClan()->name())
		return;

	if (!_data->getClan(new_clanname)) {
		_data->renameClan(currentClan(), new_clanname);
	} else {
		QMessageBox::warning(this, title, QString("The clan %1 already exists.").arg(new_clanname), QMessageBox::Ok);
	}
}
