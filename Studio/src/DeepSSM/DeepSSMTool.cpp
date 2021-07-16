// std
#include <iostream>

// qt
#include <QThread>
#include <QFileDialog>
#include <QMessageBox>

// shapeworks
#include <DeepSSM/DeepSSMTool.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/Session.h>
#include <Data/StudioMesh.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <DeepSSM/DeepSSMParameters.h>
#include <Visualization/Lightbox.h>
#include <DeepSSM/QDeepSSM.h>

#include <ui_DeepSSMTool.h>

namespace shapeworks {

//---------------------------------------------------------------------------
DeepSSMTool::DeepSSMTool(Preferences& prefs) : preferences_(prefs)
{
  this->ui_ = new Ui_DeepSSMTool;
  this->ui_->setupUi(this);

  connect(this->ui_->run_augmentation_button, &QPushButton::clicked,
          this, &DeepSSMTool::run_augmentation_clicked);
}

//---------------------------------------------------------------------------
DeepSSMTool::~DeepSSMTool()
{}

//---------------------------------------------------------------------------
void DeepSSMTool::set_session(QSharedPointer<Session> session)
{
  this->session_ = session;
  this->load_params();
}

//---------------------------------------------------------------------------
void DeepSSMTool::set_app(ShapeWorksStudioApp* app)
{
  this->app_ = app;
}

//---------------------------------------------------------------------------
void DeepSSMTool::load_params()
{
  auto params = DeepSSMParameters(this->session_->get_project());

  this->ui_->num_samples->setText(QString::number(params.get_num_samples()));
  this->ui_->num_pca_dims->setText(QString::number(params.get_num_dims()));
  this->ui_->percent_variability->setText(QString::number(params.get_percent_variability()));

  this->ui_->sampler_type->setCurrentText(QString::fromStdString(params.get_sampler_type()));

}

//---------------------------------------------------------------------------
void DeepSSMTool::store_params()
{
  auto params = DeepSSMParameters(this->session_->get_project());

  params.set_num_samples(this->ui_->num_samples->text().toInt());
  params.set_num_dims(this->ui_->num_pca_dims->text().toInt());
  params.set_percent_variability(this->ui_->percent_variability->text().toDouble());
  params.set_sampler_type(this->ui_->sampler_type->currentText().toStdString());

  params.save_to_project();
}

//---------------------------------------------------------------------------
void DeepSSMTool::shutdown()
{
}

//---------------------------------------------------------------------------
void DeepSSMTool::set_active(bool active)
{
}

//---------------------------------------------------------------------------
bool DeepSSMTool::get_active()
{
  return false;
}

//---------------------------------------------------------------------------
void DeepSSMTool::run_augmentation_clicked()
{
  emit message("Please wait: running augmentation step...");
  emit progress(-1);
  this->timer_.start();

  this->deep_ssm_ = QSharedPointer<QDeepSSM>::create(session_->get_project());

  ShapeworksWorker* worker = new ShapeworksWorker(
    ShapeworksWorker::DeepSSM_AugmentationType, nullptr, nullptr,
    nullptr, session_);
  worker->set_deep_ssm(this->deep_ssm_);

  QThread* thread = new QThread;
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, &ShapeworksWorker::finished, this, &DeepSSMTool::handle_thread_complete);
  connect(this->deep_ssm_.data(), &QDeepSSM::progress, this, &DeepSSMTool::handle_progress);
  connect(worker, &ShapeworksWorker::error_message, this, &DeepSSMTool::handle_error);
  connect(worker, &ShapeworksWorker::message, this, &DeepSSMTool::message);
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();

}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_thread_complete()
{
  emit progress(95);
  QString duration = QString::number(this->timer_.elapsed() / 1000.0, 'f', 1);
  emit message("Data Augmentation Complete.  Duration: " + duration + " seconds");
}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_progress(int val)
{

}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_error(QString msg)
{

}

}
