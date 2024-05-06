---
title: Studio/Analysis/AnalysisTool.h

---

# Studio/Analysis/AnalysisTool.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::AnalysisTool](../Classes/classshapeworks_1_1AnalysisTool.md)**  |




## Source code

```cpp
#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

// Qt
#include <QSharedPointer>
#include <QWidget>

// ShapeWorks
#include <Analyze/Analyze.h>
#include <ParticleShapeStatistics.h>

// Studio
#include <Analysis/ShapeEvaluationJob.h>
#include <Data/Preferences.h>
#include <Shape.h>
#include <Visualization/Visualizer.h>

class Ui_AnalysisTool;
class JKQTPlotter;

namespace shapeworks {

class Session;
class Lightbox;
class ShapeWorksStudioApp;
class GroupPvalueJob;
class NetworkAnalysisJob;
class StatsGroupLDAJob;
class ParticleAreaPanel;
class ShapeScalarPanel;

class AnalysisTool : public QWidget {
  Q_OBJECT;

 public:
  using AlignmentType = Analyze::AlignmentType;

  enum GroupAnalysisType { None = 0, Pvalues = 1, NetworkAnalysis = 2, LDA = 3 };

  enum McaMode { Vanilla, Within, Between };

  using PointType = itk::Point<double, 3>;

  AnalysisTool(Preferences& prefs);
  ~AnalysisTool();

  void set_session(QSharedPointer<Session> session);

  QSharedPointer<Session> get_session();

  void set_app(ShapeWorksStudioApp* app);
  ShapeWorksStudioApp* get_app() { return app_; }

  void set_active(bool active);

  bool get_active();

  bool get_group_difference_mode();

  std::vector<Shape::Point> get_group_difference_vectors();

  std::string get_analysis_mode();
  void set_analysis_mode(std::string mode);

  void set_labels(QString which, QString value);

  int get_pca_mode();

  double get_group_ratio();

  double get_pca_value();

  bool pca_animate();
  McaMode get_mca_level() const;

  int get_sample_number();

  bool compute_stats();

  void update_slider();

  void reset_stats();
  void enable_actions(bool newly_enabled = false);

  Particles get_mean_shape_points();
  ShapeHandle get_mean_shape();

  Particles get_shape_points(int mode, double value);
  Particles get_multi_level_shape_points(int mode, double value, McaMode level);
  ShapeHandle get_mode_shape(int mode, double value);
  ShapeHandle get_mca_mode_shape(int mode, double value, McaMode level);
  ShapeHandle get_current_shape();

  ParticleShapeStatistics get_stats();
  void load_settings();
  void store_settings();

  void shutdown();

  bool export_variance_graph(QString filename);

  void compute_shape_evaluations();

  static const std::string MODE_ALL_SAMPLES_C;
  static const std::string MODE_MEAN_C;
  static const std::string MODE_PCA_C;
  static const std::string MODE_SINGLE_SAMPLE_C;
  static const std::string MODE_REGRESSION_C;

  GroupAnalysisType get_group_analysis_type();

  bool pca_scalar_only_mode();
  bool pca_shape_plus_scalar_mode();
  bool pca_shape_only_mode();

 public Q_SLOTS:

  // analysis mode
  void on_tabWidget_currentChanged();

  void handle_analysis_options();
  void handle_median();

  void on_mean_button_clicked();
  void on_group1_button_clicked();
  void on_group2_button_clicked();
  void on_difference_button_clicked();

  // PCA
  void on_pcaSlider_valueChanged();
  void on_group_slider_valueChanged();
  void on_pcaModeSpinBox_valueChanged(int i);

  void handle_pca_animate_state_changed();
  void handle_pca_timer();

  void handle_group_animate_state_changed();
  void handle_group_timer();

  void handle_reconstruction_complete();

  void on_reconstructionButton_clicked();

  void set_feature_map(const std::string& feature_map);

  std::string get_display_feature_map();

  void group_changed();

  bool groups_active();

  void on_view_open_button_toggled();

  void on_surface_open_button_toggled();

  void on_metrics_open_button_toggled();

  bool is_group_active(int shape_index);

  void reconstruction_method_changed();

  void initialize_mesh_warper();

  void group_p_values_clicked();
  void network_analysis_clicked();

  void handle_eval_thread_complete(ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data);
  void handle_eval_thread_progress(ShapeEvaluationJob::JobType job_type, float progress);
  void handle_eval_particle_normals_progress(float progress);
  void handle_eval_particle_normals_complete(std::vector<bool> good_bad);

  void handle_group_pvalues_complete();
  void handle_alignment_changed(int new_alignment);

  void run_good_bad_particles();

  void handle_lda_progress(double progress);
  void handle_lda_complete();

  void handle_network_analysis_progress(int progress);
  void handle_network_analysis_complete();

  void show_difference_to_mean_clicked();

  void group_analysis_combo_changed();

  void change_pca_analysis_type();

  Eigen::VectorXd construct_mean_shape();

  void handle_samples_predicted_scalar_options();

 Q_SIGNALS:

  void update_view();
  void pca_update();
  void progress(int);
  void reconstruction_complete();

 private:
  void compute_reconstructed_domain_transforms();

  bool active_ = false;

  void pca_labels_changed(QString value, QString eigen, QString lambda);
  void update_analysis_mode();
  void update_interface();

  bool group_pvalues_valid();
  bool groups_on();

  Particles convert_from_combined(const Eigen::VectorXd& points);

  void update_group_boxes();
  void update_group_values();
  void update_domain_alignment_box();

  void update_lda_graph();

  void update_difference_particles();

  Eigen::VectorXd get_mean_shape_particles();

  ShapeHandle create_shape_from_points(Particles points);

  Preferences& preferences_;

  Ui_AnalysisTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;

  ParticleShapeStatistics stats_;
  bool stats_ready_ = false;
  bool evals_ready_ = false;
  bool large_particle_disclaimer_waived_ = false;
  bool skip_evals_ = false;

  Eigen::VectorXd eval_specificity_;
  Eigen::VectorXd eval_compactness_;
  Eigen::VectorXd eval_generalization_;

  vnl_vector<double> empty_shape_;
  Eigen::VectorXd temp_shape_;
  Eigen::VectorXd temp_shape_mca;
  std::vector<int> number_of_particles_array_;

  Eigen::VectorXd computed_scalars_;

  bool pca_animate_direction_ = true;
  QTimer pca_animate_timer_;

  bool group_animate_direction_ = true;
  QTimer group_animate_timer_;

  ShapeHandle computed_shape_;

  ShapeList group1_list_;
  ShapeList group2_list_;

  std::string feature_map_;

  std::vector<std::string> current_group_names_;
  std::vector<std::string> current_group_values_;

  std::vector<vtkSmartPointer<vtkTransform>> reconstruction_transforms_;

  QSharedPointer<GroupPvalueJob> group_pvalue_job_;
  QSharedPointer<StatsGroupLDAJob> group_lda_job_;
  QSharedPointer<NetworkAnalysisJob> network_analysis_job_;

  bool group_lda_job_running_ = false;
  bool lda_computed_ = false;
  bool block_group_change_ = false;

  AlignmentType current_alignment_{AlignmentType::Local};
  ParticleAreaPanel* particle_area_panel_{nullptr};
  ShapeScalarPanel* shape_scalar_panel_{nullptr};
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-06 at 23:55:36 +0000
