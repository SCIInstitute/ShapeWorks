
#include <MeshWorker.h>

namespace shapeworks {

//---------------------------------------------------------------------------
MeshWorker::MeshWorker(MeshWorkQueue* queue, std::shared_ptr<MeshGenerator> generator)
  : queue_(queue), mesh_generator_(generator)
{
}

//---------------------------------------------------------------------------
MeshWorker::~MeshWorker()
{
}

//---------------------------------------------------------------------------
void MeshWorker::run()
{
  // build the mesh using our MeshGenerator
  auto item = this->queue_->get_next_work_item();

  MeshHandle mesh = this->mesh_generator_->build_mesh(*item);

  if (mesh->get_poly_data()) {
    // compute the memory size of this shape
    size_t shape_size = item->points.size() * sizeof(double);
    size_t mesh_size = mesh->get_poly_data()->GetActualMemorySize() * 1024; // given in kb
    size_t combined_size = (shape_size * 2) + mesh_size;
    item->memory_size = combined_size;
  }
  emit result_ready(*item, mesh);
  emit finished();
}

}
