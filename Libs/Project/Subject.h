#pragma once

#include <string>
#include <vector>

namespace shapeworks
{

class Subject {

public:

  Subject();
  ~Subject();

  void set_segmentation_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_segmentation_filenames();

  void set_groomed_filenames(std::vector<std::string> filenames);
  std::vector<std::string> get_groomed_filenames();

  void set_number_of_domains(int number_of_domains);

private:

  int number_of_domains_ = 0;

  std::vector<std::string> segmentation_filenames_;
  std::vector<std::string> mesh_filenames_;
  std::vector<std::string> groomed_filenames_;
  std::string local_particle_filenames_;
  std::string world_particle_filenames_;
};
}
