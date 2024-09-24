#pragma once

#include <QString>

namespace monailabel {

class MonaiLabelUtils {
 public:
  static bool createDir(const QString& dirPath);
  static bool copySegmentation(const QString& sourcePath,
                               const QString& destinationPath);
//   static std::string getFeatureName();
//   static std::string setSubjectImageFileName();
};
}  // namespace monailabel