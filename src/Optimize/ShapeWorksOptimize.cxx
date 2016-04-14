#include <ShapeWorksOptimize.h>
#include <iostream>

ShapeWorksOptimize::ShapeWorksOptimize(
  std::vector<ImageType::Pointer> inputs, size_t numScales,
  std::vector<double> start_reg, std::vector<double> end_reg,
  std::vector<unsigned int> iters, std::vector<double> tolerance,
  std::vector<double> decay_span, bool verbose) 
  : images_(inputs), numScales_(numScales),
  maxIter_(iters),
  reportInterval_(10), procrustesCounter_(0),
  tolerance_(tolerance), decaySpan_(decay_span),
  regularizationInitial_(start_reg),
  regularizationFinal_(end_reg),
  verbose_(verbose) {
  this->psmFilter_ = itk::PSMEntropyModelFilter<ImageType>::New();
  this->procrustesRegistration_ = itk::PSMProcrustesRegistration<3>::New();
}

void ShapeWorksOptimize::run() {
  this->psmFilter_->SetNumberOfScales(this->numScales_);
  this->psmFilter_->SetRegularizationInitial(regularizationInitial_);
  this->psmFilter_->SetRegularizationFinal(regularizationFinal_);
  this->psmFilter_->SetRegularizationDecaySpan(decaySpan_);
  this->psmFilter_->SetTolerance(tolerance_);
  this->psmFilter_->SetMaximumNumberOfIterations(maxIter_);
  this->procrustesInterval_.resize(this->numScales_);
  size_t i = 0;
  for (auto &a : this->images_) {
    this->psmFilter_->SetDomainName("item_" + std::to_string(i), i);
    this->psmFilter_->SetInput(i, a);
    i++;
  }
  this->procrustesRegistration_->SetPSMParticleSystem(
    this->psmFilter_->GetParticleSystem());

  this->iterateCmd_ = itk::MemberCommand<ShapeWorksOptimize>::New();
  this->iterateCmd_->SetCallbackFunction(this, &ShapeWorksOptimize::IterateCallback);
  this->psmFilter_->AddObserver(itk::IterationEvent(), this->iterateCmd_);

  this->psmFilter_->Update();
  for (size_t d = 0; d < this->psmFilter_->
    GetParticleSystem()->GetNumberOfDomains(); d++) {
    this->optimizedPoints_.push_back(std::vector<itk::Point<float> >());
    for (size_t j = 0; j < this->psmFilter_->
      GetParticleSystem()->GetNumberOfParticles(d); j++) {
      auto pos = this->psmFilter_->GetParticleSystem()->GetPosition(j, d);
      this->optimizedPoints_[d].push_back(pos);
    }
  } 
}

std::vector<std::vector<itk::Point<float> > >  
ShapeWorksOptimize::optimizedPoints() {
  return this->optimizedPoints_;
}

std::vector<ImageType::Pointer> ShapeWorksOptimize::getImages() {
  return this->images_;
}

void ShapeWorksOptimize::IterateCallback(itk::Object *caller, const itk::EventObject &)
{
  itk::PSMEntropyModelFilter<ImageType> *o =
    reinterpret_cast<itk::PSMEntropyModelFilter<ImageType> *>(caller);

  // Print every 10 iterations
  if (o->GetNumberOfElapsedIterations() % this->reportInterval_ != 0) { return; }

  std::cout << "Iteration # " << o->GetNumberOfElapsedIterations() << std::endl;
  std::cout << " Eigenmode variances: ";
  for (unsigned int i = 0; i < o->GetShapePCAVariances().size(); i++)
  {
    std::cout << o->GetShapePCAVariances()[i] << " ";
  }
  std::cout << std::endl;
  std::cout << " Regularization = " << o->GetRegularizationConstant() << std::endl;

  // Check if optimization is run using scales. Get Procrustes
  // interval for the current scale.
  int interval = 0;
  std::cout << "Optimization Scale " << (o->GetCurrentScale() + 1) << "/"
    << o->GetNumberOfScales() << std::endl;

  if (o->GetNumberOfScales() > 1)
  {
    interval = this->procrustesInterval_[o->GetCurrentScale()];
    std::cout << "Interval = " << interval << std::endl;
  } else // Use the default interval
  {
    interval = this->procrustesRegistration_->GetProcrustesInterval();
  }

  // Check if the Procrustes interval is set to a value other than 0
  if (interval > 0)
  {
    this->procrustesCounter_++;

    // If the counter is greater than the interval value, run
    // Procrustes registration
    if (this->procrustesCounter_ >= interval)
    {
      // Reset the counter
      this->procrustesCounter_ = 0;
      this->procrustesRegistration_->RunRegistration();
      std::cout << "Ran Procrustes Registration" << std::endl;
    }
  }
}