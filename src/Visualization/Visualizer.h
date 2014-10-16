#ifndef STUDIO_VISUALIZATION_VISUALIZER_H
#define STUDIO_VISUALIZATION_VISUALIZER_H

#include <Data/Project.h>
#include <Visualization/Lightbox.h>

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
  Visualizer();
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
  void update_display();

  /// tmp: compute and display the mean shape
  void display_mean();

  void display_shape( const vnl_vector<double> &points );

  void display_pca(int mode, double value);


  static const QString MODE_ORIGINAL_C;
  static const QString MODE_GROOMED_C;
  static const QString MODE_RECONSTRUCTION_C;

public Q_SLOTS:

  /// update viewer properties (e.g. glyph size, quality, etc)
  void update_viewer_properties();

private:

  void compute_stats();


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

};

#endif /* STUDIO_VISUALIZATION_VISUALIZER_H */
