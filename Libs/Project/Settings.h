#pragma once

#include <map>
#include <string>

#include "Variant.h"

namespace shapeworks {

class Parameters {

public:

  static constexpr const char* ANALYSIS_PARAMS = "analysis";
  static constexpr const char* GROOM_PARAMS = "groom";
  static constexpr const char* OPTIMIZE_PARAMS = "optimize";
  static constexpr const char* STUDIO_PARAMS = "studio";

  void set_map(std::map<std::string, std::string> map);
  std::map<std::string, std::string> get_map();

  Variant get(std::string key, Variant default_value);
  void set(std::string key, Variant value);

  void remove_entry(std::string key);

private:

  std::map<std::string, std::string> map_;
};
}
