---
title: Studio/Interface/ShapeWorksStudioApp.h

---

# Studio/Interface/ShapeWorksStudioApp.h



## Namespaces

| Name           |
| -------------- |
| **[monailabel](../Namespaces/namespacemonailabel.md)**  |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md)** <br>Main ShapeWorksStudio window.  |




## Source code

```cpp
#pragma once

#include <Data/PreferencesWindow.h>
#include <Data/Telemetry.h>
#include <Interface/CustomSlider.h>
#include <Interface/LogWindow.h>
#include <Interface/StudioLogger.h>
#include <Interface/UpdateChecker.h>
#include <Visualization/StudioVtkOutputWindow.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <Eigen/Eigen>
#include <QActionGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QElapsedTimer>
#include <QErrorMessage>
#include <QLabel>
#include <QMainWindow>
#include <QPointer>
#include <QProgressBar>
#include <QSpinBox>
#include <QTimer>

// Forward Qt class declarations
class Ui_ShapeWorksStudioApp;

namespace monailabel {
  class MonaiLabelTool;
}

namespace shapeworks {
class Lightbox;
class DataTool;
class GroomTool;
class OptimizeTool;
class AnalysisTool;
class DeepSSMTool;
class Session;
class Visualizer;
class SplashScreen;
class WheelEventForwarder;
class StatusBarWidget;
class PythonWorker;
class CompareWidget;


class ShapeWorksStudioApp : public QMainWindow {
  Q_OBJECT
 public:
  ShapeWorksStudioApp();
  ~ShapeWorksStudioApp();

  void closeEvent(QCloseEvent* event) override;

  void initialize_vtk();
  void import_files(QStringList file_names);

 public Q_SLOTS:

  void open_project(QString filename);

  void on_action_new_project_triggered();
  void on_action_open_project_triggered();
  void on_action_show_project_folder_triggered();
  bool on_action_save_project_triggered();
  bool save_project_as(QString type);
  void save_as_swproj_clicked();
  void save_as_xlsx_clicked();
  void on_action_quit_triggered();
  void on_action_import_triggered();
  void splash_screen_closed();

  void on_vertical_scroll_bar_valueChanged();

  void on_action_import_mode_triggered();
  void on_action_groom_mode_triggered();
  void on_action_optimize_mode_triggered();
  void on_action_analysis_mode_triggered();
  void on_action_deepssm_mode_triggered();
  void on_action_monai_mode_triggered();
  void on_actionShow_Tool_Window_triggered();
  void on_actionExport_PCA_Mesh_triggered();
  void on_actionExport_Eigenvalues_triggered();
  void on_actionExport_Eigenvectors_triggered();
  void on_actionExport_PCA_Mode_Points_triggered();
  void on_action_preferences_triggered();
  void action_export_current_mesh_triggered(int index = 0);
  void on_action_export_current_particles_triggered();
  void on_action_export_mesh_scalars_triggered();
  void on_action_export_pca_scores_triggered();
  void action_export_pca_montage_triggered();
  void action_export_screenshot_triggered();
  void action_export_particle_scalars_triggered();
  void action_export_all_subjects_particle_scalars_triggered();

  void on_center_checkbox_stateChanged();
  void on_zoom_slider_valueChanged();
  void on_view_mode_combobox_currentIndexChanged(QString disp_mode);
  void on_auto_view_button_clicked();

  void handle_pca_changed();
  void handle_slider_update();

  void handle_mca_changed();

  void handle_project_changed();
  void handle_points_changed();
  void handle_groom_start();
  void handle_groom_complete();
  void handle_optimize_start();
  void handle_optimize_complete();
  void handle_reconstruction_complete();
  void handle_reset_stats();

  void handle_display_setting_changed();
  void handle_glyph_changed();
  void handle_opacity_changed();

  void handle_alignment_changed();
  void handle_open_recent();

  void handle_color_scheme();
  void handle_pca_update();
  void clear_message();

  // callbacks from logger
  void handle_message(std::string str);
  void handle_error(std::string str);
  void handle_warning(std::string str);
  void handle_debug(std::string str);
  void handle_status(std::string str);
  void handle_progress_with_message(int amt, std::string str);
  void handle_progress(int amt);

  void message_callback(std::string str);
  void handle_new_mesh();
  void handle_clear_cache();
  void handle_compare_settings_changed();

  void handle_lightbox_right_click(int index);

  void update_feature_map_selection(int index);
  void update_feature_map_scale();

  void image_combo_changed(int index);

  void show_splash_screen();
  void hide_splash_screen();
  void about();
  void keyboard_shortcuts();

  void toggle_log_window();

  QSharedPointer<PythonWorker> get_py_worker();

  Preferences& prefs() { return preferences_; }
  QSharedPointer<Session> session() { return session_; }

 protected:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

 private:
  void new_session();
  void update_tool_mode();
  void update_view_mode();
  void reset_num_viewers();

  void update_view_combo();
  void update_compare_menu();

  bool should_reconstruct_view_show();

  static bool write_particle_file(std::string filename, Eigen::VectorXd particles);

  static QString get_mesh_file_filter();

  static const std::string SETTING_ZOOM_C;

  void set_view_combo_item_enabled(int item, bool value);
  bool is_view_combo_item_enabled(int item);

  void disable_all_actions();

  void enable_possible_actions();

  void update_from_preferences();

  void update_scrollbar();

  void update_table();

  void update_display(bool force = false);

  void display_mode_shape();

  bool get_feature_uniform_scale();
  void set_feature_uniform_scale(bool value);

  void update_recent_files();
  void update_alignment_options();

  void save_project(QString filename);

  bool write_scalars(vtkSmartPointer<vtkPolyData> poly_data, QString filename);

  void set_message(MessageType message_type, QString message);

  void create_glyph_submenu();
  void create_iso_submenu();
  void create_compare_submenu();

  void update_window_title();

  Ui_ShapeWorksStudioApp* ui_;

  QActionGroup* action_group_ = nullptr;

  QSharedPointer<Lightbox> lightbox_;
  QSharedPointer<DataTool> data_tool_;
  QSharedPointer<GroomTool> groom_tool_;
  QSharedPointer<OptimizeTool> optimize_tool_;
  QSharedPointer<AnalysisTool> analysis_tool_;
  QSharedPointer<DeepSSMTool> deepssm_tool_;
  QSharedPointer<monailabel::MonaiLabelTool> monai_tool_;
  QSharedPointer<Visualizer> visualizer_;
  QSharedPointer<PreferencesWindow> preferences_window_;
  CompareWidget* compare_widget_ = nullptr;
  vtkSmartPointer<StudioVtkOutputWindow> studio_vtk_output_window_;

  // all the preferences
  Preferences preferences_;

  QSharedPointer<Session> session_;
  QSharedPointer<WheelEventForwarder> wheel_event_forwarder_;

  // programmatic UI elements
  CustomSlider* glyph_size_slider_;
  CustomSlider* glyph_quality_slider_;
  QLabel* glyph_size_label_;
  QLabel* glyph_quality_label_;
  QCheckBox* glyph_auto_size_;
  QCheckBox* glyph_arrow_scale_{nullptr};
  QList<QAction*> recent_file_actions_;
  LogWindow log_window_;
  QPointer<StatusBarWidget> status_bar_;
  QSharedPointer<shapeworks::SplashScreen> splash_screen_;
  QErrorMessage error_message_dialog_;
  std::vector<CustomSlider*> iso_opacity_sliders_;
  std::vector<QCheckBox*> domain_particle_checkboxes_;

  QString current_message_;

  std::string current_display_mode_;

  bool block_update_{false};

  QElapsedTimer time_since_last_update_;
  qint64 last_render_ = -1;

  QStringList current_features_;
  QStringList current_image_list_;

  QSharedPointer<PythonWorker> py_worker_;

  StudioLogger logger_;
  UpdateChecker update_checker_{preferences_};
  Telemetry telemetry_{preferences_};
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-03-18 at 23:31:55 +0000
