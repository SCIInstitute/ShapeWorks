
#include <Data/ShapeWorksWorker.h>
#include <Groom/Groom.h>
#include <Logging.h>
#include <Optimize/Optimize.h>
#include <Optimize/OptimizeParameters.h>
#include <Shape.h>
#include <SurfaceReconstructor.h>

namespace shapeworks {

//---------------------------------------------------------------------------
ShapeworksWorker::ShapeworksWorker(ThreadType type, QSharedPointer<Groom> groom, QSharedPointer<Optimize> optimize,
                                   QSharedPointer<OptimizeParameters> optimize_parameters,
                                   QSharedPointer<Session> session, double maxAngle, float decimationPercent,
                                   int numClusters)
    : type_(type),
      groom_(groom),
      optimize_(optimize),
      optimize_parameters_(optimize_parameters),
      session_(session),
      decimation_percent_(decimationPercent),
      max_angle_(maxAngle),
      num_clusters_(numClusters) {}

//---------------------------------------------------------------------------
ShapeworksWorker::~ShapeworksWorker() {}

//---------------------------------------------------------------------------
void ShapeworksWorker::process() {
  switch (this->type_) {
    case ShapeworksWorker::GroomType:
      try {
        this->groom_->run();
      } catch (itk::ExceptionObject& ex) {
        SW_ERROR(std::string("ITK Exception: ") + ex.GetDescription());
        return;
      } catch (std::runtime_error& e) {
        SW_ERROR(e.what());
        return;
      } catch (std::exception& e) {
        SW_ERROR(e.what());
        return;
      }
      if (this->groom_->get_aborted()) {
        SW_LOG("Groom Aborted!");
        return;
      }
      break;
    case ShapeworksWorker::OptimizeType:
      try {
        SW_LOG("Loading data...");
        this->optimize_parameters_->set_up_optimize(this->optimize_.data());
        SW_LOG("Optimizing correspondence...");
        this->optimize_->Run();
      } catch (std::runtime_error e) {
        SW_ERROR(e.what());
        Q_EMIT failure();
        Q_EMIT finished();
        return;
      } catch (itk::ExceptionObject& ex) {
        SW_ERROR(std::string("ITK Exception: ") + ex.GetDescription());
        Q_EMIT failure();
        Q_EMIT finished();
        return;
      } catch (std::exception& e) {
        SW_ERROR(e.what());
        Q_EMIT failure();
        Q_EMIT finished();
        return;
      } catch (...) {
        SW_ERROR("Error during optimization!");
        Q_EMIT failure();
        Q_EMIT finished();
        return;
      }
      if (this->optimize_->GetAborted()) {
        SW_LOG("Optimization Aborted!");
        Q_EMIT failure();
        return;
      }

      break;
    case ShapeworksWorker::ReconstructType:
      try {
        SW_LOG("Warping to mean space...");
        for (int i = 0; i < this->session_->get_domains_per_shape(); i++) {
          auto shapes = this->session_->get_shapes();
          std::vector<std::string> distance_transforms;
          std::vector<std::vector<itk::Point<double>>> local, global;
          for (auto& s : shapes) {
            distance_transforms.push_back(s->get_groomed_filename_with_path(i));
            auto particles = s->get_particles();
            local.push_back(particles.get_local_points(i));
            global.push_back(particles.get_world_points(i));
          }
          this->session_->get_mesh_manager()->get_surface_reconstructor(i)->initializeReconstruction(
              local, global, distance_transforms, this->max_angle_, this->decimation_percent_, this->num_clusters_);
        }
      } catch (std::runtime_error e) {
        if (std::string(e.what()).find_first_of("Warning") != std::string::npos) {
          SW_WARN(e.what());
        } else {
          SW_ERROR(e.what());
          Q_EMIT finished();
          return;
        }
      } catch (std::exception& e) {
        if (std::string(e.what()).find_first_of("Warning") != std::string::npos) {
          SW_WARN(e.what());
        } else {
          SW_ERROR(e.what());
          Q_EMIT finished();
          return;
        }
      } catch (...) {
        SW_ERROR("Error during optimization!");
        Q_EMIT finished();
        return;
      }
      break;
  }

  Q_EMIT result_ready();
  Q_EMIT finished();
}

}  // namespace shapeworks
