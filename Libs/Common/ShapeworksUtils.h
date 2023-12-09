#pragma once

#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <chrono>
#include <random>

#include "Shapeworks.h"

namespace shapeworks {

class ShapeWorksUtils {
  using time_point = std::chrono::time_point<std::chrono::steady_clock>;

 public:
  /// get the current time
  static time_point now() { return std::chrono::steady_clock::now(); }

  /// return elapsed time in seconds, optionally printing to console
  static double elapsed(time_point start, time_point end, bool print_elapsed = true);

  /// returns random seed
  static unsigned get_rng_seed() { return rng_seed_; }

  /// sets random seed
  static void set_rng_seed(const unsigned seed);

  /// generates random number
  static unsigned get_random_number() { return mt_(); }

  /// returns true if pathname is a directory
  // TODO: in C++17 this is a standard function
  static bool is_directory(const std::string& pathname);

  /// returns true if filename exists
  // TODO: in C++17 this is a standard function
  static bool file_exists(const std::string& filename);

  //! Set up the console logging options
  static void setup_console_logging(bool show_progress, bool xml_status);

  //! Set up the number of threads from TBB_NUM_THREADS env variable
  static void setup_threads();

  /// converts a vtkMatrix4x4 to a Matrix33 and corresponding translationVector
  static Matrix33 convert_matrix(const vtkSmartPointer<vtkMatrix4x4>& mat);
  static Vector3 get_offset(const vtkSmartPointer<vtkMatrix4x4>& mat);

 private:
  static unsigned rng_seed_;
  static std::mt19937 mt_;
};

}  // namespace shapeworks
