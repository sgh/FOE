#include <iostream>

#include <QGridLayout>
#include <QCheckBox>
#include <QRadioButton>
#include <QSpinBox>
#include <QComboBox>

#include "foegoods.h"
#include "foeusereditdlg.h"
#include "ui_foeusereditdlg.h"

using namespace std;


void FoeUserEditDlg::populate(QWidget* parent, FoeAge* age) {
	struct PerIDData data;

	const QList<const FoeGoods *> &list = FoeGoods::getGoodsForAge(age);
	if (!parent) {
		parent = new QWidget(this);
		ui->toolBox->addItem(parent,"");
	}

	data.toolboxIndex = ui->toolBox->indexOf(parent);
	data.age = age;

	const FoeGoods* product;
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
		connect(sb, static_cast<void (QSpinBox::*)(int)>(&QSpinBox::valueChanged), this, &FoeUserEditDlg::factories_changed);
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
		connect(cb, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, &FoeUserEditDlg::bonus_changed);

		_checkboxlist[product->id()] = data;
		row++;
	}
	layout->setRowStretch(row, 99);

	updateCounts(data.toolboxIndex, data.age);
}

void FoeUserEditDlg::updateCounts(int index, FoeAge* age) {
	int total_product = 0;
	int total_bonus = 0;
	QString title = age->name() + "   ";
	const QList<const FoeGoods *> &list = FoeGoods::getGoodsForAge(age);

	const FoeGoods* product;
	foreach (product, list) {
		total_product += _user->hasProduct(product);

		if (_user->hasBonus(product))
			total_bonus ++;
	}

	if (total_product>0 || total_bonus>0)
		title += tr("%1 factories (%2 with boost)").arg(total_product).arg(total_bonus);

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

	_bonusModel.setStringList(FoeGoods::bonusTexts());

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
	enum e_Goods id = (enum e_Goods)sender()->property("ID").toInt();
	const FoeGoods* product = FoeGoods::fromId(id);

	_user->setProduct(factories, product);
	updateCounts(_checkboxlist[id].toolboxIndex, _checkboxlist[id].age);

	_checkboxlist[id].factories->blockSignals(true);
	_checkboxlist[id].factories->setValue(_user->hasProduct(product));
	_checkboxlist[id].factories->blockSignals(false);
}

void FoeUserEditDlg::bonus_changed(int idx)
{
	enum e_Goods id = (enum e_Goods)sender()->property("ID").toInt();
	const FoeGoods* product = FoeGoods::fromId(id);
	_user->setBonus((BonusLevel)idx, product);
	updateCounts(_checkboxlist[id].toolboxIndex, _checkboxlist[id].age);


	_checkboxlist[id].bonus_combo->blockSignals(true);
	_checkboxlist[id].bonus_combo->setCurrentIndex(_user->hasBonus(product));
	_checkboxlist[id].bonus_combo->blockSignals(false);

}


