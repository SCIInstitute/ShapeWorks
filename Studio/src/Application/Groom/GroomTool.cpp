#include <iostream>

#include <QMessageBox>
#include <QThread>

#include <Data/Session.h>
#include <Data/StudioMesh.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <Groom/GroomTool.h>
#include <Visualization/ShapeWorksWorker.h>

#include <Libs/Groom/GroomParameters.h>

#include <ui_GroomTool.h>

//---------------------------------------------------------------------------
GroomTool::GroomTool()
{
  this->ui_ = new Ui_GroomTool;
  this->ui_->setupUi(this);
  qRegisterMetaType<std::string>();
}

//---------------------------------------------------------------------------
GroomTool::~GroomTool()
{}

//---------------------------------------------------------------------------
void GroomTool::on_antialias_checkbox_stateChanged(int state)
{
  this->ui_->antialias_groupbox->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::on_blur_checkbox_stateChanged(int state)
{
  this->ui_->blur_groupbox->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::on_autopad_checkbox_stateChanged(int state)
{
  this->ui_->pad_groupbox->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::handle_error(std::string msg)
{
  emit error_message(msg);
  this->ui_->run_groom_button->setEnabled(true);
  this->ui_->skipButton->setEnabled(true);
}

//---------------------------------------------------------------------------
void GroomTool::handle_progress(int val)
{
  emit progress(static_cast<size_t>(val));
}

//---------------------------------------------------------------------------
void GroomTool::on_restoreDefaults_clicked()
{
  // store a set of blank settings
  Parameters params;
  this->session_->get_project()->set_parameters(Parameters::GROOM_PARAMS, params);
  // now load those settings
  this->load_settings();
}

//---------------------------------------------------------------------------
void GroomTool::load_settings()
{
  auto params = GroomParameters(this->session_->get_project());



  this->ui_->center_checkbox->setChecked(params.get_center_tool());
  this->ui_->antialias_checkbox->setChecked(params.get_antialias_tool());
  this->ui_->autopad_checkbox->setChecked(params.get_auto_pad_tool());
  this->ui_->fastmarching_checkbox->setChecked(params.get_fast_marching());
  this->ui_->blur_checkbox->setChecked(params.get_blur_tool());
  this->ui_->isolate_checkbox->setChecked(params.get_isolate_tool());
  this->ui_->fill_holes_checkbox->setChecked(params.get_fill_holes_tool());
  this->ui_->antialias_iterations->setValue(params.get_antialias_iterations());
  this->ui_->blur_sigma->setValue(params.get_blur_amount());
  this->ui_->padding_amount->setValue(params.get_padding_amount());
}

//---------------------------------------------------------------------------
void GroomTool::disable_actions()
{
  this->ui_->skipButton->setEnabled(false);
  this->ui_->run_groom_button->setEnabled(false);
}

//---------------------------------------------------------------------------
void GroomTool::enable_actions()
{
  this->ui_->skipButton->setEnabled(true);
  this->ui_->run_groom_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void GroomTool::store_settings()
{
  auto params = GroomParameters(this->session_->get_project());

  params.set_center_tool(this->ui_->center_checkbox->isChecked());
  params.set_antialias_tool(this->ui_->antialias_checkbox->isChecked());
  params.set_auto_pad_tool(this->ui_->autopad_checkbox->isChecked());
  params.set_padding_amount(this->ui_->padding_amount->value());
  params.set_fast_marching(this->ui_->fastmarching_checkbox->isChecked());
  params.set_blur_tool(this->ui_->blur_checkbox->isChecked());
  params.set_blur_amount(this->ui_->blur_sigma->value());
  params.set_isolate_tool(this->ui_->isolate_checkbox->isChecked());
  params.set_fill_holes_tool(this->ui_->fill_holes_checkbox->isChecked());
  params.set_antialias_iterations(this->ui_->antialias_iterations->value());

  params.save_to_project();
}

//---------------------------------------------------------------------------
void GroomTool::on_run_groom_button_clicked()
{
  this->timer_.start();

  this->store_settings();
  std::cerr << "################## on run groom\n";
  emit message("Please wait: running groom step...");
  emit progress(0);

  this->groom_ = QSharedPointer<QGroom>(new QGroom(this->session_->get_project()));

  this->ui_->run_groom_button->setEnabled(false);
  this->ui_->skipButton->setEnabled(false);
  QThread* thread = new QThread;
  ShapeworksWorker* worker = new ShapeworksWorker(
    ShapeworksWorker::GroomType, this->groom_, nullptr, this->session_);
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_thread_complete()));
  connect(this->groom_.data(), &QGroom::progress, this, &GroomTool::handle_progress);
  connect(worker, SIGNAL(error_message(std::string)), this, SLOT(handle_error(std::string)));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();
}

//---------------------------------------------------------------------------
void GroomTool::handle_thread_complete()
{
  emit progress(95);

  // trigger reload of project
  auto duration = this->timer_.elapsed();
  STUDIO_LOG_MESSAGE("Groom duration: " + QString::number(duration) + "ms");

  emit progress(100);
  emit message("Groom Complete");
  emit groom_complete();
  this->ui_->run_groom_button->setEnabled(true);
  this->ui_->skipButton->setEnabled(true);
}

//---------------------------------------------------------------------------
void GroomTool::set_session(QSharedPointer<Session> session)
{
  this->session_ = session;
}

//---------------------------------------------------------------------------
void GroomTool::on_skipButton_clicked()
{
  this->store_settings();
  QVector<QSharedPointer<Shape>> shapes = this->session_->get_shapes();

  std::vector<ImageType::Pointer> imgs;
  for (QSharedPointer<Shape> s : shapes) {
    if (s->get_original_filename().toLower().endsWith(".vtk")) {
      TransformType transform;
      s->import_groomed_mesh(s->get_original_mesh(true)->get_poly_data(), transform);
      this->session_->set_groom_unsaved(true);

    }
    else {
      auto image = s->get_original_image();
      if (!image) {
        emit error_message("Error loading original images");
        return;
      }
      imgs.push_back(image);
    }
  }
  this->session_->load_groomed_images(imgs, 0.);
  this->session_->get_project()->store_subjects();

  emit message("Skipped groom.");
  emit groom_complete();
}
