#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Project/Subject.h>
#include <Utils/StringUtils.h>

#include <map>

using namespace shapeworks;
using StringMap = project::types::StringMap;
using StringList = project::types::StringList;

//---------------------------------------------------------------------------
Subject::Subject() {}

//---------------------------------------------------------------------------
Subject::~Subject() {}

//---------------------------------------------------------------------------
void Subject::set_original_filenames(StringList filenames) { original_filenames_ = filenames; }

//---------------------------------------------------------------------------
void Subject::set_groomed_filenames(StringList filenames) { groomed_filenames_ = filenames; }

//---------------------------------------------------------------------------
StringList Subject::get_groomed_filenames() { return groomed_filenames_; }

//---------------------------------------------------------------------------
StringList Subject::get_original_filenames() { return original_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_number_of_domains(int number_of_domains) { number_of_domains_ = number_of_domains; }

//---------------------------------------------------------------------------
int Subject::get_number_of_domains() { return number_of_domains_; }

//---------------------------------------------------------------------------
StringMap Subject::get_feature_filenames() const { return feature_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_feature_filenames(const StringMap& feature_filenames) { feature_filenames_ = feature_filenames; }

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
StringMap Subject::get_group_values() const { return group_values_; }

//---------------------------------------------------------------------------
void Subject::set_group_values(const StringMap& group_values) { group_values_ = group_values; }

//---------------------------------------------------------------------------
std::string Subject::get_group_value(std::string group_name) { return group_values_[group_name]; }

//---------------------------------------------------------------------------
StringMap Subject::get_extra_values() const { return extra_values_; }

//---------------------------------------------------------------------------
void Subject::set_extra_values(StringMap extra_values) { extra_values_ = extra_values; }

//---------------------------------------------------------------------------
StringMap Subject::get_table_values() const { return table_values_; }

//---------------------------------------------------------------------------
void Subject::set_table_values(StringMap table_values) { table_values_ = table_values; }

//---------------------------------------------------------------------------
std::string Subject::get_display_name() { return display_name_; }

//---------------------------------------------------------------------------
void Subject::set_display_name(std::string display_name) {
  display_name_ = display_name;
  table_values_["name"] = display_name;
}

//---------------------------------------------------------------------------
bool Subject::is_fixed() { return fixed_; }

//---------------------------------------------------------------------------
void Subject::set_fixed(bool fixed) { fixed_ = fixed; }

//---------------------------------------------------------------------------
bool Subject::is_excluded() { return excluded_; }

//---------------------------------------------------------------------------
void Subject::set_explanatory_variable(double val) { explanatory_variable_ = val; }

//---------------------------------------------------------------------------
double Subject::get_explanatory_variable() { return explanatory_variable_; }

//---------------------------------------------------------------------------
void Subject::set_excluded(bool excluded) { excluded_ = excluded; }

//---------------------------------------------------------------------------
std::string Subject::get_notes() { return notes_; }

//---------------------------------------------------------------------------
void Subject::set_notes(std::string notes) {
  notes_ = notes;
  table_values_["notes"] = notes;
}

//---------------------------------------------------------------------------
void Subject::set_local_particle_filenames(StringList filenames) { local_particle_filenames_ = filenames; }

//---------------------------------------------------------------------------
StringList Subject::get_local_particle_filenames() { return local_particle_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_world_particle_filenames(StringList filenames) { world_particle_filenames_ = filenames; }

//---------------------------------------------------------------------------
StringList Subject::get_world_particle_filenames() { return world_particle_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_landmarks_filenames(StringList filenames) { landmarks_filenames_ = filenames; }

//---------------------------------------------------------------------------
StringList Subject::get_landmarks_filenames() { return landmarks_filenames_; }

//---------------------------------------------------------------------------
void Subject::set_constraints_filenames(StringList filenames) { constraints_filenames_ = filenames; }

//---------------------------------------------------------------------------
StringList Subject::get_constraints_filenames() { return constraints_filenames_; }
//---------------------------------------------------------------------------
