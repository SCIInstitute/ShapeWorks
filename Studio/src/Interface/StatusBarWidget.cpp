#include "StatusBarWidget.h"
#include <QStatusBar>
#include <QResizeEvent>

// ui
#include <ui_StatusBarWidget.h>

namespace shapeworks {

//---------------------------------------------------------------------------
StatusBarWidget::StatusBarWidget(QMainWindow* parent) : QWidget(parent)
{
  this->ui_ = new Ui_StatusBarWidget;
  this->ui_->setupUi(this);

  this->normal_message_icon_.addFile(
    QString::fromUtf8(":/Studio/Images/log_normal.png"), QSize(), QIcon::Normal);
  this->error_message_icon_.addFile(
    QString::fromUtf8(":/Studio/Images/log_error.png"), QSize(), QIcon::Normal);
  this->warning_message_icon_.addFile(
    QString::fromUtf8(":/Studio/Images/log_warning.png"), QSize(), QIcon::Normal);

  this->q_status_bar_ = parent->statusBar();
  this->q_status_bar_->setContentsMargins(0, 0, 0, 0);
  this->q_status_bar_->addWidget(this, 1);

  this->ui_->log_button->setIcon(this->normal_message_icon_);
  this->ui_->progress_bar->setVisible(false);

  connect(this->ui_->log_button, &QToolButton::clicked, this, &StatusBarWidget::toggle_log_window);
}

//---------------------------------------------------------------------------
void StatusBarWidget::set_message(MessageType message_type, QString message)
{
  message.replace('\n', ' ');
  if (message_type == MessageType::normal) {
    this->ui_->log_button->setIcon(this->normal_message_icon_);
  }
  else if (message_type == MessageType::warning) {
    this->ui_->log_button->setIcon(this->warning_message_icon_);
  }
  else {
    this->ui_->log_button->setIcon(this->error_message_icon_);
    message = "<font color=#CC0000>" + message + "</font>";
  }

  this->ui_->message->setText(message);
  this->update_layout();
}

//---------------------------------------------------------------------------
void StatusBarWidget::set_progress(int value)
{
  if (value < 0) {
    this->ui_->progress_bar->setVisible(true);
    this->ui_->progress_bar->setMinimum(0);
    this->ui_->progress_bar->setMaximum(0);
    return;
  }
  this->ui_->progress_bar->setMinimum(0);
  this->ui_->progress_bar->setMaximum(100);

  if (value < 100) {
    this->ui_->progress_bar->setVisible(true);
    this->ui_->progress_bar->setValue(value);
  }
  else {
    this->ui_->progress_bar->setValue(100);
    this->ui_->progress_bar->setVisible(false);
  }

  this->update_layout();
}

//---------------------------------------------------------------------------
void StatusBarWidget::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);
  this->setMaximumSize(event->size());
  this->update_layout();
}

//---------------------------------------------------------------------------
void StatusBarWidget::update_layout()
{
  if (this->ui_->progress_bar->isVisible()) {
    this->ui_->message->setMaximumWidth(this->width() * 0.66);
  }
  else {
    this->ui_->message->setMaximumWidth(this->width() * 1.0);
  }
}

}