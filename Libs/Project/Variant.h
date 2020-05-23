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

  Variant() : str(), valid(false) {}
  Variant(const std::string &v) : str(v), valid(true) {}
  Variant(int v) : str(std::to_string(v)), valid(true) {}
  Variant(double v) : str(std::to_string(v)), valid(true) {}
  Variant(const char* v) : str(v), valid(true) {}
  Variant(bool v) : str(v ? "true" : "false"), valid(true) {}

  operator std::string();
  operator bool();
  operator int();
  operator unsigned int();
  operator long();
  operator unsigned long();
  operator float();
  operator double();

private:

  const std::string str;
  bool valid;
};
}
