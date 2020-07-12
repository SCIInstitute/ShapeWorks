#include <iostream>

#include <QMessageBox>
#include <QThread>

#include <Data/Session.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>
#include <Groom/GroomTool.h>
#include <Visualization/ShapeWorksWorker.h>

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
  Parameters params = this->session_->get_project()->get_parameters(Parameters::GROOM_PARAMS);
  this->ui_->center_checkbox->setChecked(params.get("center", true));
  this->ui_->antialias_checkbox->setChecked(params.get("antialias", true));
  this->ui_->autopad_checkbox->setChecked(params.get("pad", true));
  this->ui_->fastmarching_checkbox->setChecked(params.get("fastmarching", true));
  this->ui_->blur_checkbox->setChecked(params.get("blur", true));
  this->ui_->isolate_checkbox->setChecked(params.get("isolate", true));
  this->ui_->fill_holes_checkbox->setChecked(params.get("fill_holes", true));
  this->ui_->antialias_iterations->setValue(params.get("antialias_amount", 10));
  this->ui_->blur_sigma->setValue(params.get("blur_sigma", 2.0));
  this->ui_->padding_amount->setValue(params.get("pad_value", 10));
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
  Parameters params = this->session_->get_project()->get_parameters(Parameters::GROOM_PARAMS);

  params.set("center", this->ui_->center_checkbox->isChecked());
  params.set("antialias", this->ui_->antialias_checkbox->isChecked());
  params.set("pad", this->ui_->autopad_checkbox->isChecked());
  params.set("fastmarching", this->ui_->fastmarching_checkbox->isChecked());
  params.set("blur", this->ui_->blur_checkbox->isChecked());
  params.set("isolate", this->ui_->isolate_checkbox->isChecked());
  params.set("fill_holes", this->ui_->fill_holes_checkbox->isChecked());
  params.set("antialias_amount", this->ui_->antialias_iterations->value());
  params.set("blur_sigma", this->ui_->blur_sigma->value());
  params.set("pad_value", this->ui_->padding_amount->value());

  this->session_->get_project()->set_parameters(Parameters::GROOM_PARAMS, params);
}

//---------------------------------------------------------------------------
void GroomTool::on_run_groom_button_clicked()
{
  this->store_settings();
  emit message("Please wait: running groom step...");
  emit progress(5);
  QVector<QSharedPointer<Shape>> shapes = this->session_->get_shapes();
  std::vector<ImageType::Pointer> imgs;
  for (QSharedPointer<Shape> s : shapes) {
    auto image = s->get_original_image();
    if (!image) {
      emit error_message("Error loading original images");
      emit progress(100);
      return;
    }
    imgs.push_back(image);
  }
  this->groom_ = new QGroom(this, imgs, 0., 1.,
                            this->ui_->blur_sigma->value(),
                            this->ui_->padding_amount->value(),
                            this->ui_->antialias_iterations->value(),
                            true);

  emit progress(15);
  if (this->ui_->center_checkbox->isChecked()) {
    this->groom_->queueTool("center");
  }
  if (this->ui_->isolate_checkbox->isChecked()) {
    this->groom_->queueTool("isolate");
  }
  if (this->ui_->fill_holes_checkbox->isChecked()) {
    this->groom_->queueTool("hole_fill");
  }
  if (this->ui_->autopad_checkbox->isChecked()) {
    this->groom_->queueTool("auto_pad");
  }
  if (this->ui_->antialias_checkbox->isChecked()) {
    this->groom_->queueTool("antialias");
  }
  if (this->ui_->fastmarching_checkbox->isChecked()) {
    this->groom_->queueTool("fastmarching");
  }
  if (this->ui_->blur_checkbox->isChecked()) {
    this->groom_->queueTool("blur");
  }
  emit progress(10);
  this->ui_->run_groom_button->setEnabled(false);
  this->ui_->skipButton->setEnabled(false);
  QThread* thread = new QThread;
  ShapeworksWorker* worker = new ShapeworksWorker(
    ShapeworksWorker::GroomType, this->groom_, nullptr, this->session_);
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_thread_complete()));
  connect(this->groom_, SIGNAL(progress(int)), this, SLOT(handle_progress(int)));
  connect(worker, SIGNAL(error_message(std::string)), this, SLOT(handle_error(std::string)));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();
}

//---------------------------------------------------------------------------
void GroomTool::handle_thread_complete()
{
  emit progress(95);
  this->session_->load_groomed_images(this->groom_->getImages(),
                                      this->ui_->fastmarching_checkbox->isChecked() ? 0. : 0.5);
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
    auto image = s->get_original_image();
    if (!image) {
      emit error_message("Error loading original images");
      return;
    }
    imgs.push_back(image);
  }
  this->session_->load_groomed_images(imgs, 0.);
  emit message("Skipped groom.");
  emit groom_complete();
}
