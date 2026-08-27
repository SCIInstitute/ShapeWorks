#include "LogWindow.h"

#include <QAction>
#include <QApplication>
#include <QClipboard>
#include <QKeySequence>

// Automatically generated UI file
#include "ui_LogWindow.h"

namespace shapeworks {

//---------------------------------------------------------------------------
LogWindow::LogWindow(QWidget* parent) : QDialog(parent) {
  ui_ = new Ui_LogWindow;
  ui_->setupUi(this);
  ui_->history_list_->setWordWrap(true);

  // The message history is where someone goes to get the text of an error back out of Studio, so it
  // has to be possible to take it: select the lines that matter, or copy the lot.
  auto* copy = new QAction(tr("&Copy"), ui_->history_list_);
  copy->setShortcut(QKeySequence::Copy);
  // window rather than widget scope, so the shortcut works without first clicking into the list
  copy->setShortcutContext(Qt::WindowShortcut);
  connect(copy, &QAction::triggered, this, &LogWindow::copy_selected_messages);
  ui_->history_list_->addAction(copy);

  auto* copy_all = new QAction(tr("Copy &All"), ui_->history_list_);
  connect(copy_all, &QAction::triggered, this, &LogWindow::copy_all_messages);
  ui_->history_list_->addAction(copy_all);

  QIcon icon = windowIcon();
  Qt::WindowFlags flags = windowFlags();
  Qt::WindowFlags helpFlag = Qt::WindowContextHelpButtonHint;
  flags = flags & (~helpFlag);
  this->setWindowFlags(flags);
  this->setWindowIcon(icon);
}

//---------------------------------------------------------------------------
void LogWindow::add_message(MessageType message_type, QString message) {
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

  QListWidgetItem* item = new QListWidgetItem(message, this->ui_->history_list_);
  item->setForeground(color);
  this->ui_->history_list_->addItem(item);
  // follow the newest message without touching the selection, which someone may be part way through
  // making when the next message arrives
  this->ui_->history_list_->scrollToItem(item);
}

//---------------------------------------------------------------------------
void LogWindow::copy_selected_messages() {
  auto items = ui_->history_list_->selectedItems();
  if (items.isEmpty()) {
    copy_all_messages();
    return;
  }
  copy_messages(items);
}

//---------------------------------------------------------------------------
void LogWindow::copy_all_messages() {
  QList<QListWidgetItem*> items;
  for (int i = 0; i < ui_->history_list_->count(); i++) {
    items.append(ui_->history_list_->item(i));
  }
  copy_messages(items);
}

//---------------------------------------------------------------------------
void LogWindow::copy_messages(const QList<QListWidgetItem*>& items) {
  QStringList messages;
  for (const auto* item : items) {
    messages << item->text();
  }
  QApplication::clipboard()->setText(messages.join("\n"));
}

}  // namespace shapeworks
