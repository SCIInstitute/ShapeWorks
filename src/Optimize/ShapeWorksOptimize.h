#ifndef __SHAPEWORKSOPTIMIZE_H__
#define __SHAPEWORKSOPTIMIZE_H__

#include <cstdlib>
#include <string>
#include <vector>
#include <Data/Mesh.h>
#include "itkImage.h"
#include "itkPSMProcrustesRegistration.h"
#include "itkImageFileReader.h"
#include "itkPSMEntropyModelFilter.h"
#include "itkPSMProject.h"
#include "itkPSMProjectReader.h"
#include "itkPSMParticleSystem.h"
#include "itkCommand.h"

class ShapeWorksOptimize {
public:
  ShapeWorksOptimize(std::vector<ImageType::Pointer> inputs = 
    std::vector<ImageType::Pointer>(), size_t numScales = 1, 
    std::vector<double> start_reg = std::vector<double>(),
    std::vector<double> end_reg = std::vector<double>(),
    std::vector<unsigned int> iters = std::vector<unsigned int>(),
    std::vector<double> tolerance = std::vector<double>(),
    std::vector<double> decay_span = std::vector<double>(),
    bool verbose = false);
  void run();
  void queueTool(std::string tool);
  std::vector<ImageType::Pointer> getImages();
  std::vector<std::vector<itk::Point<float> > > optimizedPoints();
  std::vector<std::vector<itk::Point<float> > > initialPoints();
protected:
  void ShapeWorksOptimize::IterateCallback(
    itk::Object *caller, const itk::EventObject &);
private:
  std::vector<ImageType::Pointer> images_;
  bool verbose_;
  size_t numScales_, reportInterval_,
    procrustesCounter_;
  std::vector<unsigned int> maxIter_;
  std::vector<size_t>  procrustesInterval_;
  std::vector<double> tolerance_, decaySpan_,
    regularizationInitial_, regularizationFinal_;
  itk::PSMEntropyModelFilter<ImageType>::Pointer psmFilter_;
  itk::PSMProcrustesRegistration<3>::Pointer procrustesRegistration_;
  std::vector<std::vector<itk::Point<float> > >  optimizedPoints_, initialPoints_;
  itk::MemberCommand<ShapeWorksOptimize>::Pointer iterateCmd_;
};

#endif

