#pragma once

#include <map>
#include <string>
#include <vector>
#include <sstream>
#include <iostream>

namespace shapeworks
{

// class for automatic conversion from string -> anytype. (adapted from optparse)
class Value
{
public:

  Value() : str(), valid(false) {}

  Value(const std::string &v) : str(v), valid(true) {}
  Value(int v) : str(std::to_string(v)), valid(true) {}
  Value(double v) : str(std::to_string(v)), valid(true) {}
  Value(const char* v) : str(v), valid(true) {}
  Value(bool v) : str(v ? "true" : "false"), valid(true) {}

  operator std::vector<std::string>() {
    std::istringstream iss(str);
    std::vector<std::string> v((std::istream_iterator<std::string>(iss)),
                               std::istream_iterator<std::string>());
    return v;
  }

  operator std::string() {
    return str;
  }

  operator bool() {
    // first try to read as the word 'true' or 'false', otherwise 0 or 1
    std::string str_lower(str);
    std::transform(str.begin(), str.end(), str_lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    bool t = (valid && (std::istringstream(str_lower) >> std::boolalpha >> t)) ? t : false;
    if (!t) {
      t = (valid && (std::istringstream(str) >> t)) ? t : false;
    }
    return t;
  }

  operator short() {
    short t;
    return (valid && (std::istringstream(str) >> t)) ? t : 0;
  }

  operator unsigned short() {
    unsigned short t;
    return (valid && (std::istringstream(str) >> t)) ? t : 0;
  }

  operator int() {
    int t;
    return (valid && (std::istringstream(str) >> t)) ? t : 0;
  }

  operator unsigned int() {
    unsigned int t;
    return (valid && (std::istringstream(str) >> t)) ? t : 0;
  }

  operator long() {
    long t;
    return (valid && (std::istringstream(str) >> t)) ? t : 0;
  }

  operator unsigned long() {
    unsigned long t;
    return (valid && (std::istringstream(str) >> t)) ? t : 0;
  }

  operator float() {
    float t;
    return (valid && (std::istringstream(str) >> t)) ? t : 0;
  }

  operator double() {
    double t;
    return (valid && (std::istringstream(str) >> t)) ? t : 0;
  }

  operator long double() {
    long double t;
    return (valid && (std::istringstream(str) >> t)) ? t : 0;
  }
private:

  const std::string str;
  bool valid;
};

class Settings {

public:

  static constexpr const char* GROOM_SETTINGS = "groom";
  static constexpr const char* OPTIMIZE_SETTINGS = "optimize";
  static constexpr const char* STUDIO_SETTINGS = "studio";


  void set_map(std::map<std::string, std::string> map);
  std::map<std::string, std::string> get_map();

  Value get(std::string key, Value default_value);
  void set(std::string key, Value value);

  void remove_entry(std::string key);

private:

  std::map<std::string, std::string> map_;
};
}
