
#include "StringUtils.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>


namespace {
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
struct MatchPathSeparator
{
  bool operator()( char ch ) const
  {
      return ch == '\\' || ch == '/';
  }
};
#else
struct MatchPathSeparator {
  bool operator()(char ch) const
  {
    return ch == '/';
  }
};
#endif
}

namespace shapeworks {

//---------------------------------------------------------------------------
std::string StringUtils::removeExtension(std::string const& filename)
{
  std::string::const_reverse_iterator pivot
    = std::find(filename.rbegin(), filename.rend(), '.');
  return pivot == filename.rend() ? filename
                                  : std::string(filename.begin(), pivot.base() - 1);
}

//---------------------------------------------------------------------------
std::string StringUtils::getPath(std::string const& filename)
{
  std::string::const_reverse_iterator
    pivot
    = std::find(filename.rbegin(), filename.rend(), '/');
  return pivot == filename.rend()
         ? filename
         : std::string(filename.begin(), pivot.base() - 1);
}

//---------------------------------------------------------------------------
std::string StringUtils::toLower(std::string s)
{
  std::transform(s.begin(), s.end(), s.begin(),
                 [](unsigned char c) { return std::tolower(c); });
  return s;
}

//---------------------------------------------------------------------------
std::string StringUtils::getFilename(std::string const& pathname)
{
  return std::string(
    std::find_if(pathname.rbegin(), pathname.rend(),
                 MatchPathSeparator()).base(),
    pathname.end());
}

//---------------------------------------------------------------------------
bool StringUtils::hasSuffix(const std::string& filename, const std::string& suffix)
{
  auto file = StringUtils::toLower(filename);
  auto ext = StringUtils::toLower(suffix);

  return file.size() >= ext.size()
         && 0 == file.compare(file.size() - ext.size(), ext.size(), ext);

}

//---------------------------------------------------------------------------
std::string StringUtils::getFileNameWithoutExtension(std::string path)
{
  char* str = new char[path.length() + 1];
  strcpy(str, path.c_str());

  // separate filename from the full path
  char* fname;
  char* pch;
  pch = strtok(str, "/");
  while (pch != NULL) {
    fname = pch;
    pch = strtok(NULL, "/");
  }

  // separate filename from the extension
  char* pch2;
  pch2 = strrchr(fname, '.');
  int num = pch2 - fname + 1;
  int num2 = strlen(fname);
  strncpy(pch2, "", num2 - num);

  return std::string(fname);
}

//---------------------------------------------------------------------------
std::vector<std::string> StringUtils::getFileNamesFromPaths(const std::vector<std::string>& paths)
{
  std::vector<std::string> filenames;
  for (int i = 0; i < paths.size(); i++) {
    std::string fname = StringUtils::getFileNameWithoutExtension(paths[i]);
    filenames.push_back(std::string(fname));
  }
  return filenames;
}
}