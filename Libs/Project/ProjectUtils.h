#pragma once

#include <vtkSmartPointer.h>
#include <vtkTransform.h>

#include <vector>

#include "Project.h"

namespace shapeworks {

namespace project::prefixes {
static constexpr const char* SEGMENTATION_PREFIX = "segmentation_";
static constexpr const char* LANDMARKS_FILE_PREFIX = "landmarks_file_";

static constexpr const char* SHAPE_PREFIX = "shape_";
static constexpr const char* MESH_PREFIX = "mesh_";
static constexpr const char* CONTOUR_PREFIX = "contour_";

static constexpr const char* GROOMED_PREFIX = "groomed_";
static constexpr const char* GROOMED_CONTOUR_PREFIX = "groomed_contour_";

static constexpr const char* CONSTRAINTS_PREFIX = "constraints_";

static constexpr const char* GROOMED_TRANSFORMS_PREFIX = "alignment_";
static constexpr const char* PROCRUSTES_TRANSFORMS_PREFIX = "procrustes_";
static constexpr const char* IMAGE_PREFIX = "image_";
static constexpr const char* FEATURE_PREFIX = "feature_";
static constexpr const char* GROUP_PREFIX = "group_";

static constexpr const char* LOCAL_PARTICLES = "local_particles";
static constexpr const char* WORLD_PARTICLES = "world_particles";

static constexpr const char* LOCAL_PARTICLES_PREFIX = "local_particles_";
static constexpr const char* WORLD_PARTICLES_PREFIX = "world_particles_";

}  // namespace project::prefixes

namespace project::types {
  using StringMap = std::map<std::string, std::string>;
  using StringList = std::vector<std::string>;
  using StringMapList = std::vector<StringMap>;
}

class ProjectUtils {
 public:

  using StringList = project::types::StringList;
  using StringMap = project::types::StringMap;

  // convert a list of doubles from a spreadsheet to a vtkTransform
  static vtkSmartPointer<vtkTransform> convert_transform(std::vector<double> list);

  // convert a vtkTransform to a list of doubles (e.g. for project spreadsheet)
  static std::vector<double> convert_transform(vtkSmartPointer<vtkTransform> transform);

  //! determine domain names from a subject's keys
  static StringList determine_domain_names(StringList keys);

  //! determine and set domain types
  static void determine_domain_types(Project& project, StringMap key_map);

  //! return a list of the input prefixes
  static StringList get_input_prefixes();

  //! return a list of the groomed prefixes
  static StringList get_groomed_prefixes();

  //! extract original keys from key_map given an ordered list of domain names
  static StringList get_original_keys(StringList domain_names, StringMap key_map);

  //! get the values for a list of prefixes from key_map, ordered by domain_names
  static StringList get_values(StringList prefixes, StringList domain_names, StringMap key_map);

  //! get the transforms for a prefix from key_map, ordered by domain_names
  static std::vector<std::vector<double>> get_transforms(std::string prefix, StringList domain_names,
                                                         StringMap key_map);

  //! get a map<string,string> for a prefix from a key_map (e.g. group_, or feature_)
  static StringMap get_value_map(std::string prefix, StringMap key_map);

  //! get any extra columns that don't match prefixes
  static StringMap get_extra_columns(StringMap key_map);

  //! determine the domain type, given a filename
  static DomainType determine_domain_type(std::string filename);
};
}  // namespace shapeworks
