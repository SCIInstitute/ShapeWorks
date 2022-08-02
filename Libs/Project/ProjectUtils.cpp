#include "ProjectUtils.h"

#include <Libs/Mesh/MeshUtils.h>
#include <StringUtils.h>

namespace shapeworks {

using StringList = ProjectUtils::StringList;
using StringMap = ProjectUtils::StringMap;

using namespace project::prefixes;

static StringList input_prefixes{SEGMENTATION_PREFIX, SHAPE_PREFIX, MESH_PREFIX, CONTOUR_PREFIX};

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
StringList ProjectUtils::determine_domain_names(StringList keys) {
  StringList domain_names;

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
void ProjectUtils::determine_domain_types(Project& project, StringMap key_map) {
  auto domain_names = project.get_domain_names();

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
  project.set_original_domain_types(original_domain_types);

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
  project.set_groomed_domain_types(groomed_domain_types);
}

//---------------------------------------------------------------------------
StringList ProjectUtils::get_input_prefixes() { return input_prefixes; }

//---------------------------------------------------------------------------
StringList ProjectUtils::get_groomed_prefixes() { return {GROOMED_PREFIX, GROOMED_CONTOUR_PREFIX}; }

//---------------------------------------------------------------------------
StringList ProjectUtils::get_original_keys(StringList domain_names, StringMap key_map) {
  StringList original_keys;
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
StringList ProjectUtils::get_values(StringList prefixes, StringList domain_names, StringMap key_map) {
  StringList values;
  for (const auto& domain : domain_names) {
    for (auto& [key, value] : key_map) {
      for (const auto& prefix : prefixes) {
        if (key == prefix + domain) {
          values.push_back(value);
        }
      }
    }
  }
  return values;
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> ProjectUtils::get_transforms(std::string prefix, StringList domain_names,
                                                              StringMap key_map) {
  auto list = get_values({prefix}, domain_names, key_map);

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
StringMap ProjectUtils::get_value_map(std::string prefix, StringMap key_map) {
  StringMap map;
  for (auto& [key, value] : key_map) {
    if (key.substr(0, prefix.length()) == prefix) {
      std::string name = key.substr(prefix.length());
      map[name] = value;
    }
  }
  return map;
}

//---------------------------------------------------------------------------
StringMap ProjectUtils::get_extra_columns(StringMap key_map) {
  StringList prefixes = {SEGMENTATION_PREFIX,
                         SHAPE_PREFIX,
                         MESH_PREFIX,
                         CONTOUR_PREFIX,
                         GROOMED_PREFIX,
                         GROOMED_CONTOUR_PREFIX,
                         LANDMARKS_FILE_PREFIX,
                         CONSTRAINTS_PREFIX,
                         GROOMED_TRANSFORMS_PREFIX,
                         PROCRUSTES_TRANSFORMS_PREFIX,
                         IMAGE_PREFIX,
                         FEATURE_PREFIX,
                         GROUP_PREFIX,
                         LOCAL_PARTICLES,
                         WORLD_PARTICLES};
  StringMap map;
  for (auto& [key, value] : key_map) {
    bool match = false;
    for (const auto& prefix : prefixes) {
      if (key.substr(0, prefix.size()) == prefix) {
        match = true;
      }
    }
    if (!match) {
      map[key] = value;
    }
  }
  return map;
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

//---------------------------------------------------------------------------
bool ProjectUtils::starts_with(std::string str, std::string prefix) { return str.substr(0, prefix.size()) == prefix; }

}  // namespace shapeworks
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
