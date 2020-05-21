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
Variant Settings::get(std::string key, Variant default_value)
{
  if (this->map_.find(key) == this->map_.end()) {
    return Variant(default_value);
  }
  return Variant(this->map_[key]);
}

//---------------------------------------------------------------------------
void Settings::set(std::string key, Variant value)
{
  this->map_[key] = static_cast<std::string>(value);
}

//---------------------------------------------------------------------------
void Settings::remove_entry(std::string key)
{
  this->map_.erase(key);
}
