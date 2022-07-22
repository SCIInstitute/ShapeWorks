#include "JsonProjectWriter.h"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace shapeworks {

static json create_data_sheet(ProjectHandle project) {
  json j;

  return j;
}

bool JsonProjectWriter::write_project(ProjectHandle project, std::string filename) {
  json j;

  json data_sheet;
  json groom_sheet;
  json optimize_sheet;
  json studio_sheet;
  json project_sheet;
  json analysis_sheet;
  json deepssm_sheet;
  json landmarks_sheet;

  j["data"] = create_data_sheet(project);
  j["groom"] = groom_sheet;
  j["optimize"] = optimize_sheet;
  j["studio"] = studio_sheet;
  j["project"] = project_sheet;
  j["analysis"] = analysis_sheet;
  j["deepssm"] = deepssm_sheet;
  j["landmarks"] = landmarks_sheet;

  std::ofstream file(filename);
  if (!file.good()) {
    throw std::runtime_error("Unable to open " + filename + " for writing");
  }
  file << j.dump(4);

  return true;
}

}  // namespace shapeworks
