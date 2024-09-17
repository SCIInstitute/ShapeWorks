#include <tbb/parallel_for.h>
#include <QFileDialog>
#include <QMessageBox>
#include <QThread>

#include <Logging.h>
#include <Mesh/Mesh.h>
#include <Python/PythonWorker.h>

#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/Worker.h>
#include <ShapeWorksMONAI/MonaiParameters.h>
#include <ShapeWorksMONAI/MonaiLabelTool.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Interface/Style.h>
#include <Logging.h>
#include <Shape.h>

#include <vtkPointData.h>

#include <ui_MonaiLabelTool.h>


namespace shapeworks {
//---------------------------------------------------------------------------
MonaiLabelTool::MonaiLabelTool(Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_MonaiLabelTool;
  ui_->setupUi(this);

  ui_->server_address->setToolTip("Eneter the MONAI sever address.");

  // MONAI params panel click behavior
  connect(ui_->monai_params_open_button, &QPushButton::toggled, ui_->monai_params_content, &QWidget::setVisible);
  connect(ui_->monai_params_header, &QPushButton::clicked, ui_->monai_params_open_button, &QPushButton::toggle);


  connect(ui_->load_server_button, &QPushButton::clicked, this, &MonaiLabelTool::loadServer);
  connect(ui_->restore_defaults_button, &QPushButton::clicked, this, &MonaiLabelTool::restoreDefaults);

  connect(ui_->data_open_button, &QPushButton::clicked, this, &MonaiLabelTool::update_panels);
  connect(ui_->controls_open_button, &QPushButton::clicked, this, &MonaiLabelTool::update_panels);
  connect(ui_->training_open_button, &QPushButton::clicked, this, &MonaiLabelTool::update_panels);

  QRegularExpression urlRegex(
        R"(https?://(?:www\.)?[^\s/$.?#].[^\s]*)",
        QRegularExpression::CaseInsensitiveOption);
  QRegularExpressionValidator *urlValidator = new QRegularExpressionValidator(urlRegex, &lineEdit);
  ui_->server_address->setValidator(urlValidator);

  connect(ui_->validation_split, &QLineEdit::editingFinished, this, &MonaiLabelTool::update_split);
  connect(ui_->testing_split, &QLineEdit::editingFinished, this, &MonaiLabelTool::update_split);



  Style::apply_normal_button_style(ui_->restore_defaults);

  update_panels();
}

//---------------------------------------------------------------------------
MonaiLabelTool::~MonaiLabelTool() {//remove tmp dirs for monai output}

//---------------------------------------------------------------------------
void MonaiLabelTool::set_session(QSharedPointer<Session> session) {
  session_ = session;

  ui_->scribble_brush_size_->setValue(static_cast<int>(session->get_scribble_paint_size()));
  ui_->scribble_brush_size_->setValue(static_cast<int>(session->get_scribble_paint_size()));
  ui_->scribble_inclusive_mode_->setChecked(session->get_scribble_mode_inclusive());
  ui_->scribble_exclusive_mode_->setChecked(!session->get_scribble_mode_inclusive());
  ui_->scribble_painting_active_->setChecked(session->get_scribble_paint_active());

  connect(session.data(), &Session::scribble_changed, this, &MonaiLabelTool::update_scribble_table);
  connect(ui_->scribble_brush_size_, &CustomSlider::valueChanged, session.data(), &Session::set_ffc_paint_size);
  connect(ui_->scribble_inclusive_mode_, &QRadioButton::toggled, session.data(), &Session::set_ffc_paint_mode_inclusive);
  connect(ui_->scribble_painting_active_, &QCheckBox::toggled, session.data(), &Session::set_ffc_paint_active);
  load_params();
  update_panels();
  update_meshes();
}

//---------------------------------------------------------------------------
void MonaiLabelTool::set_app(ShapeWorksStudioApp* app) { app_ = app; }

//---------------------------------------------------------------------------
void MonaiLabelTool::load_params() {
  auto params = MonaiParameters(session_->get_project());

  ui_->server_address->setText(QString::string(params.get_server_address()));
  update_panels();
  update_meshes();
}

//---------------------------------------------------------------------------
void MonaiLabelTool::store_params() {
  auto params = DeepSSMParameters(session_->get_project());

  params.set_validation_split(ui_->validation_split->text().toDouble());
  params.set_testing_split(ui_->testing_split->text().toDouble());

  params.set_spacing(
      {ui_->spacing_x->text().toDouble(), ui_->spacing_y->text().toDouble(), ui_->spacing_z->text().toDouble()});

  params.set_aug_num_samples(ui_->num_samples->text().toInt());
  params.set_aug_percent_variability(ui_->percent_variability->text().toDouble() / 100.0);
  params.set_aug_sampler_type(ui_->sampler_type->currentText().toStdString());

  params.set_training_epochs(ui_->training_epochs->text().toInt());
  params.set_training_learning_rate(ui_->training_learning_rate->text().toDouble());
  params.set_training_decay_learning_rate(ui_->training_decay_learning->isChecked());
  params.set_training_fine_tuning(ui_->training_fine_tuning->isChecked());
  params.set_training_fine_tuning_epochs(ui_->training_fine_tuning_epochs->text().toInt());
  params.set_training_batch_size(ui_->training_batch_size->text().toInt());
  params.set_training_fine_tuning_learning_rate(ui_->training_fine_tuning_learning_rate->text().toDouble());

  params.set_tl_net_enabled(ui_->tl_net_enabled->isChecked());
  params.set_tl_net_ae_epochs(ui_->tl_ae_epochs->text().toInt());
  params.set_tl_net_tf_epochs(ui_->tl_tf_epochs->text().toInt());
  params.set_tl_net_joint_epochs(ui_->tl_joint_epochs->text().toInt());
  params.set_tl_net_alpha(ui_->tl_alpha->text().toDouble());
  params.set_tl_net_a_ae(ui_->tl_ae_a->text().toDouble());
  params.set_tl_net_c_ae(ui_->tl_ae_c->text().toDouble());
  params.set_tl_net_a_lat(ui_->tl_lat_a->text().toDouble());
  params.set_tl_net_c_lat(ui_->tl_lat_c->text().toDouble());

  params.set_loss_function(ui_->loss_function->currentText().toStdString());

  params.save_to_project();
}

//---------------------------------------------------------------------------
void MonaiLabelTool::shutdown() { app_->get_py_worker()->abort_job(); }

//---------------------------------------------------------------------------
bool MonaiLabelTool::is_active() { return session_ && session_->get_tool_state() == Session::MONAI_C }

//---------------------------------------------------------------------------
void MonaiLabelTool::loadServer() {
  if (tool_is_running_) {
    ui_->run_button->setText("Aborting...");
    ui_->run_button->setEnabled(false);
    deep_ssm_->abort();
    app_->get_py_worker()->abort_job();
  } else {
    session_->trigger_save();
    if (ui_->run_all->isChecked()) {
      run_tool(MonaiLabelTool::ToolMode::DeepSSM_PrepType);
    } else {
      run_tool(current_tool_);
    }
  }
}


//---------------------------------------------------------------------------
void MonaiLabelTool::handle_thread_complete() {
  try {
    if (!deep_ssm_->is_aborted()) {
      if (current_tool_ == MonaiLabelTool::ToolMode::DeepSSM_PrepType) {
        auto params = DeepSSMParameters(session_->get_project());
        params.set_prep_stage(static_cast<int>(prep_step_));
        if (prep_step_ == MonaiLabelTool::PrepStep::NOT_STARTED || prep_step_ == MonaiLabelTool::PrepStep::GROOM_IMAGES) {
          params.set_prep_step_complete(true);
          params.set_prep_stage(static_cast<int>(MonaiLabelTool::PrepStep::DONE));
        }
        params.save_to_project();
        update_panels();
        prep_step_ = MonaiLabelTool::PrepStep::NOT_STARTED;
      }
    }
    Q_EMIT progress(100);
    update_meshes();
    tool_is_running_ = false;
    update_panels();
    session_->reload_particles();

    if (!deep_ssm_->is_aborted()) {
      if (ui_->run_all->isChecked()) {
        if (current_tool_ == ToolMode::DeepSSM_PrepType) {
          run_tool(MonaiLabelTool::ToolMode::DeepSSM_AugmentationType);
        } else if (current_tool_ == ToolMode::DeepSSM_AugmentationType) {
          run_tool(MonaiLabelTool::ToolMode::DeepSSM_TrainingType);
        } else if (current_tool_ == ToolMode::DeepSSM_TrainingType) {
          run_tool(MonaiLabelTool::ToolMode::DeepSSM_TestingType);
        }
      }
    }
  } catch (std::exception& e) {
    SW_ERROR("{}", e.what());
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handle_progress(int val, QString message) {
  update_meshes();
  SW_PROGRESS(val, message.toStdString());
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handle_error(QString msg) {}

//---------------------------------------------------------------------------
void MonaiLabelTool::update_panels() {
  ui_->data_content->setVisible(true);
  ui_->restore_defaults_button->setEnabled(!tool_is_running_);

  if (!session_ || !session_->get_project()) {
    return;
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::handle_new_mesh() {
  if (current_tool_ == MonaiLabelTool::ToolMode::DeepSSM_TestingType) {
    update_testing_meshes();
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::show_training_meshes() {
  shapes_.clear();

  QStringList filenames;
  filenames << "deepssm/model/examples/validation_best.particles";
  filenames << "deepssm/model/examples/validation_median.particles";
  filenames << "deepssm/model/examples/validation_worst.particles";
  filenames << "deepssm/model/examples/train_best.particles";
  filenames << "deepssm/model/examples/train_median.particles";
  filenames << "deepssm/model/examples/train_worst.particles";

  QStringList names;
  names << "best validation"
        << "median validation"
        << "worst validation";
  names << "best training"
        << "median training"
        << "worst training";

  QStringList scalar_filenames;
  scalar_filenames << "deepssm/model/examples/validation_best.scalars";
  scalar_filenames << "deepssm/model/examples/validation_median.scalars";
  scalar_filenames << "deepssm/model/examples/validation_worst.scalars";
  scalar_filenames << "deepssm/model/examples/train_best.scalars";
  scalar_filenames << "deepssm/model/examples/train_median.scalars";
  scalar_filenames << "deepssm/model/examples/train_worst.scalars";

  QStringList index_filenames;
  index_filenames << "deepssm/model/examples/validation_best.index";
  index_filenames << "deepssm/model/examples/validation_median.index";
  index_filenames << "deepssm/model/examples/validation_worst.index";
  index_filenames << "deepssm/model/examples/train_best.index";
  index_filenames << "deepssm/model/examples/train_median.index";
  index_filenames << "deepssm/model/examples/train_worst.index";

  std::vector<bool> validation = {true, true, true, false, false, false};

  auto all_shapes = session_->get_shapes();
  auto all_subjects = session_->get_project()->get_subjects();

  for (int i = 0; i < names.size(); i++) {
    if (QFileInfo::exists(filenames[i])) {
      ShapeHandle shape = ShapeHandle(new Shape());
      auto subject = std::make_shared<Subject>();
      shape->set_subject(subject);
      shape->set_mesh_manager(session_->get_mesh_manager());
      shape->import_local_point_files({filenames[i].toStdString()});
      shape->import_global_point_files({filenames[i].toStdString()});
      shape->load_feature_from_scalar_file(scalar_filenames[i].toStdString(), "deepssm_error");
      shape->get_reconstructed_meshes();

      // read index from file
      std::ifstream index_file(index_filenames[i].toStdString());
      if (index_file.is_open()) {
        std::string index_string;
        std::getline(index_file, index_string);
        index_file.close();
        std::string image_filename = index_string;

        if (validation[i]) {
          image_filename = "deepssm/val_and_test_images/" + image_filename + ".nrrd";
        } else {
          if (image_filename.find("Generated") != std::string::npos) {
            image_filename = "deepssm/augmentation/Generated-Images/" + image_filename + ".nrrd";
          } else {
            image_filename = "deepssm/train_images/" + image_filename + ".nrrd";
          }
        }

        set_subject_image_filename(subject, image_filename);
      }

      std::vector<std::string> list;
      list.push_back(names[i].toStdString());
      list.push_back("");
      list.push_back("");
      list.push_back("");
      shape->set_annotations(list);

      shapes_.push_back(shape);
    }
  }
  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void MonaiLabelTool::show_testing_meshes() {
  shapes_.clear();
  deep_ssm_ = QSharedPointer<DeepSSMJob>::create(session_, MonaiLabelTool::ToolMode::DeepSSM_TestingType);
  auto id_list = get_split(session_->get_project(), SplitType::TEST);

  auto subjects = session_->get_project()->get_subjects();
  auto shapes = session_->get_shapes();

  std::string feature_name = session_->get_image_name();
  auto image_names = session_->get_project()->get_image_names();
  if (image_names.size() > 0) {
    feature_name = image_names[0];
  }

  for (auto& id : id_list) {
    QString filename =
        QString("deepssm/model/test_predictions/world_predictions/") + QString::number(id) + ".particles";

    QString mesh_filename = "deepssm/model/test_predictions/world_predictions/" + QString::number(id) + ".vtk";

    if (QFileInfo::exists(filename) && QFileInfo::exists(mesh_filename)) {
      ShapeHandle shape = ShapeHandle(new Shape());
      auto subject = std::make_shared<Subject>();
      subject->set_display_name(shapes[id]->get_display_name());
      shape->set_subject(subject);
      shape->set_mesh_manager(session_->get_mesh_manager());
      shape->import_local_point_files({filename.toStdString()});
      shape->import_global_point_files({filename.toStdString()});

      Mesh mesh(mesh_filename.toStdString());
      MeshGroup mesh_group;
      mesh_group.set_number_of_meshes(1);
      MeshHandle sw_mesh = std::make_shared<StudioMesh>();
      sw_mesh->set_poly_data(mesh.getVTKMesh());

      mesh_group.set_mesh(0, sw_mesh);
      shape->set_reconstructed_meshes(mesh_group);

      auto image_filename = "deepssm/val_and_test_images/" + std::to_string(id) + ".nrrd";

      set_subject_image_filename(subject, image_filename);

      shape->get_reconstructed_meshes();
      std::vector<std::string> list;
      list.push_back(shapes[id]->get_annotations()[0]);
      list.push_back("");
      list.push_back("");
      list.push_back("");
      shape->set_annotations(list);

      shapes_.push_back(shape);
    }
  }

  update_testing_meshes();

  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void MonaiLabelTool::update_testing_meshes() {
  try {
    deep_ssm_ = QSharedPointer<DeepSSMJob>::create(session_, MonaiLabelTool::ToolMode::DeepSSM_TestingType);
    auto id_list = get_split(session_->get_project(), SplitType::TEST);

    auto subjects = session_->get_project()->get_subjects();
    auto shapes = session_->get_shapes();

    populate_table_from_csv(ui_->testing_table, "deepssm/test_distances.csv", true);

  } catch (std::exception& e) {
    SW_ERROR("{}", e.what());
  }

  Q_EMIT update_view();
}

//---------------------------------------------------------------------------
void MonaiLabelTool::update_meshes() {
  if (!is_active()) {
    return;
  }
  switch (current_tool_) {
    case MonaiLabelTool::ToolMode::DeepSSM_PrepType:
      shapes_.clear();
      Q_EMIT update_view();
      break;
    case MonaiLabelTool::ToolMode::DeepSSM_AugmentationType:
      show_augmentation_meshes();
      break;
    case MonaiLabelTool::ToolMode::DeepSSM_TrainingType:
      show_training_meshes();
      break;
    case MonaiLabelTool::ToolMode::DeepSSM_TestingType:
      show_testing_meshes();
      break;
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::show_augmentation_meshes() {
  try {
    update_tables();
    shapes_.clear();

    QString filename = "deepssm/augmentation/TotalData.csv";
    if (QFile(filename).exists()) {
      QFile file(filename);

      if (!file.open(QIODevice::ReadOnly)) {
        SW_ERROR("Unable to open file: " + filename.toStdString());
        return;
      }

      int row = 0;
      while (!file.atEnd()) {
        QByteArray line = file.readLine();
        bool show_generated = ui_->generated_data_checkbox->isChecked();
        bool show_original = ui_->original_data_checkbox->isChecked();
        // this needs to be replaced with it's own column (in all the python code as well)
        bool is_generated = line.contains("Generated");
        if ((is_generated && show_generated) || (!is_generated && show_original)) {
          auto image_filename = line.split(',')[0].toStdString();
          std::string particle_file = (line.split(',')[1]).toStdString();

          auto subject = std::make_shared<Subject>();
          ShapeHandle shape = ShapeHandle(new Shape());
          shape->set_subject(subject);
          shape->set_mesh_manager(session_->get_mesh_manager());
          shape->import_local_point_files({particle_file});
          shape->import_global_point_files({particle_file});

          set_subject_image_filename(subject, image_filename);

          shape->get_reconstructed_meshes();

          std::vector<std::string> list;
          list.push_back((QFileInfo(QString::fromStdString(particle_file)).baseName()).toStdString());
          list.push_back("");
          list.push_back("");
          list.push_back("");
          shape->set_annotations(list);

          shapes_.push_back(shape);
          row++;
        }
      }
    }
    load_plots();
    Q_EMIT update_view();
  } catch (std::exception& e) {
    SW_ERROR("{}", e.what());
  }
}

//---------------------------------------------------------------------------
ShapeList MonaiLabelTool::get_shapes() { return shapes_; }

//---------------------------------------------------------------------------
void MonaiLabelTool::resize_plots() {
  set_plot(ui_->violin_plot, violin_plot_);
  if (ui_->tl_net_enabled->isChecked()) {
    set_plot(ui_->training_plot_2, training_plot_tl1_);
    set_plot(ui_->training_plot_3, training_plot_tl2_);
    set_plot(ui_->training_plot_4, training_plot_tl3_);
    set_plot(ui_->training_plot, QPixmap{});
  } else {
    set_plot(ui_->training_plot, training_plot_);
    set_plot(ui_->training_plot_2, training_plot_ft_);
    set_plot(ui_->training_plot_3, QPixmap{});
    set_plot(ui_->training_plot_4, QPixmap{});
  }
}

//---------------------------------------------------------------------------
std::string MonaiLabelTool::get_feature_name() {
  std::string feature_name = session_->get_image_name();
  auto image_names = session_->get_project()->get_image_names();
  if (image_names.size() > 0) {
    feature_name = image_names[0];
  }
  return feature_name;
}

//---------------------------------------------------------------------------
void MonaiLabelTool::set_subject_image_filename(const std::shared_ptr<Subject>& subject, const std::string& filename) {
  // if the filename doesn't exist, print a warning
  if (!QFileInfo(QString::fromStdString(filename)).exists()) {
    SW_WARN("File doesn't exist: {}", filename);
  } else {
    project::types::StringMap map;
    map[get_feature_name()] = filename;
    subject->set_feature_filenames(map);
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::set_plot(QLabel* qlabel, QPixmap pixmap) {
  if (!pixmap.isNull()) {
    QPixmap resized = pixmap.scaledToWidth(width() * 0.95, Qt::SmoothTransformation);
    qlabel->setPixmap(resized);
  } else {
    qlabel->setPixmap(QPixmap{});
  }
}

//---------------------------------------------------------------------------
void MonaiLabelTool::resizeEvent(QResizeEvent* event) {
  QWidget::resizeEvent(event);
  resize_plots();
}

//---------------------------------------------------------------------------
std::string MonaiLabelTool::get_display_feature() {
  if (current_tool_ == MonaiLabelTool::ToolMode::DeepSSM_TrainingType ||
      current_tool_ == MonaiLabelTool::ToolMode::DeepSSM_TestingType) {
    return "deepssm_error";
  }
  return "";
}

//---------------------------------------------------------------------------
std::vector<int> MonaiLabelTool::get_split(ProjectHandle project, SplitType split_type) {
  auto subjects = project->get_subjects();

  std::vector<int> list;

  for (int id = 0; id < subjects.size(); id++) {
    auto extra_values = subjects[id]->get_extra_values();

    std::string split = extra_values["split"];

    if (split_type == SplitType::TRAIN) {
      if (split != "train") {
        continue;
      }
    } else if (split_type == SplitType::VAL) {
      if (split != "val") {
        continue;
      }
    } else if (split_type == SplitType::TEST) {
      if (split != "test") {
        continue;
      }
    }

    list.push_back(id);
  }
  return list;
}

//---------------------------------------------------------------------------
void MonaiLabelTool::restoreDefaults() {

}

//---------------------------------------------------------------------------
void MonaiLabelTool::run_tool(MonaiLabelTool::ToolMode type) {
  current_tool_ = type;
  Q_EMIT progress(-1);

  if (type == MonaiLabelTool::ToolMode::DeepSSM_AugmentationType) {
    ui_->tab_widget->setCurrentIndex(1);

    SW_LOG("Please Wait: Running Data Augmentation...");
    // clean
    QFile("deepssm/augmentation/TotalData.csv").remove();
  } else if (type == MonaiLabelTool::ToolMode::DeepSSM_TrainingType) {
    ui_->tab_widget->setCurrentIndex(2);
    SW_LOG("Please Wait: Running Training...");

    // clean
    QDir dir("deepssm/model");
    dir.removeRecursively();

    show_training_meshes();
  } else if (type == MonaiTool::ToolMode::DeepSSM_TestingType) {
    ui_->tab_widget->setCurrentIndex(3);

    SW_LOG("Please Wait: Running Testing...");
  } else if (type == MonaiTool::ToolMode::DeepSSM_PrepType) {
    ui_->tab_widget->setCurrentIndex(0);

    SW_LOG("Please Wait: Running Groom/Optimize...");
  } else {
    SW_ERROR("Unknown tool mode");
    Q_EMIT progress(100);
    return;
  }

  load_plots();
  update_tables();

  timer_.start();

  tool_is_running_ = true;
  update_panels();

  store_params();

  deep_ssm_ = QSharedPointer<DeepSSMJob>::create(session_, type, prep_step_);
  connect(deep_ssm_.data(), &DeepSSMJob::progress, this, &MonaiTool::handle_progress);
  connect(deep_ssm_.data(), &DeepSSMJob::finished, this, &MonaiTool::handle_thread_complete);

  app_->get_py_worker()->run_job(deep_ssm_);

  // ensure someone doesn't accidentally abort right after clicking RUN
  ui_->run_button->setEnabled(false);
  QTimer::singleShot(1000, this, [=]() { ui_->run_button->setEnabled(true); });
}

//---------------------------------------------------------------------------
}  // namespace shapeworks
