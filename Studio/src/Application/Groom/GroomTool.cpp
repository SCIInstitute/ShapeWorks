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

namespace shapeworks {

//---------------------------------------------------------------------------
GroomTool::GroomTool(Preferences& prefs) : preferences_(prefs)
{
  ui_ = new Ui_GroomTool;
  ui_->setupUi(this);
  qRegisterMetaType<std::string>();

  connect(ui_->mesh_center, &QCheckBox::stateChanged,
          this, &GroomTool::centering_changed);
  connect(ui_->center_checkbox, &QCheckBox::stateChanged,
          this, &GroomTool::centering_changed);

  ui_->center_checkbox->setToolTip("Center image segmentations");
  ui_->isolate_checkbox->setToolTip("Isolate the largest object in the image segmentation");
  ui_->fill_holes_checkbox->setToolTip("Fill small holes in the image segmentation");
  ui_->autopad_checkbox->setToolTip("Add padding around the edges of the image");
  ui_->padding_amount->setToolTip("Padding amount");
  ui_->antialias_checkbox->setToolTip("Antialias the image segmentation");
  ui_->antialias_iterations->setToolTip("Number of antialias iterations");
  ui_->blur_checkbox->setToolTip("Blur/smooth image segmentation");
  ui_->blur_sigma->setToolTip("Gaussian blur sigma");
  ui_->fastmarching_checkbox->setToolTip("Create distance transform");

  connect(ui_->domain_box, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &GroomTool::domain_changed);
}

//---------------------------------------------------------------------------
GroomTool::~GroomTool()
{}

//---------------------------------------------------------------------------
void GroomTool::on_antialias_checkbox_stateChanged(int state)
{
  ui_->antialias_groupbox->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::on_blur_checkbox_stateChanged(int state)
{
  ui_->blur_groupbox->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::on_autopad_checkbox_stateChanged(int state)
{
  ui_->pad_groupbox->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::handle_error(std::string msg)
{
  emit error_message(msg);
  this->enable_actions();
}

//---------------------------------------------------------------------------
void GroomTool::handle_progress(int val)
{
  if (this->groom_is_running_) {
    emit progress(val);
  }
}

//---------------------------------------------------------------------------
void GroomTool::on_restore_defaults_clicked()
{
  // create a set of blank parameters
  GroomParameters params(session_->get_project(), current_domain_);
  params.restore_defaults();
  // now load those settings
  set_ui_from_params(params);
  store_params();
}

//---------------------------------------------------------------------------
void GroomTool::set_ui_from_params(GroomParameters params)
{
  ui_->center_checkbox->setChecked(params.get_center_tool());
  ui_->mesh_center->setChecked(params.get_center_tool());
  ui_->antialias_checkbox->setChecked(params.get_antialias_tool());
  ui_->autopad_checkbox->setChecked(params.get_auto_pad_tool());
  ui_->fastmarching_checkbox->setChecked(params.get_fast_marching());
  ui_->blur_checkbox->setChecked(params.get_blur_tool());
  ui_->isolate_checkbox->setChecked(params.get_isolate_tool());
  ui_->fill_holes_checkbox->setChecked(params.get_fill_holes_tool());
  ui_->antialias_iterations->setValue(params.get_antialias_iterations());
  ui_->blur_sigma->setValue(params.get_blur_amount());
  ui_->padding_amount->setValue(params.get_padding_amount());
}

//---------------------------------------------------------------------------
void GroomTool::load_params()
{
  auto params = GroomParameters(session_->get_project(), current_domain_);
  set_ui_from_params(params);
}

//---------------------------------------------------------------------------
void GroomTool::disable_actions()
{
  ui_->skip_button->setEnabled(false);
  //this->ui_->run_groom_button->setEnabled(false);
}

//---------------------------------------------------------------------------
void GroomTool::enable_actions()
{
  //this->ui_->run_groom_button->setEnabled(true);

  if (groom_is_running_) {
    ui_->skip_button->setEnabled(false);
    ui_->run_groom_button->setText("Abort Groom");
  }
  else {
    ui_->skip_button->setEnabled(true);
    ui_->run_groom_button->setText("Run Groom");
  }

}

//---------------------------------------------------------------------------
void GroomTool::store_params()
{

  auto params = GroomParameters(session_->get_project(), current_domain_);

  params.set_center_tool(ui_->center_checkbox->isChecked());
  params.set_antialias_tool(ui_->antialias_checkbox->isChecked());
  params.set_auto_pad_tool(ui_->autopad_checkbox->isChecked());
  params.set_padding_amount(ui_->padding_amount->value());
  params.set_fast_marching(ui_->fastmarching_checkbox->isChecked());
  params.set_blur_tool(ui_->blur_checkbox->isChecked());
  params.set_blur_amount(ui_->blur_sigma->value());
  params.set_isolate_tool(ui_->isolate_checkbox->isChecked());
  params.set_fill_holes_tool(ui_->fill_holes_checkbox->isChecked());
  params.set_antialias_iterations(ui_->antialias_iterations->value());
  params.set_groom_output_prefix(preferences_.get_groom_file_template().toStdString());

  params.save_to_project();
}

//---------------------------------------------------------------------------
void GroomTool::on_run_groom_button_clicked()
{
  if (this->groom_is_running_) {
    shutdown_threads();
    groom_is_running_ = false;
    enable_actions();
    emit progress(100);
    return;
  }
  groom_is_running_ = true;

  timer_.start();

  store_params();

  emit message("Please wait: running groom step...");
  emit progress(0);

  groom_ = QSharedPointer<QGroom>(new QGroom(session_->get_project()));

  enable_actions();

  ShapeworksWorker* worker = new ShapeworksWorker(
    ShapeworksWorker::GroomType, groom_, nullptr, nullptr, session_);

  QThread* thread = new QThread;
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, &ShapeworksWorker::result_ready, this, &GroomTool::handle_thread_complete);
  connect(this->groom_.data(), &QGroom::progress, this, &GroomTool::handle_progress);
  connect(worker, SIGNAL(error_message(std::string)), this, SLOT(handle_error(std::string)));
  connect(worker, &ShapeworksWorker::message, this, &GroomTool::message);
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();

  threads_ << thread;
}

//---------------------------------------------------------------------------
void GroomTool::handle_thread_complete()
{
  emit progress(95);

  // trigger reload of project
  auto duration = timer_.elapsed();
  STUDIO_LOG_MESSAGE("Groom duration: " + QString::number(duration) + "ms");

  for (auto shape : session_->get_shapes()) {
    shape->reset_groomed_mesh();
  }

  emit progress(100);
  emit message("Groom Complete");
  emit groom_complete();

  groom_is_running_ = false;
  enable_actions();
}

//---------------------------------------------------------------------------
void GroomTool::on_skip_button_clicked()
{
  store_params();
  groom_ = QSharedPointer<QGroom>(new QGroom(session_->get_project()));
  groom_->set_skip_grooming(true);
  groom_->run();

  emit message("Skipped Grooming");
  emit groom_complete();
}

//---------------------------------------------------------------------------
void GroomTool::set_session(QSharedPointer<Session> session)
{
  session_ = session;
}

//---------------------------------------------------------------------------
void GroomTool::activate()
{
  auto subjects = session_->get_project()->get_subjects();

  auto domain_names = session_->get_project()->get_domain_names();
  ui_->domain_box->setVisible(domain_names.size() > 1);

  if (domain_names.size() != ui_->domain_box->count()) {
    ui_->domain_box->clear();
    for (auto&& item : domain_names) {
      ui_->domain_box->addItem(QString::fromStdString(item));
    }
  }

  if (subjects.size() > 0 && subjects[0]->get_domain_types().size() > 0) {
    if (subjects[0]->get_domain_types()[0] == DomainType::Image) {
      ui_->stacked_widget->setCurrentWidget(ui_->image_page);
    }
    if (subjects[0]->get_domain_types()[0] == DomainType::Mesh) {
      ui_->stacked_widget->setCurrentWidget(ui_->mesh_page);
    }
  }
}

//---------------------------------------------------------------------------
void GroomTool::centering_changed(int state)
{
  ui_->mesh_center->setChecked(state);
  ui_->center_checkbox->setChecked(state);
}

//---------------------------------------------------------------------------
void GroomTool::shutdown_threads()
{
  std::cerr << "Shut Down Groom Threads\n";
  if (!groom_) {
    return;
  }
  groom_->abort();

  for (size_t i = 0; i < threads_.size(); i++) {
    if (threads_[i]->isRunning()) {
      std::cerr << "waiting...\n";
      threads_[i]->wait(1000);
      std::cerr << "done waiting...\n";
    }
  }
}

//---------------------------------------------------------------------------
void GroomTool::domain_changed()
{
  if (current_domain_ != "") {
    store_params();
  }
  current_domain_ = ui_->domain_box->currentText().toStdString();

  load_params();
}


}