#include <iostream>

#include <QMessageBox>
#include <QThread>

#include <Data/Session.h>
#include <Data/StudioMesh.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <Data/ShapeWorksWorker.h>
#include <Groom/GroomTool.h>

#include <Libs/Groom/GroomParameters.h>

#include <ui_GroomTool.h>

namespace shapeworks {

//---------------------------------------------------------------------------
GroomTool::GroomTool(Preferences& prefs) : preferences_(prefs)
{
  ui_ = new Ui_GroomTool;
  ui_->setupUi(this);
  qRegisterMetaType<std::string>();

  connect(ui_->alignment_image_checkbox, &QCheckBox::stateChanged,
          this, &GroomTool::alignment_checkbox_changed);
  connect(ui_->alignment_mesh_checkbox, &QCheckBox::stateChanged,
          this, &GroomTool::alignment_checkbox_changed);

  connect(ui_->fill_holes_checkbox, &QCheckBox::stateChanged,
          this, &GroomTool::fill_holes_changed);
  connect(ui_->mesh_fill_holes, &QCheckBox::stateChanged,
          this, &GroomTool::fill_holes_changed);

  ui_->alignment_image_checkbox->setToolTip("Pre-alignment options");
  ui_->alignment_mesh_checkbox->setToolTip("Pre-alignment options");
  ui_->isolate_checkbox->setToolTip("Isolate the largest object in the image segmentation");
  ui_->fill_holes_checkbox->setToolTip("Fill small holes in the image segmentation");
  ui_->autopad_checkbox->setToolTip("Add padding around the edges of the image");
  ui_->padding_amount->setToolTip("Padding amount");
  ui_->antialias_checkbox->setToolTip("Antialias the image segmentation");
  ui_->antialias_iterations->setToolTip("Number of antialias iterations");
  ui_->blur_checkbox->setToolTip("Blur/smooth image segmentation");
  ui_->blur_sigma->setToolTip("Gaussian blur sigma");
  ui_->fastmarching_checkbox->setToolTip("Create distance transform");

  //ui_->mesh_center->setToolTip("Center meshes based on center of mass");
  //ui_->mesh_qc->setToolTip("Perform mesh quality control steps");
  //ui_->mesh_qc->hide();
  ui_->mesh_smooth->setToolTip("Perform mesh smoothing");
  ui_->mesh_smooth_method->setToolTip("Mesh smoothing type");
  ui_->laplacian_iterations->setToolTip("Number of iterations");
  ui_->laplacian_relaxation->setToolTip("Laplacian relaxation factor");
  ui_->sinc_passband->setToolTip("Windowed sinc pass band");
  ui_->sinc_iterations->setToolTip("Windowed sinc iterations");

  connect(ui_->domain_box, qOverload<int>(&QComboBox::currentIndexChanged),
          this, &GroomTool::domain_changed);

  connect(ui_->mesh_smooth, &QCheckBox::stateChanged, this, &GroomTool::update_ui);
  connect(ui_->mesh_smooth_method, qOverload<const QString&>(&QComboBox::currentIndexChanged), this,
          &GroomTool::update_ui);

  QIntValidator* above_zero = new QIntValidator(1, std::numeric_limits<int>::max(), this);
  QIntValidator* zero_and_up = new QIntValidator(0, std::numeric_limits<int>::max(), this);

  QDoubleValidator* double_validator = new QDoubleValidator(0, std::numeric_limits<double>::max(),
                                                            1000, this);

  ui_->laplacian_iterations->setValidator(zero_and_up);
  ui_->laplacian_relaxation->setValidator(double_validator);
  ui_->sinc_iterations->setValidator(zero_and_up);
  ui_->sinc_passband->setValidator(double_validator);

  update_ui();

  //ui_->center_checkbox->hide();
}

//---------------------------------------------------------------------------
GroomTool::~GroomTool()
{}

//---------------------------------------------------------------------------
void GroomTool::on_antialias_checkbox_stateChanged(int state)
{
  ui_->antialias_iterations_label->setEnabled(state);
  ui_->antialias_iterations->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::on_blur_checkbox_stateChanged(int state)
{
  ui_->blur_sigma_label->setEnabled(state);
  ui_->blur_sigma->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::on_autopad_checkbox_stateChanged(int state)
{
  ui_->padding_amount->setEnabled(state);
  ui_->pad_amount_label->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::handle_error(QString msg)
{
  this->groom_is_running_ = false;
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
  this->session_->get_project()->clear_parameters(Parameters::GROOM_PARAMS);
  this->load_params();
}

//---------------------------------------------------------------------------
void GroomTool::set_ui_from_params(GroomParameters params)
{
  ui_->alignment_mesh_checkbox->setChecked(params.get_alignment_enabled());
  ui_->alignment_image_checkbox->setChecked(params.get_alignment_enabled());

  auto alignment = params.get_alignment_method();
  ui_->alignment_box->setCurrentText(QString::fromStdString(alignment));
  ui_->alignment_mesh_box->setCurrentText(QString::fromStdString(alignment));

  ui_->antialias_checkbox->setChecked(params.get_antialias_tool());
  ui_->autopad_checkbox->setChecked(params.get_auto_pad_tool());
  ui_->fastmarching_checkbox->setChecked(params.get_fast_marching());
  ui_->blur_checkbox->setChecked(params.get_blur_tool());
  ui_->isolate_checkbox->setChecked(params.get_isolate_tool());
  ui_->fill_holes_checkbox->setChecked(params.get_fill_holes_tool());
  ui_->mesh_fill_holes->setChecked(params.get_fill_holes_tool());
  ui_->antialias_iterations->setValue(params.get_antialias_iterations());
  ui_->blur_sigma->setValue(params.get_blur_amount());
  ui_->padding_amount->setValue(params.get_padding_amount());

  ui_->mesh_smooth_method->setCurrentText(
    QString::fromStdString(params.get_mesh_smoothing_method()));
  ui_->mesh_smooth->setChecked(params.get_mesh_smooth());

  ui_->laplacian_iterations->setText(QString::number(params.get_mesh_vtk_laplacian_iterations()));
  ui_->laplacian_relaxation->setText(QString::number(params.get_mesh_vtk_laplacian_relaxation()));

  ui_->sinc_iterations->setText(QString::number(params.get_mesh_vtk_windowed_sinc_iterations()));
  ui_->sinc_passband->setText(QString::number(params.get_mesh_vtk_windowed_sinc_passband()));

}

//---------------------------------------------------------------------------
void GroomTool::load_params()
{
  auto params = GroomParameters(session_->get_project(), current_domain_);
  set_ui_from_params(params);

  auto subjects = session_->get_project()->get_subjects();

  int domain_id = ui_->domain_box->currentIndex();

  if (subjects.size() > 0 && subjects[0]->get_domain_types().size() > domain_id) {

    if (subjects[0]->get_domain_types()[domain_id] == DomainType::Image) {
      ui_->stacked_widget->setCurrentWidget(ui_->image_page);
    }
    if (subjects[0]->get_domain_types()[domain_id] == DomainType::Mesh) {
      ui_->stacked_widget->setCurrentWidget(ui_->mesh_page);
    }
  }

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

  params.set_alignment_enabled(ui_->alignment_image_checkbox->isChecked());
  params.set_alignment_method(ui_->alignment_box->currentText().toStdString());

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

  params.set_mesh_smooth(ui_->mesh_smooth->isChecked());
  params.set_mesh_smoothing_method(ui_->mesh_smooth_method->currentText().toStdString());
  params.set_mesh_vtk_laplacian_iterations(ui_->laplacian_iterations->text().toInt());
  params.set_mesh_vtk_laplacian_relaxation(ui_->laplacian_relaxation->text().toDouble());
  params.set_mesh_vtk_windowed_sinc_iterations(ui_->sinc_iterations->text().toInt());
  params.set_mesh_vtk_windowed_sinc_passband(ui_->sinc_passband->text().toDouble());
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
  connect(worker, &ShapeworksWorker::finished, this, &GroomTool::handle_thread_complete);
  connect(this->groom_.data(), &QGroom::progress, this, &GroomTool::handle_progress);
  connect(worker, &ShapeworksWorker::error_message, this, &GroomTool::handle_error);
  connect(worker, &ShapeworksWorker::message, this, &GroomTool::message);
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();

  emit groom_start();
  threads_ << thread;
}

//---------------------------------------------------------------------------
void GroomTool::handle_thread_complete()
{
  emit progress(95);

  QString duration = QString::number(this->timer_.elapsed() / 1000.0, 'f', 1);
  emit message("Groom Complete.  Duration: " + duration + " seconds");

  // trigger reload of meshes
  for (auto shape : session_->get_shapes()) {
    shape->reset_groomed_mesh();
  }

  emit progress(100);
  emit groom_complete();

  groom_is_running_ = false;
  enable_actions();
}

//---------------------------------------------------------------------------
void GroomTool::on_skip_button_clicked()
{
  bool has_image = false;
  auto subjects = session_->get_project()->get_subjects();
  auto domain_names = session_->get_project()->get_domain_names();
  if (subjects.size() > 0) {
    for (int domain = 0; domain < domain_names.size(); domain++) {
      if (subjects[0]->get_domain_types()[domain] == DomainType::Image) {
        has_image = true;
      }
    }
  }

  if (has_image) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Are you sure?",
                                  "Are you sure your input data is already groomed?\n\n"
                                  "The image volumes must already be distance transforms.\n\n"
                                  "Is that correct?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
      return;
    }
  }

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
void GroomTool::alignment_checkbox_changed(int state)
{
  ui_->alignment_image_checkbox->setChecked(state);
  ui_->alignment_mesh_checkbox->setChecked(state);
  ui_->alignment_box->setEnabled(state);
  ui_->alignment_mesh_box->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::fill_holes_changed(int state)
{
  ui_->fill_holes_checkbox->setChecked(state);
  ui_->mesh_fill_holes->setChecked(state);
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

//---------------------------------------------------------------------------
void GroomTool::update_ui()
{
  ui_->mesh_smooth_stack->setCurrentIndex(ui_->mesh_smooth_method->currentIndex());
  ui_->mesh_smooth_box->setVisible(ui_->mesh_smooth->isChecked());
  ui_->mesh_smooth_box->setEnabled(ui_->mesh_smooth->isChecked());
}

}