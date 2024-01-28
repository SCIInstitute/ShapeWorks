#pragma once

#include "GroomParameters.h"
#include <Image/Image.h>
#include <Project/Project.h>

namespace shapeworks {

//! High level groom API.
/*!
 * The Groom class operates on a Project.  It is used by Studio and other tools to perform
 * Grooming operations.
 *
 */
class Groom {
 public:
  Groom(ProjectHandle project);

  //! Run the grooming
  bool run();

  //! Set abort as soon as possible
  void abort();

  //! Return if grooming was aborted
  bool get_aborted();

  //! Util to compute square distance between paired landmarks
  static double compute_landmark_distance(vtkSmartPointer<vtkPoints> one, vtkSmartPointer<vtkPoints> two);

  //! Util to compute landmark transform
  static vtkSmartPointer<vtkMatrix4x4> compute_landmark_transform(vtkSmartPointer<vtkPoints> source,
                                                                  vtkSmartPointer<vtkPoints> target);

 protected:

  std::atomic<float> progress_ = 0;
  std::atomic<int> total_ops_ = 0;
  std::atomic<int> progress_counter_ = 0;

 private:
  //! Return the number of operations that will be performed
  int get_total_ops();

  //! Increment the progress one step
  void increment_progress(int amount = 1);

  //! Run image based pipeline on a single subject
  bool image_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  bool run_image_pipeline(Image& image, GroomParameters params);

  //! Run the mesh based pipeline on a single subject
  bool mesh_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  bool run_mesh_pipeline(Mesh& mesh, GroomParameters params);

  //! Run the contour based pipeline on a single subject
  bool contour_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  //! Return the output filename for a given input file
  std::string get_output_filename(std::string input, DomainType domain_type);

  bool run_alignment();

  void assign_transforms(std::vector<std::vector<double>> transforms, int domain, bool global = false);

  static std::vector<std::vector<double>> get_icp_transforms(const std::vector<Mesh> meshes, size_t reference);
  static std::vector<std::vector<double>> get_landmark_transforms(
      const std::vector<vtkSmartPointer<vtkPoints>> landmarks, size_t reference);

  static std::vector<double> get_identity_transform();
  static void add_reflect_transform(vtkSmartPointer<vtkTransform> transform, const std::string& reflect_axis);
  static void add_center_transform(vtkSmartPointer<vtkTransform> transform, const Image& image);
  static void add_center_transform(vtkSmartPointer<vtkTransform> transform, const Mesh& mesh);
  static void add_center_transform(vtkSmartPointer<vtkTransform> transform, vtkSmartPointer<vtkPoints> points);

  std::vector<vtkSmartPointer<vtkPoints>> get_combined_points();

  Mesh get_mesh(int subject, int domain);

  vtkSmartPointer<vtkPoints> get_landmarks(int subject, int domain);

  int find_reference_landmarks(std::vector<vtkSmartPointer<vtkPoints>> landmarks);

  void fix_origin(Image& image);

  bool verbose_ = false;

  ProjectHandle project_;

  bool abort_ = false;

  std::mutex mutex_;

  std::set<std::string> used_names_;
};
}  // namespace shapeworks
