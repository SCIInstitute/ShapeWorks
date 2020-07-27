#include <Libs/Project/Subject.h>

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
const std::vector<std::string>& Subject::get_feature_filenames() const
{
  return feature_filenames_;
}

//---------------------------------------------------------------------------
void Subject::set_feature_filenames(const std::vector<std::string>& feature_filenames)
{
  feature_filenames_ = feature_filenames;
}
