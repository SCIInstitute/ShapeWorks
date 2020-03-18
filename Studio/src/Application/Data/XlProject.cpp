#include <Data/XlProject.h>

//---------------------------------------------------------------------------
XlProject::XlProject()
{}

//---------------------------------------------------------------------------
bool XlProject::load(std::string filename)
{
  try
  {
    this->wb_.load(filename);
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
std::vector<std::string> XlProject::get_original_files()
{
  return this->original_files_;
}

//---------------------------------------------------------------------------
std::vector<std::string> XlProject::get_distance_transform_files()
{
  return this->distance_transform_files_;
}

//---------------------------------------------------------------------------
std::vector<std::string> XlProject::get_local_point_files()
{
  return this->local_point_files_;
}

//---------------------------------------------------------------------------
std::vector<std::string> XlProject::get_global_point_files()
{
  return this->global_point_files_;
}

//---------------------------------------------------------------------------
int XlProject::get_index_for_column(std::string name)
{

  xlnt::worksheet ws = this->wb_.sheet_by_index(0);

  auto headers = ws.rows(false)[0];

  for (int i = 0; i < headers.length(); i++) {
    if (headers[i].to_string() == name) {
      return i;
    }
  }

  return -1;
}

//---------------------------------------------------------------------------
std::vector<std::string> XlProject::get_string_column(std::string name)
{
  int index = this->get_index_for_column(name);

  std::vector<std::string> list;
  if (index < 0) {
    return list;
  }

  xlnt::worksheet ws = this->wb_.sheet_by_index(0);

  auto rows = ws.rows(false);

  for (int i = 1; i < rows.length(); i++) {
    std::string value = rows[i][index].to_string();
    std::cerr << "value = " << value << "\n";
    if (value != "") {
      list.push_back(value);
    }
  }

  return list;
}

//---------------------------------------------------------------------------
