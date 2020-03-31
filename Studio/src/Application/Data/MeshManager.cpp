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
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    size_t tmp_max = this->prefs_.get_num_threads();
    if (this->thread_count_ < tmp_max) {
      thread->start();
      this->thread_count_++;
    }
    else {
      this->threads_.push_back(thread);
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
  if (this->prefs_.get_cache_enabled()) {
    polyData = this->meshCache_.getMesh(shape);
    if (!polyData) {
      if (prefs_.get_parallel_enabled() &&
          (this->prefs_.get_num_threads() > 0)) {
        this->generateMesh(shape);
      }
      else {
        polyData = this->meshGenerator_.build_mesh(shape);
        this->meshCache_.insertMesh(shape, polyData);
      }
    }
  }
  else {
    polyData = this->meshGenerator_.build_mesh(shape);
  }
  return polyData;
}

//---------------------------------------------------------------------------
void MeshManager::handle_thread_complete()
{
  this->thread_count_--;
  size_t tmp_max = this->prefs_.get_num_threads();
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
