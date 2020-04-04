// vtk
#include <vtkPolyData.h>

#include <Data/MeshWorker.h>

//---------------------------------------------------------------------------
MeshWorker::MeshWorker(Preferences& prefs,
                       const MeshWorkItem &item,
                       MeshWorkQueue* queue,
                       MeshCache* cache)
  : prefs_(prefs), mesh_generator_(prefs),
  item_(item), queue_(queue), cache_(cache) {}

//---------------------------------------------------------------------------
MeshWorker::~MeshWorker() {}

//---------------------------------------------------------------------------
void MeshWorker::process()
{
  std::cerr << "mesh worker here, working on item\n";
  // build the mesh using our MeshGenerator
  vtkSmartPointer<vtkPolyData> mesh = this->mesh_generator_.build_mesh(this->item_);
  this->queue_->remove(this->item_);
  this->cache_->insert_mesh(this->item_, mesh);
  emit result_ready();
  emit finished();
}

//---------------------------------------------------------------------------
MeshGenerator* MeshWorker::get_mesh_generator()
{
  return &(this->mesh_generator_);
}
