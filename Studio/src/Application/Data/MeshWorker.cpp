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

  // compute the memory size of this shape
  size_t shape_size = this->item_.shape.size() * sizeof(double);
  size_t mesh_size = mesh->GetActualMemorySize() * 1024; // given in kb
  size_t combined_size = (shape_size * 2) + mesh_size;
  this->item_.memory_size = combined_size;

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
