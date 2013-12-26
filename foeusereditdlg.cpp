#include <iostream>

#include <QGridLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QComboBox>

#include "foeproduct.h"
#include "foeusereditdlg.h"
#include "ui_foeusereditdlg.h"

using namespace std;


void FoeUserEditDlg::populate(QWidget* parent, FoeAge* age) {
	struct PerIDData data;

	const QList<const FoeProduct *> &list = FoeProduct::getProductsForAge(age);
	if (!parent) {
		parent = new QWidget(this);
		ui->toolBox->addItem(parent,"");
	}

	data.toolboxIndex = ui->toolBox->indexOf(parent);
	data.age = age;

	const FoeProduct* product;
	QGridLayout* layout = new QGridLayout(parent);
	int row=0;

	foreach (product, list) {
		int col = 0;
		QLabel* label = new QLabel(product->name());
		QSpinBox* sb = new QSpinBox(this);
		data.factories = sb;
		sb->setProperty("ID", product->id());
		int fabcount = _user->hasProduct(product);
		if (fabcount > 0)
			ui->toolBox->setCurrentWidget(parent);
		sb->setValue(fabcount);
		connect(sb, SIGNAL(valueChanged(int)), SLOT(factories_changed(int)));
		QLabel* iconlabel = new QLabel();
		iconlabel->setPixmap(product->pixmap());
		layout->addWidget(iconlabel,row, col++);
		layout->addWidget(label,row, col++);
		layout->addWidget(sb,  row,  col++);

		QComboBox* cb = new QComboBox(this);
		data.bonus_combo = cb;
		cb->setProperty("ID", product->id());
		cb->setModel(&_bonusModel);
		layout->addWidget(cb,row, col++);
		cb->setCurrentIndex(_user->hasBonus(product));
		connect(cb, SIGNAL(currentIndexChanged(int)), SLOT(bonus_changed(int)));

		_checkboxlist[product->id()] = data;
		row++;
	}
	layout->setRowStretch(row, 99);

	updateCounts(data.toolboxIndex, data.age);
}

void FoeUserEditDlg::updateCounts(int index, FoeAge* age) {
	int total_product = 0;
	int total_bonus = 0;
	QString title = age->name();
	const QList<const FoeProduct *> &list = FoeProduct::getProductsForAge(age);

	const FoeProduct* product;
	foreach (product, list) {
		if (_user->hasProduct(product))
			total_product ++;
		if (_user->hasBonus(product))
			total_bonus ++;
	}

	if (total_product>0 || total_bonus>0)
		title += QString(" (%1+%2)").arg(total_product).arg(total_bonus);

	ui->toolBox->setItemText(index, title);
}

FoeUserEditDlg::FoeUserEditDlg(FoeUser *user, QWidget *parent) :
	QDialog(parent),
	_user(user),
	ui(new Ui::FoeUserEditDlg)
{
	ui->setupUi(this);
	ui->username_label->setText(_user->name());

	ui->toolBox->setCurrentIndex(0);

	_bonusModel.setStringList(FoeProduct::bonusTexts());

	const QVector<FoeAge*>& ageList = FoeAge::getAges();

	FoeAge* age;
	int idx = 0;
	foreach (age, ageList) {
		populate( ui->toolBox->widget(idx),  age);
		idx++;
	}
}


FoeUserEditDlg::~FoeUserEditDlg() {
	delete ui;
	cout << "ERASE" << endl;
}


void FoeUserEditDlg::factories_changed(int factories) {
	enum e_Products id = (enum e_Products)sender()->property("ID").toInt();
	const FoeProduct* product = FoeProduct::fromId(id);

	_user->setProduct(factories, product);
	updateCounts(_checkboxlist[id].toolboxIndex, _checkboxlist[id].age);

	_checkboxlist[id].factories->blockSignals(true);
	_checkboxlist[id].factories->setValue(_user->hasProduct(product));
	_checkboxlist[id].factories->blockSignals(false);
}

void FoeUserEditDlg::bonus_changed(int idx)
{
	enum e_Products id = (enum e_Products)sender()->property("ID").toInt();
	const FoeProduct* product = FoeProduct::fromId(id);
	_user->setBonus((BonusLevel)idx, product);
	updateCounts(_checkboxlist[id].toolboxIndex, _checkboxlist[id].age);


	_checkboxlist[id].bonus_combo->blockSignals(true);
	_checkboxlist[id].bonus_combo->setCurrentIndex(_user->hasBonus(product));
	_checkboxlist[id].bonus_combo->blockSignals(false);

}


