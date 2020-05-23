#include <Project.h>

#include <xlnt/xlnt.hpp>

#include <memory>

using namespace shapeworks;

//---------------------------------------------------------------------------
std::string ReplaceString(std::string subject, const std::string& search,
                          const std::string& replace)
{
  size_t pos = 0;
  while ((pos = subject.find(search, pos)) != std::string::npos) {
    subject.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return subject;
}

//---------------------------------------------------------------------------
Project::Project()
{
  this->wb_ = std::unique_ptr<xlnt::workbook>(new xlnt::workbook());
}

//---------------------------------------------------------------------------
Project::~Project()
{}

//---------------------------------------------------------------------------
bool Project::load(std::string filename)
{
  try
  {
    this->wb_->load(filename);
  } catch (xlnt::exception &e) {

    std::cerr << std::string("Error reading xlsx: ")
              << std::string(e.what()) << ", " << "\n";

    return false;
  }

  this->load_subjects();

  this->loaded_ = true;
  return true;
}

//---------------------------------------------------------------------------
bool Project::save(std::string filename)
{

  try {
    this->store_subjects();
    this->wb_->save(filename);
  } catch (xlnt::exception &e) {

    std::cerr << std::string("Error writing xlsx: ")
              << std::string(e.what()) << ", " << "\n";

    throw "Error writing xlsx";
  }

  return true;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_headers()
{
  return this->get_matching_columns("");
}

//---------------------------------------------------------------------------
int Project::get_number_of_subjects()
{
  auto seg_columns = this->get_matching_columns(SEGMENTATION_PREFIX);
  auto mesh_columns = this->get_matching_columns(MESH_PREFIX);
  auto dt_columns = this->get_matching_columns(GROOMED_PREFIX);
  auto local_particle_files = this->get_matching_columns(LOCAL_PARTICLES);
  if (seg_columns.size() > 0) {
    return this->get_string_column(seg_columns[0]).size();
  }
  if (mesh_columns.size() > 0) {
    return this->get_string_column(mesh_columns[0]).size();
  }
  if (dt_columns.size() > 0) {
    return this->get_string_column(dt_columns[0]).size();
  }
  if (local_particle_files.size() > 0) {
    return this->get_string_column(local_particle_files[0]).size();
  }
  return 0;
}

//---------------------------------------------------------------------------
int Project::get_number_of_domains()
{
  auto seg_columns = this->get_matching_columns(SEGMENTATION_PREFIX);
  if (seg_columns.size() > 0) {
    return seg_columns.size();
  }

  auto mesh_columns = this->get_matching_columns("mesh_");
  if (mesh_columns.size() > 0) {
    return mesh_columns.size();
  }

  /// TODO: DT's?

  /// TODO: when only point files are specified, the user has to specify somewhere how many domains there are (if more than one)

  return 0;
}

//---------------------------------------------------------------------------
std::vector<std::shared_ptr<Subject>> &Project::get_subjects()
{
  return this->subjects_;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_matching_columns(std::string prefix)
{
  xlnt::worksheet ws = this->wb_->sheet_by_index(0);
  auto headers = ws.rows(false)[0];
  std::vector<std::string> list;

  //std::cerr << "headers.length() = " << headers.length() << "\n";

  for (int i = 0; i < headers.length(); i++) {
    if (headers[i].to_string().substr(0, prefix.size()) == prefix) {
      list.push_back(headers[i].to_string());
    }
  }
  return list;
}

//---------------------------------------------------------------------------
std::string Project::get_value(int column, int subject_id)
{
  xlnt::worksheet ws = this->wb_->sheet_by_index(0);
  return ws.cell(xlnt::cell_reference(column, subject_id)).value<std::string>();
}

//---------------------------------------------------------------------------
void Project::set_value(int column, int subject_id, std::string value)
{
  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  //std::cerr << "setting value for column = " << column << ", subject = " << subject_id << " to " <<
  //value << "\n";

  ws.cell(xlnt::cell_reference(column, subject_id)).value(value);
}

//---------------------------------------------------------------------------
void Project::set_value(std::string column_name, int subject_id, std::string value)
{
  int column_index = get_index_for_column(column_name, true);
  this->set_value(column_index, subject_id + 2, value); // +1 for header, +1 for 1-indexed
}

//---------------------------------------------------------------------------
std::string Project::get_subject_value(int column, int subject_id)
{
  return this->get_value(column, subject_id + 2); // 1-based and skip header
}

//---------------------------------------------------------------------------
void Project::load_subjects()
{
  int num_subjects = this->get_number_of_subjects();

  this->subjects_.clear();

  this->num_domains_ = this->get_number_of_domains();

  auto seg_columns = this->get_matching_columns(SEGMENTATION_PREFIX);
  auto groomed_columns = this->get_matching_columns(GROOMED_PREFIX);

  int local_particle_column = this->get_index_for_column(LOCAL_PARTICLES);
  int global_particle_column = this->get_index_for_column(WORLD_PARTICLES);

  for (int i = 0; i < num_subjects; i++) {
    std::shared_ptr<Subject> subject = std::make_shared<Subject>();

    subject->set_number_of_domains(this->num_domains_);

    subject->set_segmentation_filenames(this->get_list(seg_columns, i));
    subject->set_groomed_filenames(this->get_list(groomed_columns, i));

    if (local_particle_column > 0) {
      this->particles_present_ = true;
      subject->set_local_particle_filename(this->get_subject_value(local_particle_column, i));
    }

    if (global_particle_column > 0) {
      this->particles_present_ = true;
      subject->set_global_particle_filename(this->get_subject_value(global_particle_column, i));
    }

    this->segmentations_present_ = seg_columns.size() >= 1;
    this->groomed_present_ = groomed_columns.size() >= 1;
    this->subjects_.push_back(subject);
  }
}

//---------------------------------------------------------------------------
void Project::store_subjects()
{
  int num_subjects = this->subjects_.size();

  auto seg_columns = this->get_matching_columns(SEGMENTATION_PREFIX);

  //auto groomed_columns = this->get_matching_columns(GROOMED_PREFIX);
  std::vector<std::string> groomed_columns;

  for (int i = 0; i < seg_columns.size(); i++) {
    std::string groom_column_name = ReplaceString(seg_columns[i], SEGMENTATION_PREFIX,
                                                  GROOMED_PREFIX);
    groomed_columns.push_back(groom_column_name);
  }

  for (int i = 0; i < num_subjects; i++) {
    std::shared_ptr<Subject> subject = this->subjects_[i];

    auto seg_files = subject->get_segmentation_filenames();
    if (seg_files.size() > seg_columns.size()) {
      seg_columns.push_back(std::string(SEGMENTATION_PREFIX) + "file");
    }
    this->set_list(seg_columns, i, seg_files);
    auto groomed_files = subject->get_groomed_filenames();
    if (groomed_files.size() == groomed_columns.size()) {
      this->set_list(groomed_columns, i, groomed_files);
    }

    std::string local_filename = subject->get_local_particle_filename();
    if (local_filename != "") {
      this->set_value("local_particles", i, local_filename);
      this->particles_present_ = true;
    }
    std::string global_filename = subject->get_global_particle_filename();
    if (global_filename != "") {
      this->set_value("world_particles", i, global_filename);
      this->particles_present_ = true;
    }
  }

  this->segmentations_present_ = seg_columns.size() >= 1;
  this->groomed_present_ = groomed_columns.size() >= 1;
}

//---------------------------------------------------------------------------
int Project::get_index_for_column(std::string name, bool create_if_not_found)
{

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  auto headers = ws.rows(false)[0];

  //std::cerr << "number of headers = " << headers.length() << "\n";
  for (int i = 0; i < headers.length(); i++) {
    //std::cerr << headers[i].to_string() << "\n";
    if (headers[i].to_string() == name) {
      return i + 1;
    }
  }

  if (create_if_not_found) {
//    std::cerr << "couldn't find: " << name << "\n";
    auto column = ws.highest_column();
    //  std::cerr << "highest column is " << column.index << "\n";
    if (ws.cell(xlnt::cell_reference(column.index, 1)).value<std::string>() == "") {
      ws.cell(xlnt::cell_reference(column.index, 1)).value(name);
      return column.index;
    }
    else {
      ws.cell(xlnt::cell_reference(column.index + 1, 1)).value(name);
      return column.index + 1;
    }
  }

  return -1;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_string_column(std::string name)
{
  int index = this->get_index_for_column(name);

  //std::cerr << "index for '" << name << "' = " << index << "\n";
  std::vector<std::string> list;
  if (index < 0) {
    return list;
  }

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  auto rows = ws.rows(false);

  //std::cerr << "rows.length = " << rows.length() << "\n";

  for (int i = 1; i < rows.length(); i++) {
    std::string value = rows[i][index - 1].to_string();
    //std::cerr << "value = " << value << "\n";
    //if (value != "") {
    list.push_back(value);
    //}
  }

  return list;
}

//---------------------------------------------------------------------------
bool Project::get_segmentations_present()
{
  return this->segmentations_present_;
}

//---------------------------------------------------------------------------
bool Project::get_groomed_present()
{
  return this->groomed_present_;
}

//---------------------------------------------------------------------------
bool Project::get_particles_present()
{
  return this->particles_present_;
}

//---------------------------------------------------------------------------
Parameters Project::get_settings(std::string name)
{
  Parameters settings;
  std::map<std::string, std::string> map;

  if (!this->wb_->contains(name)) {
    return settings;
  }
  xlnt::worksheet ws = this->wb_->sheet_by_title(name);

  if (ws.highest_column().index < 2) {
    return settings;
  }

  auto rows = ws.rows(false);

  for (int i = 1; i < rows.length(); i++) {
    std::string key = rows[i][0].to_string();
    std::string value = rows[i][1].to_string();

    map[key] = value;
  }
  settings.set_map(map);
  return settings;
}

//---------------------------------------------------------------------------
void Project::set_settings(std::string name, Parameters settings)
{
  try {
    // remove the old sheet
    // we do this because otherwise keys that were removed, we would have to search
    // for and remove these keys
    if (this->wb_->contains(name)) {
      auto ws = this->wb_->sheet_by_title(name);
      this->wb_->remove_sheet(ws);
    }

    auto ws = this->wb_->create_sheet();
    ws.title(name);

    ws.cell(xlnt::cell_reference(1, 1)).value("key");
    ws.cell(xlnt::cell_reference(2, 1)).value("value");
    int row = 2; // skip header
    for (const auto& kv : settings.get_map()) {
      std::cout << "Storing " << kv.first << " with value " << kv.second << std::endl;
      ws.cell(xlnt::cell_reference(1, row)).value(kv.first);
      ws.cell(xlnt::cell_reference(2, row)).value(kv.second);
      row++;
    }
  } catch (xlnt::exception &e) {

    std::cerr << std::string("Error storing settings: ")
              << std::string(e.what()) << ", " << "\n";
  }
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_list(std::vector<std::string> columns, int subject)
{
  std::vector<std::string> list;
  for (int s = 0; s < columns.size(); s++) {
    auto column = columns[s];
    int column_index = this->get_index_for_column(column);
    auto value = this->get_value(column_index, subject + 2); // +1 for header, +1 for 1-indexed
    list.push_back(value);
  }
  return list;
}

//---------------------------------------------------------------------------
void Project::set_list(std::vector<std::string> columns, int subject,
                       std::vector<std::string> values)
{

  for (int s = 0; s < columns.size(); s++) {
    auto column = columns[s];
    int column_index = get_index_for_column(column, true);
    //std::cerr << "get_index_for_column(" << column << ") returned " << column_index << "\n";
    this->set_value(column_index, subject + 2, values[s]); // +1 for header, +1 for 1-indexed
  }
}

//---------------------------------------------------------------------------
void Project::save_string_column(std::string name, std::vector<std::string> items)
{
  int index = this->get_index_for_column(name, true);

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  ws.cell(xlnt::cell_reference(index + 1, 1)).value(name);

  for (int i = 0; i < items.size(); i++) {
    ws.cell(xlnt::cell_reference(index + 1, i + 2)).value(items[i]);
  }
}

//---------------------------------------------------------------------------
