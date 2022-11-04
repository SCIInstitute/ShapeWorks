#pragma once

#include <Eigen/Dense>
#include <Eigen/Sparse>

// Qt
#include <QSharedPointer>
#include <QWidget>

// ShapeWorks
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
class StatsGroupLDAJob;

class AnalysisTool : public QWidget {
  Q_OBJECT;

 public:
  enum AlignmentType {
    Global = -2,
    Local = -1,
  };

  using PointType = itk::Point<double, 3>;

  AnalysisTool(Preferences& prefs);
  ~AnalysisTool();

  /// set the pointer to the session
  void set_session(QSharedPointer<Session> session);

  //! Return the session
  QSharedPointer<Session> get_session();

  /// set the pointer to the application
  void set_app(ShapeWorksStudioApp* app);

  //! Set if this tool is active
  void set_active(bool active);

  //! Return if this tool is active
  bool get_active();

  bool get_group_difference_mode();

  std::vector<Shape::Point> get_group_difference_vectors();

  std::string get_analysis_mode();
  void set_analysis_mode(std::string mode);

  void set_labels(QString which, QString value);

  int get_pca_mode();
  int getMCAMode();

  double get_group_ratio();

  double get_pca_value();
  double get_mca_value();

  bool pca_animate();
  int get_mca_level();
  bool mcaAnimate();

  int get_sample_number();

  bool compute_stats();

  void update_slider();
  void updateMcaSlider();

  void reset_stats();
  void enable_actions(bool newly_enabled = false);

  Particles get_mean_shape_points();
  ShapeHandle get_mean_shape();

  Particles get_shape_points(int mode, double value);
  Particles get_multi_level_shape_points(int mode, double value, int level);
  ShapeHandle get_mode_shape(int mode, double value);
  ShapeHandle get_mca_mode_shape(int mode, double value, int level);

  ParticleShapeStatistics get_stats();
  void load_settings();
  void store_settings();

  void shutdown();

  bool export_variance_graph(QString filename);

  void compute_shape_evaluations();

  static const std::string MODE_ALL_SAMPLES_C;
  static const std::string MODE_MEAN_C;
  static const std::string MODE_PCA_C;
  static const std::string MODE_MCA_C;
  static const std::string MODE_SINGLE_SAMPLE_C;
  static const std::string MODE_REGRESSION_C;
  static const unsigned int MCA_TAB_INDEX;

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


  //MLCA
  void on_mcaSlider_valueChanged();
  void on_mcaModeSpinBox_valueChanged(int i);
  void handle_mca_animate_state_changed();
  void handle_mca_timer();
  void mca_between_radio_toggled();
  void mca_within_radio_toggled();

  //! Set the currently selected feature map
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

  void handle_eval_thread_complete(ShapeEvaluationJob::JobType job_type, Eigen::VectorXd data);
  void handle_eval_thread_progress(ShapeEvaluationJob::JobType job_type, float progress);
  void handle_eval_particle_normals_progress(float progress);
  void handle_eval_particle_normals_complete(std::vector<bool> good_bad);

  void handle_group_pvalues_complete();
  void handle_alignment_changed(int new_alignment);

  void run_good_bad_particles();

  void handle_lda_progress(double progress);
  void handle_lda_complete();

  void show_difference_to_mean_clicked();

 Q_SIGNALS:

  void update_view();
  void pca_update();
  void mca_update();
  void progress(int);
  void reconstruction_complete();

 private:
  void create_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title, QString x_label, QString y_label);

  void compute_reconstructed_domain_transforms();

  bool active_ = false;

  void pca_labels_changed(QString value, QString eigen, QString lambda);
  void mca_labels_changed(QString value, QString eigen, QString lambda);
  void compute_mca_mode_shape();
  void compute_mode_shape();
  void update_analysis_mode();
  void update_interface();

  bool group_pvalues_valid();

  //! Break apart combined points into per-domain
  Particles convert_from_combined(const Eigen::VectorXd& points);

  void update_group_boxes();
  void update_group_values();
  void update_domain_alignment_box();

  void update_lda_graph();

  void update_difference_particles();

  ShapeHandle create_shape_from_points(Particles points);

  Preferences& preferences_;

  Ui_AnalysisTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;

  /// itk particle shape statistics
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
  std::vector<int> number_of_particles_ar;

  bool pca_animate_direction_ = true;
  QTimer pca_animate_timer_;

  bool mca_animate_direction_ = true;
  QTimer mca_animate_timer_;

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
  bool group_lda_job_running_ = false;
  bool block_group_change_ = false;

  AlignmentType current_alignment_{AlignmentType::Local};
};
}  // namespace shapeworks
