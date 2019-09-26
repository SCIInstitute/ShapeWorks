/*
 * Shapeworks license
 */

// qt
#include <QThread>

// vtk
#include <vtkPolyData.h>

#include <Data/MeshManager.h>

//---------------------------------------------------------------------------
MeshManager::MeshManager(Preferences& prefs) :
  prefs_(prefs),
  meshCache_(prefs),
  meshGenerator_(prefs),
  surfaceReconstructor_(new SurfaceReconstructor())
{
  this->thread_count_ = 0;

  this->meshGenerator_.set_surface_reconstructor(this->surfaceReconstructor_);
}

//---------------------------------------------------------------------------
MeshManager::~MeshManager() {}

//---------------------------------------------------------------------------
void MeshManager::clear_cache()
{
  this->meshCache_.clear();
}

//---------------------------------------------------------------------------
void MeshManager::generateMesh(const vnl_vector<double>& shape)
{
  // check cache first
  if (!this->meshCache_.getMesh(shape)
      && !this->workQueue_.isInside(shape)) {
    this->workQueue_.push(shape);
    //todo
    QThread* thread = new QThread;
    MeshWorker* worker = new MeshWorker(this->prefs_, shape,
                                        &this->workQueue_, &this->meshCache_);
    worker->getMeshGenerator()->set_surface_reconstructor(this->surfaceReconstructor_);

    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, SIGNAL(result_ready()), this, SLOT(handle_thread_complete()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    size_t tmp_max = this->prefs_.get_preference("num_threads", 100);
    if (this->thread_count_ < tmp_max) {
      thread->start();
      this->thread_count_++;
    }
    else {
      threads_.push_back(thread);
    }
  }
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshManager::getMesh(const vnl_vector<double>& shape)
{
  vtkSmartPointer<vtkPolyData> polyData;
  if (shape.empty()) {
    return polyData;
  }
  // check cache first
  if (this->prefs_.get_preference("cache_enabled", true)) {
    polyData = this->meshCache_.getMesh(shape);
    if (!polyData) {
      if (prefs_.get_preference("parallel_enabled", true) &&
          (this->prefs_.get_preference("num_threads", 100) > 0)) {
        this->generateMesh(shape);
      }
      else {
        polyData = this->meshGenerator_.buildMesh(shape);
        this->meshCache_.insertMesh(shape, polyData);
      }
    }
  }
  else {
    polyData = this->meshGenerator_.buildMesh(shape);
  }
  return polyData;
}

//---------------------------------------------------------------------------
void MeshManager::handle_thread_complete()
{
  this->thread_count_--;
  size_t tmp_max = this->prefs_.get_preference("num_threads", 100);
  while (!this->threads_.empty() && this->thread_count_ < tmp_max) {
    QThread* thread = this->threads_.back();
    this->threads_.pop_back();
    thread->start();
    this->thread_count_++;
  }
  emit new_mesh();
}

//---------------------------------------------------------------------------
QSharedPointer<SurfaceReconstructor> MeshManager::getSurfaceReconstructor()
{
  return this->surfaceReconstructor_;
}

/*
   //---------------------------------------------------------------------------
   void MeshManager::initializeReconstruction(
   std::vector<std::vector<itk::Point<double>>> local_pts,
   std::vector<std::vector<itk::Point<double>>> global_pts,
   std::vector<ImageType::Pointer> distance_transform,
   double maxAngleDegrees,
   float decimationPercent,
   int numClusters)
   {
   //now actually generate the dense mean.
   this->construct_.reset();
   this->construct_.setDecimation(decimationPercent);
   this->construct_.setMaxAngle(maxAngleDegrees);
   this->construct_.setNumClusters(numClusters);
   this->construct_.getDenseMean(local_pts, global_pts, distance_transform);
   }

   bool MeshManager::hasDenseMean()
   {
   return this->construct_.denseDone();
   }

   void MeshManager::writeMeanInfo(std::string baseName)
   {
   this->construct_.writeMeanInfo(baseName);
   }

   void MeshManager::readMeanInfo(std::string dense,
                               std::string sparse, std::string goodPoints)
   {
   this->construct_.readMeanInfo(dense, sparse, goodPoints);
   }
 */
