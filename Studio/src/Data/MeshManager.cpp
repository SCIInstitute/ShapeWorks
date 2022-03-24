// qt
#include <QThread>
#include <QApplication>

// vtk
#include <vtkPolyData.h>

#include <Data/MeshManager.h>

namespace shapeworks {

//---------------------------------------------------------------------------
MeshManager::MeshManager(Preferences& prefs) :
  prefs_(prefs),
  mesh_cache_(prefs)
{
  this->mesh_generator_->set_mesh_reconstructors(this->reconstructors_);

  qRegisterMetaType<MeshWorkItem>("MeshWorkItem");
  qRegisterMetaType<MeshHandle>("MeshHandle");

  this->thread_pool_.setMaxThreadCount(8);
}

//---------------------------------------------------------------------------
MeshManager::~MeshManager()
{}

//---------------------------------------------------------------------------
void MeshManager::clear_cache()
{
  this->mesh_cache_.clear();
}

//---------------------------------------------------------------------------
void MeshManager::generate_mesh(const MeshWorkItem item)
{
  // check cache first
  if (!this->mesh_cache_.get_mesh(item) && !this->work_queue_.is_inside(item)) {

    this->work_queue_.push(item);

    MeshWorker* worker = new MeshWorker(&this->work_queue_, this->mesh_generator_);

    connect(worker, &MeshWorker::result_ready, this, &MeshManager::handle_thread_complete);
    this->thread_pool_.start(worker);
  }
}

//---------------------------------------------------------------------------
MeshHandle MeshManager::get_mesh(const MeshWorkItem& item, bool wait)
{
  MeshHandle mesh;

  // check cache first
  if (this->prefs_.get_cache_enabled()) {
    mesh = this->mesh_cache_.get_mesh(item);
    if (!mesh) {
      if (!wait && this->prefs_.get_parallel_enabled() && this->prefs_.get_num_threads() > 0) {
        this->generate_mesh(item);
      }
      else {
        mesh = this->mesh_generator_->build_mesh(item);
        this->check_error_status(mesh);
        this->mesh_cache_.insert_mesh(item, mesh);
      }
    }
  }
  else {
    mesh = this->mesh_generator_->build_mesh(item);
  }
  return mesh;
}

//---------------------------------------------------------------------------
MeshHandle MeshManager::get_mesh(const Eigen::VectorXd& points, int domain)
{
  MeshWorkItem item;
  item.points = points;
  item.domain = domain;
  return this->get_mesh(item);
}

//---------------------------------------------------------------------------
void MeshManager::handle_thread_complete(const MeshWorkItem& item, MeshHandle mesh)
{
  this->mesh_cache_.insert_mesh(item, mesh);
  this->work_queue_.remove(item);

  this->check_error_status(mesh);

  emit new_mesh();
}

//---------------------------------------------------------------------------
void MeshManager::check_error_status(MeshHandle mesh)
{
  if (mesh->get_error_message() != "" && !this->error_emitted_) {
    this->error_emitted_ = true;
    std::string message = "Error during mesh construction:\n\n" + mesh->get_error_message()
                          + "\n\nFurther messages will be suppressed\n";
    emit error_encountered(QString::fromStdString(message));
  }
}

//---------------------------------------------------------------------------
std::shared_ptr<MeshWarper> MeshManager::get_mesh_warper(int domain)
{
  while (domain >= this->reconstructors_->mesh_warpers_.size()) {
    auto warper = std::make_shared<QMeshWarper>(this);
    connect(warper.get(), &QMeshWarper::progress, this, &MeshManager::handle_warper_progress);
    this->reconstructors_->mesh_warpers_.push_back(warper);
  }
  return this->reconstructors_->mesh_warpers_[domain];
}

//---------------------------------------------------------------------------
std::shared_ptr<SurfaceReconstructor> MeshManager::get_surface_reconstructor(int domain)
{
  while (domain >= this->reconstructors_->surface_reconstructors_.size()) {
    this->reconstructors_->surface_reconstructors_.push_back(
      std::make_shared<SurfaceReconstructor>());
  }
  return this->reconstructors_->surface_reconstructors_[domain];
}

//---------------------------------------------------------------------------
void MeshManager::handle_warper_progress()
{
  float sum = 0;
  int num_domains = this->reconstructors_->mesh_warpers_.size();
  for (int i = 0; i < num_domains; i++) {
    sum += this->reconstructors_->mesh_warpers_[i]->get_progress();
  }
  float p = sum / num_domains * 100.0;

  if (p < 100.0) {
    emit status("Generating Mesh Warp");
  }
  else {
    emit status("");
  }
  emit progress(p);
}
}