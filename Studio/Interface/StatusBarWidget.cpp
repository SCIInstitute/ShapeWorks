#include "StatusBarWidget.h"

#include <QDebug>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QStatusBar>

// ui
#include <ui_StatusBarWidget.h>

namespace shapeworks {

//---------------------------------------------------------------------------
StatusBarWidget::StatusBarWidget(QMainWindow* parent) : QWidget(parent) {
  ui_ = new Ui_StatusBarWidget;
  ui_->setupUi(this);

  normal_message_icon_.addFile(QString::fromUtf8(":/Studio/Images/log_normal.png"), QSize(), QIcon::Normal);
  error_message_icon_.addFile(QString::fromUtf8(":/Studio/Images/log_error.png"), QSize(), QIcon::Normal);
  warning_message_icon_.addFile(QString::fromUtf8(":/Studio/Images/log_warning.png"), QSize(), QIcon::Normal);

  q_status_bar_ = parent->statusBar();
  q_status_bar_->setContentsMargins(0, 0, 0, 0);
  q_status_bar_->addWidget(this, 1);

  ui_->log_button->setIcon(normal_message_icon_);
  ui_->progress_bar->setVisible(false);

  // clicking the message clears it, for when a stale path or debug line is in the way
  ui_->message->installEventFilter(this);
  ui_->message->setToolTip("Click to clear this message");

  connect(ui_->log_button, &QToolButton::clicked, this, &StatusBarWidget::toggle_log_window);
}

//---------------------------------------------------------------------------
bool StatusBarWidget::eventFilter(QObject* watched, QEvent* event) {
  if (watched == ui_->message && event->type() == QEvent::MouseButtonPress) {
    auto* mouse_event = static_cast<QMouseEvent*>(event);
    if (mouse_event->button() == Qt::LeftButton && !ui_->message->text().isEmpty()) {
      clear_message();
      return true;
    }
  }
  return QWidget::eventFilter(watched, event);
}

//---------------------------------------------------------------------------
void StatusBarWidget::clear_message() {
  ui_->message->clear();
  ui_->message->setCursor(Qt::ArrowCursor);
  // the icon reflects the severity of the displayed message, so it goes with it.
  // History remains in the log window.
  ui_->log_button->setIcon(normal_message_icon_);
  update_layout();
}

//---------------------------------------------------------------------------
void StatusBarWidget::set_message(MessageType message_type, QString message) {
  message.replace('\n', ' ');
  // truncate long messages
  if (message.length() > 100) {
    message.truncate(100);
    message = message + "...";
  }
  if (message_type == MessageType::normal) {
    ui_->log_button->setIcon(normal_message_icon_);
  } else if (message_type == MessageType::warning) {
    ui_->log_button->setIcon(warning_message_icon_);
  } else if (message_type == MessageType::debug) {
    ui_->log_button->setIcon(normal_message_icon_);
    message = "<font color=#800080>" + message + "</font>";
  } else {
    ui_->log_button->setIcon(error_message_icon_);
    message = "<font color=#CC0000>" + message + "</font>";
  }

  ui_->message->setText(message);
  // hint that the message is clickable, but only while there is one to clear
  ui_->message->setCursor(message.isEmpty() ? Qt::ArrowCursor : Qt::PointingHandCursor);
  update_layout();
}

//---------------------------------------------------------------------------
void StatusBarWidget::set_progress(int value) {
  if (value < 0) {
    ui_->progress_bar->setVisible(true);
    ui_->progress_bar->setMinimum(0);
    ui_->progress_bar->setMaximum(0);
    return;
  }
  ui_->progress_bar->setMinimum(0);
  ui_->progress_bar->setMaximum(100);

  if (value < 100) {
    ui_->progress_bar->setVisible(true);
    ui_->progress_bar->setValue(value);
  } else {
    ui_->progress_bar->setValue(100);
    ui_->progress_bar->setVisible(false);
  }

  update_layout();
}

//---------------------------------------------------------------------------
void StatusBarWidget::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  update_layout();
}

//---------------------------------------------------------------------------
void StatusBarWidget::update_layout() {
  if (ui_->progress_bar->isVisible()) {
    ui_->message->setMaximumWidth(width() * 0.66);
  } else {
    ui_->message->setMaximumWidth(width() * 1.0);
  }
}

}  // namespace shapeworks
