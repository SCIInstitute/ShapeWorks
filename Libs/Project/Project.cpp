#include <Libs/Mesh/MeshUtils.h>
#include <Project.h>
#include <StringUtils.h>
#include <vtkPointData.h>

#include <cstring>
#include <memory>

#include "ExcelProjectReader.h"
#include "ExcelProjectWriter.h"
#include "JsonProjectReader.h"
#include "JsonProjectWriter.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
static std::string replace_string(std::string subject, const std::string& search, const std::string& replace) {
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}

//---------------------------------------------------------------------------
Project::Project() { set_default_landmark_colors(); }

//---------------------------------------------------------------------------
Project::~Project() = default;

//---------------------------------------------------------------------------
bool Project::load(const std::string& filename) {
  filename_ = filename;
  if (StringUtils::hasSuffix(filename, "swproj")) {
    JsonProjectReader reader(*this);
    return reader.read_project(filename);
  } else {
    ExcelProjectReader reader(*this);
    return reader.read_project(filename);
  }

  Parameters project_parameters = get_parameters(Parameters::PROJECT_PARAMS);

  version_ = project_parameters.get("version", -1);

  loaded_ = true;
  return true;
}

//---------------------------------------------------------------------------
bool Project::save(const std::string& filename) {
  filename_ = filename;
  if (StringUtils::hasSuffix(filename, "swproj")) {
    return JsonProjectWriter::write_project(*this, filename);
  } else {
    return ExcelProjectWriter::write_project(*this, filename);
  }
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_headers() {
  if (subjects_.empty()) {
    return {};
  }
  auto subject = subjects_[0];

  std::vector<std::string> list;
  for (auto& [key, value] : subject->get_table_values()) {
    list.push_back(key);
  }
  return list;
}

//---------------------------------------------------------------------------
int Project::get_number_of_subjects() { return subjects_.size(); }

//---------------------------------------------------------------------------
int Project::get_number_of_domains_per_subject() { return get_domain_names().size(); }

//---------------------------------------------------------------------------
std::vector<std::shared_ptr<Subject>>& Project::get_subjects() { return subjects_; }

//---------------------------------------------------------------------------
void Project::set_subjects(const std::vector<std::shared_ptr<Subject>>& subjects) {
  subjects_ = subjects;
  update_subjects();
}

//---------------------------------------------------------------------------
void Project::update_subjects() {
  if (subjects_.empty()) {
    originals_present_ = false;
    groomed_present_ = false;
    particles_present_ = false;
    return;
  }

  auto subject = subjects_[0];
  originals_present_ = !subject->get_original_filenames().empty();
  groomed_present_ = !subject->get_groomed_filenames().empty();
  particles_present_ = !subject->get_world_particle_filenames().empty();
  images_present_ = !subject->get_image_filenames().empty();
  determine_feature_names();
}

//---------------------------------------------------------------------------
int Project::get_supported_version() const { return supported_version_; }

//---------------------------------------------------------------------------
int Project::get_version() const { return version_; }

//---------------------------------------------------------------------------
std::vector<LandmarkDefinition> Project::get_landmarks(int domain_id) {
  if (domain_id < 0 || domain_id >= landmark_definitions_.size()) {
    return std::vector<LandmarkDefinition>();
  }
  return landmark_definitions_[domain_id];
}

//---------------------------------------------------------------------------
std::vector<std::vector<LandmarkDefinition>> Project::get_all_landmark_definitions() { return landmark_definitions_; }

//---------------------------------------------------------------------------
void Project::set_landmark_definitions(std::vector<std::vector<LandmarkDefinition>> defs) {
  landmark_definitions_ = defs;
  }

//---------------------------------------------------------------------------
bool Project::get_landmarks_present() {
  for (const auto& def : landmark_definitions_) {
    if (!def.empty()) {
      return true;
    }
  }
  return false;
}

//---------------------------------------------------------------------------
void Project::set_landmarks(int domain_id, std::vector<LandmarkDefinition> landmarks) {
  while (domain_id > landmark_definitions_.size()) {
    landmark_definitions_.push_back(std::vector<LandmarkDefinition>());
  }
  landmark_definitions_[domain_id] = landmarks;
}

//---------------------------------------------------------------------------
void Project::new_landmark(int domain_id) {
  auto landmarks = get_landmarks(domain_id);
  LandmarkDefinition landmark;
  landmark.domain_id_ = domain_id;
  landmark.name_ = get_next_landmark_name(domain_id);
  landmark.color_ = get_next_landmark_color(domain_id);
  landmarks.push_back(landmark);
  set_landmarks(domain_id, landmarks);
}

//---------------------------------------------------------------------------
std::vector<DomainType> Project::get_original_domain_types() { return original_domain_types_; }

//---------------------------------------------------------------------------
std::vector<DomainType> Project::get_groomed_domain_types() { return groomed_domain_types_; }

//---------------------------------------------------------------------------
void Project::set_original_domain_types(std::vector<DomainType> domain_types) { original_domain_types_ = domain_types; }

//---------------------------------------------------------------------------
void Project::set_groomed_domain_types(std::vector<DomainType> domain_types) { groomed_domain_types_ = domain_types; }

//---------------------------------------------------------------------------
void Project::set_default_landmark_colors() {
  default_landmark_colors_.push_back("#ffaf4e");  // orange
  default_landmark_colors_.push_back("#74ff7a");  // green
  default_landmark_colors_.push_back("#8fd6ff");  // light blue
  default_landmark_colors_.push_back("#ff0000");  // red
  default_landmark_colors_.push_back("#ffe900");  // yellow
  default_landmark_colors_.push_back("#6c00d4");  // grape
  default_landmark_colors_.push_back("#0000ff");  // blue
  default_landmark_colors_.push_back("#ff5e7a");  // mauve
  default_landmark_colors_.push_back("#ffffa5");  // light yellow
  default_landmark_colors_.push_back("#ff00ff");  // magenta
  default_landmark_colors_.push_back("#c27600");  // brown
  default_landmark_colors_.push_back("#9f8fff");  // light purple
}

//---------------------------------------------------------------------------
void Project::determine_feature_names() {
  if (subjects_.empty()) {
    return;
  }
  auto subject = subjects_[0];

  auto feature_map = subject->get_feature_filenames();

  feature_names_.clear();

  for (auto& [key, value] : subject->get_feature_filenames()) {
    feature_names_.push_back(key);
  }
  image_names_ = feature_names_;

  std::vector<std::string> mesh_scalars;

  for (int d = 0; d < get_original_domain_types().size(); d++) {
    if (get_original_domain_types()[d] == DomainType::Mesh) {
      if (subject->get_original_filenames().size() > d) {
        auto filename = subject->get_original_filenames()[d];
        try {
          auto poly_data = MeshUtils::threadSafeReadMesh(filename).getVTKMesh();
          if (poly_data) {
            vtkIdType num_arrays = poly_data->GetPointData()->GetNumberOfArrays();
            for (vtkIdType i = 0; i < num_arrays; i++) {
              mesh_scalars.push_back(StringUtils::safeString(poly_data->GetPointData()->GetArrayName(i)));
            }
          }
        } catch (std::exception& e) {
          std::cerr << std::string("Error reading features from mesh: ") + filename << "\n";
        }
      }
    }
  }

  // combine
  feature_names_.insert(feature_names_.end(), mesh_scalars.begin(), mesh_scalars.end());
}



//---------------------------------------------------------------------------
bool Project::get_originals_present() const { return originals_present_; }

//---------------------------------------------------------------------------
bool Project::get_groomed_present() const { return groomed_present_; }

//---------------------------------------------------------------------------
bool Project::get_particles_present() const { return particles_present_; }

//---------------------------------------------------------------------------
bool Project::get_images_present() { return images_present_; }

//---------------------------------------------------------------------------
Parameters Project::get_parameters(const std::string& name, std::string domain_name) {
  Parameters params;
  if (parameters_.find(name) == parameters_.end()) {
    return params;
  }

  std::map<std::string, Parameters> param_map = parameters_[name];

  if (param_map.find(domain_name) == param_map.end()) {
    return params;
  }

  return param_map[domain_name];
}

//---------------------------------------------------------------------------
std::map<std::string, Parameters> Project::get_parameter_map(const std::string& name) {
  std::map<std::string, Parameters> map;
  auto domains = get_domain_names();
  for (int i = 0; i < domains.size(); i++) {
    map[domains[i]] = get_parameters(name, domains[i]);
  }
  return map;
}

//---------------------------------------------------------------------------
void Project::set_parameter_map(const std::string& name, std::map<std::string, Parameters> map) {
  for (auto& [domain, params] : map) {
    set_parameters(name, params, domain);
  }
}

//---------------------------------------------------------------------------
void Project::set_parameters(const std::string& name, Parameters params, std::string domain_name) {
  if (get_number_of_domains_per_subject() == 1) {
    domain_name = "";
  }

  std::map<std::string, Parameters> param_map;

  if (parameters_.find(name) != parameters_.end()) {
    param_map = parameters_[name];
  }

  param_map[domain_name] = params;
  parameters_[name] = param_map;
}

//---------------------------------------------------------------------------
void Project::clear_parameters(const std::string& name) { parameters_.erase(name); }

//---------------------------------------------------------------------------
std::string Project::get_next_landmark_name(int domain_id) {
  return std::to_string(landmark_definitions_[domain_id].size() + 1);
}

//---------------------------------------------------------------------------
std::string Project::get_next_landmark_color(int domain_id) {
  int index = landmark_definitions_[domain_id].size() % default_landmark_colors_.size();
  std::string c = default_landmark_colors_[index];
  return c;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_feature_names() { return feature_names_; }

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_image_names() { return image_names_; }

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_group_names() {
  std::vector<std::string> group_names;
  if (subjects_.empty()) {
    return group_names;
  }

  auto& subject = subjects_[0];

  for (auto& [key, value] : subject->get_group_values()) {
    group_names.push_back(key);
  }

  return group_names;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_group_values(const std::string& group_name) const {
  std::vector<std::string> values;
  if (subjects_.empty()) {
    return values;
  }

  for (auto& subject : subjects_) {
    auto map = subject->get_group_values();
    if (map.find(group_name) != map.end()) {
      values.push_back(map[group_name]);
    }
  }

  // remove duplicates
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());

  return values;
}

//---------------------------------------------------------------------------
std::string Project::get_filename() { return filename_; }

//---------------------------------------------------------------------------
void Project::set_filename(std::string filename) { filename_ = filename; }

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_domain_names() { return domain_names_; }

//---------------------------------------------------------------------------
void Project::set_domain_names(std::vector<std::string> domain_names) { domain_names_ = domain_names; }

//---------------------------------------------------------------------------
