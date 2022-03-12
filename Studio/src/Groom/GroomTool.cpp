#include <Data/Session.h>
#include <Data/Shape.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/StudioLog.h>
#include <Data/StudioMesh.h>
#include <Groom/GroomTool.h>
#include <Libs/Groom/GroomParameters.h>
#include <ui_GroomTool.h>

#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <iostream>

namespace shapeworks {

//---------------------------------------------------------------------------
GroomTool::GroomTool(Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_GroomTool;
  ui_->setupUi(this);
  qRegisterMetaType<std::string>();

  // connect panel open buttons
  connect(ui_->image_open_button, &QPushButton::toggled, ui_->image_content, &QWidget::setVisible);
  connect(ui_->mesh_open_button, &QPushButton::toggled, ui_->mesh_content, &QWidget::setVisible);
  connect(ui_->alignment_open_button, &QPushButton::toggled, ui_->alignment_content, &QWidget::setVisible);

  connect(ui_->alignment_image_checkbox, &QCheckBox::stateChanged, this, &GroomTool::alignment_checkbox_changed);
  connect(ui_->alignment_box, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &GroomTool::alignment_option_changed);

  connect(ui_->convert_mesh_checkbox, &QCheckBox::stateChanged, this, &GroomTool::update_page);

  ui_->alignment_image_checkbox->setToolTip("Pre-alignment options");
  ui_->isolate_checkbox->setToolTip("Isolate the largest object in the image segmentation");
  ui_->fill_holes_checkbox->setToolTip("Fill small holes in the image segmentation");
  ui_->autopad_checkbox->setToolTip("Add padding around the edges of the image");
  ui_->padding_amount->setToolTip("Padding amount");
  ui_->antialias_checkbox->setToolTip("Antialias the image segmentation");
  ui_->antialias_iterations->setToolTip("Number of antialias iterations");
  ui_->blur_checkbox->setToolTip("Blur/smooth image segmentation");
  ui_->blur_sigma->setToolTip("Gaussian blur sigma");
  ui_->fastmarching_checkbox->setToolTip("Create distance transform");

  ui_->mesh_smooth->setToolTip("Perform mesh smoothing");
  ui_->mesh_smooth_method->setToolTip("Mesh smoothing type");
  ui_->laplacian_iterations->setToolTip("Number of iterations");
  ui_->laplacian_relaxation->setToolTip("Laplacian relaxation factor");
  ui_->sinc_passband->setToolTip("Windowed sinc pass band");
  ui_->sinc_iterations->setToolTip("Windowed sinc iterations.  Minimum number of iterations is 2");

  ui_->remesh_checkbox->setToolTip(
      "Enable remeshing to create a more uniform adaptive mesh.  Also fixes many mesh problems.");
  ui_->remesh_percent_checkbox->setToolTip(
      "Check this box to set the number of vertices based on a percentage of the mesh's current number of vertices");
  ui_->remesh_percent_slider->setToolTip(
      "Set the amount of vertices as a percentage of the current number of vertices.");
  ui_->remesh_num_vertices->setToolTip("Set the desired number of vertices.");
  ui_->remesh_gradation_slider->setToolTip(
      "Set the adaptivity of remeshing, higher will allocate more triangles around areas of high curvature.");
  ui_->remesh_gradation_spinbox->setToolTip(
      "Set the adaptivity of remeshing, higher will allocate more triangles around areas of high curvature.");

  // connect percent controls
  connect(ui_->remesh_percent_slider, &QSlider::valueChanged, this,
          [this](int value) { ui_->remesh_percent_spinbox->setValue(value); });
  connect(ui_->remesh_percent_spinbox, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
          [=](double value) { ui_->remesh_percent_slider->setValue(static_cast<int>(value)); });

  // connect gradation controls
  connect(ui_->remesh_gradation_slider, &QSlider::valueChanged, this,
          [=](int value) { ui_->remesh_gradation_spinbox->setValue(value / 50.0); });
  connect(ui_->remesh_gradation_spinbox, qOverload<double>(&QDoubleSpinBox::valueChanged), this,
          [=](double value) { ui_->remesh_gradation_slider->setValue(static_cast<int>(value * 50.0)); });

  connect(ui_->remesh_percent_checkbox, &QCheckBox::toggled, this, &GroomTool::update_ui);
  connect(ui_->domain_box, qOverload<int>(&QComboBox::currentIndexChanged), this, &GroomTool::domain_changed);

  connect(ui_->mesh_smooth, &QCheckBox::stateChanged, this, &GroomTool::update_ui);
  connect(ui_->mesh_smooth_method, qOverload<const QString&>(&QComboBox::currentIndexChanged), this,
          &GroomTool::update_ui);

  connect(ui_->resample_checkbox, &QCheckBox::stateChanged, this, &GroomTool::update_ui);
  connect(ui_->isotropic_checkbox, &QCheckBox::stateChanged, this, &GroomTool::update_ui);

  connect(ui_->reflect_checkbox, &QCheckBox::stateChanged, this, &GroomTool::reflect_checkbox_changed);

  connect(ui_->reflect_column, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &GroomTool::reflect_column_changed);

  connect(ui_->reflect_choice, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &GroomTool::reflect_choice_changed);

  connect(ui_->reflect_axis, qOverload<int>(&QComboBox::currentIndexChanged), this, &GroomTool::reflect_axis_changed);

  connect(ui_->remesh_checkbox, &QCheckBox::stateChanged, this, &GroomTool::update_ui);

  QIntValidator* above_zero = new QIntValidator(1, std::numeric_limits<int>::max(), this);
  QIntValidator* zero_and_up = new QIntValidator(0, std::numeric_limits<int>::max(), this);

  QDoubleValidator* double_validator = new QDoubleValidator(0, std::numeric_limits<double>::max(), 1000, this);

  ui_->laplacian_iterations->setValidator(zero_and_up);
  ui_->laplacian_relaxation->setValidator(double_validator);
  ui_->sinc_iterations->setValidator(zero_and_up);
  ui_->sinc_passband->setValidator(double_validator);

  update_ui();
}

//---------------------------------------------------------------------------
GroomTool::~GroomTool() {}

//---------------------------------------------------------------------------
void GroomTool::on_antialias_checkbox_stateChanged(int state) {
  ui_->antialias_iterations_label->setEnabled(state);
  ui_->antialias_iterations->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::on_blur_checkbox_stateChanged(int state) {
  ui_->blur_sigma_label->setEnabled(state);
  ui_->blur_sigma->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::on_autopad_checkbox_stateChanged(int state) {
  ui_->padding_amount->setEnabled(state);
  ui_->pad_amount_label->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::handle_error(QString msg) {
  this->groom_is_running_ = false;
  emit error_message(msg);
  this->enable_actions();
}

//---------------------------------------------------------------------------
void GroomTool::handle_progress(int val) {
  if (this->groom_is_running_) {
    emit progress(val);
  }
}

//---------------------------------------------------------------------------
void GroomTool::on_restore_defaults_clicked() {
  this->session_->get_project()->clear_parameters(Parameters::GROOM_PARAMS);
  this->load_params();
}

//---------------------------------------------------------------------------
void GroomTool::update_page() {
  int domain_id = ui_->domain_box->currentIndex();

  auto subjects = session_->get_project()->get_subjects();
  if (subjects.size() > 0 && subjects[0]->get_domain_types().size() > domain_id) {
    bool is_image = subjects[0]->get_domain_types()[domain_id] == DomainType::Image;
    bool is_mesh = subjects[0]->get_domain_types()[domain_id] == DomainType::Mesh;

    if (is_image) {
      ui_->image_panel->show();
    } else {
      ui_->image_panel->hide();
    }
    if (is_mesh || (is_image && this->ui_->convert_mesh_checkbox->isChecked())) {
      ui_->mesh_panel->show();
    } else {
      ui_->mesh_panel->hide();
    }
  }
}

//---------------------------------------------------------------------------
void GroomTool::update_reflect_columns() {
  if (!session_ || !session_->get_project()) {
    return;
  }
  auto project = session_->get_project();
  auto headers = project->get_headers();

  QStringList reflect_columns;
  for (auto header : headers) {
    if (header != "") {
      reflect_columns << QString::fromStdString(header);
    }
  }

  if (reflect_columns != this->reflect_columns_) {
    this->ui_->reflect_column->clear();
    this->ui_->reflect_column->addItems(reflect_columns);
  }
  this->reflect_columns_ = reflect_columns;
}

//---------------------------------------------------------------------------
void GroomTool::update_reflect_choices() {
  auto project = session_->get_project();
  auto column = ui_->reflect_column->currentText();
  auto subjects = session_->get_project()->get_subjects();

  auto rows = project->get_string_column(column.toStdString());

  ui_->reflect_choice->clear();
  QStringList list;
  for (int row = 0; row < rows.size(); row++) {
    list << QString::fromStdString(rows[row]);
  }
  list.removeDuplicates();
  Q_FOREACH (auto item, list) { ui_->reflect_choice->addItem(item); }
}

//---------------------------------------------------------------------------
void GroomTool::load_params() {
  auto params = GroomParameters(session_->get_project(), current_domain_);
  set_ui_from_params(params);

  this->update_page();
  this->update_reflect_columns();
}

//---------------------------------------------------------------------------
void GroomTool::disable_actions() {
  ui_->skip_button->setEnabled(false);
  // this->ui_->run_groom_button->setEnabled(false);
}

//---------------------------------------------------------------------------
void GroomTool::enable_actions() {
  // this->ui_->run_groom_button->setEnabled(true);

  if (groom_is_running_) {
    ui_->skip_button->setEnabled(false);
    ui_->run_groom_button->setText("Abort Groom");
  } else {
    ui_->skip_button->setEnabled(true);
    ui_->run_groom_button->setText("Run Groom");
  }
}

//---------------------------------------------------------------------------
void GroomTool::set_ui_from_params(GroomParameters params) {
  ui_->alignment_image_checkbox->setChecked(params.get_alignment_enabled());

  auto alignment = params.get_alignment_method();
  ui_->alignment_box->setCurrentText(QString::fromStdString(alignment));

  ui_->antialias_checkbox->setChecked(params.get_antialias_tool());
  ui_->autopad_checkbox->setChecked(params.get_auto_pad_tool());
  ui_->fastmarching_checkbox->setChecked(params.get_fast_marching());
  ui_->blur_checkbox->setChecked(params.get_blur_tool());
  ui_->isolate_checkbox->setChecked(params.get_isolate_tool());
  ui_->fill_holes_checkbox->setChecked(params.get_fill_holes_tool());
  ui_->mesh_fill_holes->setChecked(params.get_fill_mesh_holes_tool());
  ui_->antialias_iterations->setValue(params.get_antialias_iterations());
  ui_->blur_sigma->setValue(params.get_blur_amount());
  ui_->padding_amount->setValue(params.get_padding_amount());

  ui_->convert_mesh_checkbox->setChecked(params.get_convert_to_mesh());

  ui_->mesh_smooth_method->setCurrentText(QString::fromStdString(params.get_mesh_smoothing_method()));
  ui_->mesh_smooth->setChecked(params.get_mesh_smooth());

  ui_->laplacian_iterations->setText(QString::number(params.get_mesh_vtk_laplacian_iterations()));
  ui_->laplacian_relaxation->setText(QString::number(params.get_mesh_vtk_laplacian_relaxation()));

  ui_->sinc_iterations->setText(QString::number(params.get_mesh_vtk_windowed_sinc_iterations()));
  ui_->sinc_passband->setText(QString::number(params.get_mesh_vtk_windowed_sinc_passband()));

  ui_->crop_checkbox->setChecked(params.get_crop());
  ui_->reflect_checkbox->setChecked(params.get_reflect());
  ui_->reflect_column->setCurrentText(QString::fromStdString(params.get_reflect_column()));
  ui_->reflect_choice->setCurrentText(QString::fromStdString(params.get_reflect_choice()));
  ui_->reflect_axis->setCurrentText(QString::fromStdString(params.get_reflect_axis()));

  ui_->resample_checkbox->setChecked(params.get_resample());
  ui_->isotropic_checkbox->setChecked(params.get_isotropic());

  ui_->remesh_checkbox->setChecked(params.get_remesh());
  ui_->remesh_percent_checkbox->setChecked(params.get_remesh_percent_mode());
  ui_->remesh_percent_slider->setValue(params.get_remesh_percent());
  ui_->remesh_percent_spinbox->setValue(params.get_remesh_percent());
  ui_->remesh_num_vertices->setText(QString::number(params.get_remesh_num_vertices()));
  ui_->remesh_gradation_slider->setValue(params.get_remesh_gradation() * 50.0);
  ui_->remesh_gradation_spinbox->setValue(params.get_remesh_gradation());

  auto subjects = session_->get_project()->get_subjects();
  int domain_id = std::max<int>(ui_->domain_box->currentIndex(), 0);

  if (!subjects.empty() && !subjects[0]->get_domain_types().empty() &&
      subjects[0]->get_domain_types()[domain_id] == DomainType::Image) {
    if (params.get_iso_spacing() == 0.0) {
      if (session_ && session_->get_project()->get_subjects().size() > 0) {
        auto subject = session_->get_project()->get_subjects()[0];
        if (subject->get_segmentation_filenames().size() > domain_id) {
          try {
            auto path = subject->get_segmentation_filenames()[domain_id];
            if (path != "") {
              // load the image
              Image image(path);
              auto spacing = image.spacing();
              auto min_spacing = std::min<double>(std::min<double>(spacing[0], spacing[1]), spacing[2]);
              params.set_iso_spacing(min_spacing);
              params.set_spacing({spacing[0], spacing[1], spacing[2]});
            }
          } catch (std::exception& e) {
            emit error_message(e.what());
          }
        }
      }
    }

    auto spacing = params.get_spacing();
    ui_->spacing_iso->setText(QString::number(params.get_iso_spacing()));
    ui_->spacing_x->setText(QString::number(spacing[0]));
    ui_->spacing_y->setText(QString::number(spacing[1]));
    ui_->spacing_z->setText(QString::number(spacing[2]));
  }
}

//---------------------------------------------------------------------------
void GroomTool::store_params() {
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
  params.set_fill_mesh_holes_tool(ui_->mesh_fill_holes->isChecked());
  params.set_antialias_iterations(ui_->antialias_iterations->value());
  params.set_groom_output_prefix(preferences_.get_groom_file_template().toStdString());
  params.set_convert_to_mesh(ui_->convert_mesh_checkbox->isChecked());

  params.set_mesh_smooth(ui_->mesh_smooth->isChecked());
  params.set_mesh_smoothing_method(ui_->mesh_smooth_method->currentText().toStdString());
  params.set_mesh_vtk_laplacian_iterations(ui_->laplacian_iterations->text().toInt());
  params.set_mesh_vtk_laplacian_relaxation(ui_->laplacian_relaxation->text().toDouble());
  params.set_mesh_vtk_windowed_sinc_iterations(ui_->sinc_iterations->text().toInt());
  params.set_mesh_vtk_windowed_sinc_passband(ui_->sinc_passband->text().toDouble());

  params.set_crop(ui_->crop_checkbox->isChecked());
  params.set_reflect(ui_->reflect_checkbox->isChecked());
  params.set_reflect_column(ui_->reflect_column->currentText().toStdString());
  params.set_reflect_choice(ui_->reflect_choice->currentText().toStdString());
  params.set_reflect_axis(ui_->reflect_axis->currentText().toStdString());

  params.set_resample(ui_->resample_checkbox->isChecked());
  params.set_isotropic(ui_->isotropic_checkbox->isChecked());
  params.set_iso_spacing(ui_->spacing_iso->text().toDouble());
  params.set_spacing(
      {ui_->spacing_x->text().toDouble(), ui_->spacing_y->text().toDouble(), ui_->spacing_z->text().toDouble()});

  params.set_remesh(ui_->remesh_checkbox->isChecked());

  params.set_remesh_percent_mode(ui_->remesh_percent_checkbox->isChecked());
  params.set_remesh_percent(ui_->remesh_percent_slider->value());
  params.set_remesh_num_vertices(ui_->remesh_num_vertices->text().toInt());
  params.set_remesh_gradation(ui_->remesh_gradation_spinbox->value());

  params.save_to_project();

  // global settings
  for (const auto& domain_name : session_->get_project()->get_domain_names()) {
    params = GroomParameters(session_->get_project(), domain_name);
    params.set_groom_output_prefix(preferences_.get_groom_file_template().toStdString());
    params.save_to_project();
  }
}

//---------------------------------------------------------------------------
void GroomTool::on_run_groom_button_clicked() {
  if (this->groom_is_running_) {
    shutdown_threads();
    groom_is_running_ = false;
    enable_actions();
    emit progress(100);
    return;
  }

  store_params();

  bool question_dt = false;
  auto subjects = session_->get_project()->get_subjects();
  auto domain_names = session_->get_project()->get_domain_names();
  if (subjects.size() > 0) {
    for (int domain = 0; domain < domain_names.size(); domain++) {
      if (subjects[0]->get_domain_types()[domain] == DomainType::Image) {
        auto params = GroomParameters(session_->get_project(), domain_names[domain]);
        if (!params.get_fast_marching()) {
          question_dt = true;
        }
      }
    }
  }
  if (question_dt) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Are you sure?",
                                  "Fast Marching is not selected.\n"
                                  "Are you sure your input data is already a distance transform?\n\n"
                                  "The image volumes must already be distance transforms.\n\n"
                                  "Is that correct?",
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::No) {
      return;
    }
  }

  groom_is_running_ = true;

  timer_.start();

  emit message("Please wait: running groom step...");
  emit progress(0);

  groom_ = QSharedPointer<QGroom>(new QGroom(session_->get_project()));

  enable_actions();

  ShapeworksWorker* worker = new ShapeworksWorker(ShapeworksWorker::GroomType, groom_, nullptr, nullptr, session_);

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
void GroomTool::handle_thread_complete() {
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
void GroomTool::on_skip_button_clicked() {
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
void GroomTool::set_session(QSharedPointer<Session> session) {
  session_ = session;
  this->update_reflect_choices();
}

//---------------------------------------------------------------------------
void GroomTool::activate() {
  auto subjects = session_->get_project()->get_subjects();

  auto domain_names = session_->get_project()->get_domain_names();
  ui_->domain_box->setVisible(domain_names.size() > 1);

  if (domain_names.size() != ui_->domain_box->count()) {
    ui_->domain_box->clear();
    for (auto&& item : domain_names) {
      ui_->domain_box->addItem(QString::fromStdString(item));
    }
  }

  this->update_page();
}

//---------------------------------------------------------------------------
void GroomTool::alignment_checkbox_changed(int state) {
  ui_->alignment_image_checkbox->setChecked(state);
  ui_->alignment_box->setEnabled(state);
}

//---------------------------------------------------------------------------
void GroomTool::alignment_option_changed(int index) { ui_->alignment_box->setCurrentIndex(index); }

//---------------------------------------------------------------------------
void GroomTool::reflect_checkbox_changed(int state) {
  ui_->reflect_checkbox->setChecked(state);
  update_ui();
}

//---------------------------------------------------------------------------
void GroomTool::reflect_column_changed(int index) {
  ui_->reflect_column->setCurrentIndex(index);
  this->update_reflect_choices();
}

//---------------------------------------------------------------------------
void GroomTool::reflect_choice_changed(int index) { ui_->reflect_choice->setCurrentIndex(index); }

//---------------------------------------------------------------------------
void GroomTool::reflect_axis_changed(int index) { ui_->reflect_axis->setCurrentIndex(index); }

//---------------------------------------------------------------------------
void GroomTool::shutdown_threads() {
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
void GroomTool::domain_changed() {
  if (current_domain_ != "") {
    store_params();
  }

  if (ui_->domain_box->count() < 2) {
    current_domain_ = "";
  } else {
    current_domain_ = ui_->domain_box->currentText().toStdString();
  }

  load_params();
}

//---------------------------------------------------------------------------
void GroomTool::update_ui() {
  ui_->mesh_smooth_stack->setCurrentIndex(ui_->mesh_smooth_method->currentIndex());
  ui_->mesh_smooth_box->setVisible(ui_->mesh_smooth->isChecked());
  ui_->mesh_smooth_box->setEnabled(ui_->mesh_smooth->isChecked());
  ui_->resample_box->setVisible(ui_->resample_checkbox->isChecked());

  bool iso_mode = ui_->isotropic_checkbox->isChecked();
  ui_->spacing_label_x->setEnabled(!iso_mode);
  ui_->spacing_label_y->setEnabled(!iso_mode);
  ui_->spacing_label_z->setEnabled(!iso_mode);
  ui_->spacing_x->setEnabled(!iso_mode);
  ui_->spacing_y->setEnabled(!iso_mode);
  ui_->spacing_z->setEnabled(!iso_mode);
  ui_->spacing_iso->setEnabled(iso_mode);

  ui_->reflect_choice->setVisible(ui_->reflect_checkbox->isChecked());
  ui_->reflect_column->setVisible(ui_->reflect_checkbox->isChecked());
  ui_->reflect_axis->setVisible(ui_->reflect_checkbox->isChecked());
  ui_->reflect_equal->setVisible(ui_->reflect_checkbox->isChecked());
  ui_->reflect_on_label->setVisible(ui_->reflect_checkbox->isChecked());

  ui_->remesh_box->setVisible(ui_->remesh_checkbox->isChecked());

  bool percent_mode = ui_->remesh_percent_checkbox->isChecked();
  ui_->remesh_percent_label->setVisible(percent_mode);
  ui_->remesh_percent_slider->setVisible(percent_mode);
  ui_->remesh_percent_spinbox->setVisible(percent_mode);

  ui_->remesh_num_vertices_label->setVisible(!percent_mode);
  ui_->remesh_num_vertices->setVisible(!percent_mode);
}

//---------------------------------------------------------------------------
}  // namespace shapeworks
