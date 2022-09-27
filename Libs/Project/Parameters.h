#pragma once

#include <ProjectUtils.h>

#include "Variant.h"

namespace shapeworks {

//! Parameter settings
/*!
 * Store key/variant combinations
 * Used to store parameters for various tools
 */
class Parameters {
  using StringMap = project::types::StringMap;

 public:
  static constexpr const char* ANALYSIS_PARAMS = "analysis";
  static constexpr const char* GROOM_PARAMS = "groom";
  static constexpr const char* OPTIMIZE_PARAMS = "optimize";
  static constexpr const char* STUDIO_PARAMS = "studio";
  static constexpr const char* PROJECT_PARAMS = "project";
  static constexpr const char* DEEPSSM_PARAMS = "deepssm";

  //! default constructor
  Parameters();

  //! construct from map
  explicit Parameters(StringMap map);

  //! get a parameter based on a key, return default if it doesn't exist
  Variant get(std::string key, Variant default_value);

  //! return if a key exists or not
  bool key_exists(std::string key);

  //! set a parameter based on a key
  void set(std::string key, Variant value);

  //! remove an entry
  void remove_entry(std::string key);

  //! set underlying map
  void set_map(StringMap map);

  //! get underlying map
  StringMap get_map();

  //! reset parameters to blank
  void reset_parameters();

 private:
  StringMap map_;

  //  std::vector< std::pair<const std::string, std::string> >  container_;
  // std::pair<const std::string, std::string>  container_;
  //const std::string joke_;
  tsl::ordered_map<std::string,std::string> mappy_;
};
}  // namespace shapeworks
