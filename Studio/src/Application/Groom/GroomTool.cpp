#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>

#include <Groom/GroomTool.h>
#include <Visualization/ShapeworksWorker.h>
#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <ui_GroomTool.h>

//---------------------------------------------------------------------------
GroomTool::GroomTool(Preferences& prefs, std::vector<std::string>& files)
  : preferences_(prefs), files_(files)
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
  this->preferences_.delete_entry("groom_center");
  this->preferences_.delete_entry("groom_antialias");
  this->preferences_.delete_entry("groom_pad");
  this->preferences_.delete_entry("groom_fastmarching");
  this->preferences_.delete_entry("groom_blur");
  this->preferences_.delete_entry("groom_isolate");
  this->preferences_.delete_entry("groom_antialias_amount");
  this->preferences_.delete_entry("groom_fill_holes");
  this->preferences_.delete_entry("groom_blur_sigma");
  this->preferences_.delete_entry("groom_pad_value");
  this->preferences_.restore_defaults();
  this->set_preferences();
  qApp->processEvents();
  this->preferences_.set_saved(false);
}

//---------------------------------------------------------------------------
void GroomTool::set_preferences()
{
  this->ui_->center_checkbox->setChecked(
    this->preferences_.get_preference("groom_center", this->ui_->center_checkbox->isChecked()));
  this->ui_->antialias_checkbox->setChecked(
    this->preferences_.get_preference("groom_antialias",
                                      this->ui_->antialias_checkbox->isChecked()));
  this->ui_->autopad_checkbox->setChecked(
    this->preferences_.get_preference("groom_pad", this->ui_->autopad_checkbox->isChecked()));
  this->ui_->fastmarching_checkbox->setChecked(
    this->preferences_.get_preference("groom_fastmarching",
                                      this->ui_->fastmarching_checkbox->isChecked()));
  this->ui_->blur_checkbox->setChecked(
    this->preferences_.get_preference("groom_blur", this->ui_->blur_checkbox->isChecked()));
  this->ui_->isolate_checkbox->setChecked(
    this->preferences_.get_preference("groom_isolate", this->ui_->isolate_checkbox->isChecked()));
  this->ui_->fill_holes_checkbox->setChecked(
    this->preferences_.get_preference("groom_fill_holes",
                                      this->ui_->fill_holes_checkbox->isChecked()));
  this->ui_->antialias_iterations->setValue(
    this->preferences_.get_preference("groom_antialias_amount",
                                      this->ui_->antialias_iterations->value()));
  this->ui_->blur_sigma->setValue(
    this->preferences_.get_preference("groom_blur_sigma", this->ui_->blur_sigma->value()));
  this->ui_->padding_amount->setValue(
    this->preferences_.get_preference("groom_pad_value", this->ui_->padding_amount->value()));
}

//---------------------------------------------------------------------------
void GroomTool::disableActions()
{
  this->ui_->skipButton->setEnabled(false);
  this->ui_->run_groom_button->setEnabled(false);
}

//---------------------------------------------------------------------------
void GroomTool::enableActions()
{
  this->ui_->skipButton->setEnabled(true);
  this->ui_->run_groom_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void GroomTool::update_preferences()
{
  this->preferences_.set_preference("groom_center", this->ui_->center_checkbox->isChecked());
  this->preferences_.set_preference("groom_antialias", this->ui_->antialias_checkbox->isChecked());
  this->preferences_.set_preference("groom_pad", this->ui_->autopad_checkbox->isChecked());
  this->preferences_.set_preference("groom_fastmarching",
                                    this->ui_->fastmarching_checkbox->isChecked());
  this->preferences_.set_preference("groom_blur", this->ui_->blur_checkbox->isChecked());
  this->preferences_.set_preference("groom_isolate", this->ui_->isolate_checkbox->isChecked());
  this->preferences_.set_preference("groom_fill_holes",
                                    this->ui_->fill_holes_checkbox->isChecked());
  this->preferences_.set_preference("groom_antialias_amount",
                                    this->ui_->antialias_iterations->value());
  this->preferences_.set_preference("groom_blur_sigma", this->ui_->blur_sigma->value());
  this->preferences_.set_preference("groom_pad_value", this->ui_->padding_amount->value());
}

//---------------------------------------------------------------------------
void GroomTool::on_run_groom_button_clicked()
{
  this->update_preferences();
  emit message("Please wait: running groom step...");
  emit progress(5);
  auto shapes = this->project_->get_shapes();
  std::vector<ImageType::Pointer> imgs;
  for (auto s : shapes) {
    imgs.push_back(s->get_original_image());
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
    ShapeworksWorker::GroomType, this->groom_, nullptr, this->project_);
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
  this->project_->load_groomed_images(this->groom_->getImages(),
                                      this->ui_->fastmarching_checkbox->isChecked() ? 0. : 0.5);
  emit progress(100);
  emit message("Groom Complete");
  emit groom_complete();
  this->ui_->run_groom_button->setEnabled(true);
  this->ui_->skipButton->setEnabled(true);
}

//---------------------------------------------------------------------------
void GroomTool::set_project(QSharedPointer<Project> project)
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
void GroomTool::on_skipButton_clicked()
{
  this->update_preferences();
  auto shapes = this->project_->get_shapes();
  std::vector<ImageType::Pointer> imgs;
  for (auto s : shapes) {
    imgs.push_back(s->get_original_image());
  }
  this->project_->load_groomed_images(imgs, 0.);
  emit message("Skipped groom.");
  emit groom_complete();
}
