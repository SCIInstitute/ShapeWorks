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
}

//---------------------------------------------------------------------------
MeshManager::~MeshManager() {}

//---------------------------------------------------------------------------
void MeshManager::clear_cache()
{
  this->mesh_cache_.clear();
}

//---------------------------------------------------------------------------
void MeshManager::shutdown_threads()
{
  std::cerr << "Shut Down MeshManager Threads";
/*
  for (size_t i = 0; i < this->threads_.size(); i++) {
    if (this->threads_[i]->isRunning()) {
      this->threads_[i]->quit();
      this->threads_[i]->terminate();
      std::cerr << "waiting...\n";
      //this->threads_[i]->wait(1000);
      std::cerr << "done waiting...\n";
    }
    //this->threads_[i]->exit();

    //std::cerr << "Terminate!\n";
    //this->threads_[i]->terminate();
    //this->threads_[i]->wait(1000);
    //  }
  }
  for (size_t i = 0; i < this->threads_.size(); i++) {
    //delete this->threads_[i];
  }
  */

}

//---------------------------------------------------------------------------
void MeshManager::generate_mesh(const MeshWorkItem item)
{
  // check cache first
  if (!this->mesh_cache_.get_mesh(item)
      && !this->work_queue_.isInside(item)) {
    this->work_queue_.push(item);
    //todo
    QThread* thread = new QThread;
    MeshWorker* worker = new MeshWorker(this->prefs_, item,
                                        &this->work_queue_, &this->mesh_cache_);
    worker->get_mesh_generator()->set_surface_reconstructor(this->surface_reconstructor_);


    worker->moveToThread(thread);
    connect(thread, SIGNAL(started()), worker, SLOT(process()));
    connect(worker, &MeshWorker::result_ready, this, &MeshManager::handle_thread_complete);
    connect(worker, SIGNAL(finished()), thread, SLOT(quit()));
    connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

    if (this->thread_count_ < this->prefs_.get_num_threads()) {
      thread->start();
      this->thread_count_++;
    }
    else {
      this->threads_.push(thread);
    }
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
void MeshManager::handle_thread_complete(const MeshWorkItem &item, vtkSmartPointer<vtkPolyData> mesh)
{
  this->work_queue_.remove(item);
  this->mesh_cache_.insert_mesh(item, mesh);

  this->thread_count_--;
  int max_threads = this->prefs_.get_num_threads();
  while (!this->threads_.empty() && this->thread_count_ < max_threads) {
    QThread* thread = this->threads_.front();
    std::cerr << "starting next thread\n";
    this->threads_.pop();
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
