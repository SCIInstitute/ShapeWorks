#include <ShapeWorksMONAI/MonaiLabelUtils.h>

#include <QDir>
#include <QFile>
#include <QString>
#include <QFileInfo>
#include <QSharedPointer>

#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkBinaryThresholdImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkImageRegionIterator.h>
#include <Data/Session.h>

using namespace shapeworks;

typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
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

//---------------------------------------------------------------------------
ImageType::Pointer MonaiLabelUtils::loadNRRD(const std::string& filePath) {
    using ReaderType = itk::ImageFileReader<ImageType>;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(filePath);
    reader->Update();
    return reader->GetOutput();
}

//---------------------------------------------------------------------------
bool MonaiLabelUtils::isOrganPresent(ImageType::Pointer image) {
    itk::ImageRegionIterator<ImageType> it(image, image->GetRequestedRegion());
    while (!it.IsAtEnd()) {
        if (it.Get() > 0) {  // If any non-background pixel is found
            return true;
        }
        ++it;
    }
    return false;
}
//---------------------------------------------------------------------------
ImageType::Pointer MonaiLabelUtils::extractOrganSegmentation(ImageType::Pointer inputImage, int label) {
    using ThresholdFilterType = itk::BinaryThresholdImageFilter<ImageType, ImageType>;
    ThresholdFilterType::Pointer thresholdFilter = ThresholdFilterType::New();
    thresholdFilter->SetInput(inputImage);
    thresholdFilter->SetLowerThreshold(label);
    thresholdFilter->SetUpperThreshold(label);
    // thresholdFilter->SetInsideValue(label);
    thresholdFilter->SetInsideValue(1); // dont save as label
    thresholdFilter->SetOutsideValue(0);  // Background set to 0
    thresholdFilter->Update();

    ImageType::Pointer organImage = thresholdFilter->GetOutput();
    // Check if organ is present in segmentation
    if (thresholdFilter->GetOutput()->GetBufferedRegion().GetNumberOfPixels() == 0) {
        return nullptr;  // Return null if the organ is not present
    } // save all
    // if (!isOrganPresent(organImage)) {
    //     return nullptr;  // Return nullptr if no valid organ pixels exist
    // }
    // return thresholdFilter->GetOutput();
    return organImage;
}

//---------------------------------------------------------------------------
void MonaiLabelUtils::saveNRRD(ImageType::Pointer image, const std::string& outputPath) {
    using WriterType = itk::ImageFileWriter<ImageType>;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outputPath);
    writer->SetInput(image);
    writer->UseCompressionOn();
    writer->Update();
}

//---------------------------------------------------------------------------
void MonaiLabelUtils::processSegmentation(
    const std::string& segmentationPath,
    const std::map<int, std::string>& organLabels, const std::string& outputDir,
    const std::string& sampleId,
    std::vector<std::string>& organSegmentationPaths) {

  organSegmentationPaths.resize(0);
  ImageType::Pointer inputImage = loadNRRD(segmentationPath);
  if (!inputImage) {
    SW_ERROR("Failed to load segmentation file: {}", segmentationPath);
    return;
  }

  QDir projDir(QString::fromStdString(outputDir));
  // if (!projDir.exists()) {
  //   projDir.mkpath(".");
  // }

  // Extract and save each organ segmentation
  for (const auto& [label, organName] : organLabels) {
    ImageType::Pointer organImage = extractOrganSegmentation(inputImage, label);

    if (!organImage) {
      SW_LOG("Warning: {} (Label {}) not found in segmentation.", organName, label);
      continue;
    }

    QString destPath = projDir.filePath(
        QString::fromStdString(sampleId + "_" + organName + ".nrrd"));
    saveNRRD(organImage, destPath.toStdString());
    SW_LOG("✅ Saved segmented organ: {}", destPath.toStdString());
    organSegmentationPaths.push_back(destPath.toStdString());
  }
}

}  // namespace monailabel