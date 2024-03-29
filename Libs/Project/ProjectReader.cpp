#include "ProjectReader.h"

#include <StringUtils.h>

#include <algorithm>

namespace shapeworks {

using namespace project::prefixes;

using namespace project::types;

//---------------------------------------------------------------------------
ProjectReader::ProjectReader(Project& project) : project_(project) {}

//---------------------------------------------------------------------------
void ProjectReader::load_subjects(StringMapList list) {
  std::vector<std::shared_ptr<Subject>> subjects;
  bool first = true;
  for (auto& item : list) {
    auto subject = std::make_shared<Subject>();

    auto& key_map = item;

    if (first) {
      auto keys = get_keys(item);
      auto domain_names = ProjectUtils::determine_domain_names(keys);
      project_.set_domain_names(domain_names);
      ProjectUtils::determine_domain_types(&project_, key_map);
    }

    auto domains = project_.get_domain_names();

    subject->set_number_of_domains(domains.size());

    auto get_list = [&](auto prefix) { return ProjectUtils::get_values({prefix}, domains, key_map); };

    subject->set_original_filenames(ProjectUtils::get_values(ProjectUtils::get_input_prefixes(), domains, key_map));
    subject->set_groomed_filenames(ProjectUtils::get_values(ProjectUtils::get_groomed_prefixes(), domains, key_map));
    subject->set_landmarks_filenames(get_list(LANDMARKS_FILE_PREFIX));
    subject->set_constraints_filenames(get_list(CONSTRAINTS_PREFIX));
    auto alignment_domains = domains;
    if (domains.size() > 1) {
      alignment_domains.push_back("global");
    }
    subject->set_groomed_transforms(
        ProjectUtils::get_transforms(GROOMED_TRANSFORMS_PREFIX, alignment_domains, key_map));
    subject->set_procrustes_transforms(ProjectUtils::get_transforms(PROCRUSTES_TRANSFORMS_PREFIX, domains, key_map));

    subject->set_feature_filenames(ProjectUtils::get_value_map({IMAGE_PREFIX, FEATURE_PREFIX}, key_map));
    subject->set_group_values(ProjectUtils::get_value_map({GROUP_PREFIX}, key_map));

    subject->set_local_particle_filenames(get_list(LOCAL_PARTICLES_PREFIX));
    subject->set_world_particle_filenames(get_list(WORLD_PARTICLES_PREFIX));

    // extra
    subject->set_extra_values(ProjectUtils::get_extra_columns(key_map));
    // table values
    subject->set_table_values(key_map);

    std::string name;
    if (contains(item, "name")) {
      name = item["name"];
    }
    if (contains(item, "fixed")) {
      subject->set_fixed(Variant(item["fixed"]));
    }
    if (contains(item, "excluded")) {
      subject->set_excluded(Variant(item["excluded"]));
    }
    if (name == "") {
      if (subject->get_original_filenames().size() != 0) {
        name = StringUtils::getBaseFilenameWithoutExtension(subject->get_original_filenames()[0]);
      } else if (subject->get_groomed_filenames().size() != 0) {
        name = StringUtils::getBaseFilenameWithoutExtension(subject->get_groomed_filenames()[0]);
      } else if (subject->get_local_particle_filenames().size() > 0) {
        name = StringUtils::getBaseFilenameWithoutExtension(subject->get_local_particle_filenames()[0]);
      }
    }
    subject->set_display_name(name);

    std::string notes;
    if (contains(item, "notes")) {
      notes = item["notes"];
    }
    subject->set_notes(notes);

    subjects.push_back(subject);
  }

  project_.set_subjects(subjects);
}

//---------------------------------------------------------------------------
void ProjectReader::load_parameters() {
  for (auto i : {"optimize", "studio", "project", "analysis", "deepssm"}) {
    load_parameter(i, get_parameters(i));
  }

  // handle groom multi-map
  std::map<std::string, Parameters> map;
  for (auto& [key, value] : get_multi_parameters("groom")) {
    map[key] = Parameters(value);
  }
  project_.set_parameter_map("groom", map);
}

//---------------------------------------------------------------------------
void ProjectReader::load_parameter(std::string name, StringMap map) {
  Parameters p(map);
  project_.set_parameters(name, p);
}

//---------------------------------------------------------------------------
void ProjectReader::load_landmark_definitions(StringMapList list) {
  std::vector<std::vector<LandmarkDefinition>> definitions;

  auto domain_names = project_.get_domain_names();
  definitions.resize(domain_names.size());

  for (auto landmark : list) {
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

  project_.set_landmark_definitions(definitions);
}

//---------------------------------------------------------------------------
StringList ProjectReader::get_keys(StringMap map) {
  std::vector<std::string> keys;
  for (auto& [key, value] : map) {
    keys.push_back(key);
  }
  return keys;
}

//---------------------------------------------------------------------------
bool ProjectReader::contains(StringMap map, std::string key) { return map.find(key) != map.end(); }
//---------------------------------------------------------------------------

}  // namespace shapeworks
