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
}

//---------------------------------------------------------------------------
void DeepSSMTool::set_app(ShapeWorksStudioApp* app)
{
  this->app_ = app;
}

//---------------------------------------------------------------------------
void DeepSSMTool::load_settings()
{
  Parameters params = this->session_->get_project()->get_parameters(Parameters::DEEPSSM_PARAMS);

  this->ui_->num_samples->setText(QString::fromStdString(params.get("num_samples", 3)));
  this->ui_->num_pca_dims->setText(QString::fromStdString(params.get("num_pca_dims", 3)));
  this->ui_->percent_variability->setText(QString::fromStdString(params.get("percent_variability", 60)));

  this->ui_->sampler_type->setCurrentText(
    QString::fromStdString(params.get("sampler_type", "Gaussian")));

}

//---------------------------------------------------------------------------
void DeepSSMTool::store_settings()
{
  Parameters params = this->session_->get_project()->get_parameters(Parameters::DEEPSSM_PARAMS);
  params.set("num_samples", this->ui_->num_samples->text().toStdString());
  params.set("num_pca_dims", this->ui_->num_pca_dims->text().toStdString());
  params.set("percent_variability", this->ui_->percent_variability->text().toStdString());
  params.set("sampler_type", this->ui_->sampler_type->currentText().toStdString());
  this->session_->get_project()->set_parameters(Parameters::ANALYSIS_PARAMS, params);
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
