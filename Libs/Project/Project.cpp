#include <Libs/Mesh/MeshUtils.h>
#include <Project.h>
#include <StringUtils.h>
#include <vtkPointData.h>

#include <cstring>
#include <memory>
#include <xlnt/workbook/workbook_view.hpp>
#include <xlnt/xlnt.hpp>

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
Project::Project() {
  set_default_landmark_colors();
  this->wb_ = std::make_unique<xlnt::workbook>();

  this->input_prefixes_.push_back(SEGMENTATION_PREFIX);
  this->input_prefixes_.push_back(SHAPE_PREFIX);
}

//---------------------------------------------------------------------------
Project::~Project() = default;

//---------------------------------------------------------------------------
bool Project::load(const std::string& filename) {
  try {
    this->wb_->load(filename);
    this->filename_ = filename;
  } catch (xlnt::exception& e) {
    std::cerr << std::string("Error reading xlsx: ") << std::string(e.what()) << ", "
              << "\n";

    return false;
  }

  this->load_subjects();

  Parameters project_parameters = this->get_parameters(Parameters::PROJECT_PARAMS);

  this->version_ = project_parameters.get("version", -1);

  this->loaded_ = true;
  return true;
}

//---------------------------------------------------------------------------
bool Project::save(const std::string& filename) {
  try {
    xlnt::worksheet ws = this->wb_->sheet_by_index(0);
    ws.title("data");

    Parameters project_parameters;
    project_parameters.set("version", this->supported_version_);
    this->set_parameters(Parameters::PROJECT_PARAMS, project_parameters);

    this->store_subjects();

    // reset the view
    xlnt::workbook_view wb_view;
    wb_view.active_tab = 0;
    wb_view.x_window = 0;
    wb_view.y_window = 460;
    wb_view.window_width = 28800;
    wb_view.window_height = 17460;
    wb_view.tab_ratio = 500;
    this->wb_->view(wb_view);

    this->wb_->save(filename);
    this->filename_ = filename;
  } catch (xlnt::exception& e) {
    std::cerr << std::string("Error writing xlsx: ") << std::string(e.what()) << ", "
              << "\n";

    throw "Error writing xlsx";
  }

  return true;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_headers() { return this->get_matching_columns(""); }

//---------------------------------------------------------------------------
int Project::get_number_of_subjects() {
  auto seg_columns = this->get_matching_columns(this->input_prefixes_);
  auto dt_columns = this->get_matching_columns(GROOMED_PREFIX);
  auto local_particle_files = this->get_matching_columns(LOCAL_PARTICLES);
  if (!seg_columns.empty()) {
    return this->get_string_column(seg_columns[0]).size();
  }
  if (!dt_columns.empty()) {
    return this->get_string_column(dt_columns[0]).size();
  }
  if (!local_particle_files.empty()) {
    return this->get_string_column(local_particle_files[0]).size();
  }
  return 0;
}

//---------------------------------------------------------------------------
int Project::get_number_of_domains_per_subject() { return this->get_domain_names().size(); }

//---------------------------------------------------------------------------
std::vector<std::shared_ptr<Subject>>& Project::get_subjects() { return this->subjects_; }

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_matching_columns(const std::string& prefix) {
  std::vector<std::string> prefixes;
  prefixes.push_back(prefix);
  return this->get_matching_columns(prefixes);
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_matching_columns(const std::vector<std::string> prefixes) {
  for (const auto& prefix : prefixes) {
    if (prefix != "") {
      this->matching_columns_.insert(prefix);
    }
  }

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);
  auto headers = ws.rows(false)[0];
  std::vector<std::string> list;

  for (int i = 0; i < headers.length(); i++) {
    for (const auto& prefix : prefixes) {
      if (headers[i].to_string().substr(0, prefix.size()) == prefix) {
        list.push_back(headers[i].to_string());
      }
    }
  }
  return list;
}

//---------------------------------------------------------------------------
std::string Project::get_value(int column, int subject_id) {
  xlnt::worksheet ws = this->wb_->sheet_by_index(0);
  std::string value = ws.cell(xlnt::cell_reference(column, subject_id)).to_string();
  return value;
}

//---------------------------------------------------------------------------
void Project::set_value(int column, int subject_id, const std::string& value) {
  xlnt::worksheet ws = this->wb_->sheet_by_index(0);
  ws.cell(xlnt::cell_reference(column, subject_id)).value(value);
}

//---------------------------------------------------------------------------
void Project::set_value(const std::string& column_name, int subject_id, const std::string& value) {
  int column_index = get_index_for_column(column_name, true);
  this->set_value(column_index, subject_id + 2, value);  // +1 for header, +1 for 1-indexed
}

//---------------------------------------------------------------------------
std::string Project::get_subject_value(int column, int subject_id) {
  return this->get_value(column, subject_id + 2);  // 1-based and skip header
}

//---------------------------------------------------------------------------
void Project::load_subjects() {
  int num_subjects = this->get_number_of_subjects();

  this->subjects_.clear();

  this->num_domains_per_subject_ = this->get_number_of_domains_per_subject();

  auto seg_columns = this->get_matching_columns(this->input_prefixes_);
  auto groomed_columns = this->get_matching_columns(GROOMED_PREFIX);
  auto groomed_transform_columns = this->get_matching_columns(GROOMED_TRANSFORMS_PREFIX);
  auto procrustes_transform_columns = this->get_matching_columns(PROCRUSTES_TRANSFORMS_PREFIX);
  auto feature_columns = this->get_feature_names();
  auto group_names = this->get_matching_columns(GROUP_PREFIX);
  auto local_particle_columns = this->get_matching_columns(LOCAL_PARTICLES);
  auto world_particle_columns = this->get_matching_columns(WORLD_PARTICLES);
  auto image_columns = this->get_matching_columns(IMAGE_PREFIX);
  auto name_column = this->get_index_for_column(NAME);
  auto landmarks_columns = this->get_matching_columns(LANDMARKS_FILE_PREFIX);
  auto constraints_columns = get_matching_columns(CONSTRAINTS_PREFIX);

  auto extra_columns = this->get_extra_columns();
  auto all_columns = this->get_headers();

  for (int i = 0; i < num_subjects; i++) {
    std::shared_ptr<Subject> subject = std::make_shared<Subject>();

    subject->set_number_of_domains(this->num_domains_per_subject_);
    subject->set_segmentation_filenames(this->get_list(seg_columns, i));
    subject->set_groomed_filenames(this->get_list(groomed_columns, i));
    subject->set_landmarks_filenames(this->get_list(landmarks_columns, i));
    subject->set_constraints_filenames(this->get_list(constraints_columns, i));
    subject->set_groomed_transforms(this->get_transform_list(groomed_transform_columns, i));
    subject->set_procrustes_transforms(this->get_transform_list(procrustes_transform_columns, i));
    subject->set_image_filenames(this->get_list(image_columns, i));

    auto feature_list = this->get_list(feature_columns, i);
    std::map<std::string, std::string> map;
    for (int j = 0; j < feature_columns.size(); j++) {
      std::string feature = feature_columns[j].substr(std::strlen(FEATURE_PREFIX));
      map[feature] = feature_list[j];
    }
    subject->set_feature_filenames(map);

    auto group_values = this->get_list(group_names, i);
    std::map<std::string, std::string> group_map;
    for (int j = 0; j < group_names.size(); j++) {
      std::string name = group_names[j].substr(std::strlen(GROUP_PREFIX));
      group_map[name] = group_values[j];
    }
    subject->set_group_values(group_map);

    auto locals = this->get_list(local_particle_columns, i);
    auto worlds = this->get_list(world_particle_columns, i);
    subject->set_local_particle_filenames(locals);
    subject->set_world_particle_filenames(worlds);

    if (locals.size() > 0) {
      this->particles_present_ = true;
    }

    std::string name = "";
    if (name_column >= 0) {
      name = this->get_value(name_column, i + 2);  //+1 for header, +1 for 1-based index
    } else if (subject->get_segmentation_filenames().size() != 0) {
      name = StringUtils::getFileNameWithoutExtension(subject->get_segmentation_filenames()[0]);
    } else if (subject->get_groomed_filenames().size() != 0) {
      name = StringUtils::getFileNameWithoutExtension(subject->get_groomed_filenames()[0]);
    } else if (locals.size() > 0) {
      name = StringUtils::getFileNameWithoutExtension(locals[0]);
    }
    subject->set_display_name(name);

    std::map<std::string, std::string> extra_values;
    for (const auto& elem : this->get_extra_columns()) {
      auto value = this->get_value(this->get_index_for_column(elem),
                                   i + 2);  //+1 for header, +1 for 1-based index
      extra_values[elem] = value;
    }
    subject->set_extra_values(extra_values);

    std::map<std::string, std::string> table_values;
    for (const auto& elem : this->get_headers()) {
      auto value = this->get_value(this->get_index_for_column(elem),
                                   i + 2);  //+1 for header, +1 for 1-based index
      table_values[elem] = value;
    }
    subject->set_table_values(table_values);

    this->segmentations_present_ = !seg_columns.empty();
    this->groomed_present_ = !groomed_columns.empty();
    this->subjects_.push_back(subject);
  }
}

//---------------------------------------------------------------------------
void Project::store_subjects() {
  int num_subjects = this->subjects_.size();
  if (num_subjects < 1) {
    return;
  }

  // segmentation columns
  auto seg_columns = this->get_matching_columns(this->input_prefixes_);
  auto image_columns = this->get_matching_columns(IMAGE_PREFIX);
  auto landmarks_columns = this->get_matching_columns(LANDMARKS_FILE_PREFIX);
  landmarks_columns.clear();
  auto constraints_columns = get_matching_columns(CONSTRAINTS_PREFIX);
  constraints_columns.clear();

  // groomed columns
  std::vector<std::string> groomed_columns;
  for (int i = 0; i < seg_columns.size(); i++) {
    std::string groom_column_name = GROOMED_PREFIX + this->get_column_identifier(seg_columns[i]);
    groomed_columns.push_back(groom_column_name);
  }

  // groomed transform columns (e.g. centering, etc)
  std::vector<std::string> groomed_transform_columns;
  for (int i = 0; i < groomed_columns.size(); i++) {
    std::string groomed_transform_column_name =
        replace_string(groomed_columns[i], GROOMED_PREFIX, GROOMED_TRANSFORMS_PREFIX);
    groomed_transform_columns.push_back(groomed_transform_column_name);
  }
  if (groomed_transform_columns.size() > 1) {
    groomed_transform_columns.push_back(std::string(GROOMED_TRANSFORMS_PREFIX) + "global");
  }

  // procrustes transform columns
  std::vector<std::string> procrustes_transform_columns;
  for (int i = 0; i < groomed_columns.size(); i++) {
    std::string procrustes_transform_column_name =
        replace_string(groomed_columns[i], GROOMED_PREFIX, PROCRUSTES_TRANSFORMS_PREFIX);
    procrustes_transform_columns.push_back(procrustes_transform_column_name);
  }

  // local and world particle columns
  std::vector<std::string> local_columns;
  std::vector<std::string> world_columns;
  for (int i = 0; i < seg_columns.size(); i++) {
    std::string column_name = std::string(LOCAL_PARTICLES) + "_" + this->get_column_identifier(seg_columns[i]);
    local_columns.push_back(column_name);
    column_name = std::string(WORLD_PARTICLES) + "_" + this->get_column_identifier(seg_columns[i]);
    world_columns.push_back(column_name);
    column_name = std::string(LANDMARKS_FILE_PREFIX) + this->get_column_identifier(seg_columns[i]);
    landmarks_columns.push_back(column_name);
    column_name = std::string(CONSTRAINTS_PREFIX) + this->get_column_identifier(seg_columns[i]);
    landmarks_columns.push_back(column_name);
  }

  bool groomed_present = false;

  // clear
  xlnt::worksheet ws = this->wb_->sheet_by_index(0);
  for (int i = 2; i <= ws.highest_row(); i++) {
    ws.clear_row(i);
  }

  for (int i = 0; i < num_subjects; i++) {
    std::shared_ptr<Subject> subject = this->subjects_[i];

    // segmentations
    auto seg_files = subject->get_segmentation_filenames();
    if (seg_files.size() > seg_columns.size()) {
      seg_columns.push_back(std::string(SHAPE_PREFIX) + "1");
    }
    this->set_list(seg_columns, i, seg_files);

    auto groups = subject->get_group_values();
    this->set_map(i, GROUP_PREFIX, groups);

    // images
    auto image_files = subject->get_image_filenames();
    this->set_list(image_columns, i, image_files);

    // groomed files
    auto groomed_files = subject->get_groomed_filenames();
    if (groomed_files.size() >= groomed_columns.size() && groomed_files.size() > 0) {
      groomed_present = true;
      int count = 0;
      while (groomed_files.size() > groomed_columns.size()) {
        groomed_columns.push_back(this->get_new_file_column(GROOMED_PREFIX, count++));
      }
      this->set_list(groomed_columns, i, groomed_files);

      this->set_transform_list(groomed_transform_columns, i, subject->get_groomed_transforms());
      this->set_transform_list(procrustes_transform_columns, i, subject->get_procrustes_transforms());
    }

    // landmarks
    auto landmark_files = subject->get_landmarks_filenames();
    if (landmark_files.size() > 0) {
      this->set_list(landmarks_columns, i, landmark_files);
    }

    // constraints
    auto constraints_files = subject->get_constraints_filenames();
    if (constraints_files.size() > 0) {
      this->set_list(constraints_columns, i, constraints_files);
    }

    // features
    auto features = subject->get_feature_filenames();
    for (auto const& x : features) {
      int idx = this->get_index_for_column("feature_" + x.first, true);
      this->set_value(idx, i + 2, x.second);  // +1 for header, +1 for 1-based index
    }

    // extra values
    auto extra_values = subject->get_extra_values();
    for (auto const& x : extra_values) {
      int idx = this->get_index_for_column(x.first, true);
      this->set_value(idx, i + 2, x.second);  // +1 for header, +1 for 1-based index
    }

    // local and world particle files
    auto local_files = subject->get_local_particle_filenames();
    auto world_files = subject->get_world_particle_filenames();
    if (local_files.size() > 0) {
      this->particles_present_ = true;

      // add columns if necessary
      int count = 0;
      while (local_files.size() > local_columns.size()) {
        local_columns.push_back(this->get_new_file_column(std::string(LOCAL_PARTICLES) + "_", count));
        world_columns.push_back(this->get_new_file_column(std::string(WORLD_PARTICLES) + "_", count));
        count++;
      }

      this->set_list(local_columns, i, local_files);
      this->set_list(world_columns, i, world_files);
    } else {
      this->particles_present_ = false;
    }
  }

  this->segmentations_present_ = !seg_columns.empty();
  this->groomed_present_ = groomed_present;
}

//---------------------------------------------------------------------------
int Project::get_supported_version() const { return this->supported_version_; }

//---------------------------------------------------------------------------
int Project::get_version() const { return this->version_; }

//---------------------------------------------------------------------------
std::vector<LandmarkDefinition> Project::get_landmarks(int domain_id) {
  load_landmark_definitions();
  if (domain_id < 0 || domain_id >= landmark_definitions_.size()) {
    return std::vector<LandmarkDefinition>();
  }
  return landmark_definitions_[domain_id];
}

//---------------------------------------------------------------------------
std::vector<std::vector<LandmarkDefinition>> Project::get_all_landmark_definitions() { return landmark_definitions_; }

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
  store_landmark_definitions();
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
void Project::load_landmark_definitions() {
  auto domain_names = get_domain_names();
  landmark_definitions_.clear();
  landmark_definitions_.resize(domain_names.size());

  if (!this->wb_->contains("landmarks")) {
    return;
  }
  xlnt::worksheet ws = this->wb_->sheet_by_title("landmarks");

  auto rows = ws.rows(false);

  for (int i = ws.lowest_row(); i < ws.highest_row(); i++) {
    std::string name = rows[i][0].to_string();
    auto f = std::find(domain_names.begin(), domain_names.end(), name);
    if (f == domain_names.end()) {
      // skip, nonexistent domain
      continue;
    }
    int domain_id = f - domain_names.begin();
    LandmarkDefinition landmark;
    landmark.domain_ = name;
    landmark.domain_id_ = domain_id;
    landmark.name_ = rows[i][1].to_string();
    landmark.visible_ = rows[i][2].to_string() == "true";
    landmark.color_ = rows[i][3].to_string();
    landmark.comment_ = rows[i][4].to_string();
    landmark_definitions_[domain_id].push_back(landmark);
  }
}

//---------------------------------------------------------------------------
void Project::store_landmark_definitions() {
  std::string name = "landmarks";
  if (this->wb_->contains(name)) {
    this->wb_->remove_sheet(this->wb_->sheet_by_title(name));
  }

  int id = this->get_or_create_worksheet(name);
  auto ws = this->wb_->sheet_by_index(id);

  auto domain_names = get_domain_names();

  ws.cell(xlnt::cell_reference(1, 1)).value("domain");
  ws.cell(xlnt::cell_reference(2, 1)).value("name");
  ws.cell(xlnt::cell_reference(3, 1)).value("visible");
  ws.cell(xlnt::cell_reference(4, 1)).value("color");
  ws.cell(xlnt::cell_reference(5, 1)).value("comment");

  int row = 2;
  for (int domain_id = 0; domain_id < landmark_definitions_.size(); domain_id++) {
    auto landmarks = landmark_definitions_[domain_id];
    for (int i = 0; i < landmarks.size(); i++) {
      ws.cell(xlnt::cell_reference(1, row)).value(domain_names[domain_id]);
      ws.cell(xlnt::cell_reference(2, row)).value(landmarks[i].name_);
      ws.cell(xlnt::cell_reference(3, row)).value(landmarks[i].visible_ ? "true" : "false");
      ws.cell(xlnt::cell_reference(4, row)).value(landmarks[i].color_);
      ws.cell(xlnt::cell_reference(5, row)).value(landmarks[i].comment_);
      row++;
    }
  }
}

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
int Project::get_index_for_column(const std::string& name, bool create_if_not_found, int sheet) const {
  xlnt::worksheet ws = this->wb_->sheet_by_index(sheet);

  auto headers = ws.rows(false)[0];
  // std::cerr << "Get index for column: " << name << "\n";
  // std::cerr << "number of headers = " << headers.length() << "\n";
  for (int i = 0; i < headers.length(); i++) {
    // std::cerr << "header: " << headers[i].to_string() << "\n";
    if (headers[i].to_string() == name) {
      return i + 1;
    }
  }

  if (create_if_not_found) {
    // std::cerr << "couldn't find: " << name << "\n";
    auto column = ws.highest_column();
    // std::cerr << "highest column is " << column.index << "\n";
    if (ws.cell(xlnt::cell_reference(column.index, 1)).value<std::string>().empty()) {
      ws.cell(xlnt::cell_reference(column.index, 1)).value(name);
      // std::cerr << "returning " << column.index << "\n";
      return column.index;
    } else {
      ws.cell(xlnt::cell_reference(column.index + 1, 1)).value(name);
      // std::cerr << "returning " << column.index + 1 << "\n";
      return column.index + 1;
    }
  }

  return -1;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_string_column(const std::string& name) const {
  int index = this->get_index_for_column(name);

  std::vector<std::string> list;
  if (index < 0) {
    return list;
  }

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  auto rows = ws.rows(false);

  for (int i = ws.lowest_row(); i < ws.highest_row(); i++) {
    std::string value = rows[i][index - 1].to_string();
    list.push_back(value);
  }

  return list;
}

//---------------------------------------------------------------------------
bool Project::get_segmentations_present() const { return this->segmentations_present_; }

//---------------------------------------------------------------------------
bool Project::get_groomed_present() const { return this->groomed_present_; }

//---------------------------------------------------------------------------
bool Project::get_particles_present() const { return this->particles_present_; }

//---------------------------------------------------------------------------
bool Project::get_images_present() { return this->get_matching_columns(IMAGE_PREFIX).size() > 0; }

//---------------------------------------------------------------------------
Parameters Project::get_parameters(const std::string& name, const std::string& domain_name) {
  Parameters params;
  std::map<std::string, std::string> map;

  if (!this->wb_->contains(name)) {
    return params;
  }
  xlnt::worksheet ws = this->wb_->sheet_by_title(name);

  if (ws.highest_column().index < 2) {
    return params;
  }

  auto rows = ws.rows(false);

  int value_column = 1;  // single domain
  if (domain_name != "") {
    for (int i = ws.lowest_row(); i < ws.highest_row(); i++) {
      if (rows[0][i].to_string() == "value_" + domain_name) {
        value_column = i;
      }
    }
  }

  for (int i = ws.lowest_row(); i < ws.highest_row(); i++) {
    std::string key = rows[i][0].to_string();
    std::string value = rows[i][value_column].to_string();
    map[key] = value;
  }
  params.set_map(map);
  return params;
}

//---------------------------------------------------------------------------
void Project::set_parameters(const std::string& name, Parameters params, const std::string& domain_name) {
  try {
    int id = this->get_or_create_worksheet(name);
    auto ws = this->wb_->sheet_by_index(id);

    int key_column = 1;
    int value_column = 2;

    if (domain_name == "") {
      ws.cell(xlnt::cell_reference(1, 1)).value("key");
      ws.cell(xlnt::cell_reference(2, 1)).value("value");
    } else {
      int sheet = this->wb_->index(ws);
      key_column = this->get_index_for_column("key", true, sheet);
      value_column = this->get_index_for_column("value_" + domain_name, true, sheet);
    }
    int row = 2;  // skip header
    for (const auto& kv : params.get_map()) {
      ws.cell(xlnt::cell_reference(key_column, row)).value(kv.first);
      ws.cell(xlnt::cell_reference(value_column, row)).value(kv.second);
      row++;
    }
  } catch (xlnt::exception& e) {
    std::cerr << std::string("Error storing parameters: ") << std::string(e.what()) << ", "
              << "\n";
  }
}

//---------------------------------------------------------------------------
void Project::clear_parameters(const std::string& name) {
  if (this->wb_->contains(name)) {
    this->wb_->remove_sheet(this->wb_->sheet_by_title(name));
  }
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_list(std::vector<std::string> columns, int subject) {
  std::vector<std::string> list;
  for (int s = 0; s < columns.size(); s++) {
    auto column = columns[s];
    int column_index = this->get_index_for_column(column);
    auto value = this->get_value(column_index, subject + 2);  // +1 for header, +1 for 1-indexed
    list.push_back(value);
  }
  return list;
}

//---------------------------------------------------------------------------
void Project::set_list(std::vector<std::string> columns, int subject, std::vector<std::string> values) {
  assert(columns.size() == values.size());
  for (int s = 0; s < columns.size(); s++) {
    auto column = columns[s];
    int column_index = get_index_for_column(column, true);
    // std::cerr << "get_index_for_column(" << column << ") returned " << column_index << "\n";
    this->set_value(column_index, subject + 2, values[s]);  // +1 for header, +1 for 1-indexed
  }
}

//---------------------------------------------------------------------------
void Project::set_map(int subject, const std::string& prefix, const std::map<std::string, std::string>& map) {
  for (const auto& pair : map) {
    int column_index = get_index_for_column(prefix + pair.first, true);
    this->set_value(column_index, subject + 2, pair.second);  // +1 for header, +1 for 1-indexed
  }
}

//---------------------------------------------------------------------------
void Project::save_string_column(const std::string& name, std::vector<std::string> items) {
  int index = this->get_index_for_column(name, true);

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  ws.cell(xlnt::cell_reference(index + 1, 1)).value(name);

  for (int i = 0; i < items.size(); i++) {
    ws.cell(xlnt::cell_reference(index + 1, i + 2)).value(items[i]);
  }
}

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
std::vector<std::string> Project::get_feature_names() {
  if (!this->feature_names_read_done_) {
    // grab feature volumes
    auto feature_names = this->get_matching_columns(FEATURE_PREFIX);

    // now check for meshes that have scalars
    if (!this->subjects_.empty() && this->mesh_scalars_.empty()) {
      auto subject = this->subjects_[0];  // we have to assume that all subjects have the same features

      // int domains_per_subject = this->get_number_of_domains_per_subject();
      for (int d = 0; d < subject->get_domain_types().size(); d++) {
        if (subject->get_domain_types()[d] == DomainType::Mesh) {
          if (subject->get_segmentation_filenames().size() > d) {
            auto filename = subject->get_segmentation_filenames()[d];
            try {
              auto poly_data = MeshUtils::threadSafeReadMesh(filename).getVTKMesh();
              if (poly_data) {
                vtkIdType num_arrays = poly_data->GetPointData()->GetNumberOfArrays();
                for (vtkIdType i = 0; i < num_arrays; i++) {
                  this->mesh_scalars_.push_back(poly_data->GetPointData()->GetArrayName(i));
                }
              }
            } catch (std::exception& e) {
              std::cerr << std::string("Error reading: ") + filename;
            }
          }
        }
      }
      this->feature_names_read_done_ = true;
    }

    // combine
    feature_names.insert(feature_names.end(), this->mesh_scalars_.begin(), this->mesh_scalars_.end());
    this->feature_names_ = feature_names;
  }

  return this->feature_names_;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_image_names() {
  // grab feature volumes
  auto feature_names = this->get_matching_columns(FEATURE_PREFIX);
  return feature_names;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_group_names() {
  auto raw_names = this->get_matching_columns(GROUP_PREFIX);

  std::vector<std::string> group_names;

  for (std::string group : raw_names) {
    group.erase(0, std::strlen(GROUP_PREFIX));  // erase "group_"
    group_names.push_back(group);
  }

  return group_names;
}

//---------------------------------------------------------------------------
std::vector<std::vector<double>> Project::get_transform_list(std::vector<std::string> columns, int subject) {
  auto list = this->get_list(columns, subject);

  std::vector<std::vector<double>> transforms;
  for (int i = 0; i < list.size(); i++) {
    std::string str = list[i];

    std::istringstream ss(str);

    std::vector<double> values;

    for (double value; ss >> value;) {
      values.push_back(value);
    }

    transforms.push_back(values);
  }
  return transforms;
}

//---------------------------------------------------------------------------
void Project::set_transform_list(const std::vector<std::string>& columns, int subject,
                                 std::vector<std::vector<double>> transforms) {
  std::vector<std::string> transform_strings;
  for (int i = 0; i < transforms.size(); i++) {
    std::string str;
    for (int j = 0; j < transforms[i].size(); j++) {
      str = str + " " + std::to_string(transforms[i][j]);
    }
    transform_strings.push_back(str);
  }

  while (transform_strings.size() < columns.size()) {
    transform_strings.push_back("");
  }

  this->set_list(columns, subject, transform_strings);
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_group_values(const std::string& group_name) const {
  auto values = this->get_string_column(group_name);

  // remove duplicates
  std::sort(values.begin(), values.end());
  values.erase(std::unique(values.begin(), values.end()), values.end());

  return values;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_extra_columns() const {
  xlnt::worksheet ws = this->wb_->sheet_by_index(0);
  auto headers = ws.rows(false)[0];
  std::vector<std::string> list;

  for (int i = 0; i < headers.length(); i++) {
    bool match = false;
    for (const auto& prefix : this->matching_columns_) {
      if (headers[i].to_string().substr(0, prefix.size()) == prefix) {
        match = true;
      }
    }
    if (!match) {
      list.push_back(headers[i].to_string());
    }
  }
  return list;
}

//---------------------------------------------------------------------------
std::string Project::get_filename() { return this->filename_; }

//---------------------------------------------------------------------------
void Project::set_filename(std::string filename) { this->filename_ = filename; }

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_domain_names() {
  auto seg_columns = this->get_matching_columns(this->input_prefixes_);
  if (!seg_columns.empty()) {
    std::vector<std::string> names;
    for (auto&& item : seg_columns) {
      names.push_back(this->get_column_identifier(item));
    }
    return names;
  }

  auto groom_columns = this->get_matching_columns(GROOMED_PREFIX);
  if (!groom_columns.empty()) {
    std::vector<std::string> names;
    for (auto&& item : groom_columns) {
      names.push_back(item.erase(0, std::strlen(GROOMED_PREFIX)));
    }
    return names;
  }

  /// TODO: when only point files are specified,
  /// the user has to specify somewhere how many domains there are (if more than one)

  // default 1
  std::vector<std::string> list{"1"};
  return list;
}

//---------------------------------------------------------------------------
int Project::get_or_create_worksheet(std::string name) {
  if (this->wb_->contains(name)) {
    auto ws = this->wb_->sheet_by_title(name);
    return this->wb_->index(ws);
  } else {
    auto ws = this->wb_->create_sheet();
    ws.title(name);
    return this->wb_->index(ws);
  }
}

//---------------------------------------------------------------------------
std::string Project::get_new_file_column(std::string name, int idx) { return name + std::to_string(idx + 1); }

//---------------------------------------------------------------------------
std::string Project::get_column_identifier(std::string name) {
  for (const auto& prefix : this->input_prefixes_) {
    if (name.substr(0, prefix.size()) == prefix) {
      return name.substr(prefix.size(), name.size());
    }
  }
  return name;
}

//---------------------------------------------------------------------------
