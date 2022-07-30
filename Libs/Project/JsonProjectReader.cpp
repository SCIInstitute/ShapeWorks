#include "JsonProjectReader.h"

#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

#include "ProjectUtils.h"
using json = nlohmann::ordered_json;

namespace shapeworks {

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
static std::vector<std::string> get_keys(json item) {
  std::vector<std::string> keys;
  std::map<std::string, std::string> key_map;
  for (auto& [key, value] : item.items()) {
    std::cerr << "key: " << key << ", value = " << value << "\n";
    keys.push_back(key);
  }
  return keys;
}

//---------------------------------------------------------------------------
static std::map<std::string, std::string> get_key_map(json item) {
  std::map<std::string, std::string> key_map;
  for (auto& [key, value] : item.items()) {
    key_map[key] = value;
  }
  return key_map;
}

//---------------------------------------------------------------------------
static void read_subjects(ProjectHandle project, const json& j) {
  if (!j.contains("data")) {
    return;
  }

  auto data = j["data"];
  std::vector<std::shared_ptr<Subject>> subjects;
  bool first = true;
  std::vector<std::string> original_keys;
  for (const auto& item : data) {
    auto subject = std::make_shared<Subject>();

    auto key_map = get_key_map(item);

    if (first) {
      auto keys = get_keys(item);
      auto domain_names = ProjectUtils::determine_domain_names(keys);
      project->set_domain_names(domain_names);
      ProjectUtils::determine_domain_types(project, key_map);
      original_keys = ProjectUtils::get_original_keys(domain_names, key_map);
    }

    auto domains = project->get_domain_names();
    if (item.contains("name")) {
      subject->set_display_name(item["name"]);
    }
    subject->set_number_of_domains(domains.size());

    auto get_list = [&](auto prefix) { return ProjectUtils::get_matching_values({prefix}, domains, key_map); };

    subject->set_original_filenames(
        ProjectUtils::get_matching_values(ProjectUtils::get_input_prefixes(), domains, key_map));
    subject->set_groomed_filenames(
        ProjectUtils::get_matching_values(ProjectUtils::get_groomed_prefixes(), domains, key_map));
    subject->set_landmarks_filenames(get_list(ProjectUtils::LANDMARKS_FILE_PREFIX));
    subject->set_constraints_filenames(get_list(ProjectUtils::CONSTRAINTS_PREFIX));
    subject->set_groomed_transforms(
        ProjectUtils::get_transforms(ProjectUtils::GROOMED_TRANSFORMS_PREFIX, domains, key_map));
    subject->set_procrustes_transforms(
        ProjectUtils::get_transforms(ProjectUtils::PROCRUSTES_TRANSFORMS_PREFIX, domains, key_map));
    subject->set_image_filenames(get_list(ProjectUtils::IMAGE_PREFIX));

    subject->set_feature_filenames(ProjectUtils::get_value_map(ProjectUtils::FEATURE_PREFIX, key_map));
    subject->set_group_values(ProjectUtils::get_value_map(ProjectUtils::GROUP_PREFIX, key_map));

    subject->set_local_particle_filenames(get_list(ProjectUtils::LOCAL_PARTICLES));
    subject->set_world_particle_filenames(get_list(ProjectUtils::WORLD_PARTICLES));

    // extra
    subject->set_extra_values(ProjectUtils::get_extra_columns(key_map));
    // table values
    subject->set_table_values(key_map);

    subjects.push_back(subject);
  }

  project->set_subjects(subjects);
}

//---------------------------------------------------------------------------
static void read_landmark_definitions(ProjectHandle project, const json& j) {
  if (!j.contains("landmarks")) {
    return;
  }

  auto landmarks = j["landmarks"];

  std::vector<std::vector<LandmarkDefinition>> definitions;

  auto domain_names = project->get_domain_names();
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

  project->set_landmark_definitions(definitions);
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
bool JsonProjectReader::read_project(ProjectHandle project, std::string filename) {
  try {
    std::ifstream ifs(filename);
    json j = json::parse(ifs);

    read_subjects(project, j);

    read_landmark_definitions(project, j);

    // read parameter sheets
    project->set_parameter_map("groom", read_parameter_map(j, "groom)"));
    project->set_parameters("optimize", read_parameters(j, "optimize)"));
    project->set_parameters("studio", read_parameters(j, "studio"));
    project->set_parameters("project", read_parameters(j, "project"));
    project->set_parameters("analysis", read_parameters(j, "analysis"));
    project->set_parameters("deepssm", read_parameters(j, "deepssm"));

  } catch (std::exception& e) {
    std::cerr << "Error reading " << filename << " : " << e.what() << "\n";
    return false;
  }
  return true;
}

}  // namespace shapeworks
