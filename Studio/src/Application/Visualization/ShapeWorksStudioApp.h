#pragma once

#include <QMainWindow>
#include <QActionGroup>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QTimer>
#include <QProgressBar>

#include <Data/PreferencesWindow.h>

class Lightbox;
class GroomTool;
class OptimizeTool;
class AnalysisTool;
class Session;
class Visualizer;

namespace shapeworks {
class SplashScreen;
}

// Forward Qt class declarations
class Ui_ShapeWorksStudioApp;

class LightboxLayout;
class WheelEventForwarder;

//! Main ShapeWorksStudio window
/*!
 * This class represents the primary ShapeWorksStudio window interface
 */
class ShapeWorksStudioApp : public QMainWindow {
Q_OBJECT
public:

  ShapeWorksStudioApp();
  ~ShapeWorksStudioApp();

  void closeEvent(QCloseEvent* event) override;

  void initialize_vtk();
  void import_files(QStringList file_names);
  void open_project(QString filename);

public Q_SLOTS:

  void on_action_new_project_triggered();
  void on_action_open_project_triggered();
  bool on_action_save_project_triggered();
  bool on_action_save_project_as_triggered();
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
  void on_actionExport_Eigenvalues_triggered();
  void on_actionExport_Eigenvectors_triggered();
  void on_actionExport_PCA_Mode_Points_triggered();
  void on_actionExport_Variance_Graph_triggered();
  void on_action_preferences_triggered();
  void on_action_export_current_mesh_triggered();
  void on_action_export_mesh_scalars_triggered();
  void on_action_export_pca_scores_triggered();

  void on_center_checkbox_stateChanged();
  void on_zoom_slider_valueChanged();
  void on_view_mode_combobox_currentIndexChanged(QString disp_mode);
  void on_auto_view_button_clicked();

  void handle_pca_changed();
  void handle_slider_update();

  void handle_project_changed();
  void handle_points_changed();
  void handle_groom_complete();
  void handle_optimize_start();
  void handle_optimize_complete();
  void handle_reconstruction_complete();

  void handle_display_setting_changed();
  void handle_glyph_changed();

  void handle_open_recent();

  void handle_color_scheme();
  void handle_pca_update();
  void handle_message(std::string str);
  void handle_error(std::string str);
  void handle_warning(std::string str);
  void handle_progress(size_t amt);
  void handle_new_mesh();
  void handle_clear_cache();

  void show_splash_screen();

private:

  void new_session();
  void update_tool_mode();
  void update_view_mode();

  enum VIEW_MODE {
    ORIGINAL = 0,
    GROOMED = 1,
    RECONSTRUCTED = 2
  };

  enum DISPLAY_MODE {
    ALL_SAMPLES = 0,
    MEAN = 1,
    PCA = 2,
    SINGLE_SAMPLE = 3
  };

  static const std::string MODE_ORIGINAL_C;
  static const std::string MODE_GROOMED_C;
  static const std::string MODE_RECONSTRUCTION_C;

  static const std::string SETTING_ZOOM_C;

  std::string get_view_mode();

  void set_view_combo_item_enabled(int item, bool value);

  void disableAllActions();

  void enable_possible_actions();

  void update_from_preferences();

  void update_scrollbar();

  void update_table();

  void update_display(bool force = false);

  void compute_mode_shape();

  bool set_view_mode(std::string view_mode);

  void update_recent_files();

  void save_project(std::string filename);

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

  QSharedPointer<Session> session_;

  QSharedPointer<WheelEventForwarder> wheel_event_forwarder_;

  // programatic UI elements
  QSlider* glyph_size_slider_;
  QSlider* glyph_quality_slider_;
  QLabel* glyph_size_label_;
  QLabel* glyph_quality_label_;
  QList<QAction*> recent_file_actions_;

  QProgressBar* progress_bar_;
  std::string currentMessage_;

  std::string current_display_mode_;

  QSharedPointer<shapeworks::SplashScreen> splash_screen_;

  bool block_update_{false};
  bool is_loading_{false};

};
