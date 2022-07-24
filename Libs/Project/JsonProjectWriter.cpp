#include "JsonProjectWriter.h"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace shapeworks {

//---------------------------------------------------------------------------
static void assign_keys(json &j, std::string prefix, std::vector<std::string> filenames,
                        std::vector<std::string> domains) {
  if (filenames.empty()) {
    return;
  }
  if (filenames.size() != domains.size()) {
    throw std::runtime_error(prefix + " filenames and number of domains mismatch");
  }
  for (int i = 0; i < domains.size(); i++) {
    std::string key = prefix + "_" + domains[i];
    std::string value = filenames[i];
    j[key] = value;
  }
}

//---------------------------------------------------------------------------
static std::string transform_to_string(std::vector<double> transform) {
  std::string str;
  for (int j = 0; j < transform.size(); j++) {
    str = str + " " + std::to_string(transform[j]);
  }
  return str;
}

//---------------------------------------------------------------------------
static void assign_transforms(json &j, std::string prefix, std::vector<std::vector<double>> transforms,
                              std::vector<std::string> domains) {
  if (transforms.empty()) {
    return;
  }
  if (transforms.size() != domains.size()) {
    throw std::runtime_error(prefix + " filenames and number of domains mismatch");
  }
  for (int i = 0; i < domains.size(); i++) {
    std::string key = prefix + "_" + domains[i];
    std::string value = transform_to_string(transforms[i]);
    j[key] = value;
  }
}

//---------------------------------------------------------------------------
static json create_data_sheet(ProjectHandle project) {
  // json j;

  auto subjects = project->get_subjects();

  auto domains = project->get_domain_names();

  std::vector<json> list;
  for (int i = 0; i < subjects.size(); i++) {
    std::shared_ptr<Subject> subject = subjects[i];
    json j;
    j["name"] = subject->get_display_name();

    assign_keys(j, "shape", subject->get_original_filenames(), domains);
    assign_keys(j, "landmarks_file", subject->get_landmarks_filenames(), domains);
    assign_keys(j, "groomed", subject->get_groomed_filenames(), domains);
    assign_keys(j, "local_particles", subject->get_local_particle_filenames(), domains);
    assign_keys(j, "world_particles", subject->get_world_particle_filenames(), domains);
    assign_transforms(j, "alignment", subject->get_groomed_transforms(), domains);
    assign_transforms(j, "procrustes", subject->get_procrustes_transforms(), domains);
  }

  return list;
}

//---------------------------------------------------------------------------
bool JsonProjectWriter::write_project(ProjectHandle project, std::string filename) {
  json j;

  json data_sheet;
  json groom_sheet;
  json optimize_sheet;
  json studio_sheet;
  json project_sheet;
  json analysis_sheet;
  json deepssm_sheet;
  json landmarks_sheet;

  j["data"] = create_data_sheet(project);
  j["groom"] = groom_sheet;
  j["optimize"] = optimize_sheet;
  j["studio"] = studio_sheet;
  j["project"] = project_sheet;
  j["analysis"] = analysis_sheet;
  j["deepssm"] = deepssm_sheet;
  j["landmarks"] = landmarks_sheet;

  std::ofstream file(filename);
  if (!file.good()) {
    throw std::runtime_error("Unable to open " + filename + " for writing");
  }
  file << j.dump(4);

  return true;
}

}  // namespace shapeworks
