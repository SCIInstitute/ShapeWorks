#include <QWidget>

class Ui_GroomTool;

class GroomTool : public QWidget
{
  Q_OBJECT;
public:

  GroomTool();
  ~GroomTool();

public Q_SLOTS:

  void on_toolListWidget_currentRowChanged(int row);
  void on_addToolButton_clicked();
  void on_deleteToolButton_clicked();


private:

  Ui_GroomTool *ui;
};
