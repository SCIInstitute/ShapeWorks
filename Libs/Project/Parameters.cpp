#include "Parameters.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
void Parameters::set_map(std::map<std::string, std::string> map)
{
  this->map_ = map;
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> Parameters::get_map()
{
  return this->map_;
}

//---------------------------------------------------------------------------
Variant Parameters::get(std::string key, Variant default_value)
{
  if (this->map_.find(key) == this->map_.end()) {
    return Variant(default_value);
  }
  return Variant(this->map_[key]);
}

//---------------------------------------------------------------------------
void Parameters::set(std::string key, Variant value)
{
  this->map_[key] = static_cast<std::string>(value);
}

//---------------------------------------------------------------------------
void Parameters::remove_entry(std::string key)
{
  this->map_.erase(key);
}
