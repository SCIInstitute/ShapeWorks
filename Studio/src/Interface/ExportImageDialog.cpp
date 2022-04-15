#include <iostream>

// Qt includes
#include <Applications/Configuration.h>
#include <Data/StudioLog.h>

#include <QFileDialog>
#include <QMessageBox>

#include "ExportImageDialog.h"
#include "ui_ExportImageDialog.h"

namespace shapeworks {

//---------------------------------------------------------------------------
ExportImageDialog::ExportImageDialog(QWidget* parent, Preferences& preferences)
    : QDialog(parent), preferences_(preferences) {
  this->ui_ = new Ui_ExportImageDialog;
  this->ui_->setupUi(this);

  this->setWindowFlags(Qt::Dialog | Qt::FramelessWindowHint);
  this->setModal(true);

  // Set up the private internals of the AppSplash class
  this->setObjectName(QString::fromUtf8("Export Image Dialog"));
}

//---------------------------------------------------------------------------
ExportImageDialog::~ExportImageDialog() {}

}  // namespace shapeworks
