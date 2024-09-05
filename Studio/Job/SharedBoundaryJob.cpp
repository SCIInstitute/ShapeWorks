
#include "SharedBoundaryJob.h"

#include <Data/Session.h>
#include <Logging.h>
#include <Mesh/MeshUtils.h>

namespace shapeworks {

//---------------------------------------------------------------------------
SharedBoundaryJob::SharedBoundaryJob(QSharedPointer<Session> session, int first_domain, int second_domain)
    : session_(std::move(session)), first_domain_(first_domain), second_domain_(second_domain) {}

//---------------------------------------------------------------------------
void SharedBoundaryJob::run() {
  SW_DEBUG("Running shared boundary job");
  Q_EMIT progress(0);

  auto shapes = session_->get_shapes();

  for (int i = 0; i < shapes.size(); i++) {
    SW_LOG("Generating Shared Boundary for Shape " + std::to_string(i));
    SW_PROGRESS(i / session_->get_num_shapes(), "Generating Shared Boundary");
    auto shape = shapes[i];
    auto mesh_group = shape->get_original_meshes(true);
    Q_ASSERT(first_domain_ < mesh_group.meshes().size());
    Q_ASSERT(second_domain_ < mesh_group.meshes().size());

    Mesh first_domain_mesh = mesh_group.meshes()[first_domain_]->get_sw_mesh();
    Mesh second_domain_mesh = mesh_group.meshes()[second_domain_]->get_sw_mesh();

    auto [extracted_l, extracted_r, extracted_s] =
        MeshUtils::sharedBoundaryExtractor(first_domain_mesh, second_domain_mesh, 1e-1);

    auto output_contour = MeshUtils::boundaryLoopExtractor(extracted_s);
  }

  Q_EMIT progress(100);
}

}  // namespace shapeworks
