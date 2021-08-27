#pragma once

#include <Libs/Project/Project.h>
#include <Libs/Image/Image.h>

#include <tbb/atomic.h>
#include <tbb/mutex.h>

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
  virtual bool run();

  //! Set if grooming steps should be skipped
  void set_skip_grooming(bool skip);

  //! Set abort as soon as possible
  void abort();

  //! Return if grooming was aborted
  bool get_aborted();

  static std::vector<std::vector<double>> get_transforms(const std::vector<Mesh> meshes, size_t reference);

protected:

  //! call to be overridden by subclasses
  virtual void update_progress()
  {};

  tbb::atomic<float> progress_ = 0;
  tbb::atomic<int> total_ops_ = 0;
  tbb::atomic<int> progress_counter_ = 0;

private:

  bool run_alignment();

  //! Return the number of operations that will be performed
  int get_total_ops();

  //! Increment the progress one step
  void increment_progress(int amount = 1);

  //! Run image based pipeline on a single subject
  bool image_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  //! Run the mesh based pipeline on a single subject
  bool mesh_pipeline(std::shared_ptr<Subject> subject, size_t domain);

  //! Return the output filename for a given intpu tfile
  std::string get_output_filename(std::string input, DomainType domain_type);

  Mesh get_mesh(int subject, int domain);


  Vector3 center(Image& image);
  void isolate(Image& image);

  bool verbose_ = false;

  ProjectHandle project_;

  bool skip_grooming_ = false;

  bool abort_ = false;

  tbb::mutex mutex_;

};
}
