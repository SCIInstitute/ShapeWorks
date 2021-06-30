#pragma once

#include <QMainWindow>

// Forward Qt class declarations
class Ui_StatusBarWidget;

namespace shapeworks {

enum class MessageType {
  normal,
  warning,
  error,
  debug
};

//! StatusBarWidget
/*!
 * This widget is used in place of the standard QStatusBar
 * It supports a log window toggle, current message, with color, and progressbar
 *
 */
class StatusBarWidget : public QWidget {
Q_OBJECT

public:
  StatusBarWidget(QMainWindow* parent = 0);

  void set_message(MessageType message_type, QString message);
  void set_progress(int value);

  void resizeEvent(QResizeEvent* event) override;

Q_SIGNALS:
  void toggle_log_window();

private:

  void update_layout();

  /// designer form
  Ui_StatusBarWidget* ui_;

  QStatusBar* q_status_bar_;

  QIcon normal_message_icon_;
  QIcon warning_message_icon_;
  QIcon error_message_icon_;

};

} // shapeworks
