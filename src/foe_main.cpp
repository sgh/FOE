#include <iostream>

#include <QMessageBox>
#include <QInputDialog>
#include <QToolTip>
#include <QHelpEvent>
#include <QHelpEvent>
#include <QFileDialog>

#include "foeoverviewmodel.h"
#include "foedatamanger.h"
#include "foe_main.h"
#include "foeusereditdlg.h"
#include "datasharingdlg.h"
#include "foeuser.h"
#include "foegoods.h"
#include "actionhelpers.h"
#include "remote/foepusher.h"

#include "ui_foe_main.h"
#include "ui_foe_clan.h"

using namespace std;

FoeClan*FOE_Main::currentClan()
{
	QWidget* w = _ui->tabWidget->currentWidget();
	if (w)
		return  _widget2clan[w];
	return NULL;
}

Ui::FOE_Clan* FOE_Main::currentClanui()
{
	QWidget* w = _ui->tabWidget->currentWidget();
	if (w)
		return  _widget2clanui[w];
	else
		return NULL;
}


void FOE_Main::readSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	QVariant var = settings.value("geometry");
	if (var.isValid()) {
		QRect geometry = var.toRect();
		setGeometry(geometry);
	}
	settings.endGroup();

	settings.beginGroup("Session");
	_data->loadFile(settings.value("lastfile").toString(), false);
	settings.endGroup();

}

void FOE_Main::writeSettings()
{
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("geometry", geometry());
	settings.endGroup();

	settings.beginGroup("Session");
	settings.setValue("lastfile", _data->currentFile());
	settings.endGroup();

}


FOE_Main::FOE_Main(QWidget *parent)
	: QMainWindow(parent)
	, _data(NULL)
	, _ui(new Ui::FOE_Main)
{
	_ui->setupUi(this);
	setupTitle("");

	FoeAge::initialize();
	FoeGoods::initialize();

	QCoreApplication::setOrganizationName("SGH Software");
	QCoreApplication::setOrganizationDomain("sgh.dk");
	QCoreApplication::setApplicationName("FOE ClanManager");
	QCoreApplication::setApplicationVersion("0.4.1");

	// Setup data
	_persist = new FoePersistence();
	_data  = new FoeDataManager(*_persist);

	connect( _data, &FoeDataManager::fileChanged, this, &FOE_Main::fileChanged);
	connect( _data, &FoeDataManager::clanAdded,   this, &FOE_Main::clanAdded);
	connect( _data, &FoeDataManager::clanAboutToBeRemoved, this, &FOE_Main::clanRemoved);
	connect( _data, &FoeDataManager::clanRenamed, this, &FOE_Main::clanRenamed);

	_ui->mainToolBar->hide();

	updatebuttons();
	readSettings();

	_pusherHandler = new PusherHandler(*_persist, *_data);
	Actions::setPusher(_pusherHandler);
}


FOE_Main::~FOE_Main() {
	writeSettings();
	delete _pusherHandler;
	delete _data;
	delete _ui;
	FoeGoods::deinitialize();
	FoeAge::deinitialize();
}


void FOE_Main::userDoubleClicked(const QModelIndex &index)
{
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
		Actions::addUser(_data, currentClan(), new_username);
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
			Actions::removeUser(_data, currentClan(), user);
	}
}

void FOE_Main::on_actionOpen_triggered()
{
	QFileDialog dlg;
	dlg.setFileMode(QFileDialog::ExistingFile);
	dlg.selectNameFilter("*.sqlite");
	if (dlg.exec()) {
		QString f = dlg.selectedFiles()[0];
		_data->loadFile(f, false);
	}
	updatebuttons();
}

void FOE_Main::on_actionNew_triggered() {
	QFileDialog dlg;
	dlg.setFileMode(QFileDialog::AnyFile);
	dlg.setAcceptMode(QFileDialog::AcceptSave);
	dlg.selectNameFilter("*.sqlite");
	dlg.setDefaultSuffix(".sqlite");
	if (dlg.exec()) {
		QString f = dlg.selectedFiles()[0];
		_data->loadFile(f, true);
	}
	updatebuttons();
}


