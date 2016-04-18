#ifndef STUDIO_APPLICATION_SHAPEWORKSSTUDIOAPP_H
#define STUDIO_APPLICATION_SHAPEWORKSSTUDIOAPP_H

#include <QMainWindow>
#include <QActionGroup>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QTimer>
#include "Data/PreferencesWindow.h"
#include <vtkCamera.h>
#include <QProgressBar>

class Lightbox;
class GroomTool;
class OptimizeTool;
class AnalysisTool;
class Project;
class Visualizer;

// Forward Qt class declarations
class Ui_ShapeWorksStudioApp;

class LightboxLayout;
class WheelEventForwarder;

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

public Q_SLOTS:

  void on_action_new_project_triggered();
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
  void on_action_analysis_mode_triggered();
  void on_actionShow_Tool_Window_triggered();
  void on_actionExport_PCA_Mesh_triggered();
  void on_actionSet_Data_Directory_triggered();

  void on_action_preferences_triggered();

  void on_center_checkbox_stateChanged();
  void on_thumbnail_size_slider_valueChanged();
  void on_view_mode_combobox_currentIndexChanged();
  void on_auto_view_button_clicked();
  
  void handle_pca_changed();

  void handle_project_changed();
  void handle_groom_complete();
  void handle_optimize_complete();

  void handle_display_setting_changed();
  void handle_glyph_changed();

  void handle_open_recent();

  void handle_color_scheme();
  void handle_new_mesh();
  void handle_message(std::string str);
  void handle_error(std::string str);
  void handle_progress(size_t amt);

private:

  void update_from_preferences();

  void update_scrollbar();

  void update_table();

  void update_display();

  void compute_mode_shape();

  void update_recent_files();

  void reset();

  /// designer form
  Ui_ShapeWorksStudioApp* ui_;

  QActionGroup* action_group_;

  QSharedPointer<Lightbox> lightbox_;

  QSharedPointer<GroomTool> groom_tool_;
  QSharedPointer<OptimizeTool> optimize_tool_;
  QSharedPointer<AnalysisTool> analysis_tool_;
  QSharedPointer<Visualizer> visualizer_;
  QSharedPointer<PreferencesWindow> preferences_window_;
  //all the preferences
  Preferences preferences_;

  QSharedPointer<Project> project_;

  QSharedPointer<WheelEventForwarder> wheel_event_forwarder_;

  // programatic UI elements
  QSlider* glyph_size_slider_;
  QSlider* glyph_quality_slider_;
  QLabel* glyph_size_label_;
  QLabel* glyph_quality_label_;
  QList<QAction*> recent_file_actions_;
  //isosurface widgets
  QLabel* iso_smoothing_label_;
  QSpinBox* iso_neighborhood_spinner_;
  QDoubleSpinBox* iso_spacing_spinner_;
  QSlider* iso_smoothing_slider_;
  std::vector<std::string> originalFilenames_;
  QProgressBar * progressBar_;
  std::string data_dir_;
};

#endif /* STUDIO_APPLICATION_SHAPEWORKSSTUDIOAPP_H */
