#pragma once

// qt
#include <QDialog>

class Ui_KeyboardShortcuts;

namespace shapeworks {

/*!
 * The KeyboardShortcuts is a QDialog that implements the dialog for displaying the keyboard
 * shortcuts
 */
class KeyboardShortcuts : public QDialog {
Q_OBJECT

public:

  //! constructor
  KeyboardShortcuts(QWidget* parent);

  //! destructor
  virtual ~KeyboardShortcuts();

private:

  Ui_KeyboardShortcuts* ui_;

};

} // end namespace
