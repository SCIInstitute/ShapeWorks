#pragma once

#include <sstream>
#include <string>
#include <vector>

namespace shapeworks {

template <typename T>
std::string variant_to_string(T begin, T end) {
  std::stringstream ss;
  bool first = true;
  for (; begin != end; begin++) {
    if (!first) {
      ss << " ";
    }
    ss << *begin;
    first = false;
  }
  return ss.str();
}

//! Variant class to represent multiple types
/*!
 * This class was adapted from optparse to contain multiple types as a string
 * This is used for storing parameters in a spreadsheet
 */
class Variant {
 public:
  Variant() : str_(), valid_(false) {}
  Variant(const std::string& v) : str_(v), valid_(true) {}
  Variant(int v) : str_(std::to_string(v)), valid_(true) {}
  Variant(double v) : str_(std::to_string(v)), valid_(true) {}
  Variant(const char* v) : str_(v), valid_(true) {}
  Variant(bool v) : str_(v ? "true" : "false"), valid_(true) {}
  Variant(std::vector<double> v) : str_(variant_to_string(v.begin(), v.end())), valid_(true) {}
  Variant(std::vector<int> v) : str_(variant_to_string(v.begin(), v.end())), valid_(true) {}
  Variant(std::vector<bool> v) : str_(variant_to_string(v.begin(), v.end())), valid_(true) {}

  operator std::string() const;
  operator bool() const;
  operator int() const;
  operator unsigned int() const;
  operator long() const;
  operator unsigned long() const;
  operator float() const;
  operator double() const;
  operator std::vector<double>() const;
  operator std::vector<int>() const;
  operator std::vector<bool>() const;

 private:
  std::string str_;
  bool valid_;
};
}  // namespace shapeworks
