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
  std::cerr << "searching for key: " << key << "\n";
  if (this->map_.find(key) == this->map_.end()) {
    std::cerr << "did not find, returning default: " << default_value.operator std::string() << "\n";
    return Value(default_value);
  }
  std::cerr << "found: " << this->map_[key] << "\n";
  return Value(this->map_[key]);
}

//---------------------------------------------------------------------------
void Settings::set(std::string key, Value value)
{
  this->map_[key] = static_cast<std::string>(value);
}

//---------------------------------------------------------------------------
void Settings::remove_entry(std::string key)
{
  this->map_.erase(key);
}
