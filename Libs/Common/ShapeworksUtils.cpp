#include "ShapeworksUtils.h"

#include <sys/stat.h>
#include <vtkPLYReader.h>
#include <vtkPolyDataReader.h>

namespace shapeworks {

// Windows doesn't have S_ISDIR and S_ISREG macros
#if !defined(S_ISREG) && defined(S_IFMT) && defined(S_IFREG)
#define S_ISREG(m) (((m) & S_IFMT) == S_IFREG)
#endif

#if !defined(S_ISDIR) && defined(S_IFMT) && defined(S_IFDIR)
#define S_ISDIR(m) (((m) & S_IFMT) == S_IFDIR)
#endif

unsigned ShapeworksUtils::_rngSeed = std::chrono::system_clock::now().time_since_epoch().count();;

/// looks at the pathname to see if it's a file or a directory or neither
bool statdatpath(const std::string &pathname, bool isdir = false)
{
  struct stat info;
  if (stat(pathname.c_str(), &info) != 0) {
    return false;
  }
  else {
    return isdir ? S_ISDIR(info.st_mode) : S_ISREG(info.st_mode);
  }
}

bool ShapeworksUtils::is_directory(const std::string &pathname)
{
  return statdatpath(pathname, true);
}

bool ShapeworksUtils::exists(const std::string& filename)
{
  return statdatpath(filename, false);
}

Matrix33 ShapeworksUtils::getMatrix(const vtkSmartPointer<vtkMatrix4x4>& mat)
{
  Matrix33 m;

  for (int i = 0; i < 3; i++)
  {
    for (int j = 0; j < 3; j++)
    {
      m[i][j] = mat->GetElement(i, j);
    }
  }

  return m;
}

Vector3 ShapeworksUtils::getOffset(const vtkSmartPointer<vtkMatrix4x4>& mat)
{
  return makeVector({mat->GetElement(0,3), mat->GetElement(1,3), mat->GetElement(2,3)});
}

double ShapeworksUtils::elapsed(ShapeworksUtils::time_point start,
                                ShapeworksUtils::time_point end,
                                bool print_elapsed)
{
  // Calculating total time taken by the program.
  double time_taken = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
  time_taken *= 1e-9;

  if (print_elapsed)
    cout << "Elapsed: " << std::fixed << time_taken << std::setprecision(9) << " sec" << endl;

  return time_taken;
}

} // shapeworks
