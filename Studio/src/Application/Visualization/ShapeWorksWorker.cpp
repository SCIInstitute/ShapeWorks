#include <sstream>
#include <fstream>
#include <iostream>

#include <QProcess>
#include <QMessageBox>

#include <Visualization/ShapeWorksWorker.h>
#include <Libs/Optimize/Optimize.h>

ShapeworksWorker::ShapeworksWorker(ThreadType type,
                                   ShapeWorksGroom* groom,
                                   Optimize* optimize,
                                   QSharedPointer<Session> project,
                                   std::vector<std::vector<itk::Point<double>>> local_pts,
                                   std::vector<std::vector<itk::Point<double>>> global_pts,
                                   std::vector<ImageType::Pointer> distance_transform,
                                   double maxAngle,
                                   float decimationPercent,
                                   int numClusters) : type_(type), groom_(groom),
  optimize_(optimize), project_(project),
  local_pts_(local_pts), global_pts_(global_pts),
  distance_transform_(distance_transform),
  decimationPercent_(decimationPercent), maxAngle_(maxAngle),
  numClusters_(numClusters)
{}

ShapeworksWorker::~ShapeworksWorker() {}

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
    break;
  case ShapeworksWorker::OptimizeType:
    try {
      this->optimize_->Run();
    } catch (std::runtime_error e) {
      std::cerr << "Exception: " << e.what() << "\n";
      emit error_message(std::string("Error: ") + e.what());
      return;
    } catch (itk::ExceptionObject & ex) {
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
      this->project_->get_mesh_manager()->get_surface_reconstructor()->initializeReconstruction(
        this->local_pts_, this->global_pts_, this->distance_transform_,
        this->maxAngle_, this->decimationPercent_,
        this->numClusters_);
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
}
