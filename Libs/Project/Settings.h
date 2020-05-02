#include <map>
#include <string>
#include <vector>
#include <sstream>

namespace shapeworks
{

// Class for automatic conversion from string -> anytype. (from optparse)
class Value
{
public:

  Value() : str(), valid(false) {}

  explicit Value(const std::string &v) : str(v), valid(true) {}

  operator std::vector<std::string>() {
    std::istringstream iss(str);
    std::vector<std::string> v((std::istream_iterator<std::string>(iss)),
                               std::istream_iterator<std::string>());
    return v;
  }

  std::string as_string()
  {
    return this->str;
  }

  operator const char*() {
    return str.c_str();
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
  static constexpr const char* STUDIO_SETTINGS = "studio";
  static constexpr const char* GROOM_CENTER_OPTION = "center";

  void set_map(std::map<std::string, std::string> map);
  std::map<std::string, std::string> get_map();

  Value get(std::string key, std::string default_value);
  void set(std::string key, std::string value);

private:

  std::map<std::string, std::string> map_;
};
}
