#pragma once

#include <string>
#include <vector>
#include <map>

namespace shapeworks {

//! Representation of a single subject.
/*!
 * The Subject class encapsulates one "sample" in the shapeworks system (e.g. one row in a spreadsheet project)
 * A Subject may have multiple segmentations/domains associated with it.
 *
 */
class Subject {

public:

  Subject();
  ~Subject();

  //! Set segmentatation filenames (one per domain)
  void set_segmentation_filenames(std::vector<std::string> filenames);
  //! Get segmentation filenames
  std::vector<std::string> get_segmentation_filenames();

  //! Set groomed filenames
  void set_groomed_filenames(std::vector<std::string> filenames);
  //! Get groomed filenames
  std::vector<std::string> get_groomed_filenames();

  //! Set local particle filename (only one per subject)
  void set_local_particle_filename(std::string filename);
  //! Get local particle filename
  std::string get_local_particle_filename();

  //! Set the world particle filename
  void set_global_particle_filename(std::string filename);
  //! Get the world particle filename
  std::string get_global_particle_filename();

  //! Set the number of domains
  void set_number_of_domains(int number_of_domains);
  //! Get the number of domains
  int get_number_of_domains();

  //! Get the feature map filenames
  std::map<std::string, std::string> get_feature_filenames() const;
  //! Set the feature map filenames
  void set_feature_filenames(const std::map<std::string, std::string>& feature_filenames);

  //! Get the groomed transforms (one vector per domain)
  std::vector<std::vector<double>> get_groomed_transforms() const;
  //! Set the groomed transforms (one vector per domain)
  void set_groomed_transforms(std::vector<std::vector<double>> transforms);

private:

  int number_of_domains_ = 0;

  std::vector<std::string> segmentation_filenames_;
  std::vector<std::string> mesh_filenames_;
  std::vector<std::string> groomed_filenames_;
  std::string local_particle_filename_;
  std::string global_particle_filename_;
  std::vector<std::vector<double>> groomed_transforms_;

  std::map<std::string, std::string> feature_filenames_;
};
}
