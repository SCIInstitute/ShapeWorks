#ifndef MEANDIALOG_H
#define MEANDIALOG_H
#include <QDialog>

#include "ui_SamplesDialog.h"

class SamplesDialog : public QDialog, public Ui::SamplesDialog
{

	Q_OBJECT
	public:
		SamplesDialog(QWidget *parent =0);
};
#endif

