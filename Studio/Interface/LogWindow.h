#pragma once

#include <QDialog>
#include <QListWidget>

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

private Q_SLOTS:

  //! put the selected messages on the clipboard, or every message when none are selected
  void copy_selected_messages();

  //! put every message on the clipboard
  void copy_all_messages();

private:

  //! put the given messages on the clipboard, one per line
  void copy_messages(const QList<QListWidgetItem*>& items);

  Ui_LogWindow* ui_;

};

}
