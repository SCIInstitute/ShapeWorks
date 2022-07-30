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
static std::map<std::string,std::string> get_key_map(json item) {
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

    auto get_list = [&](auto prefix) {
      return ProjectUtils::get_matching_values({prefix}, domains, key_map);
    };


    subject->set_original_filenames(ProjectUtils::get_matching_values(ProjectUtils::get_input_prefixes(), domains, key_map));
    subject->set_groomed_filenames(ProjectUtils::get_matching_values(ProjectUtils::get_groomed_prefixes(), domains, key_map));
    subject->set_landmarks_filenames(get_list(ProjectUtils::LANDMARKS_FILE_PREFIX));
    subject->set_constraints_filenames(get_list(ProjectUtils::CONSTRAINTS_PREFIX));
    subject->set_groomed_transforms(ProjectUtils::get_transforms(ProjectUtils::GROOMED_TRANSFORMS_PREFIX, domains, key_map));
    subject->set_procrustes_transforms(ProjectUtils::get_transforms(ProjectUtils::PROCRUSTES_TRANSFORMS_PREFIX, domains, key_map));
    subject->set_image_filenames(get_list(ProjectUtils::IMAGE_PREFIX));



    subjects.push_back(subject);
  }

  project->set_subjects(subjects);
}

//---------------------------------------------------------------------------
bool JsonProjectReader::read_project(ProjectHandle project, std::string filename) {
  std::ifstream ifs(filename);
  json j = json::parse(ifs);

  read_subjects(project, j);

  return true;
}

}  // namespace shapeworks
