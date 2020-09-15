#pragma once

// Qt
#include <QSharedPointer>
#include <QWidget>

// ShapeWorks
#include <itkParticleShapeStatistics.h>

// Studio
#include <Data/Shape.h>
#include <Data/Preferences.h>
#include <Visualization/Visualizer.h>
#include <itkParticleShapeStatistics.h>
#include <Visualization/BarGraph.h>

class Session;
class Lightbox;
class ShapeWorksStudioApp;
class Ui_AnalysisTool;

class AnalysisTool : public QWidget {
Q_OBJECT;
public:

  using PointType = itk::Point<double, 3>;

  AnalysisTool(Preferences& prefs);
  ~AnalysisTool();

  /// set the pointer to the session
  void set_session(QSharedPointer<Session> session);

  /// set the pointer to the application
  void set_app(ShapeWorksStudioApp* app);

  bool get_group_difference_mode();

  std::vector<Point> get_group_difference_vectors();

  std::string get_analysis_mode();
  void set_analysis_mode(std::string mode);

  void setLabels(QString which, QString value);

  int getPCAMode();

  double get_group_value();

  double get_pca_value();

  bool pcaAnimate();
  bool groupAnimate();

  int get_sample_number();

  bool compute_stats();

  void updateSlider();

  void reset_stats();
  void enable_actions();

  const vnl_vector<double>& get_mean_shape_points();
  ShapeHandle get_mean_shape();

  const vnl_vector<double>& get_shape_points(int mode, double value, double group_value = 0.5);

  ShapeHandle get_shape(int mode, double value, double group_value = 0.5);

  ParticleShapeStatistics<3> get_stats();
  void load_settings();
  void store_settings();

  void shutdown();

  bool export_variance_graph(QString filename);

  static const std::string MODE_ALL_SAMPLES_C;
  static const std::string MODE_MEAN_C;
  static const std::string MODE_PCA_C;
  static const std::string MODE_SINGLE_SAMPLE_C;
  static const std::string MODE_REGRESSION_C;

public Q_SLOTS:

  // analysis mode
  void on_tabWidget_currentChanged();

  void handle_analysis_options();
  void handle_median();

  void on_overall_button_clicked();
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

  void on_linear_radio_toggled(bool b);

  void handle_reconstruction_complete();

  void on_reconstructionButton_clicked();

  //! Set the currently selected feature map
  void set_feature_map(const std::string& feature_map);

  void handle_error(std::string message);
  void handle_warning(std::string message);
  void handle_message(std::string message);

signals:

  void update_view();
  void pca_update();
  void progress(size_t);
  void message(std::string);
  void error(std::string);
  void reconstruction_complete();

private:

  void pca_labels_changed(QString value, QString eigen, QString lambda);
  void compute_mode_shape();
  void update_analysis_mode();

  void update_group_boxes();
  void update_group_values();

  ShapeHandle create_shape_from_points(const vnl_vector<double>& points);

  Preferences& preferences_;

  Ui_AnalysisTool* ui_;
  QSharedPointer<Session> session_;
  ShapeWorksStudioApp* app_;

  /// itk particle shape statistics
  ParticleShapeStatistics<3> stats_;
  bool stats_ready_;

  vnl_vector<double> empty_shape_;
  vnl_vector<double> temp_shape_;

  bool pca_animate_direction_ = true;
  QTimer pca_animate_timer_;

  bool group_animate_direction_ = true;
  QTimer group_animate_timer_;

  ShapeHandle computed_shape_;

  std::string feature_map_;

  std::vector<std::string> current_group_names_;
  std::vector<std::string> current_group_values_;

};
