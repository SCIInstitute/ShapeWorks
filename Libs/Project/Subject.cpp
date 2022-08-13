#include <Libs/Mesh/Mesh.h>
#include <Libs/Project/Subject.h>
#include <Libs/Utils/StringUtils.h>

#include <map>

using namespace shapeworks;

//---------------------------------------------------------------------------
Subject::Subject() {}

//---------------------------------------------------------------------------
Subject::~Subject() {}

//---------------------------------------------------------------------------
void Subject::set_original_filenames(std::vector<std::string> filenames) { original_filenames_ = filenames; }

//---------------------------------------------------------------------------
void Subject::set_groomed_filenames(std::vector<std::string> filenames) { groomed_filenames_ = filenames; }

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_groomed_filenames() { return groomed_filenames_; }

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_original_filenames() { return original_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_number_of_domains(int number_of_domains) { number_of_domains_ = number_of_domains; }

//---------------------------------------------------------------------------
int Subject::get_number_of_domains() { return number_of_domains_; }

//---------------------------------------------------------------------------
std::map<std::string, std::string> Subject::get_feature_filenames() const { return feature_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_feature_filenames(const std::map<std::string, std::string>& feature_filenames) {
  feature_filenames_ = feature_filenames;
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> Subject::get_groomed_transforms() const { return groomed_transforms_; }

//---------------------------------------------------------------------------
void Subject::set_groomed_transforms(std::vector<std::vector<double>> transforms) { groomed_transforms_ = transforms; }

//---------------------------------------------------------------------------
void Subject::set_groomed_transform(int i, std::vector<double> transform) {
  if (i >= groomed_transforms_.size()) {
    groomed_transforms_.resize(i + 1);
  }
  groomed_transforms_[i] = transform;
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> Subject::get_procrustes_transforms() const { return procrustes_transforms_; }

//---------------------------------------------------------------------------
void Subject::set_procrustes_transforms(std::vector<std::vector<double>> transforms) {
  procrustes_transforms_ = transforms;
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> Subject::get_group_values() const { return group_values_; }

//---------------------------------------------------------------------------
void Subject::set_group_values(const std::map<std::string, std::string>& group_values) { group_values_ = group_values; }

//---------------------------------------------------------------------------
std::string Subject::get_group_value(std::string group_name) { return group_values_[group_name]; }

//---------------------------------------------------------------------------
std::map<std::string, std::string> Subject::get_extra_values() const { return extra_values_; }

//---------------------------------------------------------------------------
void Subject::set_extra_values(std::map<std::string, std::string> extra_values) { extra_values_ = extra_values; }

//---------------------------------------------------------------------------
std::map<std::string, std::string> Subject::get_table_values() const { return table_values_; }

//---------------------------------------------------------------------------
void Subject::set_table_values(std::map<std::string, std::string> table_values) { table_values_ = table_values; }

//---------------------------------------------------------------------------
std::string Subject::get_display_name() { return display_name_; }

//---------------------------------------------------------------------------
void Subject::set_display_name(std::string display_name) { display_name_ = display_name; }

//---------------------------------------------------------------------------
void Subject::set_local_particle_filenames(std::vector<std::string> filenames) {
  local_particle_filenames_ = filenames;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_local_particle_filenames() { return local_particle_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_world_particle_filenames(std::vector<std::string> filenames) {
  world_particle_filenames_ = filenames;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_world_particle_filenames() { return world_particle_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_landmarks_filenames(std::vector<std::string> filenames) { landmarks_filenames_ = filenames; }

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_landmarks_filenames() { return landmarks_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_constraints_filenames(std::vector<std::string> filenames) { constraints_filenames_ = filenames; }

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_constraints_filenames() { return constraints_filenames_; }
//---------------------------------------------------------------------------
