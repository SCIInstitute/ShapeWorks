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
{

}

//---------------------------------------------------------------------------
bool Project::load(std::string filename)
{
  try
  {
    this->wb_->load(filename);
  } catch (xlnt::exception &e) {
    throw "Error reading xlsx";
  }

  this->original_files_ = this->get_string_column("original_files");
  this->distance_transform_files_ = this->get_string_column("distance_transforms");
  this->local_point_files_ = this->get_string_column("local_point_files");
  this->global_point_files_ = this->get_string_column("world_point_files");

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
int Project::get_index_for_column(std::string name, bool create_if_not_found)
{

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  auto headers = ws.rows(false)[0];

  std::cerr << "headers = " << headers.length() << "\n";
  for (int i = 0; i < headers.length(); i++) {
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

  std::vector<std::string> list;
  if (index < 0) {
    return list;
  }

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  auto rows = ws.rows(false);

  for (int i = 1; i < rows.length(); i++) {
    std::string value = rows[i][index].to_string();
    if (value != "") {
      list.push_back(value);
    }
  }

  return list;
}

//---------------------------------------------------------------------------
void Project::save_string_column(std::string name, std::vector<std::string> items)
{
  int index = this->get_index_for_column(name, true);

  xlnt::worksheet ws = this->wb_->sheet_by_index(0);

  ws.cell(xlnt::cell_reference(index, 1)).value(name);

  for (int i = 0; i < items.size(); i++) {
    ws.cell(xlnt::cell_reference(index, i + 2)).value(items[i]);
  }
}

//---------------------------------------------------------------------------
