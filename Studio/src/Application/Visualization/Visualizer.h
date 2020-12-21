#pragma once

#include <map>
#include <string>

#include <Data/Session.h>
#include <Data/Preferences.h>
#include <Visualization/Lightbox.h>

namespace shapeworks {

class Visualizer;

typedef QSharedPointer<Visualizer> VisualizerHandle;

//! Controls display of objects in viewers
/*!
 * The Visualizer class maintains state and control over viewer properties.
 * Additionally, it performs all shape-based statistical functions.
 */
class Visualizer : public QObject {
Q_OBJECT;

public:
  Visualizer(Preferences& prefs);
  ~Visualizer();

  /// set the lightbox
  void set_lightbox(LightboxHandle lightbox);

  /// set the session
  void set_session(SessionHandle session);

  /// set display mode (original, groomed, reconstructed)
  void set_display_mode(std::string mode);

  std::string get_display_mode();

  /// turn automatic centering on/off
  void set_center(bool center);

  bool get_center();

  /// turn on/off glyph display
  void set_show_glyphs(bool show);

  /// turn on/off surface display
  void set_show_surface(bool show);

  /// update the display using the current settings
  void display_samples();

  void update_samples();

  void display_sample(int i);

  void display_shape(const vnl_vector<double>& points);

  void display_shape(const vnl_vector<double>& points, const std::vector<Shape::Point>& vectors);

  void display_shape(ShapeHandle shape);

  void set_selected_point_one(int id);
  void set_selected_point_two(int id);

  static const std::string MODE_ORIGINAL_C;
  static const std::string MODE_GROOMED_C;
  static const std::string MODE_RECONSTRUCTION_C;

  void set_mean(const vnl_vector<double>& mean);

  void reset_camera();

  void update_lut();

  vnl_vector<double> getCurrentShape();

  void handle_new_mesh();
  vtkSmartPointer<vtkPolyData> get_current_mesh();

  //! Get the currently selected feature map
  const std::string& get_feature_map() const;

  //! Set if we are using a uniform feature range
  void set_uniform_feature_range(bool value);

  //! Return if we are using a uniform feature range
  bool get_uniform_feature_range(void);

  //! Set the currently selected feature map
  void set_feature_map(const std::string& feature_map);

  //! clear out the viewers
  void clear_viewers();

  //! Reset the feature range (e.g. for a new feature)
  void reset_feature_range();

  //! Get the current feature range
  double* get_feature_range();

  //! Update the feature range with a given range
  void update_feature_range(double* range);

public Q_SLOTS:

  /// update viewer properties (e.g. glyph size, quality, etc)
  void update_viewer_properties();

private:
  ShapeHandle create_display_object(const vnl_vector<double>& points,
                                    const std::vector<Shape::Point>& vectors);
  Preferences& preferences_;

  void compute_measurements();

  std::string display_mode_;
  std::string feature_map_;

private:

  bool center_;
  bool needs_camera_reset_ = true;

  bool show_glyphs_;
  bool show_surface_;

  LightboxHandle lightbox_;
  SessionHandle session_;

  vtkSmartPointer<vtkLookupTable> glyph_lut_;
  int selected_point_one_;
  int selected_point_two_;

  vnl_vector<double> cached_mean_;
  vnl_vector<double> current_shape_;

  double feature_range_[2] = {0, 0};
  bool feature_range_valid_ = false;
  bool feature_range_uniform_ = true;

};

}