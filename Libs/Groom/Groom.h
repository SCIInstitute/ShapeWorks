#pragma once

#include <Libs/Project/Project.h>

namespace shapeworks {

class Groom {

public:

  explicit Groom(shapeworks::ProjectHandle project);

  //! Return the progress (0-100)
  float get_current_progress();

  //! Run the grooming
  virtual void run();

protected:

  //! call to be overridden by subclasses
  virtual void update_progress()
  {};

  tbb::atomic<float> progress_ = 0;
  tbb::atomic<int> total_ops_ = 0;
  tbb::atomic<int> progress_counter_;

private:

  //! Return the number of operations that will be performed
  int get_total_ops();

  //! Increment the progress one step
  void increment_progress();

  //! Run image based pipeline on a single subject
  void image_pipeline(std::shared_ptr<Subject> subject);

  //! Run the mesh based pipeline on a single subject
  void image_mesh_pipeline(std::shared_ptr<Subject> subject);

  Vector3 center(Image& image);
  void isolate(Image& image);
  void hole_fill(Image& image);
  void auto_pad(Image& image, int padding_amount);
  void antialias(Image& image, int iterations);

  bool verbose_;

  shapeworks::ProjectHandle project_;
};
}
