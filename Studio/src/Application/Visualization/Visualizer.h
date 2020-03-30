#pragma once

#include <map>
#include <string>

#include <Data/Session.h>
#include <Data/Preferences.h>
#include <Visualization/Lightbox.h>
#include <Visualization/DisplayObject.h>

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
  void set_lightbox(LightboxHandle lightbox);

  /// set the project
  void set_project(ProjectHandle project);

  /// set display mode (original, groomed, reconstructed)
  void set_display_mode(std::string mode);

  /// turn automatic centering on/off
  void set_center(bool center);

  /// turn on/off glyph display
  void set_show_glyphs(bool show);

  /// turn on/off surface display
  void set_show_surface(bool show);

  /// update the display using the current settings
  void display_samples();

  void update_samples();

  void display_sample(size_t i);

  void display_shape(const vnl_vector<double> &points);

  void display_shape(const vnl_vector<double> &points, const std::vector<Point> &vectors);

  void set_selected_point_one(int id);
  void set_selected_point_two(int id);

  static const std::string MODE_ORIGINAL_C;
  static const std::string MODE_GROOMED_C;
  static const std::string MODE_RECONSTRUCTION_C;

  void setMean(const vnl_vector<double> &mean);

  void reset_camera();

  void update_lut();

  vnl_vector<double> getCurrentShape();

public Q_SLOTS:

  /// update viewer properties (e.g. glyph size, quality, etc)
  void update_viewer_properties();

private:
  DisplayObjectHandle create_display_object(const vnl_vector<double> &points,
                                            const std::vector<Point> &vectors);
  std::map<double, QVector<DisplayObjectHandle>> disp_handles_;
  Preferences &preferences_;

  void compute_measurements();

  std::string display_mode_;
  bool center_;

  bool show_glyphs_;
  bool show_surface_;

  LightboxHandle lightbox_;
  ProjectHandle project_;

  vtkSmartPointer<vtkLookupTable> glyph_lut_;
  int selected_point_one_;
  int selected_point_two_;

  vnl_vector<double> cached_mean_;
  vnl_vector<double> currentShape_;

  QVector < QSharedPointer < DisplayObject >> display_objects_;
};
