// vtk
#include <vtkPolyData.h>

#include <Data/MeshWorker.h>

//---------------------------------------------------------------------------
MeshWorker::MeshWorker(Preferences& prefs,
                       const vnl_vector<double> shape,
                       MeshWorkQueue* queue,
                       MeshCache* cache)
  : prefs_(prefs), meshGenerator_(prefs),
  shape_(shape), queue_(queue), cache_(cache) {}

//---------------------------------------------------------------------------
MeshWorker::~MeshWorker() {}

//---------------------------------------------------------------------------
void MeshWorker::process()
{
  // build the mesh using our MeshGenerator
  vtkSmartPointer<vtkPolyData> mesh = this->meshGenerator_.build_mesh(this->shape_);
  this->queue_->remove(this->shape_);
  this->cache_->insertMesh(this->shape_, mesh);
  emit result_ready();
  emit finished();
}

//---------------------------------------------------------------------------
MeshGenerator* MeshWorker::getMeshGenerator()
{
  return &(this->meshGenerator_);
}
