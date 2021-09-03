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

  //! Get the landmarks filenames (one per domain)
  void set_landmarks_filenames(std::vector<std::string> filenames);
  //! Set the landmarks filenames
  std::vector<std::string> get_landmarks_filenames();

  //! Set the number of domains
  void set_number_of_domains(int number_of_domains);
  //! Get the number of domains
  int get_number_of_domains();

  //! Set image filenames
  void set_image_filenames(std::vector<std::string> filenames);
  //! Get image filenames
  std::vector<std::string> get_image_filenames();

  //! Get the feature map filenames
  std::map<std::string, std::string> get_feature_filenames() const;
  //! Set the feature map filenames
  void set_feature_filenames(const std::map<std::string, std::string>& feature_filenames);

  //! Get the groomed transforms (one vector per domain)
  std::vector<std::vector<double>> get_groomed_transforms() const;
  //! Set the groomed transforms (one vector per domain)
  void set_groomed_transforms(std::vector<std::vector<double>> transforms);

  //! Get the procrustes transforms (one vector per domain)
  std::vector<std::vector<double>> get_procrustes_transforms() const;
  //! Set the procrustes transforms (one vector per domain)
  void set_procrustes_transforms(std::vector<std::vector<double>> transforms);

  //! Get the group values
  std::map<std::string, std::string> get_group_values() const;
  //! Get a specific group value
  std::string get_group_value(std::string group_name);

  //! Set the group values
  void set_group_values(const std::map<std::string, std::string>& group_values);

  //! Get extra values (extra columns we don't interpret)
  std::map<std::string, std::string> get_extra_values() const;
  void set_extra_values(std::map<std::string, std::string> extra_values);

  //! Get the display name
  std::string get_display_name();
  //! Set the display name
  void set_display_name(std::string display_name);

private:

  int number_of_domains_ = 0;

  std::string display_name_;
  std::vector<std::string> image_filenames_;
  std::vector<std::string> segmentation_filenames_;
  std::vector<std::string> groomed_filenames_;
  std::vector<std::string> local_particle_filenames_;
  std::vector<std::string> world_particle_filenames_;
  std::vector<std::string> landmarks_filenames_;
  std::vector<std::vector<double>> groomed_transforms_;
  std::vector<std::vector<double>> procrustes_transforms_;

  std::map<std::string, std::string> feature_filenames_;
  std::map<std::string, std::string> group_values_;
  std::map<std::string, std::string> extra_values_;
};
}
