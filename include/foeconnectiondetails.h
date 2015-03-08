#ifndef FOECONNECTIONDETAILS_H
#define FOECONNECTIONDETAILS_H

#include <QDialog>

namespace Ui {
class FoeConnectionDetails;
}

class FoeDataManager;

class FoeConnectionDetails : public QDialog
{
	Q_OBJECT
	FoeDataManager* _data;

public:
	explicit FoeConnectionDetails(FoeDataManager* data, QWidget *parent = 0);
	~FoeConnectionDetails();

private slots:
	void on_buttonBox_accepted();

private:
	Ui::FoeConnectionDetails *ui;
};

#endif // FOECONNECTIONDETAILS_H
