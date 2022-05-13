#pragma once

#include <string>
#include <vector>

namespace shapeworks {

//! String utility functions
class StringUtils {
 public:
  static std::string removeExtension(std::string const& filename);
  static std::string getPath(std::string const& filename);
  //! Remove path from filename and return
  static std::string getFilename(std::string const& pathname);
  //! Get just the filename without path and without extension
  static std::string getBaseFilenameWithoutExtension(std::string const& pathname);
  static std::string toLower(std::string s);
  static bool hasSuffix(std::string const& filename, std::string const& suffix);
  static std::vector<std::string> getFileNamesFromPaths(const std::vector<std::string>& paths);

  // safely convert a const char* to string, allowing for nullptr
  static std::string safeString(const char* s) { return s == nullptr ? std::string() : s; }
};

}  // namespace shapeworks
