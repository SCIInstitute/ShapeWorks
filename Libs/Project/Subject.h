#pragma once

#include <Optimize/DomainType.h>
#include "ProjectUtils.h"

#include <map>
#include <string>
#include <vector>

namespace shapeworks {

//! Representation of a single subject.
/*!
 * The Subject class encapsulates one "sample" in the shapeworks system (e.g. one row in a spreadsheet project)
 * A Subject may have multiple segmentations/domains associated with it.
 *
 */
class Subject {
 public:
  using StringMap = project::types::StringMap;
  using StringList = project::types::StringList;

  Subject();
  ~Subject();

  //! Set original filenames (one per domain)
  void set_original_filenames(StringList filenames);
  //! Get original filenames
  StringList get_original_filenames();

  //! Set groomed filenames
  void set_groomed_filenames(StringList filenames);
  //! Get groomed filenames
  StringList get_groomed_filenames();

  //! Set local particle filenames (one per domain)
  void set_local_particle_filenames(StringList filenames);
  //! Get local particle filenames
  StringList get_local_particle_filenames();

  //! Set the world particle filenames
  void set_world_particle_filenames(StringList filenames);
  //! Get the world particle filenames
  StringList get_world_particle_filenames();

  //! Get the landmarks filenames (one per domain)
  void set_landmarks_filenames(StringList filenames);
  //! Set the landmarks filenames
  StringList get_landmarks_filenames();

  //! Get the constraints filenames (one per domain)
  void set_constraints_filenames(StringList filenames);
  //! Set the constratins filenames
  StringList get_constraints_filenames();

  //! Set the number of domains
  void set_number_of_domains(int number_of_domains);
  //! Get the number of domains
  int get_number_of_domains();

  //! Get the feature map filenames
  StringMap get_feature_filenames() const;
  //! Set the feature map filenames
  void set_feature_filenames(const StringMap& feature_filenames);

  //! Get the groomed transforms (one vector per domain)
  std::vector<std::vector<double>> get_groomed_transforms() const;
  //! Set the groomed transforms (one vector per domain)
  void set_groomed_transforms(std::vector<std::vector<double>> transforms);
  //! Set the i-th groomed transform
  void set_groomed_transform(int i, std::vector<double> transform);

  //! Get the procrustes transforms (one vector per domain)
  std::vector<std::vector<double>> get_procrustes_transforms() const;
  //! Set the procrustes transforms (one vector per domain)
  void set_procrustes_transforms(std::vector<std::vector<double>> transforms);

  //! Get the group values
  StringMap get_group_values() const;
  //! Get a specific group value
  std::string get_group_value(std::string group_name);

  //! Set the group values
  void set_group_values(const StringMap& group_values);

  //! Get extra values (extra columns we don't interpret)
  StringMap get_extra_values() const;
  void set_extra_values(StringMap extra_values);

  //! Get all table values
  StringMap get_table_values() const;
  void set_table_values(StringMap table_values);

  //! Get the display name
  std::string get_display_name();
  //! Set the display name
  void set_display_name(std::string display_name);

 private:
  int number_of_domains_ = 0;

  std::string display_name_;
  StringList original_filenames_;
  StringList groomed_filenames_;
  StringList local_particle_filenames_;
  StringList world_particle_filenames_;
  StringList landmarks_filenames_;
  StringList constraints_filenames_;
  std::vector<std::vector<double>> groomed_transforms_;
  std::vector<std::vector<double>> procrustes_transforms_;

  StringMap feature_filenames_;
  StringMap group_values_;
  StringMap extra_values_;
  StringMap table_values_;
};
}  // namespace shapeworks
