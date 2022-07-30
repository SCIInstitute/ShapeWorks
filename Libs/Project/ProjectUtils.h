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
  static std::vector<std::string> determine_domain_names(std::vector<std::string> keys);

  //! determine and set domain types
  static void determine_domain_types(std::shared_ptr<Project> project, std::map<std::string, std::string> key_map);

  static std::vector<std::string> get_input_prefixes();

  static std::vector<std::string> get_groomed_prefixes();

  static std::vector<std::string> get_original_keys(std::vector<std::string> domain_names,
                                                    std::map<std::string, std::string> key_map);

  static std::vector<std::string> get_matching_values(std::vector<std::string> prefixes,
                                                      std::vector<std::string> domain_names,
                                                      std::map<std::string, std::string> key_map);

  static std::vector<std::vector<double>> get_transforms(std::string prefix,
                                                 std::vector<std::string> domain_names,
                                                 std::map<std::string, std::string> key_map);

  static std::map<std::string, std::string> get_value_map(std::string prefix,
                                                          std::map<std::string, std::string> key_map);

  static std::map<std::string, std::string> get_extra_columns(std::map<std::string, std::string> key_map);

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
