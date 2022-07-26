#include "JsonProjectReader.h"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::ordered_json;

namespace shapeworks {

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
static void read_subjects(ProjectHandle project, const json& j) {
  if (!j.contains("data")) {
    return;
  }

  std::vector<std::shared_ptr<Subject>> subjects;
  for (const auto& item : j) {
    auto subject = std::make_shared<Subject>();

    if (item.contains("name")) {
      subject->set_display_name(item["name"]);
    }

    subjects.push_back(subject);
  }


  project->set_subjects(subjects);
}

//---------------------------------------------------------------------------
bool JsonProjectReader::read_project(ProjectHandle project, std::string filename) {
  std::ifstream ifs(filename);
  json j = json::parse(ifs);

  read_subjects(project, j);

  return true;
}

}  // namespace shapeworks
