---
title: shapeworks::AnalysisTool

---

# shapeworks::AnalysisTool





Inherits from QWidget

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[AlignmentType](../Classes/classshapeworks_1_1AnalysisTool.md#enum-alignmenttype)** { Global = -2, Local = -1} |
| using itk::Point< double, 3 > | **[PointType](../Classes/classshapeworks_1_1AnalysisTool.md#using-pointtype)**  |

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[on_tabWidget_currentChanged](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-tabwidget-currentchanged)**() |
| void | **[handle_analysis_options](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-analysis-options)**() |
| void | **[handle_median](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-median)**() |
| void | **[on_mean_button_clicked](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-mean-button-clicked)**() |
| void | **[on_group1_button_clicked](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-group1-button-clicked)**() |
| void | **[on_group2_button_clicked](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-group2-button-clicked)**() |
| void | **[on_difference_button_clicked](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-difference-button-clicked)**() |
| void | **[on_pcaSlider_valueChanged](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-pcaslider-valuechanged)**() |
| void | **[on_group_slider_valueChanged](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-group-slider-valuechanged)**() |
| void | **[on_pcaModeSpinBox_valueChanged](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-pcamodespinbox-valuechanged)**(int i) |
| void | **[handle_pca_animate_state_changed](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-pca-animate-state-changed)**() |
| void | **[handle_pca_timer](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-pca-timer)**() |
| void | **[handle_group_animate_state_changed](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-group-animate-state-changed)**() |
| void | **[handle_group_timer](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-group-timer)**() |
| void | **[on_linear_radio_toggled](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-linear-radio-toggled)**(bool b) |
| void | **[handle_reconstruction_complete](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-reconstruction-complete)**() |
| void | **[on_reconstructionButton_clicked](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-reconstructionbutton-clicked)**() |
| void | **[set_feature_map](../Classes/classshapeworks_1_1AnalysisTool.md#slot-set-feature-map)**(const std::string & feature_map)<br>Set the currently selected feature map.  |
| std::string | **[get_display_feature_map](../Classes/classshapeworks_1_1AnalysisTool.md#slot-get-display-feature-map)**() |
| void | **[group_changed](../Classes/classshapeworks_1_1AnalysisTool.md#slot-group-changed)**() |
| bool | **[groups_active](../Classes/classshapeworks_1_1AnalysisTool.md#slot-groups-active)**() |
| void | **[on_view_open_button_toggled](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-view-open-button-toggled)**() |
| void | **[on_surface_open_button_toggled](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-surface-open-button-toggled)**() |
| void | **[on_metrics_open_button_toggled](../Classes/classshapeworks_1_1AnalysisTool.md#slot-on-metrics-open-button-toggled)**() |
| bool | **[is_group_active](../Classes/classshapeworks_1_1AnalysisTool.md#slot-is-group-active)**(int shape_index) |
| void | **[reconstruction_method_changed](../Classes/classshapeworks_1_1AnalysisTool.md#slot-reconstruction-method-changed)**() |
| void | **[initialize_mesh_warper](../Classes/classshapeworks_1_1AnalysisTool.md#slot-initialize-mesh-warper)**() |
| void | **[group_p_values_clicked](../Classes/classshapeworks_1_1AnalysisTool.md#slot-group-p-values-clicked)**() |
| void | **[handle_eval_thread_complete](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-eval-thread-complete)**(ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data) |
| void | **[handle_eval_thread_progress](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-eval-thread-progress)**(ShapeEvaluationJob::JobType job_type, float progress) |
| void | **[handle_group_pvalues_complete](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-group-pvalues-complete)**() |
| void | **[handle_alignment_changed](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-alignment-changed)**(int new_alignment) |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[update_view](../Classes/classshapeworks_1_1AnalysisTool.md#signal-update-view)**() |
| void | **[pca_update](../Classes/classshapeworks_1_1AnalysisTool.md#signal-pca-update)**() |
| void | **[progress](../Classes/classshapeworks_1_1AnalysisTool.md#signal-progress)**(int ) |
| void | **[message](../Classes/classshapeworks_1_1AnalysisTool.md#signal-message)**(QString ) |
| void | **[error](../Classes/classshapeworks_1_1AnalysisTool.md#signal-error)**(QString ) |
| void | **[warning](../Classes/classshapeworks_1_1AnalysisTool.md#signal-warning)**(QString ) |
| void | **[reconstruction_complete](../Classes/classshapeworks_1_1AnalysisTool.md#signal-reconstruction-complete)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[AnalysisTool](../Classes/classshapeworks_1_1AnalysisTool.md#function-analysistool)**([Preferences](../Classes/classPreferences.md) & prefs) |
| | **[~AnalysisTool](../Classes/classshapeworks_1_1AnalysisTool.md#function-~analysistool)**() |
| void | **[set_session](../Classes/classshapeworks_1_1AnalysisTool.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session)<br>set the pointer to the session  |
| void | **[set_app](../Classes/classshapeworks_1_1AnalysisTool.md#function-set-app)**([ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md) * app)<br>set the pointer to the application  |
| void | **[set_active](../Classes/classshapeworks_1_1AnalysisTool.md#function-set-active)**(bool active)<br>Set if this tool is active.  |
| bool | **[get_active](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-active)**()<br>Return if this tool is active.  |
| bool | **[get_group_difference_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-group-difference-mode)**() |
| std::vector< [Shape::Point](../Classes/classshapeworks_1_1Shape_1_1Point.md) > | **[get_group_difference_vectors](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-group-difference-vectors)**() |
| std::string | **[get_analysis_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-analysis-mode)**() |
| void | **[set_analysis_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-set-analysis-mode)**(std::string mode) |
| void | **[setLabels](../Classes/classshapeworks_1_1AnalysisTool.md#function-setlabels)**(QString which, QString value) |
| int | **[getPCAMode](../Classes/classshapeworks_1_1AnalysisTool.md#function-getpcamode)**() |
| double | **[get_group_value](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-group-value)**() |
| double | **[get_pca_value](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-pca-value)**() |
| bool | **[pcaAnimate](../Classes/classshapeworks_1_1AnalysisTool.md#function-pcaanimate)**() |
| int | **[get_sample_number](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-sample-number)**() |
| bool | **[compute_stats](../Classes/classshapeworks_1_1AnalysisTool.md#function-compute-stats)**() |
| void | **[updateSlider](../Classes/classshapeworks_1_1AnalysisTool.md#function-updateslider)**() |
| void | **[reset_stats](../Classes/classshapeworks_1_1AnalysisTool.md#function-reset-stats)**() |
| void | **[enable_actions](../Classes/classshapeworks_1_1AnalysisTool.md#function-enable-actions)**(bool newly_enabled =false) |
| [StudioParticles](../Classes/classshapeworks_1_1StudioParticles.md) | **[get_mean_shape_points](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-mean-shape-points)**() |
| ShapeHandle | **[get_mean_shape](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-mean-shape)**() |
| [StudioParticles](../Classes/classshapeworks_1_1StudioParticles.md) | **[get_shape_points](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-shape-points)**(int mode, double value) |
| ShapeHandle | **[get_mode_shape](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-mode-shape)**(int mode, double value) |
| [ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md) | **[get_stats](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-stats)**() |
| void | **[load_settings](../Classes/classshapeworks_1_1AnalysisTool.md#function-load-settings)**() |
| void | **[store_settings](../Classes/classshapeworks_1_1AnalysisTool.md#function-store-settings)**() |
| void | **[shutdown](../Classes/classshapeworks_1_1AnalysisTool.md#function-shutdown)**() |
| bool | **[export_variance_graph](../Classes/classshapeworks_1_1AnalysisTool.md#function-export-variance-graph)**(QString filename) |
| void | **[compute_shape_evaluations](../Classes/classshapeworks_1_1AnalysisTool.md#function-compute-shape-evaluations)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const std::string | **[MODE_ALL_SAMPLES_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-all-samples-c)**  |
| const std::string | **[MODE_MEAN_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-mean-c)**  |
| const std::string | **[MODE_PCA_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-pca-c)**  |
| const std::string | **[MODE_SINGLE_SAMPLE_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-single-sample-c)**  |
| const std::string | **[MODE_REGRESSION_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-regression-c)**  |

## Public Types Documentation

### enum AlignmentType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Global | -2|   |
| Local | -1|   |




### using PointType

```cpp
using shapeworks::AnalysisTool::PointType =  itk::Point<double, 3>;
```


## Public Slots Documentation

### slot on_tabWidget_currentChanged

```cpp
void on_tabWidget_currentChanged()
```


### slot handle_analysis_options

```cpp
void handle_analysis_options()
```


### slot handle_median

```cpp
void handle_median()
```


### slot on_mean_button_clicked

```cpp
void on_mean_button_clicked()
```


### slot on_group1_button_clicked

```cpp
void on_group1_button_clicked()
```


### slot on_group2_button_clicked

```cpp
void on_group2_button_clicked()
```


### slot on_difference_button_clicked

```cpp
void on_difference_button_clicked()
```


### slot on_pcaSlider_valueChanged

```cpp
void on_pcaSlider_valueChanged()
```


### slot on_group_slider_valueChanged

```cpp
void on_group_slider_valueChanged()
```


### slot on_pcaModeSpinBox_valueChanged

```cpp
void on_pcaModeSpinBox_valueChanged(
    int i
)
```


### slot handle_pca_animate_state_changed

```cpp
void handle_pca_animate_state_changed()
```


### slot handle_pca_timer

```cpp
void handle_pca_timer()
```


### slot handle_group_animate_state_changed

```cpp
void handle_group_animate_state_changed()
```


### slot handle_group_timer

```cpp
void handle_group_timer()
```


### slot on_linear_radio_toggled

```cpp
void on_linear_radio_toggled(
    bool b
)
```


### slot handle_reconstruction_complete

```cpp
void handle_reconstruction_complete()
```


### slot on_reconstructionButton_clicked

```cpp
void on_reconstructionButton_clicked()
```


### slot set_feature_map

```cpp
void set_feature_map(
    const std::string & feature_map
)
```

Set the currently selected feature map. 

### slot get_display_feature_map

```cpp
std::string get_display_feature_map()
```


### slot group_changed

```cpp
void group_changed()
```


### slot groups_active

```cpp
bool groups_active()
```


### slot on_view_open_button_toggled

```cpp
void on_view_open_button_toggled()
```


### slot on_surface_open_button_toggled

```cpp
void on_surface_open_button_toggled()
```


### slot on_metrics_open_button_toggled

```cpp
void on_metrics_open_button_toggled()
```


### slot is_group_active

```cpp
bool is_group_active(
    int shape_index
)
```


### slot reconstruction_method_changed

```cpp
void reconstruction_method_changed()
```


### slot initialize_mesh_warper

```cpp
void initialize_mesh_warper()
```


### slot group_p_values_clicked

```cpp
void group_p_values_clicked()
```


### slot handle_eval_thread_complete

```cpp
void handle_eval_thread_complete(
    ShapeEvaluationJob::JobType job_type,
    Eigen::VectorXd data
)
```


### slot handle_eval_thread_progress

```cpp
void handle_eval_thread_progress(
    ShapeEvaluationJob::JobType job_type,
    float progress
)
```


### slot handle_group_pvalues_complete

```cpp
void handle_group_pvalues_complete()
```


### slot handle_alignment_changed

```cpp
void handle_alignment_changed(
    int new_alignment
)
```


## Public Signals Documentation

### signal update_view

```cpp
void update_view()
```


### signal pca_update

```cpp
void pca_update()
```


### signal progress

```cpp
void progress(
    int 
)
```


### signal message

```cpp
void message(
    QString 
)
```


### signal error

```cpp
void error(
    QString 
)
```


### signal warning

```cpp
void warning(
    QString 
)
```


### signal reconstruction_complete

```cpp
void reconstruction_complete()
```


## Public Functions Documentation

### function AnalysisTool

```cpp
AnalysisTool(
    Preferences & prefs
)
```


### function ~AnalysisTool

```cpp
~AnalysisTool()
```


### function set_session

```cpp
void set_session(
    QSharedPointer< Session > session
)
```

set the pointer to the session 

### function set_app

```cpp
void set_app(
    ShapeWorksStudioApp * app
)
```

set the pointer to the application 

### function set_active

```cpp
void set_active(
    bool active
)
```

Set if this tool is active. 

### function get_active

```cpp
bool get_active()
```

Return if this tool is active. 

### function get_group_difference_mode

```cpp
bool get_group_difference_mode()
```


### function get_group_difference_vectors

```cpp
std::vector< Shape::Point > get_group_difference_vectors()
```


### function get_analysis_mode

```cpp
std::string get_analysis_mode()
```


### function set_analysis_mode

```cpp
void set_analysis_mode(
    std::string mode
)
```


### function setLabels

```cpp
void setLabels(
    QString which,
    QString value
)
```


### function getPCAMode

```cpp
int getPCAMode()
```


### function get_group_value

```cpp
double get_group_value()
```


### function get_pca_value

```cpp
double get_pca_value()
```


### function pcaAnimate

```cpp
bool pcaAnimate()
```


### function get_sample_number

```cpp
int get_sample_number()
```


### function compute_stats

```cpp
bool compute_stats()
```


### function updateSlider

```cpp
void updateSlider()
```


### function reset_stats

```cpp
void reset_stats()
```


### function enable_actions

```cpp
void enable_actions(
    bool newly_enabled =false
)
```


### function get_mean_shape_points

```cpp
StudioParticles get_mean_shape_points()
```


### function get_mean_shape

```cpp
ShapeHandle get_mean_shape()
```


### function get_shape_points

```cpp
StudioParticles get_shape_points(
    int mode,
    double value
)
```


### function get_mode_shape

```cpp
ShapeHandle get_mode_shape(
    int mode,
    double value
)
```


### function get_stats

```cpp
ParticleShapeStatistics get_stats()
```


### function load_settings

```cpp
void load_settings()
```


### function store_settings

```cpp
void store_settings()
```


### function shutdown

```cpp
void shutdown()
```


### function export_variance_graph

```cpp
bool export_variance_graph(
    QString filename
)
```


### function compute_shape_evaluations

```cpp
void compute_shape_evaluations()
```


## Public Attributes Documentation

### variable MODE_ALL_SAMPLES_C

```cpp
static const std::string MODE_ALL_SAMPLES_C;
```


### variable MODE_MEAN_C

```cpp
static const std::string MODE_MEAN_C;
```


### variable MODE_PCA_C

```cpp
static const std::string MODE_PCA_C;
```


### variable MODE_SINGLE_SAMPLE_C

```cpp
static const std::string MODE_SINGLE_SAMPLE_C;
```


### variable MODE_REGRESSION_C

```cpp
static const std::string MODE_REGRESSION_C;
```


-------------------------------

Updated on 2022-01-14 at 16:56:16 +0000