#pragma once

#include <vtkSmartPointer.h>
#include <vtkTransform.h>

#include <vector>

#include "Project.h"

namespace shapeworks {

class ProjectUtils {
 public:
  using StringMap = std::map<std::string, std::string>;
  using StringList = std::vector<std::string>;

  // convert a list of doubles from a spreadsheet to a vtkTransform
  static vtkSmartPointer<vtkTransform> convert_transform(std::vector<double> list);

  // convert a vtkTransform to a list of doubles (e.g. for project spreadsheet)
  static std::vector<double> convert_transform(vtkSmartPointer<vtkTransform> transform);

  //! determine domain names from a subject's keys
  static StringList determine_domain_names(StringList keys);

  //! determine and set domain types
  static void determine_domain_types(std::shared_ptr<Project> project, StringMap key_map);

  static StringList get_input_prefixes();

  static StringList get_groomed_prefixes();

  static StringList get_original_keys(StringList domain_names, StringMap key_map);

  static StringList get_matching_values(StringList prefixes, StringList domain_names, StringMap key_map);

  static std::vector<std::vector<double>> get_transforms(std::string prefix, StringList domain_names,
                                                         StringMap key_map);

  static StringMap get_value_map(std::string prefix, StringMap key_map);

  static StringMap get_extra_columns(StringMap key_map);

 public:
  static DomainType determine_domain_type(std::string filename);

  static constexpr const char* SEGMENTATION_PREFIX = "segmentation_";
  static constexpr const char* SHAPE_PREFIX = "shape_";
  static constexpr const char* MESH_PREFIX = "mesh_";
  static constexpr const char* CONTOUR_PREFIX = "contour_";

  static constexpr const char* GROOMED_PREFIX = "groomed_";
  static constexpr const char* GROOMED_CONTOUR_PREFIX = "groomed_contour_";

  static constexpr const char* LANDMARKS_FILE_PREFIX = "landmarks_file_";
  static constexpr const char* CONSTRAINTS_PREFIX = "constraints_";

  static constexpr const char* GROOMED_TRANSFORMS_PREFIX = "alignment_";
  static constexpr const char* PROCRUSTES_TRANSFORMS_PREFIX = "procrustes_";
  static constexpr const char* IMAGE_PREFIX = "image_";
  static constexpr const char* FEATURE_PREFIX = "feature_";
  static constexpr const char* GROUP_PREFIX = "group_";

  static constexpr const char* LOCAL_PARTICLES = "local_particles";
  static constexpr const char* WORLD_PARTICLES = "world_particles";
};
}  // namespace shapeworks
