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

  //! Return the progress (0-100)
  float get_current_progress();

  //! Run the grooming
  virtual bool run();

  //! Set if grooming steps should be skipped
  void set_skip_grooming(bool skip);

  //! Set abort as soon as possible
  void abort();

  //! Return if grooming was aborted
  bool get_aborted();

protected:

  //! call to be overridden by subclasses
  virtual void update_progress()
  {};

  tbb::atomic<float> progress_ = 0;
  tbb::atomic<int> total_ops_ = 0;
  tbb::atomic<int> progress_counter_ = 0;

private:

  //! Return the number of operations that will be performed
  int get_total_ops();

  //! Increment the progress one step
  void increment_progress(int amount = 1);

  //! Run image based pipeline on a single subject
  bool image_pipeline(std::shared_ptr<Subject> subject);

  //! Run the mesh based pipeline on a single subject
  bool mesh_pipeline(std::shared_ptr<Subject> subject);

  //! Load a mesh
  std::shared_ptr<Mesh> load_mesh(std::string filename);

  //! Save a mesh
  bool save_mesh(std::shared_ptr<Mesh> mesh, std::string filename);

  Vector3 center(Image& image);
  void isolate(Image& image);

  bool verbose_ = false;

  ProjectHandle project_;

  bool skip_grooming_ = false;

  bool abort_ = false;

  // locking to handle non-thread-safe code
  tbb::mutex mutex_;
};
}
