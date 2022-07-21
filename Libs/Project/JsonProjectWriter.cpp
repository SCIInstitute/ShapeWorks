#include "JsonProjectWriter.h"

#include <fstream>
#include <nlohmann/json.hpp>
using json = nlohmann::json;

namespace shapeworks {

bool JsonProjectWriter::writeProject(ProjectHandle project, std::string filename) {
  json j;

  std::ofstream file(filename);
  if (!file.good()) {
    throw std::runtime_error("Unable to open " + filename + " for writing");
  }
  file << j.dump(4);

  return true;
}

}  // namespace shapeworks
