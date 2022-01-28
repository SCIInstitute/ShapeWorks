#pragma once

#include <Data/Preferences.h>
#include <Data/Session.h>
#include <Visualization/Lightbox.h>

#include <map>
#include <string>

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

  //! return the current display mode
  std::string get_display_mode();

  /// turn automatic centering on/off
  void set_center(bool center);

  //! get centering on/off
  bool get_center();

  //! set the alignment domain
  void set_alignment_domain(int domain);
  //! get the current alignment domain
  int get_alignment_domain();

  /// turn on/off glyph display
  void set_show_glyphs(bool show);

  /// turn on/off surface display
  void set_show_surface(bool show);

  /// turn on/off landmark display
  void set_show_landmarks(bool show);

  /// update the display using the current settings
  void display_samples();

  void update_samples();

  void update_landmarks();

  void display_sample(int i);

  void display_shape(ShapeHandle shape);
  void display_shapes(QVector<QSharedPointer<Shape>> shapes);

  void set_selected_point_one(int id);
  void set_selected_point_two(int id);

  static const std::string MODE_ORIGINAL_C;
  static const std::string MODE_GROOMED_C;
  static const std::string MODE_RECONSTRUCTION_C;

  void set_mean(const Eigen::VectorXd& mean);

  void reset_camera();

  void update_lut();

  StudioParticles get_current_shape();

  void handle_new_mesh();
  vtkSmartPointer<vtkPolyData> get_current_mesh();
  std::vector<vtkSmartPointer<vtkPolyData>> get_current_meshes_transformed();

  //! Get the currently selected feature map
  const std::string& get_feature_map() const;

  //! Set if we are using a uniform feature range
  void set_uniform_feature_range(bool value);

  //! Return if we are using a uniform feature range
  bool get_uniform_feature_range(void);

  //! Set the currently selected feature map
  void set_feature_map(const std::string& feature_map);

  //! Set the currently selected image volume
  void set_image_volume(const std::string& image_volume);

  //! Get the currently selected image volume
  const std::string& get_image_volume() const;

  //! clear out the viewers
  void clear_viewers();

  //! Reset the feature range (e.g. for a new feature)
  void reset_feature_range();

  //! Get the current feature range
  double* get_feature_range();

  //! Get the current raw feature range
  double* get_feature_raw_range();

  //! Return if the feature range is valid or not
  bool get_feature_range_valid();

  //! Update the feature range with a given range
  void update_feature_range(double* range);

  //! Request the transform for a given shape and domain
  vtkSmartPointer<vtkTransform> get_transform(QSharedPointer<Shape> shape, int alignment_domain, int domain);

  //! Set domain opacities
  void set_opacities(std::vector<float> opacities);

  //! Get domain opacities
  std::vector<float> get_opacities();

  //! Get the current glyph size
  double get_current_glyph_size();

  //! Handle ctrl click
  void handle_ctrl_click(PickResult result);

 public Q_SLOTS:

  /// update viewer properties (e.g. glyph size, quality, etc)
  void update_viewer_properties();

  void handle_feature_range_changed();

 private:
  ShapeHandle create_display_object(const StudioParticles& points, const std::vector<Shape::Point>& vectors);
  Preferences& preferences_;

  void compute_measurements();

  std::string display_mode_;
  std::string feature_map_;
  std::string image_volume_;
  int alignment_domain_;

  bool center_;
  bool needs_camera_reset_ = true;

  bool show_glyphs_ = true;
  bool show_surface_ = true;
  bool show_landmarks_ = true;

  LightboxHandle lightbox_;
  SessionHandle session_;

  vtkSmartPointer<vtkLookupTable> glyph_lut_;
  int selected_point_one_;
  int selected_point_two_;

  Eigen::VectorXd cached_mean_;
  StudioParticles current_shape_;

  double feature_range_[2] = {0, 0};
  double feature_manual_range_[2] = {0, 0};
  bool feature_range_valid_ = false;
  bool feature_range_uniform_ = true;

  std::vector<float> opacities_;

  double current_glyph_size_{0};
};

}  // namespace shapeworks
