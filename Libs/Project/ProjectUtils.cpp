#include "ProjectUtils.h"

#include <Libs/Mesh/MeshUtils.h>
#include <StringUtils.h>

namespace shapeworks {

static std::vector<std::string> input_prefixes{ProjectUtils::SEGMENTATION_PREFIX, ProjectUtils::SHAPE_PREFIX,
                                               ProjectUtils::MESH_PREFIX, ProjectUtils::CONTOUR_PREFIX};

//---------------------------------------------------------------------------
vtkSmartPointer<vtkTransform> shapeworks::ProjectUtils::convert_transform(std::vector<double> list) {
  vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();
  transform->Identity();
  if (list.size() == 12) {
    double tx = list[9];
    double ty = list[10];
    double tz = list[11];
    transform->Translate(tx, ty, tz);
  } else if (list.size() == 16) {
    vtkSmartPointer<vtkMatrix4x4> matrix = vtkSmartPointer<vtkMatrix4x4>::New();
    for (int i = 0; i < 16; i++) {
      matrix->GetData()[i] = list[i];
    }
    transform->SetMatrix(matrix);
  }
  return transform;
}

//---------------------------------------------------------------------------
std::vector<double> ProjectUtils::convert_transform(vtkSmartPointer<vtkTransform> transform) {
  std::vector<double> list;
  for (int i = 0; i < 16; i++) {
    list.push_back(transform->GetMatrix()->GetData()[i]);
  }
  return list;
}

//---------------------------------------------------------------------------
std::vector<std::string> ProjectUtils::determine_domain_names(std::vector<std::string> keys) {
  std::vector<std::string> domain_names;

  // check for original first
  for (const auto& key : keys) {
    for (const auto& prefix : input_prefixes) {
      if (key.substr(0, prefix.length()) == prefix) {
        domain_names.push_back(key.substr(prefix.length()));
      }
    }
  }
  if (!domain_names.empty()) {
    return domain_names;
  }

  // check for groomed
  for (const auto& key : keys) {
    std::string prefix = GROOMED_PREFIX;
    if (key.substr(0, prefix.length()) == prefix) {
      domain_names.push_back(key.substr(prefix.length()));
    }
  }
  if (!domain_names.empty()) {
    return domain_names;
  }

  // particles only?

  domain_names.push_back("1");
  return domain_names;
}

//---------------------------------------------------------------------------
void ProjectUtils::determine_domain_types(std::shared_ptr<Project> project,
                                          std::map<std::string, std::string> key_map) {
  auto domain_names = project->get_domain_names();

  std::vector<DomainType> original_domain_types;
  for (const auto& domain : domain_names) {
    for (auto& [key, value] : key_map) {
      if (key == std::string(SEGMENTATION_PREFIX) + "_" + domain) {
        original_domain_types.push_back(DomainType::Image);
      } else if (key == std::string(MESH_PREFIX) + "_" + domain) {
        original_domain_types.push_back(DomainType::Mesh);
      } else if (key == std::string(CONTOUR_PREFIX) + "_" + domain) {
        original_domain_types.push_back(DomainType::Contour);
      } else if (key == std::string(SHAPE_PREFIX) + "_" + domain) {
        original_domain_types.push_back(determine_domain_type(value));
      }
    }
  }
  project->set_original_domain_types(original_domain_types);

  // groomed types
  std::vector<DomainType> groomed_domain_types;
  for (const auto& domain : domain_names) {
    for (auto& [key, value] : key_map) {
      if (key == std::string(GROOMED_CONTOUR_PREFIX) + "_" + domain) {
        groomed_domain_types.push_back(DomainType::Contour);
      } else if (key == std::string(GROOMED_PREFIX) + "_" + domain) {
        groomed_domain_types.push_back(determine_domain_type(value));
      }
    }
  }
  project->set_groomed_domain_types(groomed_domain_types);
}

//---------------------------------------------------------------------------
std::vector<std::string> ProjectUtils::get_groomed_prefixes() {
  return {ProjectUtils::GROOMED_PREFIX, GROOMED_CONTOUR_PREFIX};
}

//---------------------------------------------------------------------------
std::vector<std::string> ProjectUtils::get_original_keys(std::vector<std::string> domain_names,
                                                         std::map<std::string, std::string> key_map) {
  std::vector<std::string> original_keys;
  for (const auto& domain : domain_names) {
    for (auto& [key, value] : key_map) {
      if (key == std::string(SEGMENTATION_PREFIX) + "_" + domain) {
        original_keys.push_back(key);
      } else if (key == std::string(MESH_PREFIX) + "_" + domain) {
        original_keys.push_back(key);
      } else if (key == std::string(CONTOUR_PREFIX) + "_" + domain) {
        original_keys.push_back(key);
      } else if (key == std::string(SHAPE_PREFIX) + "_" + domain) {
        original_keys.push_back(key);
      }
    }
  }
  return original_keys;
}

//---------------------------------------------------------------------------
std::vector<std::string> ProjectUtils::get_matching_values(std::vector<std::string> prefixes,
                                                           std::vector<std::string> domain_names,
                                                           std::map<std::string, std::string> key_map) {
  std::vector<std::string> values;
  for (const auto& domain : domain_names) {
    for (auto& [key, value] : key_map) {
      for (const auto& prefix : prefixes) {
        if (key == prefix + "_" + domain) {
          values.push_back(value);
        }
      }
    }
  }
  return values;
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> ProjectUtils::get_transforms(std::string prefix, std::vector<std::string> domain_names,
                                                              std::map<std::string, std::string> key_map) {
  auto list = get_matching_values({prefix}, domain_names, key_map);

  std::vector<std::vector<double>> transforms;
  for (int i = 0; i < list.size(); i++) {
    std::string str = list[i];
    std::istringstream ss(str);
    std::vector<double> values;
    for (double value = 0; ss >> value;) {
      values.push_back(value);
    }
    transforms.push_back(values);
  }
  return transforms;
}

//---------------------------------------------------------------------------
DomainType ProjectUtils::determine_domain_type(std::string filename) {
  std::transform(filename.begin(), filename.end(), filename.begin(), ::tolower);

  for (const auto& type : Mesh::getSupportedTypes()) {
    if (StringUtils::hasSuffix(filename, type)) {
      return DomainType::Mesh;
    }
  }
  return DomainType::Image;
}

}  // namespace shapeworks
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
