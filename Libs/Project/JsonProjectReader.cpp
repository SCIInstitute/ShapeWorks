#include "JsonProjectReader.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::ordered_json;

namespace shapeworks {

using namespace project::prefixes;
using namespace project::types;

//---------------------------------------------------------------------------
class JsonProjectReader::Container {
 public:
  Container(){};
  ~Container(){};
  json j;

  //---------------------------------------------------------------------------
  // grab table from json and return StringMapList
  //---------------------------------------------------------------------------
  StringMapList object_to_map_list(std::string name) {
    StringMapList list;
    if (!j.contains(name)) {
      return list;
    }

    auto data = j[name];
    for (const auto& item : data) {
      auto key_map = item.get<StringMap>();
      list.push_back(key_map);
    }

    return list;
  }

  //---------------------------------------------------------------------------
  // grab table from json and return StringMap
  //---------------------------------------------------------------------------
  StringMap object_to_map(std::string name) {
    if (!j.contains(name)) {
      return {};
    }
    return j[name].get<StringMap>();
  }

  //---------------------------------------------------------------------------
  // grab table from json and return StringMultiMap
  //---------------------------------------------------------------------------
  StringMultiMap object_to_multi_map(std::string name) {
    StringMultiMap map;
    if (!j.contains(name)) {
      return map;
    }
    for (auto& [key, value] : j[name].items()) {
      if (!value.is_null()) {
        map[key] = value.get<StringMap>();
      }
    }
    return map;
  }
};

//---------------------------------------------------------------------------
JsonProjectReader::JsonProjectReader(Project& project) : ProjectReader(project), container_(new Container) {}

//---------------------------------------------------------------------------
JsonProjectReader::~JsonProjectReader() {}

//---------------------------------------------------------------------------
bool JsonProjectReader::read_project(std::string filename) {
  //try {
    std::ifstream ifs(filename);
    container_->j = json::parse(ifs);

    load_subjects(container_->object_to_map_list("data"));
    load_landmark_definitions(container_->object_to_map_list("landmarks"));
    load_parameters();

    /*
  } catch (std::exception& e) {
    std::cerr << "Error reading " << filename << " : " << e.what() << "\n";
    return false;
  }
  */
  return true;
}

//---------------------------------------------------------------------------
StringMap JsonProjectReader::get_parameters(std::string name) { return container_->object_to_map(name); }

//---------------------------------------------------------------------------
StringMultiMap JsonProjectReader::get_multi_parameters(std::string name) {
  return container_->object_to_multi_map(name);
}

//---------------------------------------------------------------------------

}  // namespace shapeworks
