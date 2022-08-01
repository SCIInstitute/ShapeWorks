#include "ExcelProjectReader.h"

#include <xlnt/workbook/workbook_view.hpp>
#include <xlnt/xlnt.hpp>

#include "ProjectUtils.h"

namespace shapeworks {

using namespace project::prefixes;
using namespace project::types;

//---------------------------------------------------------------------------
ExcelProjectReader::ExcelProjectReader(Project &project) : ProjectReader(project) {
  wb_ = std::make_unique<xlnt::workbook>();
}

//---------------------------------------------------------------------------
bool ExcelProjectReader::read_project(std::string filename) {
  wb_->load(filename);

  return true;
}

//---------------------------------------------------------------------------
void ExcelProjectReader::load_subjects() {}

//---------------------------------------------------------------------------
StringMapList ExcelProjectReader::sheet_to_map(std::string name) {
  StringMapList list;

  if (!wb_->contains(name)) {
    return list;
  }
  xlnt::worksheet ws = wb_->sheet_by_title(name);

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

}  // namespace shapeworks
