#include <sstream>
#include <iostream>

#include <QProcess>
#include <QMessageBox>

#include <Groom/QGroom.h>
#include <Data/Shape.h>
#include <Data/ShapeWorksWorker.h>
#include <DeepSSM/QDeepSSM.h>

#include <Libs/Optimize/Optimize.h>
#include <Libs/Optimize/OptimizeParameters.h>

namespace shapeworks {

//---------------------------------------------------------------------------
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

//---------------------------------------------------------------------------
ShapeworksWorker::~ShapeworksWorker()
{}

//---------------------------------------------------------------------------
void ShapeworksWorker::process()
{
  switch (this->type_) {
    case ShapeworksWorker::GroomType:
      try {
        this->groom_->run();
      } catch (itk::ExceptionObject& ex) {
        std::cerr << "ITK Exception: " << ex << std::endl;
        emit error_message(QString("ITK Exception: ") + ex.GetDescription());
        return;
      } catch (std::runtime_error& e) {
        emit error_message(QString("Error: ") + e.what());
        return;
      } catch (std::exception& e) {
        emit error_message(QString("Error: ") + e.what());
        return;
      } catch (...) {
        emit error_message(QString("Error during grooming!"));
        return;
      }
      if (this->groom_->get_aborted()) {
        emit error_message(QString("Groom Aborted!"));
        return;
      }
      break;
    case ShapeworksWorker::OptimizeType:
      try {
        emit message("Loading data...");
        this->optimize_parameters_->set_up_optimize(this->optimize_.data());
        emit message("Optimizing correspondence...");
        this->optimize_->Run();
      } catch (std::runtime_error e) {
        std::cerr << "Exception: " << e.what() << "\n";
        emit error_message(QString("Error: ") + e.what());
        return;
      } catch (itk::ExceptionObject& ex) {
        std::cerr << "ITK Exception: " << ex << std::endl;
        emit error_message(QString("ITK Exception: ") + ex.GetDescription());
        return;
      } catch (std::exception& e) {
        emit error_message(QString("Error: ") + e.what());
        return;
      } catch (...) {
        emit error_message("Error during optimization!");
        return;
      }
      if (this->optimize_->GetAborted()) {
        emit message("Optimization Aborted!");
        return;
      }

      break;
    case ShapeworksWorker::ReconstructType:
      try {
        emit message("Warping to mean space...");
        for (int i = 0; i < this->session_->get_domains_per_shape(); i++) {

          auto shapes = this->session_->get_shapes();
          std::vector<std::string> distance_transforms;
          std::vector<std::vector<itk::Point<double>>> local, global;
          for (auto& s : shapes) {
            distance_transforms.push_back(s->get_groomed_filename_with_path(i).toStdString());
            auto particles = s->get_particles();
            local.push_back(particles.get_local_points(i));
            global.push_back(particles.get_world_points(i));
          }
          this->session_->get_mesh_manager()
            ->get_surface_reconstructor(i)->initializeReconstruction(
              local, global, distance_transforms,
              this->max_angle_, this->decimation_percent_,
              this->num_clusters_);
        }
      } catch (std::runtime_error e) {
        if (std::string(e.what()).find_first_of("Warning") != std::string::npos) {
          emit warning_message(e.what());
        }
        else {
          emit error_message(QString("Error: ") + e.what());
          return;
        }
      } catch (std::exception& e) {
        if (std::string(e.what()).find_first_of("Warning") != std::string::npos) {
          emit warning_message(e.what());
        }
        else {
          emit error_message(QString("Error: ") + e.what());
          return;
        }
      } catch (...) {
        emit error_message(QString("Error during optimization!"));
        return;
      }
      break;
    case ShapeworksWorker::DeepSSM_AugmentationType :
      this->run_deep_ssm_augmentation();
      break;
  }


  emit result_ready();
  emit finished();
}

//---------------------------------------------------------------------------
void ShapeworksWorker::set_deep_ssm(QSharedPointer<QDeepSSM> deep_ssm)
{
  this->deep_ssm_ = deep_ssm;
}

//---------------------------------------------------------------------------
void ShapeworksWorker::run_deep_ssm_augmentation()
{
  this->deep_ssm_->run();
}

}