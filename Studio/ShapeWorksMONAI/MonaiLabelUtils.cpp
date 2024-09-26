#include <ShapeWorksMONAI/MonaiLabelUtils.h>

#include <QDir>
#include <QFile>
#include <QString>
#include <QFileInfo>
#include <QSharedPointer>

#include <Data/Session.h>

using namespace shapeworks;

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

bool MonaiLabelUtils::deleteTempFile(const QString& filePath) {
  QFile file(filePath);
  if (file.exists())
    return file.remove();

  else
    return false;
}

//---------------------------------------------------------------------------
std::string MonaiLabelUtils::getFeatureName(QSharedPointer<Session> session) {
  std::string feature_name = session->get_image_name();
  auto image_names = session->get_project()->get_image_names();
  if (image_names.size() > 0) {
    feature_name = image_names[0];
  }
  return feature_name;
}

}  // namespace monailabel