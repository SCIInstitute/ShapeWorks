#ifndef PCADIALOG_H
#define PCADIALOG_H
#include <QDialog>

#include "ui_PCADialog.h"

class PCADialog : public QDialog, public Ui::PCADialog
{

	Q_OBJECT
	public:
		PCADialog(QWidget *parent =0);
};
#endif

