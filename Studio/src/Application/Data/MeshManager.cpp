// qt
#include <QThread>

// vtk
#include <vtkPolyData.h>

#include <Data/MeshManager.h>

//---------------------------------------------------------------------------
MeshManager::MeshManager(Preferences& prefs) :
  prefs_(prefs),
  mesh_cache_(prefs),
  mesh_generator_(prefs),
  surface_reconstructor_(new SurfaceReconstructor())
{
  this->thread_count_ = 0;

  this->mesh_generator_.set_surface_reconstructor(this->surface_reconstructor_);
}

//---------------------------------------------------------------------------
MeshManager::~MeshManager() {}

//---------------------------------------------------------------------------
void MeshManager::clear_cache()
{
  this->mesh_cache_.clear();
}

//---------------------------------------------------------------------------
void MeshManager::generate_mesh(const vnl_vector<double>& shape)
{
  // check cache first
  if (!this->mesh_cache_.getMesh(shape)
      && !this->work_queue_.isInside(shape)) {
    this->work_queue_.push(shape);
    //todo
    QThread* thread = new QThread;
    MeshWorker* worker = new MeshWorker(this->prefs_, shape,
                                        &this->work_queue_, &this->mesh_cache_);
    worker->getMeshGenerator()->set_surface_reconstructor(this->surface_reconstructor_);

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
vtkSmartPointer<vtkPolyData> MeshManager::get_mesh(const vnl_vector<double>& shape)
{
  vtkSmartPointer<vtkPolyData> poly_data;
  if (shape.empty()) {
    return poly_data;
  }
  // check cache first
  if (this->prefs_.get_cache_enabled()) {
    poly_data = this->mesh_cache_.getMesh(shape);
    if (!poly_data) {
      if (prefs_.get_parallel_enabled() &&
          (this->prefs_.get_num_threads() > 0)) {
        this->generate_mesh(shape);
      }
      else {
        poly_data = this->mesh_generator_.build_mesh(shape, 0);
        this->mesh_cache_.insertMesh(shape, poly_data);
      }
    }
  }
  else {
    poly_data = this->mesh_generator_.build_mesh(shape, 0);
  }
  return poly_data;
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
QSharedPointer<SurfaceReconstructor> MeshManager::get_surface_reconstructor()
{
  return this->surface_reconstructor_;
}
