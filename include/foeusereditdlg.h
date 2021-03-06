#ifndef FOEUSEREDITDLG_H
#define FOEUSEREDITDLG_H

#include <memory>

#include <QVBoxLayout>
#include <QDialog>
#include <QStringListModel>

#include "foegoods.h"
#include "foeuser.h"

class QSpinBox;
class QComboBox;

namespace Ui {
class FoeUserEditDlg;
}

class FoeUserEditDlg : public QDialog
{
	Q_OBJECT
	FoeUser* _user;

	QStringListModel _boostModel;

	struct PerIDData {
		QSpinBox*     factories;
		QComboBox*    boost_combo;
		int toolboxIndex;
		FoeAge* age;
	};

	QMap<enum e_Goods, struct PerIDData> _checkboxlist;

public:
	explicit FoeUserEditDlg(FoeUser* user, QWidget *parent = 0);
	~FoeUserEditDlg();

	void populate(QWidget *parent, FoeAge *age);
	void updateCounts(int index, FoeAge *age);

private slots:
	void factories_changed(int factories);
	void boost_changed(int idx);

private:
	std::unique_ptr<Ui::FoeUserEditDlg> ui;
};

#endif // FOEUSEREDITDLG_H
