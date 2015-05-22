#ifndef STUDIO_VISUALIZATION_VISUALIZER_H
#define STUDIO_VISUALIZATION_VISUALIZER_H

#include <Data/Project.h>
#include <Data/MeshMarchingWorker.h>
#include <Visualization/Lightbox.h>
#include <Visualization/DisplayObject.h>
#include "Application/Preferences.h"
#include <map>

//#include "itkParticleShapeLinearRegressionMatrixAttribute.h"
#include <Analysis/itkParticleShapeStatistics.h>

class Visualizer;
typedef QSharedPointer< Visualizer > VisualizerHandle;

//! Controls display of objects in viewers
/*!
 * The Visualizer class maintains state and control over viewer properties.
 * Additionally, it performs all shape-based statistical functions.
 */
class Visualizer : public QObject
{
  Q_OBJECT;

public:
  Visualizer(Preferences &prefs);
  ~Visualizer();

  /// set the lightbox
  void set_lightbox( LightboxHandle lightbox );

  /// set the project
  void set_project( ProjectHandle project );

  /// set display mode (original, groomed, reconstructed)
  void set_display_mode( QString mode );

  /// turn automatic centering on/off
  void set_center( bool center );

  /// turn on/off glyph display
  void set_show_glyphs( bool show );

  /// turn on/off surface display
  void set_show_surface( bool show );

  /// update the display using the current settings
  void display_samples();

  /// tmp: compute and display the mean shape
  void display_mean();

  void display_shape( const vnl_vector<double> &points, double value );

  bool is_cached() { return cached_; }

  void set_cached(bool b) { cached_ = b; }

  void cache_data(int mode, double value);

  void display_pca( int mode, double value );

  void set_selected_point_one( int id );
  void set_selected_point_two( int id );

  void set_stats_ready(bool b) {this->stats_ready_ = b; }

  static const QString MODE_ORIGINAL_C;
  static const QString MODE_GROOMED_C;
  static const QString MODE_RECONSTRUCTION_C;

Q_SIGNALS:
  void pca_labels_changed( QString value, QString eigen, QString lambda );

public Q_SLOTS:

  /// update viewer properties (e.g. glyph size, quality, etc)
  void update_viewer_properties();

private:
  vnl_vector<double> getShape(int mode, double value);
  QVector<DisplayObjectHandle> * getList( const vnl_vector<double> &points , double value);
  bool cached_;
  std::map<double,QVector<DisplayObjectHandle>> disp_handles_;
  Preferences &preferences_;
  bool compute_stats();

  void update_lut();

  void compute_measurements();

  QString display_mode_;
  bool center_;

  bool show_glyphs_;
  bool show_surface_;

  LightboxHandle lightbox_;
  ProjectHandle project_;

  /// itk particle shape statistics
  ParticleShapeStatistics<3> stats;
  bool stats_ready_;

  int pca_mode;
  double pca_value;

  vtkSmartPointer<vtkLookupTable> glyph_lut_;
  int selected_point_one_;
  int selected_point_two_;

  MeshMarchingWorker meshing_queue_;
};

#endif /* STUDIO_VISUALIZATION_VISUALIZER_H */
