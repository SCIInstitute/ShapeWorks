#include <iostream>

// Qt includes
#include <Applications/Configuration.h>

#include <QFileDialog>
#include <QMessageBox>

#include "KeyboardShortcuts.h"
#include "ui_KeyboardShortcuts.h"

namespace shapeworks {

//---------------------------------------------------------------------------
KeyboardShortcuts::KeyboardShortcuts(QWidget* parent) : QDialog(parent) {
  this->ui_ = new Ui_KeyboardShortcuts;
  this->ui_->setupUi(this);

  this->setWindowFlags(Qt::Dialog);
  this->setModal(true);

  // Set up the private internals of the AppSplash class
  this->setObjectName(QString::fromUtf8("keyboard_shortcuts"));

  connect(this->ui_->close, &QPushButton::clicked, this, &KeyboardShortcuts::close);
}

//---------------------------------------------------------------------------
KeyboardShortcuts::~KeyboardShortcuts() {}

}  // namespace shapeworks
