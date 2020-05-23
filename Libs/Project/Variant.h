#pragma once

#include <string>

namespace shapeworks
{

//! Variant class to represent multiple types
/*!
 * This class was adapted from optparse to contain multiple types as a string
 * This is used for storing parameters in a spreadsheet
 */
class Variant
{
public:

  Variant() : str_(), valid_(false) {}
  Variant(const std::string &v) : str_(v), valid_(true) {}
  Variant(int v) : str_(std::to_string(v)), valid_(true) {}
  Variant(double v) : str_(std::to_string(v)), valid_(true) {}
  Variant(const char* v) : str_(v), valid_(true) {}
  Variant(bool v) : str_(v ? "true" : "false"), valid_(true) {}

  operator std::string();
  operator bool();
  operator int();
  operator unsigned int();
  operator long();
  operator unsigned long();
  operator float();
  operator double();

private:

  const std::string str_;
  bool valid_;
};
}
