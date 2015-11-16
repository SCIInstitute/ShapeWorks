#ifndef COLORDIALOG_H
#define COLORDIALOG_H
#include <QDialog>

#include "ui_ColorDialog.h"

class ColorDialog : public QDialog, public Ui::ColorDialog
{

	Q_OBJECT
	public:
		ColorDialog(QWidget *parent =0);
};
#endif

