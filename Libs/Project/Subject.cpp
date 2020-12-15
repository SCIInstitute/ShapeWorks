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
void Subject::set_segmentation_filenames(std::vector<std::string> filenames)
{
  this->segmentation_filenames_ = filenames;
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
void Subject::set_local_particle_filename(std::string filename)
{
  this->local_particle_filename_ = filename;
}

//---------------------------------------------------------------------------
std::string Subject::get_local_particle_filename()
{
  return this->local_particle_filename_;
}

//---------------------------------------------------------------------------
void Subject::set_global_particle_filename(std::string filename)
{
  this->global_particle_filename_ = filename;
}

//---------------------------------------------------------------------------
std::string Subject::get_global_particle_filename()
{
  return this->global_particle_filename_;
}

//---------------------------------------------------------------------------
std::vector<std::string> Subject::get_segmentation_filenames()
{
  return this->segmentation_filenames_;
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
std::vector<DomainType> Subject::get_domain_types()
{
  std::vector<DomainType> domain_types;
  for (auto name: this->segmentation_filenames_) {
    std::transform(name.begin(), name.end(), name.begin(), ::tolower);

    bool mesh = false;

    for (auto type : Mesh::get_supported_types()) {
      if (StringUtils::hasSuffix(name, type)) {
        mesh = true;
      }
    }

    if (mesh) {
      domain_types.push_back(DomainType::Mesh);
    }
    else {
      domain_types.push_back(DomainType::Image);
    }

  }
  return domain_types;
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
