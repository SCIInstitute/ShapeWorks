#include <sstream>
#include <fstream>
#include <iostream>

#include <QProcess>
#include <QMessageBox>

#include <Visualization/ShapeWorksWorker.h>
#include <Libs/Optimize/Optimize.h>
#include <Application/Groom/QGroom.h>
#include <Libs/Optimize/OptimizeParameters.h>

namespace shapeworks {

ShapeworksWorker::ShapeworksWorker(ThreadType type,
                                   QSharedPointer<QGroom> groom,
                                   QSharedPointer<Optimize> optimize,
                                   QSharedPointer<OptimizeParameters> optimize_parameters,
                                   QSharedPointer<Session> session,
                                   std::vector<std::vector<itk::Point<double>>> local_pts,
                                   std::vector<std::vector<itk::Point<double>>> global_pts,
                                   std::vector<std::string> distance_transform,
                                   double maxAngle,
                                   float decimationPercent,
                                   int numClusters) : type_(type), groom_(groom),
                                                      optimize_(optimize),
                                                      optimize_parameters_(optimize_parameters),
                                                      session_(session),
                                                      local_pts_(local_pts),
                                                      global_pts_(global_pts),
                                                      distance_transform_(distance_transform),
                                                      decimation_percent_(decimationPercent),
                                                      max_angle_(maxAngle),
                                                      num_clusters_(numClusters)
{}

ShapeworksWorker::~ShapeworksWorker()
{}

void ShapeworksWorker::process()
{
  switch (this->type_) {
    case ShapeworksWorker::GroomType:
      try {
        this->groom_->run();
      } catch (std::runtime_error e) {
        emit error_message(std::string("Error: ") + e.what());
        return;
      } catch (std::exception e) {
        emit error_message(std::string("Error: ") + e.what());
        return;
      } catch (...) {
        emit error_message(std::string("Error during optimization!"));
        return;
      }
      if (this->groom_->get_aborted()) {
        emit error_message(std::string("Groom Aborted!"));
        return;
      }
      break;
    case ShapeworksWorker::OptimizeType:
      try {
        emit message(std::string("Loading data..."));
        this->optimize_parameters_->set_up_optimize(this->optimize_.data());
        emit message(std::string("Optimizing correspondence..."));
        this->optimize_->Run();
      } catch (std::runtime_error e) {
        std::cerr << "Exception: " << e.what() << "\n";
        emit error_message(std::string("Error: ") + e.what());
        return;
      } catch (itk::ExceptionObject& ex) {
        std::cerr << "ITK Exception: " << ex << std::endl;
        emit error_message(std::string("ITK Exception: ") + ex.GetDescription());
        return;
      } catch (std::exception e) {
        emit error_message(std::string("Error: ") + e.what());
        return;
      } catch (...) {
        emit error_message(std::string("Error during optimization!"));
        return;
      }
      if (this->optimize_->GetAborted()) {
        emit error_message(std::string("Optimization Aborted!"));
        return;
      }

      break;
    case ShapeworksWorker::ReconstructType:
      try {
        emit message(std::string("Warping optimizations to mean space..."));
        this->session_->get_mesh_manager()->get_surface_reconstructor()->initializeReconstruction(
          this->local_pts_, this->global_pts_, this->distance_transform_,
          this->max_angle_, this->decimation_percent_,
          this->num_clusters_);
      } catch (std::runtime_error e) {
        if (std::string(e.what()).find_first_of("Warning") != std::string::npos) {
          emit warning_message(e.what());
        }
        else {
          emit error_message(std::string("Error: ") + e.what());
          return;
        }
      } catch (std::exception e) {
        if (std::string(e.what()).find_first_of("Warning") != std::string::npos) {
          emit warning_message(e.what());
        }
        else {
          emit error_message(std::string("Error: ") + e.what());
          return;
        }
      } catch (...) {
        emit error_message(std::string("Error during optimization!"));
        return;
      }
      break;
  }
  emit result_ready();
  emit finished();
}
}