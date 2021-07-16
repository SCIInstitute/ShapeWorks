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

#include <ui_DeepSSMTool.h>

namespace shapeworks {

//---------------------------------------------------------------------------
DeepSSMTool::DeepSSMTool(Preferences& prefs) : preferences_(prefs)
{
  this->ui_ = new Ui_DeepSSMTool;
  this->ui_->setupUi(this);
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

}
