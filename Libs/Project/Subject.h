#pragma once

#include <string>
#include <vector>
#include <map>

#include <Libs/Optimize/ParticleSystem/DomainType.h>

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

  //! Return the domain types
  std::vector<DomainType> get_domain_types();

  //! Set groomed filenames
  void set_groomed_filenames(std::vector<std::string> filenames);
  //! Get groomed filenames
  std::vector<std::string> get_groomed_filenames();

  //! Set local particle filenames (one per domain)
  void set_local_particle_filenames(std::vector<std::string> filenames);
  //! Get local particle filenames
  std::vector<std::string> get_local_particle_filenames();

  //! Set the world particle filenames
  void set_world_particle_filenames(std::vector<std::string> filenames);
  //! Get the world particle filenames
  std::vector<std::string> get_world_particle_filenames();

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

  //! Get the group values
  std::map<std::string, std::string> get_group_values() const;
  //! Get a specific group value
  std::string get_group_value(std::string group_name);

  //! Set the group values
  void set_group_values(const std::map<std::string, std::string>& group_values);

  //! Get extra values (extra columns we don't interpret)
  std::map<std::string, std::string> get_extra_values() const;
  void set_extra_values(std::map<std::string, std::string> extra_values);

private:

  int number_of_domains_ = 0;

  std::vector<std::string> segmentation_filenames_;
  std::vector<std::string> groomed_filenames_;
  std::vector<std::string> local_particle_filenames_;
  std::vector<std::string> world_particle_filenames_;
  std::vector<std::vector<double>> groomed_transforms_;

  std::map<std::string, std::string> feature_filenames_;
  std::map<std::string, std::string> group_values_;
  std::map<std::string, std::string> extra_values_;
};
}
