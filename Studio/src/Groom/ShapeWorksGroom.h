#pragma once

#include <cstdlib>
#include <string>
#include <vector>
#include <map>

#include <itkConnectedThresholdImageFilter.h>
#include <itkAffineTransform.h>
#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>

#include <Libs/Project/Project.h>

typedef float PixelType;
typedef itk::Image<PixelType, 3> ImageType;
typedef itk::ImageFileReader<ImageType> ReaderType;
typedef itk::ImageFileWriter<ImageType> WriterType;
typedef itk::AffineTransform<double, 3>::ParametersType TransformType;

class ShapeWorksGroom {
public:

  explicit ShapeWorksGroom(shapeworks::ProjectHandle project);

  explicit ShapeWorksGroom(std::vector<ImageType::Pointer> inputs = std::vector<ImageType::Pointer>(),
                  double background = 0., double foreground = 0.,
                  double sigma = 2.0,
                  size_t padding = 0, size_t iterations = 100,
                  bool verbose = false);
  virtual void run();
  void queueTool(const std::string& tool);
  std::vector<ImageType::Pointer> getImages();
  double foreground() const;
  std::map<std::string, bool> tools();

  std::vector<TransformType> get_transforms();

protected:
  void isolate(int which = -1);
  void hole_fill(int which = -1);
  void center(int which = -1);
  void antialias(int which = -1);
  void fastmarching(int which = -1);
  void blur(int which = -1);
  void auto_pad(int which = -1);
  //member variables
  std::vector<ImageType::Pointer> images_;
  bool verbose_;
  double background_, foreground_, blurSigma_;
  std::vector<TransformType> transforms_;
  size_t padding_, iterations_;
  std::map<std::string, bool> runTools_;
  ImageType::IndexType upper_ = {0,0,0};
  ImageType::IndexType lower_ = {0,0,0};
  bool paddingInit_ = false;

  shapeworks::ProjectHandle project_;
};
