/*
 * Shapeworks license
 */

 // qt
#include <QThread>

// vtk
#include <vtkPolyData.h>

#include <Data/MeshManager.h>

MeshManager::MeshManager(Preferences& prefs) : prefs_(prefs),
meshCache_(prefs), meshGenerator_(prefs, this->construct_) {
  this->thread_count_ = 0;
}

MeshManager::~MeshManager() {}

void MeshManager::clear_cache() {
  this->meshCache_.clear();
}

void MeshManager::generateMesh(const vnl_vector<double>& shape)
{
  // check cache first
  if (!this->meshCache_.getMesh(shape)
    && !this->workQueue_.isInside(shape))
  {
    this->workQueue_.push(shape);
    //todo
    QThread *thread = new QThread;
    MeshWorker *worker = new MeshWorker(this->prefs_, shape, 
      this->construct_,
      &this->workQueue_, &this->meshCache_);
    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(result_ready()), this, SLOT(handle_thread_complete()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    size_t tmp_max = this->prefs_.get_preference("num_threads", 100);
    if (this->thread_count_ < tmp_max) {
      thread->start();
      this->thread_count_++;
    } else
      threads_.push_back(thread);
  }
}

vtkSmartPointer<vtkPolyData> MeshManager::getMesh(const vnl_vector<double>& shape)
{
  vtkSmartPointer<vtkPolyData> polyData;
  // check cache first
  if (this->prefs_.get_preference("cache_enabled", true)) {
    polyData = this->meshCache_.getMesh(shape);
    if (!polyData) {
      if (prefs_.get_preference("parallel_enabled", true) &&
        (this->prefs_.get_preference("num_threads", 100) > 0)) {
        this->generateMesh(shape);
      } else {
        polyData = this->meshGenerator_.buildMesh(shape);
        this->meshCache_.insertMesh(shape, polyData);
      }
    }
  } else {
    polyData = this->meshGenerator_.buildMesh(shape);
  }
  return polyData;
}

void MeshManager::handle_thread_complete() {
  this->thread_count_--;
  size_t tmp_max = this->prefs_.get_preference("num_threads", 100);
  while (!this->threads_.empty() && this->thread_count_ < tmp_max) {
    QThread *thread = this->threads_.back();
    this->threads_.pop_back();
    thread->start();
    this->thread_count_++;
  }
  emit new_mesh();
}


void MeshManager::initializeReconstruction(
  std::vector<std::vector<itk::Point<float> > > local_pts,
  std::vector<std::vector<itk::Point<float> > > global_pts,
  std::vector<ImageType::Pointer> distance_transform) {
  //turn the sets of global points to one sparse global mean.
  float init[] = { 0.f,0.f,0.f };
  std::vector<itk::Point<float> > sparseMean =
    std::vector<itk::Point<float> >(global_pts[0].size(), itk::Point<float>(init));
  for (auto &a : global_pts) {
    for (size_t i = 0; i < a.size(); i++) {
      init[0] = a[i][0]; init[1] = a[i][1]; init[2] = a[i][2];
      itk::Vector<float> vec(init);
      sparseMean[i] = sparseMean[i] + vec;
    }
  }
  auto div = static_cast<float>(global_pts.size());
  for (size_t i = 0; i < sparseMean.size(); i++) {
    init[0] = sparseMean[i][0] / div;
    init[1] = sparseMean[i][1] / div;
    init[2] = sparseMean[i][2] / div;
    sparseMean[i] = itk::Point<float>(init);
  }
  //now actually generate the dense mean.
  this->construct_.getDenseMean(local_pts, sparseMean, distance_transform);
}

bool MeshManager::hasDenseMean() {
  return this->construct_.denseDone();
}

void MeshManager::writeMeanInfo(std::string baseName) {
  this->construct_.writeMeanInfo(baseName);
}

void MeshManager::readMeanInfo(std::string dense, 
  std::string sparse, std::string goodPoints) {
  this->construct_.readMeanInfo(dense, sparse, goodPoints);
}
