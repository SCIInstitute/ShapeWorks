#include <ShapeWorksMONAI/MonaiLabelUtils.h>

#include <QDir>
#include <QFile>
#include <QString>



namespace monailabel {

bool MonaiLabelUtils::createDir(const QString& dirPath) {
  QDir dir;
  return dir.mkpath(dirPath);
}

bool MonaiLabelUtils::copySegmentation(const QString& sourcePath,
                                       const QString& destinationPath) {
  QFile destFile(destinationPath);
  if (destFile.exists()) {
    destFile.remove();
  }
  return QFile::copy(sourcePath, destinationPath); 
}

}  // namespace monailabel