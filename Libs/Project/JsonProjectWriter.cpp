#include "JsonProjectWriter.h"

#include <fstream>
#include <nlohmann/json.hpp>

#include "ProjectUtils.h"
using json = nlohmann::ordered_json;

namespace shapeworks {

//---------------------------------------------------------------------------
static void assign_keys(json& j, std::vector<std::string> prefixes, std::vector<std::string> filenames,
                        std::vector<std::string> domains) {
  if (filenames.empty()) {
    return;
  }
  auto prefix = prefixes[0];
  if (filenames.size() != domains.size()) {
    throw std::runtime_error(prefix + " filenames and number of domains mismatch");
  }
  for (int i = 0; i < domains.size(); i++) {
    if (prefixes.size() == domains.size()) {
      prefix = prefixes[i];
    }
    std::string key = prefix + "_" + domains[i];
    std::string value = filenames[i];
    j[key] = value;
  }
}

//---------------------------------------------------------------------------
static void assign_transforms(json& j, std::string prefix, std::vector<std::vector<double>> transforms,
                              std::vector<std::string> domains) {
  if (transforms.empty()) {
    return;
  }
  if (transforms.size() != domains.size() && transforms.size() != domains.size() + 1) {
    throw std::runtime_error(prefix + " filenames and number of domains mismatch");
  }
  for (int i = 0; i < transforms.size(); i++) {
    std::string key = prefix + "_";
    if (i < domains.size()) {
      key = key + domains[i];
    } else {
      key = key + "global";
    }
    std::string value = ProjectUtils::transform_to_string(transforms[i]);
    j[key] = value;
  }
}

//---------------------------------------------------------------------------
static json create_data_object(Project& project) {
  auto subjects = project.get_subjects();
  auto domains = project.get_domain_names();

  std::vector<json> list;
  for (int i = 0; i < subjects.size(); i++) {
    auto subject = subjects[i];
    json j;
    j["name"] = subject->get_display_name();

    auto original_prefixes = ProjectUtils::convert_domain_types(project.get_original_domain_types());
    auto groomed_prefixes = ProjectUtils::convert_groomed_domain_types(project.get_groomed_domain_types());

    assign_keys(j, original_prefixes, subject->get_original_filenames(), domains);
    assign_keys(j, {"landmarks_file"}, subject->get_landmarks_filenames(), domains);
    assign_keys(j, groomed_prefixes, subject->get_groomed_filenames(), domains);
    assign_keys(j, {"local_particles"}, subject->get_local_particle_filenames(), domains);
    assign_keys(j, {"world_particles"}, subject->get_world_particle_filenames(), domains);
    assign_transforms(j, {"alignment"}, subject->get_groomed_transforms(), domains);
    assign_transforms(j, {"procrustes"}, subject->get_procrustes_transforms(), domains);

    // write out extra values
    for (auto& [key, value] : subject->get_extra_values()) {
      j[key] = value;
    }
    list.push_back(j);
  }

  return list;
}

//---------------------------------------------------------------------------
static json create_parameter_object(Parameters params) {
  json j;
  for (const auto& kv : params.get_map()) {
    j[kv.first] = kv.second;
  }
  return j;
}

//---------------------------------------------------------------------------
static json create_parameter_map_object(std::map<std::string, Parameters> parameter_map) {
  json j;
  for (auto& [key, params] : parameter_map) {
    j[key] = create_parameter_object(params);
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
  j["optimize"] = create_parameter_object(project.get_parameters("optimize"));
  j["studio"] = create_parameter_object(project.get_parameters("studio"));
  j["project"] = create_parameter_object(project.get_parameters("project"));
  j["analysis"] = create_parameter_object(project.get_parameters("analysis"));
  j["deepssm"] = create_parameter_object(project.get_parameters("deepssm"));
  j["landmarks"] = create_landmark_definition_object(project);

  std::ofstream file(filename);
  if (!file.good()) {
    throw std::runtime_error("Unable to open " + filename + " for writing");
  }
  file << j.dump(4);

  return true;
}

}  // namespace shapeworks
