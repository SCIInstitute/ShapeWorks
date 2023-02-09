#pragma once

#include <QDialog>

#include <Interface/StatusBarWidget.h>

// Forward Qt class declarations
class Ui_LogWindow;

namespace shapeworks {

//! Log Window
/*!
 * This dialog displays the history of messages (e.g. status bar messages)
 *
 */
class LogWindow : public QDialog {
Q_OBJECT

public:

  LogWindow(QWidget* parent = 0);

  //! add a message to the history
  void add_message(MessageType message_type, QString message);

private:

  Ui_LogWindow* ui_;

};

}
