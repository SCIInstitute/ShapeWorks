#ifndef STUDIO_APPLICATION_SHAPEWORKSSTUDIOAPP_H
#define STUDIO_APPLICATION_SHAPEWORKSSTUDIOAPP_H

#include <QMainWindow>
#include <QActionGroup>
#include <QSlider>
#include <QLabel>
#include <QTimer>

class Lightbox;
class GroomTool;
class OptimizeTool;
class AnalysisTool;
class Project;
class Visualizer;

// Forward Qt class declarations
class Ui_ShapeWorksStudioApp;

class LightboxLayout;

//! Main ShapeWorksStudio window
/*!
 * This class represents the primary ShapeWorksStudio window interface
 */
class ShapeWorksStudioApp : public QMainWindow
{
  Q_OBJECT
public:

  ShapeWorksStudioApp( int argc, char** argv );
  ~ShapeWorksStudioApp();

  virtual void closeEvent( QCloseEvent* event );

  void initialize_vtk();
  void import_files( QStringList file_names );
  void open_project( QString filename );

  void set_status_bar( QString status );

public Q_SLOTS:

  void on_action_open_project_triggered();
  void on_action_save_project_triggered();
  void on_action_save_project_as_triggered();
  void on_action_quit_triggered();
  void on_action_import_triggered();
  void on_vertical_scroll_bar_valueChanged();
  void on_add_button_clicked();
  void on_delete_button_clicked();

  void on_action_import_mode_triggered();
  void on_action_groom_mode_triggered();
  void on_action_optimize_mode_triggered();

  void on_action_preferences_triggered();

  void on_center_checkbox_stateChanged();
  void on_thumbnail_size_slider_valueChanged();
  void on_view_mode_combobox_currentIndexChanged();

  void mode_changed();

  void handle_project_changed();
  void handle_groom_complete();

  void handle_display_setting_changed();

  // PCA
  void on_pcaSlider_valueChanged();
  void on_pcaModeSpinBox_valueChanged();

  void handle_pca_animate_state_changed();
  void handle_pca_timer();
  void handle_pca_labels_changed( QString value, QString eigen, QString lambda );

private:

  void update_from_preferences();

  void update_scrollbar();

  void update_table();

  void update_display();

  double get_pca_value( int slider_value );

  void compute_mode_shape();

  /// designer form
  Ui_ShapeWorksStudioApp* ui_;

  QActionGroup* action_group_;

  QSharedPointer<Lightbox> lightbox_;

  QSharedPointer<GroomTool> groom_tool_;
  QSharedPointer<OptimizeTool> optimize_tool_;
  QSharedPointer<AnalysisTool> analysis_tool_;
  QSharedPointer<Visualizer> visualizer_;

  QSharedPointer<Project> project_;

  // programatic UI elements
  QSlider* glyph_size_slider_;
  QSlider* glyph_quality_slider_;
  QLabel* glyph_size_label_;
  QLabel* glyph_quality_label_;

  bool pcaAnimateDirection;
  QTimer pcaAnimateTimer;
};

#endif /* STUDIO_APPLICATION_SHAPEWORKSSTUDIOAPP_H */
