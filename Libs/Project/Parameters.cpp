#include "Parameters.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
Parameters::Parameters() {}

//---------------------------------------------------------------------------
Parameters::Parameters(std::map<std::string, std::string> map) { set_map(map); }

//---------------------------------------------------------------------------
void Parameters::set_map(std::map<std::string, std::string> map) { map_ = map; }

//---------------------------------------------------------------------------
std::map<std::string, std::string> Parameters::get_map() { return map_; }

//---------------------------------------------------------------------------
Variant Parameters::get(std::string key, Variant default_value) {
  if (!key_exists(key)) {
    return Variant(default_value);
  }
  return Variant(map_[key]);
}

//---------------------------------------------------------------------------
void Parameters::set(std::string key, Variant value) { map_[key] = static_cast<std::string>(value); }

//---------------------------------------------------------------------------
void Parameters::remove_entry(std::string key) { map_.erase(key); }

//---------------------------------------------------------------------------
void Parameters::reset_parameters() { map_.clear(); }

//---------------------------------------------------------------------------
bool Parameters::key_exists(std::string key) { return map_.find(key) != map_.end(); }
