#include "JsonProjectReader.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "ProjectUtils.h"
using json = nlohmann::ordered_json;

namespace shapeworks {

using namespace project::prefixes;
using namespace project::types;

//---------------------------------------------------------------------------
class JsonProjectReader::JsonContainer {
 public:
  JsonContainer(){};
  ~JsonContainer(){};
  json j;
};

//---------------------------------------------------------------------------
static std::map<std::string, std::string> get_key_map(json item) {
  std::map<std::string, std::string> key_map;
  for (auto& [key, value] : item.items()) {
    key_map[key] = value;
  }
  return key_map;
}

//---------------------------------------------------------------------------
void JsonProjectReader::read_subjects() {
  auto& j = container_->j;

  if (!j.contains("data")) {
    return;
  }

  load_subjects(object_to_map_list(j["data"]));
}

//---------------------------------------------------------------------------
static void read_landmark_definitions(Project& project, const json& j) {
  if (!j.contains("landmarks")) {
    return;
  }

  auto landmarks = j["landmarks"];

  std::vector<std::vector<LandmarkDefinition>> definitions;

  auto domain_names = project.get_domain_names();
  definitions.resize(domain_names.size());

  for (auto landmark : landmarks) {
    LandmarkDefinition def;
    def.name_ = landmark["name"];
    def.domain_ = landmark["domain"];

    auto f = std::find(domain_names.begin(), domain_names.end(), def.domain_);
    if (f == domain_names.end()) {
      // skip, nonexistent domain
      continue;
    }
    int domain_id = f - domain_names.begin();
    def.domain_id_ = domain_id;
    def.visible_ = landmark["visible"] == "true";
    def.color_ = landmark["color"];
    def.comment_ = landmark["comment"];
    definitions[domain_id].push_back(def);
  }

  project.set_landmark_definitions(definitions);
}

//---------------------------------------------------------------------------
static Parameters read_parameters(json j, std::string name) {
  Parameters p;
  if (!j.contains(name)) {
    return p;
  }

  ProjectUtils::StringMap map;
  for (auto& [key, value] : j[name].items()) {
    map[key] = value;
  }

  p.set_map(map);
  return p;
}

//---------------------------------------------------------------------------
static std::map<std::string, Parameters> read_parameter_map(json j, std::string name) {
  std::map<std::string, Parameters> map;
  if (!j.contains(name)) {
    return map;
  }

  for (auto& [domain, params] : j[name].items()) {
    map[domain] = read_parameters(j[name], domain);
  }
  return map;
}

//---------------------------------------------------------------------------
JsonProjectReader::JsonProjectReader(Project& project) : ProjectReader(project), container_(new JsonContainer) {}

//---------------------------------------------------------------------------
JsonProjectReader::~JsonProjectReader() {}

//---------------------------------------------------------------------------
bool JsonProjectReader::read_project(std::string filename) {
  try {
    std::ifstream ifs(filename);
    container_->j = json::parse(ifs);
    auto& j = container_->j;

    read_subjects();

    read_landmark_definitions(project_, j);

    // read parameter sheets
    project_.set_parameter_map("groom", read_parameter_map(j, "groom)"));
    project_.set_parameters("optimize", read_parameters(j, "optimize)"));
    project_.set_parameters("studio", read_parameters(j, "studio"));
    project_.set_parameters("project_", read_parameters(j, "project_"));
    project_.set_parameters("analysis", read_parameters(j, "analysis"));
    project_.set_parameters("deepssm", read_parameters(j, "deepssm"));

  } catch (std::exception& e) {
    std::cerr << "Error reading " << filename << " : " << e.what() << "\n";
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------
StringMapList JsonProjectReader::object_to_map_list(std::string name) {
  StringMapList list;
  auto& j = container_->j;
  if (!j.contains("data")) {
    return list;
  }

  auto data = j["data"];
  for (const auto& item : data) {
    auto subject = std::make_shared<Subject>();
    auto key_map = get_key_map(item);
    list.push_back(key_map);
  }

  return list;
}

}  // namespace shapeworks
