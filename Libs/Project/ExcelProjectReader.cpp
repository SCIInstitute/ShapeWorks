#include "ExcelProjectReader.h"

#include <xlnt/workbook/workbook_view.hpp>
#include <xlnt/xlnt.hpp>

#include "ProjectUtils.h"

namespace shapeworks {

using namespace project::prefixes;
using namespace project::types;

//---------------------------------------------------------------------------
class ExcelProjectReader::Container {
 public:
  Container() {};
  ~Container() {};

  xlnt::workbook wb;

  //---------------------------------------------------------------------------
  // read an excel sheet into a list of string maps with headers as keys
  //---------------------------------------------------------------------------
  StringMapList sheet_to_map_list(std::string name) {
    StringMapList list;

    if (!wb.contains(name)) {
      return list;
    }
    xlnt::worksheet ws = wb.sheet_by_title(name);

    auto rows = ws.rows(false);
    auto headers = rows[0];

    for (int i = ws.lowest_row(); i < ws.highest_row(); i++) {
      StringMap map;
      for (int h = 0; h < headers.length(); h++) {
        std::string header = headers[h].to_string();
        std::string value = rows[i][h].to_string();
        map[header] = value;
      }
      list.push_back(map);
    }

    return list;
  }

  //---------------------------------------------------------------------------
  StringMultiMap sheet_to_multi_map(std::string name) {
    if (!wb.contains(name)) {
      return {};
    }

    xlnt::worksheet ws = wb.sheet_by_title(name);

    if (ws.highest_column().index < 2) {
      return {};
    }

    StringMultiMap mmap;
    auto rows = ws.rows(false);
    auto headers = rows[0];

    for (int h = 1; h < headers.length(); h++) {
      if (headers[h].to_string().empty()) {
        continue;
      }
      StringMap map;
      auto header = rows[0][h].to_string();
      std::string domain;
      std::string prefix = "value_";
      if (ProjectUtils::starts_with(header, prefix)) {
        domain = header.substr(prefix.length());
      }
      if (header == "value") {
        domain = ""; // default
      }
      for (int i = ws.lowest_row(); i < ws.highest_row(); i++) {
        std::string key = rows[i][0].to_string();
        std::string value = rows[i][1].to_string();
        if (value != "") {
          map[key] = value;
        }
      }
      mmap[domain] = map;
    }

    return mmap;
  }

  //---------------------------------------------------------------------------
  StringMap sheet_to_map(std::string name) {
    if (!wb.contains(name)) {
      return {};
    }

    xlnt::worksheet ws = wb.sheet_by_title(name);

    if (ws.highest_column().index < 2) {
      return {};
    }

    StringMap map;
    auto rows = ws.rows(false);

    for (int i = ws.lowest_row(); i < ws.highest_row(); i++) {
      std::string key = rows[i][0].to_string();
      std::string value = rows[i][1].to_string();
      if (value != "") {
        map[key] = value;
      }
    }

    return map;
  }

  //---------------------------------------------------------------------------
  std::string get_data_sheet() {
    if (wb.contains("data")) {
      return "data";
    } else {
      return wb.sheet_by_id(1).title();
    }
  }
};

//---------------------------------------------------------------------------
ExcelProjectReader::ExcelProjectReader(Project& project) : ProjectReader(project), container_(new Container) {}

//---------------------------------------------------------------------------
ExcelProjectReader::~ExcelProjectReader() {}

//---------------------------------------------------------------------------
bool ExcelProjectReader::read_project(std::string filename) {
  container_->wb.load(filename);

  load_subjects(container_->sheet_to_map_list(container_->get_data_sheet()));
  load_landmark_definitions(container_->sheet_to_map_list("landmarks"));
  load_parameters();

  return true;
}

//---------------------------------------------------------------------------
StringMap ExcelProjectReader::get_parameters(std::string name) { return container_->sheet_to_map(name); }

//---------------------------------------------------------------------------
StringMultiMap ExcelProjectReader::get_multi_parameters(std::string name) {
  return container_->sheet_to_multi_map(name);
}



//---------------------------------------------------------------------------

}  // namespace shapeworks
