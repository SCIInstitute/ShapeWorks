#pragma once

#include "Shapeworks.h"

#include <vtkSmartPointer.h>
#include <vtkMatrix4x4.h>
#include <vtkPolyData.h>

#include <chrono>
#include <random>

namespace shapeworks {

class ShapeworksUtils
{
  using time_point = std::chrono::time_point<std::chrono::steady_clock>;

public:

  /// get the current time
  static time_point now() { return std::chrono::steady_clock::now(); }

  /// return elapsed time in seconds, optionally printing to console
  static double elapsed(time_point start, time_point end, bool print_elapsed = true);

  /// returns random seed
  static unsigned rngSeed() { return _rngSeed; }

  /// sets random seed
  static void setRngSeed(const unsigned seed);

  /// generates random number
  static unsigned generateNumber() { return mt(); }

  /// returns true if pathname is a directory
  // TODO: in C++17 this is a standard function
  static bool is_directory(const std::string &pathname);

  /// returns true if filename exists
  // TODO: in C++17 this is a standard function
  static bool exists(const std::string& filename);

  /// converts a vtkMatrix4x4 to a Matrix33 and corresponding translationVector
  static Matrix33 getMatrix(const vtkSmartPointer<vtkMatrix4x4>& mat);
  static Vector3 getOffset(const vtkSmartPointer<vtkMatrix4x4>& mat);

private:
  static unsigned _rngSeed;
  static std::mt19937 mt;
};

} // shapeworks
