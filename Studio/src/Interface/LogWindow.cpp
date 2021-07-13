#include "LogWindow.h"

// Automatically generated UI file
#include "ui_LogWindow.h"

namespace shapeworks {

//---------------------------------------------------------------------------
LogWindow::LogWindow(QWidget* parent) : QDialog(parent)
{
  this->ui_ = new Ui_LogWindow;
  this->ui_->setupUi(this);

  QIcon icon = windowIcon();
  Qt::WindowFlags flags = windowFlags();
  Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
  flags = flags & (~helpFlag);
  this->setWindowFlags(flags);
  this->setWindowIcon(icon);
}

//---------------------------------------------------------------------------
void LogWindow::add_message(MessageType message_type, QString message)
{
  message = message.trimmed();
  QColor color = QColor(255, 255, 255);

  switch (message_type) {
    case MessageType::error:
      color = QColor(200, 0, 0);
      break;
    case MessageType::warning:
      color = QColor(165, 161, 34);
      break;
    case MessageType::normal:
      color = QColor(3, 86, 2);
      break;
    case MessageType::debug:
      color = QColor("purple");
      break;
    default:
      break;
  }

  QListWidgetItem* item = new QListWidgetItem(message,
                                              this->ui_->history_list_);
  item->setTextColor(color);
  this->ui_->history_list_->addItem(item);
  this->ui_->history_list_->setCurrentItem(item);
}

}

