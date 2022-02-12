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
#include <Data/Shape.h>
#include <Data/Preferences.h>
#include <Visualization/Visualizer.h>
#include <Visualization/BarGraph.h>

class Ui_AnalysisTool;
class JKQTPlotter;

namespace shapeworks {

class Session;
class Lightbox;
class ShapeWorksStudioApp;
class GroupPvalueJob;

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

  void setLabels(QString which, QString value);

  int getPCAMode();

  int getRPPCAMode();

  double get_group_value();

  double get_pca_value();

  double get_rppca_value();

  bool pcaAnimate();

  bool rppcaAnimate();

  int get_sample_number();

  bool compute_stats();

  void updateSlider();

  void updateRPPCASlider();

  void reset_stats();
  void enable_actions(bool newly_enabled = false);

  StudioParticles get_mean_shape_points();
  ShapeHandle get_mean_shape();

  StudioParticles get_shape_points(int mode, double value);
  // StudioParticles get_rppca_shape_points(int mode, double value);
  ShapeHandle get_mode_shape(int mode, double value);
  // ShapeHandle get_rppca_mode_shape(int mode, double value);

  ParticleShapeStatistics get_stats();
  void load_settings();
  void store_settings();

  void shutdown();

  bool export_variance_graph(QString filename);

  void compute_shape_evaluations();

  static const std::string MODE_ALL_SAMPLES_C;
  static const std::string MODE_MEAN_C;
  static const std::string MODE_PCA_C;
  static const std::string MODE_RPPCA_C;
  static const std::string MODE_SINGLE_SAMPLE_C;
  static const std::string MODE_REGRESSION_C;

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

  //RPPCA
  void on_rppcaSlider_valueChanged();
  void on_rppcaModeSpinBox_valueChanged(int i);
  void handle_rppca_animate_state_changed();
  void handle_rppca_timer();

  void handle_group_animate_state_changed();
  void handle_group_timer();

  void on_linear_radio_toggled(bool b);

  void handle_reconstruction_complete();

  void on_reconstructionButton_clicked();

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

  void handle_group_pvalues_complete();
  void handle_alignment_changed(int new_alignment);

signals:

  void update_view();
  void pca_update();
  void rppca_update();
  void progress(int);
  void message(QString);
  void error(QString);
  void warning(QString);
  void reconstruction_complete();

private:

  void create_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title, QString x_label, QString y_label);

  void compute_reconstructed_domain_transforms();

  bool active_ = false;

  void pca_labels_changed(QString value, QString eigen, QString lambda);
  void rppca_labels_changed(QString value, QString eigen, QString lambda);
  void compute_mode_shape();
  void update_analysis_mode();

  bool group_pvalues_valid();

  //! Break apart combined points into per-domain
  StudioParticles convert_from_combined(const Eigen::VectorXd& points);

  void update_group_boxes();
  void update_group_values();
  void update_domain_alignment_box();

  ShapeHandle create_shape_from_points(StudioParticles points);

  Preferences& preferences_;

  Ui_AnalysisTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;

  /// itk particle shape statistics
  ParticleShapeStatistics stats_;
  bool stats_ready_ = false;
  bool evals_ready_ = false;

  Eigen::VectorXd eval_specificity_;
  Eigen::VectorXd eval_compactness_;
  Eigen::VectorXd eval_generalization_;

  vnl_vector<double> empty_shape_;
  Eigen::VectorXd temp_shape_;

  bool pca_animate_direction_ = true;
  QTimer pca_animate_timer_;

  bool rppca_animate_direction_ = true;
  QTimer rppca_animate_timer_;

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

  AlignmentType current_alignment_{AlignmentType::Local};
};
}
