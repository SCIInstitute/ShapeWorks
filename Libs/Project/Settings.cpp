#include <Libs/Project/Settings.h>

using namespace shapeworks;

//---------------------------------------------------------------------------
void Settings::set_map(std::map<std::string, std::string> map)
{
  this->map_ = map;
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> Settings::get_map()
{
  return this->map_;
}

//---------------------------------------------------------------------------
Value Settings::get(std::string key, Value default_value)
{
  if (this->map_.find(key) == this->map_.end()) {
    return Value(default_value);
  }
  return Value(this->map_[key]);
}

//---------------------------------------------------------------------------
void Settings::set(std::string key, Value value)
{
  this->map_[key] = value.as_string();
}
