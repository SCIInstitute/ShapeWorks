#include <Project.h>

#include <xlnt/xlnt.hpp>
#include <memory>

using namespace shapeworks;

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
    throw "Error reading xlsx";
  }

  this->load_subjects();
/*
  this->original_files_ = this->get_string_column("original_files");
  this->distance_transform_files_ = this->get_string_column("distance_transforms");
  this->local_point_files_ = this->get_string_column("local_point_files");
  this->global_point_files_ = this->get_string_column("world_point_files");
*/
  this->loaded_ = true;
  return true;
}

//---------------------------------------------------------------------------
bool Project::save(std::string filename)
{

  this->save_string_column("original_files", this->original_files_);
  this->save_string_column("distance_transforms", this->distance_transform_files_);
  this->save_string_column("local_point_files", this->local_point_files_);
  this->save_string_column("world_point_files", this->global_point_files_);

  this->wb_->save(filename);

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
  auto seg_columns = this->get_matching_columns("segmentation_");
  auto mesh_columns = this->get_matching_columns("mesh_");
  auto dt_columns = this->get_matching_columns("distance_transform_");
  auto local_particle_files = this->get_matching_columns("local_particles");
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
std::vector<std::string> Project::get_segmentations()
{
  return this->get_matching_columns("segmentation_");
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_original_files()
{
  return this->original_files_;
}

//---------------------------------------------------------------------------
void Project::set_original_files(std::vector<std::string> files)
{
  this->original_files_ = files;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_distance_transform_files()
{
  return this->distance_transform_files_;
}

//---------------------------------------------------------------------------
void Project::set_distance_transform_files(std::vector<std::string> files)
{
  this->distance_transform_files_ = files;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_local_point_files()
{
  return this->local_point_files_;
}

//---------------------------------------------------------------------------
void Project::set_local_point_files(std::vector<std::string> files)
{
  this->local_point_files_ = files;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_global_point_files()
{
  return this->global_point_files_;
}

//---------------------------------------------------------------------------
void Project::set_global_point_files(std::vector<std::string> files)
{
  this->global_point_files_ = files;
}

//---------------------------------------------------------------------------
std::vector<Subject> Project::get_subjects()
{
  return this->subjects_;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_matching_columns(std::string prefix)
{
  xlnt::worksheet ws = this->wb_->sheet_by_index(0);
  auto headers = ws.rows(false)[0];
  std::vector<std::string> list;

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

  return ws.cell(xlnt::cell_reference(column + 1, subject_id + 1)).value<std::string>();
}

//---------------------------------------------------------------------------
void Project::load_subjects()
{
  int num_subjects = this->get_number_of_subjects();

  this->subjects_.clear();

  this->num_domains_ = this->get_number_of_domains();

  auto seg_columns = this->get_matching_columns(SEGMENTATION_PREFIX);

  for (int i = 0; i < num_subjects; i++) {
    Subject subject;
    subject.set_number_of_domains(this->num_domains_);

    std::vector<std::string> list;

    for (int s = 0; s < seg_columns.size(); s++) {
      auto column = seg_columns[s];
      int column_index = get_index_for_column(column);
      auto value = get_value(column_index, i + 1);
      //std::cerr << "## value = " << value << "\n";
      list.push_back(value);
    }

    subject.set_segmentation_filenames(list);
    this->segmentations_present_ = true;
    this->subjects_.push_back(subject);
  }
}

//---------------------------------------------------------------------------
int Project::get_index_for_column(std::string name, bool create_if_not_found)
{

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  auto headers = ws.rows(false)[0];

  //std::cerr << "headers = " << headers.length() << "\n";
  for (int i = 0; i < headers.length(); i++) {
    //std::cerr << headers[i].to_string() << "\n";
    if (headers[i].to_string() == name) {
      return i;
    }
  }

  if (create_if_not_found) {
    auto column = ws.highest_column();
    if (ws.cell(xlnt::cell_reference(column, 1)).value<std::string>() == "") {
      return column.index;
    }
    else {
      return column.index + 1;
    }
  }

  return -1;
}

//---------------------------------------------------------------------------
std::vector<std::string> Project::get_string_column(std::string name)
{
  int index = this->get_index_for_column(name);

  std::cerr << "index for '" << name << "' = " << index << "\n";
  std::vector<std::string> list;
  if (index < 0) {
    return list;
  }

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  auto rows = ws.rows(false);

  //std::cerr << "rows.length = " << rows.length() << "\n";

  for (int i = 1; i < rows.length(); i++) {
    std::string value = rows[i][index].to_string();
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
