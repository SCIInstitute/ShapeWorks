#ifndef __SHAPEWORKSGROOM_H__
#define __SHAPEWORKSGROOM_H__

#include <cstdlib>
#include <string>
#include <vector>
#include "itkConnectedThresholdImageFilter.h"
#include "itkTranslationTransform.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include <map>
#include <Data/Mesh.h>
#include <string>

typedef itk::Image<int, 3> isolate_type;
typedef itk::Image<unsigned char, 3> crop_type;
typedef itk::ConnectedThresholdImageFilter<ImageType, ImageType> flood_fill_filter_type;
typedef itk::TranslationTransform<double, 3>::ParametersType transform_type;

class ShapeWorksGroom {
public:
  ShapeWorksGroom(std::vector<ImageType::Pointer> inputs = std::vector<ImageType::Pointer>(),
    std::vector<std::string> input_names = std::vector<std::string>(),
    double background = 0., double foreground = 0.,
    double sigma = 2.0, 
    size_t padding = 0, size_t iterations = 100, 
    size_t levelsetValue = 100,
    bool verbose = false);
  void run();
  void queueTool(std::string tool);
  std::vector<std::string> getGroomFileNames();
private:
  void isolate();
  void hole_fill();
  void center();
  void antialias();
  void fastmarching();
  void blur();
  void auto_crop();
  void auto_pad();
  std::vector<ImageType::Pointer> images_;
  std::vector<std::string> input_names_;
  std::vector<std::string> groomNames_;
  bool verbose_;
  double background_, foreground_, sigma_;
  flood_fill_filter_type::IndexType seed_;
  transform_type transform_;
  size_t padding_, iterations_, levelSetValue_;
  std::map<std::string, bool> runTools_;
};

#endif

