#include <Libs/Project/Subject.h>
#include <Libs/Mesh/Mesh.h>
#include <Libs/Utils/StringUtils.h>

#include <map>

using namespace shapeworks;

//---------------------------------------------------------------------------
Subject::Subject()
{}

//---------------------------------------------------------------------------
Subject::~Subject()
{}

//---------------------------------------------------------------------------
void Subject::set_original_filenames(std::vector<std::string> filenames)
{
  this->original_filenames_ = filenames;
}

//---------------------------------------------------------------------------
void Subject::set_groomed_filenames(std::vector<std::string> filenames)
{
  this->groomed_filenames_ = filenames;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_groomed_filenames()
{
  return this->groomed_filenames_;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_original_filenames()
{
  return this->original_filenames_;
}

//---------------------------------------------------------------------------
void Subject::set_number_of_domains(int number_of_domains)
{
  this->number_of_domains_ = number_of_domains;
}

//---------------------------------------------------------------------------
int Subject::get_number_of_domains()
{
  return this->number_of_domains_;
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> Subject::get_feature_filenames() const
{
  return feature_filenames_;
}

//---------------------------------------------------------------------------
void Subject::set_feature_filenames(const std::map<std::string, std::string>& feature_filenames)
{
  this->feature_filenames_ = feature_filenames;
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> Subject::get_groomed_transforms() const
{
  return this->groomed_transforms_;
}

//---------------------------------------------------------------------------
void Subject::set_groomed_transforms(std::vector<std::vector<double>> transforms)
{
  this->groomed_transforms_ = transforms;
}

//---------------------------------------------------------------------------
void Subject::set_groomed_transform(int i, std::vector<double> transform)
{
  if (i >= this->groomed_transforms_.size()) {
    this->groomed_transforms_.resize(i + 1);
  }
  this->groomed_transforms_[i] = transform;
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> Subject::get_procrustes_transforms() const
{
  return this->procrustes_transforms_;
}

//---------------------------------------------------------------------------
void Subject::set_procrustes_transforms(std::vector<std::vector<double>> transforms)
{
  this->procrustes_transforms_ = transforms;
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> Subject::get_group_values() const
{
  return this->group_values_;
}

//---------------------------------------------------------------------------
void Subject::set_group_values(const std::map<std::string, std::string>& group_values)
{
  this->group_values_ = group_values;
}

//---------------------------------------------------------------------------
std::string Subject::get_group_value(std::string group_name)
{
  return this->group_values_[group_name];
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> Subject::get_extra_values() const
{
  return this->extra_values_;
}

//---------------------------------------------------------------------------
void Subject::set_extra_values(std::map<std::string, std::string> extra_values)
{
  this->extra_values_ = extra_values;
}

//---------------------------------------------------------------------------
std::map<std::string, std::string> Subject::get_table_values() const
{
  return this->table_values_;
}

//---------------------------------------------------------------------------
void Subject::set_table_values(std::map<std::string, std::string> table_values)
{
  this->table_values_ = table_values;
}

//---------------------------------------------------------------------------
std::string Subject::get_display_name()
{
  return this->display_name_;
}

//---------------------------------------------------------------------------
void Subject::set_display_name(std::string display_name)
{
  this->display_name_ = display_name;
}

//---------------------------------------------------------------------------
void Subject::set_local_particle_filenames(std::vector<std::string> filenames)
{
  this->local_particle_filenames_ = filenames;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_local_particle_filenames()
{
  return this->local_particle_filenames_;
}

//---------------------------------------------------------------------------
void Subject::set_world_particle_filenames(std::vector<std::string> filenames)
{
  this->world_particle_filenames_ = filenames;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_world_particle_filenames()
{
  return this->world_particle_filenames_;
}

//---------------------------------------------------------------------------
void Subject::set_landmarks_filenames(std::vector<std::string> filenames)
{
  this->landmarks_filenames_ = filenames;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_landmarks_filenames()
{
  return this->landmarks_filenames_;
}

//---------------------------------------------------------------------------
void Subject::set_constraints_filenames(std::vector<std::string> filenames)
{
  constraints_filenames_ = filenames;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_constraints_filenames()
{
  return constraints_filenames_;
}

//---------------------------------------------------------------------------
void Subject::set_image_filenames(std::vector<std::string> filenames)
{
  this->image_filenames_ = filenames;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_image_filenames()
{
  return this->image_filenames_;
}
