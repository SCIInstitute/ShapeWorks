---
title: shapeworks::AnalysisTool

---

# shapeworks::AnalysisTool





Inherits from QWidget

## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[GroupAnalysisType](../Classes/classshapeworks_1_1AnalysisTool.md#enum-groupanalysistype)** { None = 0, Pvalues = 1, NetworkAnalysis = 2, LDA = 3} |
| enum| **[McaMode](../Classes/classshapeworks_1_1AnalysisTool.md#enum-mcamode)** { Vanilla, Within, Between} |
| using Analyze::AlignmentType | **[AlignmentType](../Classes/classshapeworks_1_1AnalysisTool.md#using-alignmenttype)**  |
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
| void | **[network_analysis_clicked](../Classes/classshapeworks_1_1AnalysisTool.md#slot-network-analysis-clicked)**() |
| void | **[handle_eval_thread_complete](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-eval-thread-complete)**(ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data) |
| void | **[handle_eval_thread_progress](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-eval-thread-progress)**(ShapeEvaluationJob::JobType job_type, float progress) |
| void | **[handle_eval_particle_normals_progress](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-eval-particle-normals-progress)**(float progress) |
| void | **[handle_eval_particle_normals_complete](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-eval-particle-normals-complete)**(std::vector< bool > good_bad) |
| void | **[handle_group_pvalues_complete](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-group-pvalues-complete)**() |
| void | **[handle_alignment_changed](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-alignment-changed)**(int new_alignment) |
| void | **[handle_distance_method_changed](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-distance-method-changed)**() |
| void | **[run_good_bad_particles](../Classes/classshapeworks_1_1AnalysisTool.md#slot-run-good-bad-particles)**() |
| void | **[handle_lda_progress](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-lda-progress)**(double progress) |
| void | **[handle_lda_complete](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-lda-complete)**() |
| void | **[handle_network_analysis_progress](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-network-analysis-progress)**(int progress) |
| void | **[handle_network_analysis_complete](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-network-analysis-complete)**() |
| void | **[show_difference_to_mean_clicked](../Classes/classshapeworks_1_1AnalysisTool.md#slot-show-difference-to-mean-clicked)**() |
| void | **[group_analysis_combo_changed](../Classes/classshapeworks_1_1AnalysisTool.md#slot-group-analysis-combo-changed)**() |
| void | **[change_pca_analysis_type](../Classes/classshapeworks_1_1AnalysisTool.md#slot-change-pca-analysis-type)**() |
| Eigen::VectorXd | **[construct_mean_shape](../Classes/classshapeworks_1_1AnalysisTool.md#slot-construct-mean-shape)**()<br>Compute the mean shape outside of the PCA in case we are using scalars only.  |
| void | **[handle_samples_predicted_scalar_options](../Classes/classshapeworks_1_1AnalysisTool.md#slot-handle-samples-predicted-scalar-options)**() |
| void | **[samples_table_context_menu](../Classes/classshapeworks_1_1AnalysisTool.md#slot-samples-table-context-menu)**() |
| void | **[samples_table_copy_to_clipboard](../Classes/classshapeworks_1_1AnalysisTool.md#slot-samples-table-copy-to-clipboard)**() |

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[update_view](../Classes/classshapeworks_1_1AnalysisTool.md#signal-update-view)**() |
| void | **[pca_update](../Classes/classshapeworks_1_1AnalysisTool.md#signal-pca-update)**() |
| void | **[progress](../Classes/classshapeworks_1_1AnalysisTool.md#signal-progress)**(int ) |
| void | **[reconstruction_complete](../Classes/classshapeworks_1_1AnalysisTool.md#signal-reconstruction-complete)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[AnalysisTool](../Classes/classshapeworks_1_1AnalysisTool.md#function-analysistool)**([Preferences](../Classes/classPreferences.md) & prefs) |
| | **[~AnalysisTool](../Classes/classshapeworks_1_1AnalysisTool.md#function-~analysistool)**() |
| void | **[set_session](../Classes/classshapeworks_1_1AnalysisTool.md#function-set-session)**(QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > session)<br>set the pointer to the session  |
| QSharedPointer< [Session](../Classes/classshapeworks_1_1Session.md) > | **[get_session](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-session)**()<br>Return the session.  |
| void | **[set_app](../Classes/classshapeworks_1_1AnalysisTool.md#function-set-app)**([ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md) * app)<br>set the pointer to the application  |
| [ShapeWorksStudioApp](../Classes/classshapeworks_1_1ShapeWorksStudioApp.md) * | **[get_app](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-app)**() |
| void | **[set_active](../Classes/classshapeworks_1_1AnalysisTool.md#function-set-active)**(bool active)<br>Set if this tool is active.  |
| bool | **[get_active](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-active)**()<br>Return if this tool is active.  |
| bool | **[get_group_difference_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-group-difference-mode)**() |
| std::vector< [Shape::Point](../Classes/classshapeworks_1_1Shape_1_1Point.md) > | **[get_group_difference_vectors](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-group-difference-vectors)**() |
| std::string | **[get_analysis_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-analysis-mode)**() |
| void | **[set_analysis_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-set-analysis-mode)**(std::string mode) |
| void | **[set_labels](../Classes/classshapeworks_1_1AnalysisTool.md#function-set-labels)**(QString which, QString value) |
| int | **[get_pca_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-pca-mode)**() |
| double | **[get_group_ratio](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-group-ratio)**() |
| double | **[get_pca_value](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-pca-value)**() |
| bool | **[pca_animate](../Classes/classshapeworks_1_1AnalysisTool.md#function-pca-animate)**() |
| McaMode | **[get_mca_level](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-mca-level)**() const |
| int | **[get_sample_number](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-sample-number)**() |
| bool | **[compute_stats](../Classes/classshapeworks_1_1AnalysisTool.md#function-compute-stats)**() |
| void | **[update_slider](../Classes/classshapeworks_1_1AnalysisTool.md#function-update-slider)**() |
| void | **[reset_stats](../Classes/classshapeworks_1_1AnalysisTool.md#function-reset-stats)**() |
| void | **[enable_actions](../Classes/classshapeworks_1_1AnalysisTool.md#function-enable-actions)**(bool newly_enabled =false) |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_mean_shape_points](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-mean-shape-points)**() |
| ShapeHandle | **[get_mean_shape](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-mean-shape)**() |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_shape_points](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-shape-points)**(int mode, double value) |
| [Particles](../Classes/classshapeworks_1_1Particles.md) | **[get_multi_level_shape_points](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-multi-level-shape-points)**(int mode, double value, McaMode level) |
| ShapeHandle | **[get_mode_shape](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-mode-shape)**(int mode, double value) |
| ShapeHandle | **[get_mca_mode_shape](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-mca-mode-shape)**(int mode, double value, McaMode level) |
| ShapeHandle | **[get_current_shape](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-current-shape)**() |
| [ParticleShapeStatistics](../Classes/classshapeworks_1_1ParticleShapeStatistics.md) | **[get_stats](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-stats)**() |
| void | **[load_settings](../Classes/classshapeworks_1_1AnalysisTool.md#function-load-settings)**() |
| void | **[store_settings](../Classes/classshapeworks_1_1AnalysisTool.md#function-store-settings)**() |
| void | **[shutdown](../Classes/classshapeworks_1_1AnalysisTool.md#function-shutdown)**() |
| bool | **[export_variance_graph](../Classes/classshapeworks_1_1AnalysisTool.md#function-export-variance-graph)**(QString filename) |
| void | **[compute_shape_evaluations](../Classes/classshapeworks_1_1AnalysisTool.md#function-compute-shape-evaluations)**() |
| GroupAnalysisType | **[get_group_analysis_type](../Classes/classshapeworks_1_1AnalysisTool.md#function-get-group-analysis-type)**() |
| bool | **[pca_scalar_only_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-pca-scalar-only-mode)**() |
| bool | **[pca_shape_plus_scalar_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-pca-shape-plus-scalar-mode)**() |
| bool | **[pca_shape_only_mode](../Classes/classshapeworks_1_1AnalysisTool.md#function-pca-shape-only-mode)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const std::string | **[MODE_ALL_SAMPLES_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-all-samples-c)**  |
| const std::string | **[MODE_MEAN_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-mean-c)**  |
| const std::string | **[MODE_PCA_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-pca-c)**  |
| const std::string | **[MODE_SINGLE_SAMPLE_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-single-sample-c)**  |
| const std::string | **[MODE_REGRESSION_C](../Classes/classshapeworks_1_1AnalysisTool.md#variable-mode-regression-c)**  |

## Public Types Documentation

### enum GroupAnalysisType

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| None | 0|   |
| Pvalues | 1|   |
| NetworkAnalysis | 2|   |
| LDA | 3|   |




### enum McaMode

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Vanilla | |   |
| Within | |   |
| Between | |   |




### using AlignmentType

```cpp
using shapeworks::AnalysisTool::AlignmentType =  Analyze::AlignmentType;
```


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


### slot network_analysis_clicked

```cpp
void network_analysis_clicked()
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


### slot handle_eval_particle_normals_progress

```cpp
void handle_eval_particle_normals_progress(
    float progress
)
```


### slot handle_eval_particle_normals_complete

```cpp
void handle_eval_particle_normals_complete(
    std::vector< bool > good_bad
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


### slot handle_distance_method_changed

```cpp
void handle_distance_method_changed()
```


### slot run_good_bad_particles

```cpp
void run_good_bad_particles()
```


### slot handle_lda_progress

```cpp
void handle_lda_progress(
    double progress
)
```


### slot handle_lda_complete

```cpp
void handle_lda_complete()
```


### slot handle_network_analysis_progress

```cpp
void handle_network_analysis_progress(
    int progress
)
```


### slot handle_network_analysis_complete

```cpp
void handle_network_analysis_complete()
```


### slot show_difference_to_mean_clicked

```cpp
void show_difference_to_mean_clicked()
```


### slot group_analysis_combo_changed

```cpp
void group_analysis_combo_changed()
```


### slot change_pca_analysis_type

```cpp
void change_pca_analysis_type()
```


### slot construct_mean_shape

```cpp
Eigen::VectorXd construct_mean_shape()
```

Compute the mean shape outside of the PCA in case we are using scalars only. 

### slot handle_samples_predicted_scalar_options

```cpp
void handle_samples_predicted_scalar_options()
```


### slot samples_table_context_menu

```cpp
void samples_table_context_menu()
```


### slot samples_table_copy_to_clipboard

```cpp
void samples_table_copy_to_clipboard()
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

### function get_session

```cpp
QSharedPointer< Session > get_session()
```

Return the session. 

### function set_app

```cpp
void set_app(
    ShapeWorksStudioApp * app
)
```

set the pointer to the application 

### function get_app

```cpp
inline ShapeWorksStudioApp * get_app()
```


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


### function set_labels

```cpp
void set_labels(
    QString which,
    QString value
)
```


### function get_pca_mode

```cpp
int get_pca_mode()
```


### function get_group_ratio

```cpp
double get_group_ratio()
```


### function get_pca_value

```cpp
double get_pca_value()
```


### function pca_animate

```cpp
bool pca_animate()
```


### function get_mca_level

```cpp
McaMode get_mca_level() const
```


### function get_sample_number

```cpp
int get_sample_number()
```


### function compute_stats

```cpp
bool compute_stats()
```


### function update_slider

```cpp
void update_slider()
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
Particles get_mean_shape_points()
```


### function get_mean_shape

```cpp
ShapeHandle get_mean_shape()
```


### function get_shape_points

```cpp
Particles get_shape_points(
    int mode,
    double value
)
```


### function get_multi_level_shape_points

```cpp
Particles get_multi_level_shape_points(
    int mode,
    double value,
    McaMode level
)
```


### function get_mode_shape

```cpp
ShapeHandle get_mode_shape(
    int mode,
    double value
)
```


### function get_mca_mode_shape

```cpp
ShapeHandle get_mca_mode_shape(
    int mode,
    double value,
    McaMode level
)
```


### function get_current_shape

```cpp
ShapeHandle get_current_shape()
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


### function get_group_analysis_type

```cpp
GroupAnalysisType get_group_analysis_type()
```


### function pca_scalar_only_mode

```cpp
bool pca_scalar_only_mode()
```


### function pca_shape_plus_scalar_mode

```cpp
bool pca_shape_plus_scalar_mode()
```


### function pca_shape_only_mode

```cpp
bool pca_shape_only_mode()
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

Updated on 2024-07-26 at 20:42:13 +0000