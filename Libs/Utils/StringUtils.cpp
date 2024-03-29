
#include "StringUtils.h"

#include <algorithm>
#include <boost/algorithm/string/case_conv.hpp>
#include <boost/filesystem.hpp>
#include <cstdlib>
#include <cstring>
#include <sstream>

namespace {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
struct MatchPathSeparator {
  bool operator()(char ch) const { return ch == '\\' || ch == '/'; }
};
#else
struct MatchPathSeparator {
  bool operator()(char ch) const { return ch == '/'; }
};
#endif
}  // namespace

namespace shapeworks {

//---------------------------------------------------------------------------
std::string StringUtils::removeExtension(std::string const& filename) {
  std::string::const_reverse_iterator pivot = std::find(filename.rbegin(), filename.rend(), '.');
  return pivot == filename.rend() ? filename : std::string(filename.begin(), pivot.base() - 1);
}

//---------------------------------------------------------------------------
std::string StringUtils::getPath(std::string const& filename) {
  std::string::const_reverse_iterator pivot = std::find(filename.rbegin(), filename.rend(), '/');
  return pivot == filename.rend() ? filename : std::string(filename.begin(), pivot.base() - 1);
}

//---------------------------------------------------------------------------
std::string StringUtils::toLower(std::string s) {
  std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) { return std::tolower(c); });
  return s;
}

//---------------------------------------------------------------------------
std::string StringUtils::getFilename(std::string const& pathname) {
  return std::string(std::find_if(pathname.rbegin(), pathname.rend(), MatchPathSeparator()).base(), pathname.end());
}

//---------------------------------------------------------------------------
std::string StringUtils::getBaseFilenameWithoutExtension(const std::string& pathname) {
  return removeExtension(getFilename(pathname));
}

//---------------------------------------------------------------------------
bool StringUtils::hasSuffix(const std::string& filename, const std::string& suffix) {
  auto file = StringUtils::toLower(filename);
  auto ext = StringUtils::toLower(suffix);

  return file.size() >= ext.size() && 0 == file.compare(file.size() - ext.size(), ext.size(), ext);
}

//---------------------------------------------------------------------------
std::vector<std::string> StringUtils::getFileNamesFromPaths(const std::vector<std::string>& paths) {
  std::vector<std::string> filenames;
  for (int i = 0; i < paths.size(); i++) {
    std::string fname = StringUtils::getBaseFilenameWithoutExtension(paths[i]);
    filenames.push_back(std::string(fname));
  }
  return filenames;
}

//---------------------------------------------------------------------------
std::string StringUtils::getLowerExtension(const std::string& filename) {
  return boost::algorithm::to_lower_copy(boost::filesystem::extension(filename));
}

//---------------------------------------------------------------------------
std::string StringUtils::replace_string(std::string str, const std::string& search, const std::string& replace) {
  size_t pos = 0;
  while ((pos = str.find(search, pos)) != std::string::npos) {
    str.replace(pos, search.length(), replace);
    pos += replace.length();
  }
  return str;
}

//---------------------------------------------------------------------------
std::string StringUtils::join(const std::vector<std::string>& strings, const std::string& delimiter) {
  // join strings with delemeter between
  std::stringstream ss;
  for (size_t i = 0; i < strings.size(); i++) {
    if (i > 0) {
      ss << delimiter;
    }
    ss << strings[i];
  }
  return ss.str();
}

}  // namespace shapeworks
