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
  qRegisterMetaType<MeshHandle>("MeshHandle");

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
  if (!this->mesh_cache_.get_mesh(item) && !this->work_queue_.isInside(item)) {

    this->work_queue_.push(item);

    MeshWorker* worker = new MeshWorker(this->prefs_, item,
                                        &this->work_queue_, &this->mesh_cache_);
    worker->get_mesh_generator()->set_surface_reconstructor(this->surface_reconstructor_);

    connect(worker, &MeshWorker::result_ready, this, &MeshManager::handle_thread_complete);

    this->thread_pool_.start(worker);
  }
}

//---------------------------------------------------------------------------
MeshHandle MeshManager::get_mesh(const MeshWorkItem &item)
{
  MeshHandle mesh;

  // check cache first
  if (this->prefs_.get_cache_enabled()) {
    mesh = this->mesh_cache_.get_mesh(item);
    if (!mesh) {
      if (prefs_.get_parallel_enabled() &&
          (this->prefs_.get_num_threads() > 0)) {
        this->generate_mesh(item);
      }
      else {
        mesh = this->mesh_generator_.build_mesh(item);
        this->mesh_cache_.insert_mesh(item, mesh);
      }
    }
  }
  else {
    mesh = this->mesh_generator_.build_mesh(item);
  }
  return mesh;
}

//---------------------------------------------------------------------------
MeshHandle MeshManager::get_mesh(const vnl_vector<double> &points)
{
  MeshWorkItem item;
  item.points = points;
  return this->get_mesh(item);
}

//---------------------------------------------------------------------------
void MeshManager::handle_thread_complete(const MeshWorkItem &item, MeshHandle mesh)
{
  if (mesh->get_error_message() != "") {
    std::string message = "Error during mesh construction:\n\n" + mesh->get_error_message()
                          + "\n\nFurther messagess will be suppressed\n";
    Q_EMIT error_encountered(message);
    this->error_emitted_ = true;
  }
  this->mesh_cache_.insert_mesh(item, mesh);
  this->work_queue_.remove(item);

  emit new_mesh();
}

//---------------------------------------------------------------------------
QSharedPointer<SurfaceReconstructor> MeshManager::get_surface_reconstructor()
{
  return this->surface_reconstructor_;
}
