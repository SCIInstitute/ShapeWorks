// qt
#include <QApplication>
#include <QThread>

// vtk
#include <MeshManager.h>
#include <QMeshWarper.h>
#include <Logging.h>
#include <vtkPolyData.h>

#include "SurfaceReconstructor.h"

namespace shapeworks {

//---------------------------------------------------------------------------
MeshManager::MeshManager() {
  mesh_generator_->set_mesh_reconstructors(reconstructors_);

  qRegisterMetaType<MeshWorkItem>("MeshWorkItem");
  qRegisterMetaType<MeshHandle>("MeshHandle");

  thread_pool_.setMaxThreadCount(8);
}

//---------------------------------------------------------------------------
MeshManager::~MeshManager() {}

//---------------------------------------------------------------------------
void MeshManager::set_cache_enabled(bool enabled) {
  cache_enabled_ = enabled;
  mesh_cache_.set_cache_enabled(enabled);
}

//---------------------------------------------------------------------------
void MeshManager::set_cache_memory_percent(int percent) { mesh_cache_.set_memory_percent(percent); }

//---------------------------------------------------------------------------
void MeshManager::clear_cache() { mesh_cache_.clear(); }

//---------------------------------------------------------------------------
void MeshManager::generate_mesh(const MeshWorkItem item) {
  // check cache first
  if (!mesh_cache_.get_mesh(item) && !work_queue_.is_inside(item)) {
    work_queue_.push(item);

    MeshWorker* worker = new MeshWorker(&work_queue_, mesh_generator_);

    connect(worker, &MeshWorker::result_ready, this, &MeshManager::handle_thread_complete);
    thread_pool_.start(worker);
  }
}

//---------------------------------------------------------------------------
MeshHandle MeshManager::get_mesh(const MeshWorkItem& item, bool wait) {
  MeshHandle mesh;

  // check cache first
  if (cache_enabled_) {
    mesh = mesh_cache_.get_mesh(item);
    if (!mesh) {
      if (!wait && parallel_enabled_ && num_threads_ > 0) {
        generate_mesh(item);
      } else {
        mesh = mesh_generator_->build_mesh(item);
        check_error_status(mesh);
        mesh_cache_.insert_mesh(item, mesh);
      }
    }
  } else {
    mesh = mesh_generator_->build_mesh(item);
  }
  return mesh;
}

//---------------------------------------------------------------------------
MeshHandle MeshManager::get_mesh(const Eigen::VectorXd& points, int domain) {
  MeshWorkItem item;
  item.points = points;
  item.domain = domain;
  return get_mesh(item);
}

//---------------------------------------------------------------------------
void MeshManager::handle_thread_complete(const MeshWorkItem& item, MeshHandle mesh) {
  mesh_cache_.insert_mesh(item, mesh);
  work_queue_.remove(item);

  check_error_status(mesh);

  emit new_mesh();
}

//---------------------------------------------------------------------------
void MeshManager::check_error_status(MeshHandle mesh) {
  if (mesh->get_error_message() != "" && !error_emitted_) {
    error_emitted_ = true;
    std::string message =
        "Error during mesh construction:\n\n" + mesh->get_error_message() + "\n\nFurther messages will be suppressed\n";
    SW_LOG_ERROR(message);
  }
}

//---------------------------------------------------------------------------
std::shared_ptr<MeshWarper> MeshManager::get_mesh_warper(int domain) {
  while (domain >= reconstructors_->mesh_warpers_.size()) {
    auto warper = std::make_shared<QMeshWarper>(this);
    connect(warper.get(), &QMeshWarper::progress, this, &MeshManager::handle_warper_progress);
    reconstructors_->mesh_warpers_.push_back(warper);
  }
  return reconstructors_->mesh_warpers_[domain];
}

//---------------------------------------------------------------------------
std::shared_ptr<SurfaceReconstructor> MeshManager::get_surface_reconstructor(int domain) {
  while (domain >= reconstructors_->surface_reconstructors_.size()) {
    reconstructors_->surface_reconstructors_.push_back(std::make_shared<SurfaceReconstructor>());
  }
  return reconstructors_->surface_reconstructors_[domain];
}

//---------------------------------------------------------------------------
void MeshManager::handle_warper_progress() {
  float sum = 0;
  int num_domains = reconstructors_->mesh_warpers_.size();
  for (int i = 0; i < num_domains; i++) {
    sum += reconstructors_->mesh_warpers_[i]->get_progress();
  }
  float p = sum / num_domains * 100.0;

  if (p < 100.0) {
    emit status("Generating Mesh Warp");
  } else {
    emit status("");
  }
  emit progress(p);
}
}  // namespace shapeworks
