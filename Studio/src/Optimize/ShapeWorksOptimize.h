#ifndef __SHAPEWORKSOPTIMIZE_H__
#define __SHAPEWORKSOPTIMIZE_H__

#include <cstdlib>
#include <string>
#include <vector>
#include <array>
#include <Groom/ShapeWorksGroom.h>
#include "itkImage.h"
#include <ParticleShapeworks/include/itkPSMProcrustesRegistration.h>
#include "itkImageFileReader.h"
#include <ParticleShapeworks/include/itkPSMEntropyModelFilter.h>
#include <ParticleShapeworks/include/itkPSMProject.h>
#include <ParticleShapeworks/include/itkPSMProjectReader.h>
#include <ParticleShapeworks/include/itkPSMParticleSystem.h>
#include <itkCommand.h>

class ShapeWorksOptimize { 
public:
  ShapeWorksOptimize(
    std::vector<ImageType::Pointer> inputs =
    std::vector<ImageType::Pointer>(),
    std::vector<std::array<itk::Point<float>, 3 > > cutPlanes =
    std::vector<std::array<itk::Point<float>, 3 > >(),
    size_t numScales = 1,
    std::vector<double> start_reg = std::vector<double>(),
    std::vector<double> end_reg = std::vector<double>(),
    std::vector<unsigned int> iters = std::vector<unsigned int>(),
    std::vector<double> decay_span = std::vector<double>(),
    std::vector<size_t> procrustes_interval = std::vector<size_t>(),
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
  std::vector<std::array<itk::Point<float>, 3 > > cutPlanes_;
};

#endif

