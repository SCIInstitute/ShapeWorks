#pragma once

#include <Data/Session.h>

#include <QWidget>

namespace Ui {
class CompareWidget;
}

namespace shapeworks {

class CompareWidget : public QWidget {
  Q_OBJECT

 public:
  explicit CompareWidget(QWidget *parent = nullptr);
  ~CompareWidget();

  CompareSettings get_settings();

 Q_SIGNALS:
  void settings_changed();

 private:
  Ui::CompareWidget *ui_;
  CompareSettings settings_;
};

}  // namespace shapeworks
