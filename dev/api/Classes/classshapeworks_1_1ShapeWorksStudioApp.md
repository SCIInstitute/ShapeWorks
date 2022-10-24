---
title: shapeworks::ShapeWorksStudioApp
summary: Main ShapeWorksStudio window. 

---

# shapeworks::ShapeWorksStudioApp



Main ShapeWorksStudio window.  [More...](#detailed-description)


`#include <ShapeWorksStudioApp.h>`

Inherits from QMainWindow

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[open_project](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-open-project)**(QString filename) |
| void | **[on_action_new_project_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-new-project-triggered)**() |
| void | **[on_action_open_project_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-open-project-triggered)**() |
| void | **[on_action_show_project_folder_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-show-project-folder-triggered)**() |
| bool | **[on_action_save_project_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-save-project-triggered)**() |
| bool | **[save_project_as](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-save-project-as)**(QString type) |
| void | **[save_as_swproj_clicked](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-save-as-swproj-clicked)**() |
| void | **[save_as_xlsx_clicked](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-save-as-xlsx-clicked)**() |
| void | **[on_action_quit_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-quit-triggered)**() |
| void | **[on_action_import_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-import-triggered)**() |
| void | **[on_vertical_scroll_bar_valueChanged](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-vertical-scroll-bar-valuechanged)**() |
| void | **[on_action_import_mode_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-import-mode-triggered)**() |
| void | **[on_action_groom_mode_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-groom-mode-triggered)**() |
| void | **[on_action_optimize_mode_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-optimize-mode-triggered)**() |
| void | **[on_action_analysis_mode_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-analysis-mode-triggered)**() |
| void | **[on_action_deepssm_mode_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-deepssm-mode-triggered)**() |
| void | **[on_actionShow_Tool_Window_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-actionshow-tool-window-triggered)**() |
| void | **[on_actionExport_PCA_Mesh_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-actionexport-pca-mesh-triggered)**() |
| void | **[on_actionExport_Eigenvalues_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-actionexport-eigenvalues-triggered)**() |
| void | **[on_actionExport_Eigenvectors_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-actionexport-eigenvectors-triggered)**() |
| void | **[on_actionExport_PCA_Mode_Points_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-actionexport-pca-mode-points-triggered)**() |
| void | **[on_action_preferences_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-preferences-triggered)**() |
| void | **[on_action_export_current_mesh_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-export-current-mesh-triggered)**() |
| void | **[on_action_export_current_particles_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-export-current-particles-triggered)**() |
| void | **[on_action_export_mesh_scalars_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-export-mesh-scalars-triggered)**() |
| void | **[on_action_export_pca_scores_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-action-export-pca-scores-triggered)**() |
| void | **[action_export_pca_montage_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-action-export-pca-montage-triggered)**() |
| void | **[action_export_screenshot_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-action-export-screenshot-triggered)**() |
| void | **[action_export_particle_scalars_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-action-export-particle-scalars-triggered)**() |
| void | **[action_export_all_subjects_particle_scalars_triggered](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-action-export-all-subjects-particle-scalars-triggered)**() |
| void | **[on_center_checkbox_stateChanged](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-center-checkbox-statechanged)**() |
| void | **[on_zoom_slider_valueChanged](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-zoom-slider-valuechanged)**() |
| void | **[on_view_mode_combobox_currentIndexChanged](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-view-mode-combobox-currentindexchanged)**(QString disp_mode) |
| void | **[on_auto_view_button_clicked](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-on-auto-view-button-clicked)**() |
| void | **[handle_pca_changed](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-pca-changed)**() |
| void | **[handle_slider_update](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-slider-update)**() |
| void | **[handle_project_changed](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-project-changed)**() |
| void | **[handle_points_changed](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-points-changed)**() |
| void | **[handle_groom_start](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-groom-start)**() |
| void | **[handle_groom_complete](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-groom-complete)**() |
| void | **[handle_optimize_start](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-optimize-start)**() |
| void | **[handle_optimize_complete](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-optimize-complete)**() |
| void | **[handle_reconstruction_complete](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-reconstruction-complete)**() |
| void | **[handle_display_setting_changed](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-display-setting-changed)**() |
| void | **[handle_glyph_changed](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-glyph-changed)**() |
| void | **[handle_opacity_changed](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-opacity-changed)**() |
| void | **[handle_alignment_changed](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-alignment-changed)**() |
| void | **[handle_open_recent](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-open-recent)**() |
| void | **[handle_color_scheme](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-color-scheme)**() |
| void | **[handle_pca_update](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-pca-update)**() |
| void | **[clear_message](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-clear-message)**() |
| void | **[handle_message](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-message)**(std::string str) |
| void | **[handle_error](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-error)**(std::string str) |
| void | **[handle_warning](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-warning)**(std::string str) |
| void | **[handle_debug](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-debug)**(std::string str) |
| void | **[message_callback](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-message-callback)**(std::string str) |
| void | **[handle_status](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-status)**(QString str) |
| void | **[handle_progress](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-progress)**(int amt) |
| void | **[handle_new_mesh](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-new-mesh)**() |
| void | **[handle_clear_cache](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-clear-cache)**() |
| void | **[handle_compare_settings_changed](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-handle-compare-settings-changed)**() |
| void | **[update_feature_map_selection](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-update-feature-map-selection)**(int index) |
| void | **[update_feature_map_scale](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-update-feature-map-scale)**() |
| void | **[image_combo_changed](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-image-combo-changed)**(int index) |
| void | **[show_splash_screen](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-show-splash-screen)**() |
| void | **[about](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-about)**() |
| void | **[keyboard_shortcuts](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-keyboard-shortcuts)**() |
| void | **[toggle_log_window](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-toggle-log-window)**() |
| QSharedPointer< [PythonWorker](../Classes/classshapeworks_1_1PythonWorker.md) > | **[get_py_worker](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-get-py-worker)**() |
| [Preferences](../Classes/classPreferences.md) & | **[prefs](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-prefs)**() |
| QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > | **[session](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#slot-session)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#function-shapeworksstudioapp)**() |
| | **[~ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#function-~shapeworksstudioapp)**() |
| void | **[closeEvent](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#function-closeevent)**(QCloseEvent * event) override |
| void | **[initialize_vtk](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#function-initialize-vtk)**() |
| void | **[import_files](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#function-import-files)**(QStringList file_names) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| void | **[dragEnterEvent](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#function-dragenterevent)**(QDragEnterEvent * event) override |
| void | **[dragLeaveEvent](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#function-dragleaveevent)**(QDragLeaveEvent * event) override |
| void | **[dropEvent](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md#function-dropevent)**(QDropEvent * event) override |

## Detailed Description

```cpp
class shapeworks::ShapeWorksStudioApp;
```

Main ShapeWorksStudio window. 

This class represents the primary ShapeWorksStudio window interface 

## Public Slots Documentation

### slot open_project

```cpp
void open_project(
    QString filename
)
```


### slot on_action_new_project_triggered

```cpp
void on_action_new_project_triggered()
```


### slot on_action_open_project_triggered

```cpp
void on_action_open_project_triggered()
```


### slot on_action_show_project_folder_triggered

```cpp
void on_action_show_project_folder_triggered()
```


### slot on_action_save_project_triggered

```cpp
bool on_action_save_project_triggered()
```


### slot save_project_as

```cpp
bool save_project_as(
    QString type
)
```


### slot save_as_swproj_clicked

```cpp
void save_as_swproj_clicked()
```


### slot save_as_xlsx_clicked

```cpp
void save_as_xlsx_clicked()
```


### slot on_action_quit_triggered

```cpp
void on_action_quit_triggered()
```


### slot on_action_import_triggered

```cpp
void on_action_import_triggered()
```


### slot on_vertical_scroll_bar_valueChanged

```cpp
void on_vertical_scroll_bar_valueChanged()
```


### slot on_action_import_mode_triggered

```cpp
void on_action_import_mode_triggered()
```


### slot on_action_groom_mode_triggered

```cpp
void on_action_groom_mode_triggered()
```


### slot on_action_optimize_mode_triggered

```cpp
void on_action_optimize_mode_triggered()
```


### slot on_action_analysis_mode_triggered

```cpp
void on_action_analysis_mode_triggered()
```


### slot on_action_deepssm_mode_triggered

```cpp
void on_action_deepssm_mode_triggered()
```


### slot on_actionShow_Tool_Window_triggered

```cpp
void on_actionShow_Tool_Window_triggered()
```


### slot on_actionExport_PCA_Mesh_triggered

```cpp
void on_actionExport_PCA_Mesh_triggered()
```


### slot on_actionExport_Eigenvalues_triggered

```cpp
void on_actionExport_Eigenvalues_triggered()
```


### slot on_actionExport_Eigenvectors_triggered

```cpp
void on_actionExport_Eigenvectors_triggered()
```


### slot on_actionExport_PCA_Mode_Points_triggered

```cpp
void on_actionExport_PCA_Mode_Points_triggered()
```


### slot on_action_preferences_triggered

```cpp
void on_action_preferences_triggered()
```


### slot on_action_export_current_mesh_triggered

```cpp
void on_action_export_current_mesh_triggered()
```


### slot on_action_export_current_particles_triggered

```cpp
void on_action_export_current_particles_triggered()
```


### slot on_action_export_mesh_scalars_triggered

```cpp
void on_action_export_mesh_scalars_triggered()
```


### slot on_action_export_pca_scores_triggered

```cpp
void on_action_export_pca_scores_triggered()
```


### slot action_export_pca_montage_triggered

```cpp
void action_export_pca_montage_triggered()
```


### slot action_export_screenshot_triggered

```cpp
void action_export_screenshot_triggered()
```


### slot action_export_particle_scalars_triggered

```cpp
void action_export_particle_scalars_triggered()
```


### slot action_export_all_subjects_particle_scalars_triggered

```cpp
void action_export_all_subjects_particle_scalars_triggered()
```


### slot on_center_checkbox_stateChanged

```cpp
void on_center_checkbox_stateChanged()
```


### slot on_zoom_slider_valueChanged

```cpp
void on_zoom_slider_valueChanged()
```


### slot on_view_mode_combobox_currentIndexChanged

```cpp
void on_view_mode_combobox_currentIndexChanged(
    QString disp_mode
)
```


### slot on_auto_view_button_clicked

```cpp
void on_auto_view_button_clicked()
```


### slot handle_pca_changed

```cpp
void handle_pca_changed()
```


### slot handle_slider_update

```cpp
void handle_slider_update()
```


### slot handle_project_changed

```cpp
void handle_project_changed()
```


### slot handle_points_changed

```cpp
void handle_points_changed()
```


### slot handle_groom_start

```cpp
void handle_groom_start()
```


### slot handle_groom_complete

```cpp
void handle_groom_complete()
```


### slot handle_optimize_start

```cpp
void handle_optimize_start()
```


### slot handle_optimize_complete

```cpp
void handle_optimize_complete()
```


### slot handle_reconstruction_complete

```cpp
void handle_reconstruction_complete()
```


### slot handle_display_setting_changed

```cpp
void handle_display_setting_changed()
```


### slot handle_glyph_changed

```cpp
void handle_glyph_changed()
```


### slot handle_opacity_changed

```cpp
void handle_opacity_changed()
```


### slot handle_alignment_changed

```cpp
void handle_alignment_changed()
```


### slot handle_open_recent

```cpp
void handle_open_recent()
```


### slot handle_color_scheme

```cpp
void handle_color_scheme()
```


### slot handle_pca_update

```cpp
void handle_pca_update()
```


### slot clear_message

```cpp
void clear_message()
```


### slot handle_message

```cpp
void handle_message(
    std::string str
)
```


### slot handle_error

```cpp
void handle_error(
    std::string str
)
```


### slot handle_warning

```cpp
void handle_warning(
    std::string str
)
```


### slot handle_debug

```cpp
void handle_debug(
    std::string str
)
```


### slot message_callback

```cpp
void message_callback(
    std::string str
)
```


### slot handle_status

```cpp
void handle_status(
    QString str
)
```


### slot handle_progress

```cpp
void handle_progress(
    int amt
)
```


### slot handle_new_mesh

```cpp
void handle_new_mesh()
```


### slot handle_clear_cache

```cpp
void handle_clear_cache()
```


### slot handle_compare_settings_changed

```cpp
void handle_compare_settings_changed()
```


### slot update_feature_map_selection

```cpp
void update_feature_map_selection(
    int index
)
```


### slot update_feature_map_scale

```cpp
void update_feature_map_scale()
```


### slot image_combo_changed

```cpp
void image_combo_changed(
    int index
)
```


### slot show_splash_screen

```cpp
void show_splash_screen()
```


### slot about

```cpp
void about()
```


### slot keyboard_shortcuts

```cpp
void keyboard_shortcuts()
```


### slot toggle_log_window

```cpp
void toggle_log_window()
```


### slot get_py_worker

```cpp
QSharedPointer< PythonWorker > get_py_worker()
```


### slot prefs

```cpp
inline Preferences & prefs()
```


### slot session

```cpp
inline QSharedPointer< Session > session()
```


## Public Functions Documentation

### function ShapeWorksStudioApp

```cpp
ShapeWorksStudioApp()
```


### function ~ShapeWorksStudioApp

```cpp
~ShapeWorksStudioApp()
```


### function closeEvent

```cpp
void closeEvent(
    QCloseEvent * event
) override
```


### function initialize_vtk

```cpp
void initialize_vtk()
```


### function import_files

```cpp
void import_files(
    QStringList file_names
)
```


## Protected Functions Documentation

### function dragEnterEvent

```cpp
void dragEnterEvent(
    QDragEnterEvent * event
) override
```


### function dragLeaveEvent

```cpp
void dragLeaveEvent(
    QDragLeaveEvent * event
) override
```


### function dropEvent

```cpp
void dropEvent(
    QDropEvent * event
) override
```


-------------------------------

Updated on 2022-10-24 at 22:34:04 +0000