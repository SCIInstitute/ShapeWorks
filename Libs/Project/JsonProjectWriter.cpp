#include "JsonProjectWriter.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "ProjectUtils.h"
using json = nlohmann::ordered_json;

namespace shapeworks {

//---------------------------------------------------------------------------
static json create_data_object(Project& project) {
  auto subjects = project.get_subjects();

  std::vector<json> list;
  for (int i = 0; i < subjects.size(); i++) {
    auto subject = subjects[i];
    json j = ProjectUtils::convert_subject_to_map(&project, subject.get());
    list.push_back(j);
  }

  return list;
}

//---------------------------------------------------------------------------
static json create_parameter_map_object(std::map<std::string, Parameters> parameter_map) {
  json j;
  for (auto& [key, params] : parameter_map) {
    j[key] = params.get_map();
  }
  return j;
}

//---------------------------------------------------------------------------
static json create_landmark_definition_object(Project& project) {
  std::vector<json> list;

  auto all_definitions = project.get_all_landmark_definitions();
  auto domains = project.get_domain_names();

  for (int d = 0; d < all_definitions.size(); d++) {
    auto definitions = all_definitions[d];

    for (int i = 0; i < definitions.size(); i++) {
      auto def = definitions[i];
      json item;
      item["domain"] = domains[d];
      item["name"] = def.name_;
      item["visible"] = def.visible_ ? "true" : "false";
      item["color"] = def.color_;
      item["comment"] = def.comment_;
      list.push_back(item);
    }
  }

  return list;
}

//---------------------------------------------------------------------------
bool JsonProjectWriter::write_project(Project& project, std::string filename) {
  json j;

  json landmarks_sheet;

  j["data"] = create_data_object(project);
  j["groom"] = create_parameter_map_object(project.get_parameter_map("groom"));
  j["optimize"] = project.get_parameters("optimize").get_map();
  j["studio"] = project.get_parameters("studio").get_map();
  j["project"] = project.get_parameters("project").get_map();
  j["analysis"] = project.get_parameters("analysis").get_map();
  j["deepssm"] = project.get_parameters("deepssm").get_map();
  j["landmarks"] = create_landmark_definition_object(project);

  std::ofstream file(filename);
  if (!file.good()) {
    throw std::runtime_error("Unable to open " + filename + " for writing");
  }
  file << j.dump(4);

  return true;
}

}  // namespace shapeworks
