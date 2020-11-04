#pragma once

//#include <cstdlib>
//#include <string>
//#include <vector>
//#include <map>

#include <Libs/Project/Project.h>

namespace shapeworks {

class ShapeWorksGroom2 {

public:

  explicit ShapeWorksGroom2(shapeworks::ProjectHandle project);

  //! Return the progress (0-100)
  float get_current_progress();

  virtual void run();

protected:

  virtual void update_progress()
  {};

  tbb::atomic<float> progress_ = 0;
  tbb::atomic<int> total_ops_ = 0;
  tbb::atomic<int> progress_counter_;

private:

  //! Return the number of operations that will be performed
  int get_total_ops();

  virtual void increment_progress();

  Vector3 center(Image& image);
  void isolate(Image& image);
  void hole_fill(Image& image);
  void auto_pad(Image& image, int padding_amount);
  void antialias(Image& image, int iterations);

  bool verbose_;

  shapeworks::ProjectHandle project_;
};
}
