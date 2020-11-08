#pragma once

#include <string>
#include <vector>

namespace shapeworks {

//! String utility functions
class StringUtils {

public:


  static std::string removeExtension(std::string const& filename);
  static std::string getPath(std::string const& filename);
  static std::string getFilename(std::string const& pathname);
  static std::string toLower(std::string s);
  static bool hasSuffix(std::string const& filename, std::string const& suffix);
  static std::vector<std::string> getFileNamesFromPaths(const std::vector<std::string>& paths);

  /// TODO: is this a duplicate of removeExtension?
  static std::string getFileNameWithoutExtension(std::string path);

};

}