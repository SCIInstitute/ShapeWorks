#ifndef __SHAPEWORKSOPTIMIZE_H__
#define __SHAPEWORKSOPTIMIZE_H__

#include <cstdlib>
#include <string>
#include <vector>
#include <ShapeWorksGroom.h>
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
  ShapeWorksOptimize(
    std::vector<ImageType::Pointer> inputs =
    std::vector<ImageType::Pointer>(), size_t numScales = 1,
    std::vector<double> start_reg = std::vector<double>(),
    std::vector<double> end_reg = std::vector<double>(),
    std::vector<unsigned int> iters = std::vector<unsigned int>(),
    std::vector<double> decay_span = std::vector<double>(),
    double weighting = 1.,
    bool verbose = false);
  void run();
  std::vector<ImageType::Pointer> getImages();
  std::vector<std::vector<itk::Point<float> > > localPoints();
  std::vector<std::vector<itk::Point<float> > > globalPoints();
protected:
  virtual void iterateCallback(
    itk::Object *caller, const itk::EventObject &);
protected:
  std::vector<ImageType::Pointer> images_;
  bool verbose_;
  size_t numScales_;
  double weighting_;
  std::vector<unsigned int> maxIter_;
  std::vector<double> decaySpan_,
    regularizationInitial_, regularizationFinal_;
  itk::PSMEntropyModelFilter<ImageType>::Pointer psmFilter_;
  std::vector<std::vector<itk::Point<float> > >  localPoints_, globalPoints_;
  itk::MemberCommand<ShapeWorksOptimize>::Pointer iterateCmd_;
  itk::PSMProcrustesRegistration<3>::Pointer procrustesRegistration_;
  size_t reportInterval_, procrustesCounter_, totalIters_, iterCount_;
  std::vector<size_t>  procrustesInterval_;
};

#endif

