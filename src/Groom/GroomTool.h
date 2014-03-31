#include <QSharedPointer>
#include <QWidget>

class DataManager;
class Ui_GroomTool;

class GroomTool : public QWidget
{
  Q_OBJECT;
public:

  GroomTool();
  ~GroomTool();
  
  /// export XML for ShapeWorksGroom
  bool export_xml(QString filename);

  void set_data_manager(QSharedPointer<DataManager> data_manager);

public Q_SLOTS:

  void on_antialias_checkbox_stateChanged( int state );
  void on_blur_checkbox_stateChanged( int state );


  /// Export XML
  void on_export_xml_button_clicked();

  /// Run groom tool
  void on_run_groom_button_clicked();

private:

  Ui_GroomTool* ui_;
  QSharedPointer<DataManager> data_manager_;
};
