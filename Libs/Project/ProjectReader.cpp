#include "ProjectReader.h"

namespace shapeworks {

using namespace project::prefixes;

using namespace project::types;

//---------------------------------------------------------------------------
ProjectReader::ProjectReader(Project& project) : project_(project) {}

//---------------------------------------------------------------------------
void ProjectReader::load_subjects(StringMapList list) {
  std::vector<std::shared_ptr<Subject>> subjects;
  bool first = true;
  std::vector<std::string> original_keys;
  for (auto& item : list) {
    auto subject = std::make_shared<Subject>();

    auto& key_map = item;

    if (first) {
      auto keys = get_keys(item);
      auto domain_names = ProjectUtils::determine_domain_names(keys);
      project_.set_domain_names(domain_names);
      ProjectUtils::determine_domain_types(project_, key_map);
      original_keys = ProjectUtils::get_original_keys(domain_names, key_map);
    }

    auto domains = project_.get_domain_names();
    if (contains(item, "name")) {
      subject->set_display_name(item["name"]);
    }
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
    subject->set_image_filenames(get_list(IMAGE_PREFIX));

    subject->set_feature_filenames(ProjectUtils::get_value_map(FEATURE_PREFIX, key_map));
    subject->set_group_values(ProjectUtils::get_value_map(GROUP_PREFIX, key_map));

    subject->set_local_particle_filenames(get_list(LOCAL_PARTICLES_PREFIX));
    subject->set_world_particle_filenames(get_list(WORLD_PARTICLES_PREFIX));

    // extra
    subject->set_extra_values(ProjectUtils::get_extra_columns(key_map));
    // table values
    subject->set_table_values(key_map);

    subjects.push_back(subject);
  }

  project_.set_subjects(subjects);
}

//---------------------------------------------------------------------------
std::vector<std::string> ProjectReader::get_keys(project::types::StringMap map) {
  std::vector<std::string> keys;
  for (auto& [key, value] : map) {
    keys.push_back(key);
  }
  return keys;
}

//---------------------------------------------------------------------------
bool ProjectReader::contains(project::types::StringMap map, std::string key) { return map.find(key) != map.end(); }

}  // namespace shapeworks
