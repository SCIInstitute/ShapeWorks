#include <ShapeWorksOptimize.h>
#include <iostream>

ShapeWorksOptimize::ShapeWorksOptimize(
  //QObject* parent,
  std::vector<ImageType::Pointer> inputs,
  std::vector<std::array<itk::Point<float>, 3 > > cutPlanes,
  size_t numScales,
  std::vector<double> start_reg, std::vector<double> end_reg,
  std::vector<unsigned int> iters, 
  std::vector<double> decay_span, double weighting,
  bool verbose)
  :// QObject(parent), 
  images_(inputs), 
  cutPlanes_(cutPlanes),
  numScales_(numScales),
  maxIter_(iters), 
  reportInterval_(10), procrustesCounter_(0),
  decaySpan_(decay_span),
  regularizationInitial_(start_reg),
  regularizationFinal_(end_reg),
  weighting_(weighting),
  verbose_(verbose) {
  this->psmFilter_ = itk::PSMEntropyModelFilter<ImageType>::New();
  this->procrustesRegistration_ = itk::PSMProcrustesRegistration<3>::New();
}

void ShapeWorksOptimize::run() {
  this->psmFilter_->SetNumberOfScales(this->numScales_);
  this->psmFilter_->SetRegularizationInitial(this->regularizationInitial_);
  this->psmFilter_->SetRegularizationFinal(this->regularizationFinal_);
  this->psmFilter_->SetRegularizationDecaySpan(this->decaySpan_);
  std::vector<double> tolerance(this->numScales_, 0.01); // this is constant
  this->psmFilter_->SetTolerance(tolerance);
  this->psmFilter_->SetMaximumNumberOfIterations(this->maxIter_);
  this->procrustesInterval_.resize(this->numScales_, this->reportInterval_); 
  this->psmFilter_->SetShapeEntropyWeighting(this->weighting_);
  //this->psmFilter_->AddCuttingPlane(x(3),y(3),z(3), shape);
  this->totalIters_ = 0;
  for (auto &a : maxIter_) {
    this->totalIters_ += a;
  }
  size_t i = 0;
  this->iterCount_ = 0;
  for (auto &a : this->images_) {
    this->psmFilter_->SetDomainName("item_" + std::to_string(i), i);
    this->psmFilter_->SetInput(i, a);
    if ((this->cutPlanes_.size() == 1) || 
      (this->cutPlanes_.size() == this->images_.size())) {
      vnl_vector_fixed<double, 3> x;
      vnl_vector_fixed<double, 3> y;
      vnl_vector_fixed<double, 3> z;
      size_t which = (this->cutPlanes_.size() == 1) ? 0 : i;
      for (size_t idx = 0; idx < 3; idx++) {
        x[idx] = this->cutPlanes_[which][idx][0];
        y[idx] = this->cutPlanes_[which][idx][1];
        z[idx] = this->cutPlanes_[which][idx][2];
      }
      this->psmFilter_->AddCuttingPlane(x, y, z, "item_" + std::to_string(i));
    }
    i++;
  }
  this->procrustesRegistration_->SetPSMParticleSystem(
    this->psmFilter_->GetParticleSystem());

  this->iterateCmd_ = itk::MemberCommand<ShapeWorksOptimize>::New();
  this->iterateCmd_->SetCallbackFunction(this, &ShapeWorksOptimize::iterateCallback);
  this->psmFilter_->AddObserver(itk::IterationEvent(), this->iterateCmd_);

  this->psmFilter_->Update();
  for (size_t d = 0; d < this->psmFilter_->
    GetParticleSystem()->GetNumberOfDomains(); d++) {
    this->localPoints_.push_back(std::vector<itk::Point<float> >());
    this->globalPoints_.push_back(std::vector<itk::Point<float> >());
    for (size_t j = 0; j < this->psmFilter_->
      GetParticleSystem()->GetNumberOfParticles(d); j++) {
      auto pos = this->psmFilter_->GetParticleSystem()->GetPosition(j, d);
      auto pos2 = this->psmFilter_->GetParticleSystem()->GetTransformedPosition(j, d);
      this->localPoints_[d].push_back(pos);
      this->globalPoints_[d].push_back(pos2);
    }
  } 
}

std::vector<std::vector<itk::Point<float> > >  
ShapeWorksOptimize::localPoints() {
  return this->localPoints_;
}

std::vector<std::vector<itk::Point<float> > >
ShapeWorksOptimize::globalPoints() {
  return this->globalPoints_;
}

std::vector<ImageType::Pointer> ShapeWorksOptimize::getImages() {
  return this->images_;
}
void ShapeWorksOptimize::iterateCallback(itk::Object *caller, const itk::EventObject &e)
{
  itk::PSMEntropyModelFilter<ImageType> *o =
    reinterpret_cast<itk::PSMEntropyModelFilter<ImageType> *>(caller);

  // Print every 10 iterations
  if (o->GetNumberOfElapsedIterations() % this->reportInterval_ != 0) { return; }
  std::cout << "Iteration # " << o->GetNumberOfElapsedIterations() << std::endl;
  std::cout << " Eigenmode variances: ";
  for (unsigned int i = 0; i < o->GetShapePCAVariances().size(); i++) {
    std::cout << o->GetShapePCAVariances()[i] << " ";
  }
  std::cout << std::endl;
  std::cout << " Regularization = " << o->GetRegularizationConstant() << std::endl;

  // Check if optimization is run using scales. Get Procrustes
  // interval for the current scale.
  int interval = 0;
  std::cout << "Optimization Scale " << (o->GetCurrentScale() + 1) << "/"
    << o->GetNumberOfScales() << std::endl;

  if (o->GetNumberOfScales() > 1) {
    interval = this->procrustesInterval_[o->GetCurrentScale()];
    std::cout << "Interval = " << interval << std::endl;
  } else {
    // Use the default interval
    interval = this->procrustesRegistration_->GetProcrustesInterval();
  }

  // Check if the Procrustes interval is set to a value other than 0
  if (interval > 0) {
    this->procrustesCounter_++;

    // If the counter is greater than the interval value, run
    // Procrustes registration
    if (this->procrustesCounter_ >= interval) {
      // Reset the counter
      this->procrustesCounter_ = 0;
      this->procrustesRegistration_->RunRegistration();
      std::cout << "Ran Procrustes Registration" << std::endl;
    }
  }
}