// qt
#include <QThread>
#include <QApplication>

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

  qRegisterMetaType<MeshWorkItem>("MeshWorkItem");
  qRegisterMetaType<vtkSmartPointer<vtkPolyData>>("vtkSmartPointer<vtkPolyData>");

  this->thread_pool_.setMaxThreadCount(8);
}

//---------------------------------------------------------------------------
MeshManager::~MeshManager() {}

//---------------------------------------------------------------------------
void MeshManager::clear_cache()
{
  this->mesh_cache_.clear();
}

//---------------------------------------------------------------------------
void MeshManager::generate_mesh(const MeshWorkItem item)
{
  // check cache first
  if (!this->mesh_cache_.get_mesh(item)
      && !this->work_queue_.isInside(item)) {

    this->work_queue_.push(item);

    MeshWorker* worker = new MeshWorker(this->prefs_, item,
                                        &this->work_queue_, &this->mesh_cache_);
    worker->get_mesh_generator()->set_surface_reconstructor(this->surface_reconstructor_);

    connect(worker, &MeshWorker::result_ready, this, &MeshManager::handle_thread_complete);

    this->thread_pool_.start(worker);
  }
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshManager::get_mesh(const MeshWorkItem &item)
{
  vtkSmartPointer<vtkPolyData> poly_data;

  // check cache first
  if (this->prefs_.get_cache_enabled()) {
    poly_data = this->mesh_cache_.get_mesh(item);
    if (!poly_data) {
      if (prefs_.get_parallel_enabled() &&
          (this->prefs_.get_num_threads() > 0)) {
        this->generate_mesh(item);
      }
      else {
        poly_data = this->mesh_generator_.build_mesh(item);
        this->mesh_cache_.insert_mesh(item, poly_data);
      }
    }
  }
  else {
    poly_data = this->mesh_generator_.build_mesh(item);
  }
  return poly_data;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> MeshManager::get_mesh(const vnl_vector<double> &points)
{
  MeshWorkItem item;
  item.points = points;
  return this->get_mesh(item);
}

//---------------------------------------------------------------------------
void MeshManager::handle_thread_complete(const MeshWorkItem &item,
                                         vtkSmartPointer<vtkPolyData> mesh)
{
  this->mesh_cache_.insert_mesh(item, mesh);
  this->work_queue_.remove(item);

  emit new_mesh();
}

//---------------------------------------------------------------------------
QSharedPointer<SurfaceReconstructor> MeshManager::get_surface_reconstructor()
{
  return this->surface_reconstructor_;
}
