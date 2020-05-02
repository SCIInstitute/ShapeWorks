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
std::vector<std::string> Subject::get_segmentation_filenames()
{
  return this->segmentation_filenames_;
}

//---------------------------------------------------------------------------
void Subject::set_number_of_domains(int number_of_domains)
{
  this->number_of_domains_ = number_of_domains;
}
