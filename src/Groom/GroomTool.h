#include <QWidget>

class Ui_GroomTool;

class GroomTool : public QWidget
{
  Q_OBJECT;
public:

  GroomTool();
  ~GroomTool();

public Q_SLOTS:

  void on_antialias_checkbox_stateChanged( int state );
  void on_blur_checkbox_stateChanged( int state );

private:

  Ui_GroomTool* ui_;
};
