---
title: Studio/src/Interface/ShapeWorksStudioApp.h

---

# Studio/src/Interface/ShapeWorksStudioApp.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md)** <br>Main ShapeWorksStudio window.  |




## Source code

```cpp
#pragma once

#include <QMainWindow>
#include <QActionGroup>
#include <QSlider>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QLabel>
#include <QTimer>
#include <QCheckBox>
#include <QProgressBar>
#include <QElapsedTimer>
#include <QPointer>
#include <QErrorMessage>

#include <Interface/LogWindow.h>
#include <Data/PreferencesWindow.h>
#include <Visualization/StudioVtkOutputWindow.h>

#include <Eigen/Eigen>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

// Forward Qt class declarations
class Ui_ShapeWorksStudioApp;

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
  bool on_action_save_project_as_triggered();
  void on_action_quit_triggered();
  void on_action_import_triggered();

  void on_vertical_scroll_bar_valueChanged();

  void on_action_import_mode_triggered();
  void on_action_groom_mode_triggered();
  void on_action_optimize_mode_triggered();
  void on_action_analysis_mode_triggered();
  void on_action_deepssm_mode_triggered();
  void on_actionShow_Tool_Window_triggered();
  void on_actionExport_PCA_Mesh_triggered();
  void on_actionExport_Eigenvalues_triggered();
  void on_actionExport_Eigenvectors_triggered();
  void on_actionExport_PCA_Mode_Points_triggered();
  void on_actionExport_Variance_Graph_triggered();
  void on_action_preferences_triggered();
  void on_action_export_current_mesh_triggered();
  void on_action_export_current_particles_triggered();
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
  void handle_groom_start();
  void handle_groom_complete();
  void handle_optimize_start();
  void handle_optimize_complete();
  void handle_reconstruction_complete();

  void handle_display_setting_changed();
  void handle_glyph_changed();
  void handle_opacity_changed();

  void handle_alignment_changed();
  void handle_open_recent();

  void handle_color_scheme();
  void handle_pca_update();
  void handle_message(QString str);
  void handle_status(QString str);
  void handle_error(QString str);
  void handle_warning(QString str);
  void handle_progress(int amt);
  void handle_new_mesh();
  void handle_clear_cache();

  void update_feature_map_selection(const QString& feature_map);
  void update_feature_map_scale();

  void show_splash_screen();
  void about();
  void keyboard_shortcuts();

  void toggle_log_window();

  QSharedPointer<PythonWorker> get_py_worker();

protected:
  void dragEnterEvent(QDragEnterEvent* event) override;
  void dragLeaveEvent(QDragLeaveEvent* event) override;
  void dropEvent(QDropEvent* event) override;

private:

  void new_session();
  void update_tool_mode();
  void update_view_mode();
  void reset_num_viewers();

  static bool write_particle_file(std::string filename, Eigen::VectorXd particles);

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
  bool is_view_combo_item_enabled(int item);


  std::string get_tool_state();

  void disable_all_actions();

  void enable_possible_actions();

  void update_from_preferences();

  void update_scrollbar();

  void update_table();

  void update_display(bool force = false);

  void compute_mode_shape();

  bool set_view_mode(std::string view_mode);

  bool set_feature_map(std::string feature_map);
  std::string get_feature_map();

  bool get_feature_uniform_scale();
  void set_feature_uniform_scale(bool value);

  void update_recent_files();
  void update_alignment_options();

  void save_project(std::string filename);

  bool write_mesh(vtkSmartPointer<vtkPolyData> poly_data, QString filename);
  bool write_scalars(vtkSmartPointer<vtkPolyData> poly_data, QString filename);

  void set_message(MessageType message_type, QString message);

  void create_glyph_submenu();
  void create_iso_submenu();

  Ui_ShapeWorksStudioApp* ui_;

  QActionGroup* action_group_;

  QSharedPointer<Lightbox> lightbox_;
  QSharedPointer<DataTool> data_tool_;
  QSharedPointer<GroomTool> groom_tool_;
  QSharedPointer<OptimizeTool> optimize_tool_;
  QSharedPointer<AnalysisTool> analysis_tool_;
  QSharedPointer<DeepSSMTool> deepssm_tool_;
  QSharedPointer<Visualizer> visualizer_;
  QSharedPointer<PreferencesWindow> preferences_window_;
  vtkSmartPointer<StudioVtkOutputWindow> studio_vtk_output_window_;

  //all the preferences
  Preferences preferences_;

  QSharedPointer<Session> session_;
  QSharedPointer<WheelEventForwarder> wheel_event_forwarder_;

  // programmatic UI elements
  QSlider* glyph_size_slider_;
  QSlider* glyph_quality_slider_;
  QLabel* glyph_size_label_;
  QLabel* glyph_quality_label_;
  QCheckBox* glyph_auto_size_;
  QList<QAction*> recent_file_actions_;
  LogWindow log_window_;
  QPointer<StatusBarWidget> status_bar_;
  QSharedPointer<shapeworks::SplashScreen> splash_screen_;
  QErrorMessage error_message_dialog_;
  std::vector<QSlider*> iso_opacity_sliders_;


  QString current_message_;

  std::string current_display_mode_;

  bool block_update_{false};
  bool is_loading_{false};

  QElapsedTimer time_since_last_update_;
  qint64 last_render_ = -1;

  QSharedPointer<PythonWorker> py_worker_;
};
}
```


-------------------------------

Updated on 2022-01-28 at 21:13:54 +0000