void FOE_Main::on_actionData_sharing_triggered() {
	_pusherHandler->setup();
	DataSharingDlg dlg(_pusherHandler, _persist);
	dlg.exec();
}


void FOE_Main::updateUserCount(Ui::FOE_Clan *clanui) {
	if (!clanui)
		return;
	clanui->usercount->setText(QString("(%1)").arg(clanui->listView->model()->rowCount()));
}


void FOE_Main::updatebuttons()
{
	bool b_clanValid = currentClan() != NULL;

	_ui->deleteUserButton->setEnabled( b_clanValid );
	_ui->addUserButton->setEnabled(    b_clanValid );
	_ui->addUserButton->setEnabled(    b_clanValid );
	_ui->deleteUserButton->setEnabled( b_clanValid );

	_ui->addClanButton->setEnabled(   _data->isValid() );
	_ui->removeClanButton->setEnabled( b_clanValid );
	_ui->renameClanButton->setEnabled( b_clanValid );
}


void FOE_Main::userlistChanged()
{
	updateUserCount(currentClanui());
}

void FOE_Main::fileChanged(const QString& filename) {
	writeSettings();
	setupTitle(filename);
}


void FOE_Main::setupTitle(const QString& filename)
{
	if (filename.isEmpty())
		setWindowTitle(QCoreApplication::applicationName());
	else
		setWindowTitle(QCoreApplication::applicationName() + " - " + QFileInfo(filename).baseName());
}


void FOE_Main::clanAdded(FoeClan* clan)
{
	cerr << "Clan \"" << clan->name().toStdString() <<  "\" added." << endl;
	QWidget* w = new QWidget();
	_ui->tabWidget->addTab(w, clan->name());

	Ui::FOE_Clan* clanui = new Ui::FOE_Clan;
	clanui->setupUi(w);
	connect( clanui->listView, &QListView::doubleClicked, this, &FOE_Main::userDoubleClicked);

	// Watch model changes
	connect(clan->userModel(), &QStringListModel::modelReset, this, &FOE_Main::userlistChanged);

	clanui->listView->setModel(clan->userModel());
	clanui->overview->setModel(clan->getOverviewModel());
	clanui->overview->setColumnWidth(0, 300);
	connect( clanui->splitter, &QSplitter::splitterMoved, this, &FOE_Main::splitterMoved );
	_widget2clan[w] = clan;
	_clan2widget[clan] = w;
	_widget2clanui[w] = clanui;
	updateUserCount(clanui);
	_ui->tabWidget->setCurrentWidget(w);

	QSettings settings;
	bool ok;
	settings.beginGroup("MainWindow");
	double userlistpercentage = settings.value("splitter").toDouble(&ok);
	if (!ok)
		userlistpercentage = 0.30;

	QList<int> sizes;
	sizes << userlistpercentage*1000 << 1000;
	clanui->splitter->setSizes(sizes);


	updatebuttons();
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
	updatebuttons();
}


void FOE_Main::clanRenamed(FoeClan* clan)
{
	QWidget* w = _clan2widget[clan];
	int index = _ui->tabWidget->indexOf(w);
	_ui->tabWidget->setTabText(index, clan->name());
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
		if (!_data->renameClan(currentClan(), new_clanname))
			QMessageBox::warning(this, title, QString("Unable to rename the clan %1.").arg(new_clanname), QMessageBox::Ok);
	} else {
		QMessageBox::warning(this, title, QString("The clan %1 already exists.").arg(new_clanname), QMessageBox::Ok);
	}
}


void FOE_Main::splitterMoved(int pos, int index)
{
	Q_UNUSED(pos);
	Q_UNUSED(index);
	Ui::FOE_Clan* current = currentClanui();
	QSettings settings;
	settings.beginGroup("MainWindow");
	settings.setValue("splitter", current->splitter->sizes().at(0) / (double)current->splitter->sizes().at(1));
	settings.endGroup();

	Ui::FOE_Clan* clanui;
	foreach (clanui, _widget2clanui) {
		clanui->splitter->blockSignals(true);
		clanui->splitter->setSizes( current->splitter->sizes() );
		clanui->splitter->blockSignals(false);
	}
}
