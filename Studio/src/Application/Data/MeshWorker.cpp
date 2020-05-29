// vtk
#include <vtkPolyData.h>

#include <Data/MeshWorker.h>
#include <QThread>

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
void MeshWorker::run()
{
  // build the mesh using our MeshGenerator
  auto item = this->queue_->get_next_work_item();

  vtkSmartPointer<vtkPolyData> mesh = this->mesh_generator_.build_mesh(*item);

  // compute the memory size of this shape
  size_t shape_size = item->points.size() * sizeof(double);
  size_t mesh_size = mesh->GetActualMemorySize() * 1024; // given in kb
  size_t combined_size = (shape_size * 2) + mesh_size;
  item->memory_size = combined_size;

  emit result_ready(*item, mesh);
  emit finished();
}

//---------------------------------------------------------------------------
MeshGenerator* MeshWorker::get_mesh_generator()
{
  return &(this->mesh_generator_);
}
