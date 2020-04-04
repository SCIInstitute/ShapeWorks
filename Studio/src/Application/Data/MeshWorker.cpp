// vtk
#include <vtkPolyData.h>

#include <Data/MeshWorker.h>

//---------------------------------------------------------------------------
MeshWorker::MeshWorker(Preferences& prefs,
                       const vnl_vector<double> shape,
                       MeshWorkQueue* queue,
                       MeshCache* cache)
  : prefs_(prefs), mesh_generator_(prefs),
  shape_(shape), queue_(queue), cache_(cache) {}

//---------------------------------------------------------------------------
MeshWorker::~MeshWorker() {}

//---------------------------------------------------------------------------
void MeshWorker::process()
{
  // build the mesh using our MeshGenerator
  vtkSmartPointer<vtkPolyData> mesh = this->mesh_generator_.build_mesh(this->shape_, 0);
  this->queue_->remove(this->shape_);
  this->cache_->insert_mesh(this->shape_, mesh);
  emit result_ready();
  emit finished();
}

//---------------------------------------------------------------------------
MeshGenerator* MeshWorker::get_mesh_generator()
{
  return &(this->mesh_generator_);
}
