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
#include <Data/Shape.h>
#include <DeepSSM/DeepSSMParameters.h>
#include <DeepSSM/QDeepSSM.h>
#include <Interface/Style.h>

#include <ui_DeepSSMTool.h>

namespace shapeworks {

//---------------------------------------------------------------------------
DeepSSMTool::DeepSSMTool(Preferences& prefs) : preferences_(prefs)
{
  this->ui_ = new Ui_DeepSSMTool;
  this->ui_->setupUi(this);

  connect(this->ui_->run_button, &QPushButton::clicked,
          this, &DeepSSMTool::run_clicked);
  connect(this->ui_->restore_defaults, &QPushButton::clicked,
          this, &DeepSSMTool::restore_defaults);

  connect(this->ui_->data_open_button, &QPushButton::clicked,
          this, &DeepSSMTool::update_panels);
  connect(this->ui_->controls_open_button, &QPushButton::clicked,
          this, &DeepSSMTool::update_panels);
  connect(this->ui_->training_open_button, &QPushButton::clicked,
          this, &DeepSSMTool::update_panels);

  connect(this->ui_->generated_data_checkbox, &QCheckBox::stateChanged,
          this, &DeepSSMTool::show_augmentation_meshes);
  connect(this->ui_->original_data_checkbox, &QCheckBox::stateChanged,
          this, &DeepSSMTool::show_augmentation_meshes);

  this->py_worker = QSharedPointer<PythonWorker>::create();

  connect(this->py_worker.data(), &PythonWorker::job_finished,
          this, &DeepSSMTool::handle_thread_complete);
  connect(this->py_worker.data(), &PythonWorker::message,
          this, &DeepSSMTool::message);
  connect(this->py_worker.data(), &PythonWorker::error_message,
          this, &DeepSSMTool::error);
  connect(this->py_worker.data(), &PythonWorker::progress,
          this, &DeepSSMTool::handle_progress);

  connect(this->ui_->tab_widget, &QTabWidget::currentChanged, this, &DeepSSMTool::tab_changed);

  QIntValidator* zero_to_hundred = new QIntValidator(0, 100, this);
  this->ui_->validation_split->setValidator(zero_to_hundred);
  this->ui_->testing_split->setValidator(zero_to_hundred);

  connect(this->ui_->validation_split, &QLineEdit::textChanged,
          this, [ = ]() {
      this->update_split(this->ui_->validation_split);
    });
  connect(this->ui_->testing_split, &QLineEdit::textChanged,
          this, [ = ]() {
      this->update_split(this->ui_->testing_split);
    });

  this->ui_->tab_widget->setCurrentIndex(0);
  this->tab_changed(0);

  Style::apply_normal_button_style(this->ui_->restore_defaults);
  this->ui_->violin_plot->setText("");
  this->ui_->training_plot->setText("");
  this->update_panels();
}

//---------------------------------------------------------------------------
void DeepSSMTool::tab_changed(int tab)
{
  switch (tab) {
  case 0:
    this->current_tool_ = PythonWorker::JobType::DeepSSM_SplitType;
    break;
  case 1:
    this->current_tool_ = PythonWorker::JobType::DeepSSM_AugmentationType;
    break;
  case 2:
    this->current_tool_ = PythonWorker::JobType::DeepSSM_TrainingType;
    break;
  case 3:
    this->current_tool_ = PythonWorker::JobType::DeepSSM_TestingType;
    break;
  }
  this->update_panels();
  this->update_meshes();
}

//---------------------------------------------------------------------------
DeepSSMTool::~DeepSSMTool()
{}

//---------------------------------------------------------------------------
void DeepSSMTool::set_session(QSharedPointer<Session> session)
{
  this->session_ = session;
  this->load_params();
  this->update_meshes();
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

  this->ui_->validation_split->setText(QString::number(params.get_validation_split()));
  this->ui_->testing_split->setText(QString::number(params.get_testing_split()));

  this->ui_->num_samples->setText(QString::number(params.get_aug_num_samples()));
  this->ui_->num_pca_dims->setText(QString::number(params.get_aug_num_dims()));
  this->ui_->percent_variability->setText(QString::number(params.get_aug_percent_variability()));
  this->ui_->sampler_type->setCurrentText(QString::fromStdString(params.get_aug_sampler_type()));

  this->ui_->training_epochs->setText(QString::number(params.get_training_epochs()));
  this->ui_->training_learning_rate->setText(QString::number(params.get_training_learning_rate()));
  this->ui_->training_decay_learning->setChecked(params.get_training_decay_learning_rate());
  this->ui_->training_fine_tuning->setChecked(params.get_training_fine_tuning());

  this->update_meshes();
}

//---------------------------------------------------------------------------
void DeepSSMTool::store_params()
{
  auto params = DeepSSMParameters(this->session_->get_project());

  params.set_validation_split(this->ui_->validation_split->text().toDouble());
  params.set_testing_split(this->ui_->testing_split->text().toDouble());

  params.set_aug_num_samples(this->ui_->num_samples->text().toInt());
  params.set_aug_num_dims(this->ui_->num_pca_dims->text().toInt());
  params.set_aug_percent_variability(this->ui_->percent_variability->text().toDouble());
  params.set_aug_sampler_type(this->ui_->sampler_type->currentText().toStdString());

  params.set_training_epochs(this->ui_->training_epochs->text().toInt());
  params.set_training_learning_rate(this->ui_->training_learning_rate->text().toDouble());
  params.set_training_decay_learning_rate(this->ui_->training_decay_learning->isChecked());
  params.set_training_fine_tuning(this->ui_->training_fine_tuning->isChecked());

  params.save_to_project();
}

//---------------------------------------------------------------------------
void DeepSSMTool::shutdown()
{
  this->py_worker->abort_job();
}

//---------------------------------------------------------------------------
void DeepSSMTool::set_active(bool active)
{}

//---------------------------------------------------------------------------
bool DeepSSMTool::get_active()
{
  return false;
}

//---------------------------------------------------------------------------
void DeepSSMTool::run_clicked()
{
  if (this->tool_is_running_) {
    this->ui_->run_button->setText("Aborting...");
    this->ui_->run_button->setEnabled(false);
    this->py_worker->abort_job();
  }
  else {
    this->run_tool(this->current_tool_);
  }
}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_thread_complete()
{
  emit progress(100);
  QString duration = QString::number(this->timer_.elapsed() / 1000.0, 'f', 1);

  if (this->current_tool_ == PythonWorker::JobType::DeepSSM_AugmentationType) {
    emit message("Data Augmentation Complete.  Duration: " + duration + " seconds");
  }
  else if (this->current_tool_ == PythonWorker::JobType::DeepSSM_TrainingType) {
    emit message("Training Complete.  Duration: " + duration + " seconds");
  }
  else {
    emit message("Testing Complete.  Duration: " + duration + " seconds");
  }

  this->update_meshes();
  this->tool_is_running_ = false;
  this->update_panels();
}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_progress(int val)
{
  this->load_plots();
  this->update_tables();
  this->update_meshes();
  emit progress(val);
}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_error(QString msg)
{}

//---------------------------------------------------------------------------
void DeepSSMTool::update_panels()
{
  this->ui_->data_content->setVisible(this->ui_->data_open_button->isChecked());
  this->ui_->controls_content->setVisible(this->ui_->controls_open_button->isChecked());
  this->ui_->training_content->setVisible(this->ui_->training_open_button->isChecked());
  this->ui_->tab_widget->setEnabled(!this->tool_is_running_);
  this->ui_->restore_defaults->setEnabled(!this->tool_is_running_);

  QString string = "";
  this->ui_->run_button->show();
  this->ui_->data_panel->hide();
  this->ui_->training_panel->hide();
  switch (this->current_tool_) {
  case PythonWorker::JobType::DeepSSM_SplitType:
    this->ui_->run_button->hide();
    break;
  case PythonWorker::JobType::DeepSSM_AugmentationType:
    string = "Data Augmentation";
    this->ui_->data_panel->show();
    break;
  case PythonWorker::JobType::DeepSSM_TrainingType:
    string = "Training";
    this->ui_->training_panel->show();
    break;
  case PythonWorker::JobType::DeepSSM_TestingType:
    string = "Testing";
    break;
  }

  if (this->tool_is_running_) {
    Style::apply_abort_button_style(this->ui_->run_button);
    this->ui_->run_button->setText("Abort");
  }
  else {
    Style::apply_normal_button_style(this->ui_->run_button);
    this->ui_->run_button->setText("Run " + string);
  }
  this->ui_->run_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void DeepSSMTool::update_split(QLineEdit* source)
{}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_new_mesh()
{
  if (this->current_tool_ == PythonWorker::JobType::DeepSSM_TestingType) {
    this->update_testing_meshes();
  }
}

//---------------------------------------------------------------------------
void DeepSSMTool::update_tables()
{
  this->populate_table_from_csv(this->ui_->training_table, "deepssm/model/train_log.csv", true);
  this->populate_table_from_csv(this->ui_->table, "deepssm/Augmentation/TotalData.csv", false);
}

//---------------------------------------------------------------------------
void DeepSSMTool::populate_table_from_csv(QTableWidget* table, QString filename, bool header)
{
  table->clear();
  if (!QFile(filename).exists()) {
    return;
  }
  QFile file(filename);
  if (!file.open(QIODevice::ReadOnly)) {
    emit error("Unable to open file: " + filename);
    return;
  }

  auto data = QString(file.readAll()).trimmed();
  auto lines = data.split('\n');
  file.close();

  if (lines.empty()) {
    return;
  }
  auto headers = lines[0].split(',');
  table->setColumnCount(headers.size());
  table->horizontalHeader()->setVisible(true);
  if (header) {
    table->setHorizontalHeaderLabels(headers);
    lines.pop_front();
    table->verticalHeader()->setVisible(false);
  }
  else {
    table->verticalHeader()->setVisible(true);
  }
  table->setRowCount(lines.size());

  int row = 0;
  for (auto line : lines) {
    int col = 0;
    for (auto item : line.split(',')) {
      item = item.trimmed();
      if (item != "") {
        QTableWidgetItem* new_item = new QTableWidgetItem(QString(item));
        table->setItem(row, col++, new_item);
      }
    }
    row++;
  }
}

//---------------------------------------------------------------------------
void DeepSSMTool::show_training_meshes()
{
  this->shapes_.clear();

  QStringList filenames;
  filenames << "deepssm/model/val_examples/best_validation.particles";
  filenames << "deepssm/model/val_examples/mean_validation.particles";
  filenames << "deepssm/model/val_examples/worst_validation.particles";

  QStringList names;
  names << "best" << "mean" << "worst";

  QStringList scalar_filenames;
  scalar_filenames << "deepssm/model/val_examples/best_validation.scalars";
  scalar_filenames << "deepssm/model/val_examples/mean_validation.scalars";
  scalar_filenames << "deepssm/model/val_examples/worst_validation.scalars";

  for (int i = 0; i < names.size(); i++) {
    if (QFileInfo(filenames[i]).exists()) {
      ShapeHandle shape = ShapeHandle(new Shape());
      auto subject = std::make_shared<Subject>();
      shape->set_subject(subject);
      shape->set_mesh_manager(this->session_->get_mesh_manager());
      shape->import_local_point_files({filenames[i].toStdString()});
      shape->import_global_point_files({filenames[i].toStdString()});
      shape->load_feature_from_scalar_file(scalar_filenames[i].toStdString(),
                                           "deepssm_error");
      shape->get_reconstructed_meshes();
      QStringList list;
      list << names[i];
      list << "";
      list << "";
      list << "";
      shape->set_annotations(list);

      this->shapes_.push_back(shape);
    }
  }
  emit update_view();
}

//---------------------------------------------------------------------------
void DeepSSMTool::show_testing_meshes()
{
  this->shapes_.clear();
  this->deep_ssm_ = QSharedPointer<QDeepSSM>::create(session_->get_project());
  auto id_list = this->deep_ssm_->get_list(QDeepSSM::FileType::ID, QDeepSSM::SplitType::TEST);

  auto subjects = this->session_->get_project()->get_subjects();
  auto shapes = this->session_->get_shapes();

  for (auto& id : id_list) {
    int i = QString::fromStdString(id).toInt();

    auto mesh_group = shapes[i]->get_groomed_meshes(true);
    std::string filename = "deepssm/model/predictions/FT_Predictions/predicted_ft_" + id +
                           ".particles";

    if (QFileInfo(QString::fromStdString(filename)).exists()) {
      ShapeHandle shape = ShapeHandle(new Shape());
      auto subject = std::make_shared<Subject>();
      shape->set_subject(subject);
      shape->set_mesh_manager(this->session_->get_mesh_manager());
      shape->import_local_point_files({filename});
      shape->import_global_point_files({filename});
      shape->get_reconstructed_meshes();
      QStringList list;
      list << shapes[i]->get_annotations()[0];
      list << "";
      list << "";
      list << "";
      shape->set_annotations(list);

      this->shapes_.push_back(shape);
    }
  }

  this->update_testing_meshes();

  emit update_view();
}

//---------------------------------------------------------------------------
void DeepSSMTool::update_testing_meshes()
{
  this->deep_ssm_ = QSharedPointer<QDeepSSM>::create(session_->get_project());
  auto id_list = this->deep_ssm_->get_list(QDeepSSM::FileType::ID, QDeepSSM::SplitType::TEST);

  auto subjects = this->session_->get_project()->get_subjects();
  auto shapes = this->session_->get_shapes();

  QTableWidget* table = this->ui_->testing_table;

  table->clear();
  QStringList headers;
  headers << "name" << "average distance";
  table->setColumnCount(headers.size());
  table->horizontalHeader()->setVisible(true);
  table->setHorizontalHeaderLabels(headers);
  table->verticalHeader()->setVisible(false);
  table->setRowCount(this->shapes_.size());

  int idx = 0;
  for (auto& id : id_list) {
    int i = QString::fromStdString(id).toInt();
    auto name = shapes[i]->get_annotations()[0];

    QTableWidgetItem* new_item = new QTableWidgetItem(QString(name));
    table->setItem(idx, 0, new_item);

    auto mesh_group = shapes[i]->get_groomed_meshes(true);
    if (mesh_group.valid()) {
      Mesh base(mesh_group.meshes()[0]->get_poly_data());
      std::string filename = "deepssm/model/predictions/FT_Predictions/predicted_ft_" + id +
                             ".particles";
      if (QFileInfo(QString::fromStdString(filename)).exists()) {
        if (idx < this->shapes_.size()) {
          auto shape = this->shapes_[idx++];
          MeshGroup group = shape->get_reconstructed_meshes();
          if (group.valid()) {
            Mesh m(group.meshes()[0]->get_poly_data());
            m.distance(base);
            double average_distance = m.getFieldMean("distance");

            QTableWidgetItem* new_item = new QTableWidgetItem(QString::number(average_distance));
            table->setItem(idx - 1, 1, new_item);

            auto field = m.getField<vtkDataArray>("distance");
            field->SetName("deepssm_error");
            group.meshes()[0]->get_poly_data()->GetPointData()->AddArray(field);
          }
          else {
            QTableWidgetItem* new_item = new QTableWidgetItem("computing...");
            table->setItem(idx - 1, 1, new_item);
          }
        }
      }
    }
  }

  emit update_view();
}

//---------------------------------------------------------------------------
void DeepSSMTool::update_meshes()
{
  switch (this->current_tool_) {
  case PythonWorker::JobType::DeepSSM_SplitType:
    this->shapes_.clear();
    emit update_view();
    break;
  case PythonWorker::JobType::DeepSSM_AugmentationType:
    this->show_augmentation_meshes();
    break;
  case PythonWorker::JobType::DeepSSM_TrainingType:
    this->show_training_meshes();
    break;
  case PythonWorker::JobType::DeepSSM_TestingType:
    this->show_testing_meshes();
    break;
  }
}

//---------------------------------------------------------------------------
void DeepSSMTool::show_augmentation_meshes()
{
  this->update_tables();
  this->shapes_.clear();

  QString filename = "deepssm/Augmentation/TotalData.csv";
  if (QFile(filename).exists()) {

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
      emit error("Unable to open file: " + filename);
      return;
    }

    int row = 0;
    while (!file.atEnd()) {
      QByteArray line = file.readLine();
      bool show_generated = this->ui_->generated_data_checkbox->isChecked();
      bool show_original = this->ui_->original_data_checkbox->isChecked();
      // this needs to be replaced with it's own column (in all the python code as well)
      bool is_generated = line.contains("Generated");
      if ((is_generated && show_generated) || (!is_generated && show_original)) {
        auto image_file = line.split(',')[0].toStdString();
        auto particle_file = line.split(',')[1].toStdString();

        auto subject = std::make_shared<Subject>();
        ShapeHandle shape = ShapeHandle(new Shape());
        shape->set_subject(subject);
        shape->set_mesh_manager(this->session_->get_mesh_manager());
        shape->import_local_point_files({particle_file});
        shape->import_global_point_files({particle_file});

        shape->get_reconstructed_meshes();

        QStringList list;
        list << QFileInfo(QString::fromStdString(particle_file)).baseName();
        list << "";
        list << "";
        list << "";
        shape->set_annotations(list);

        this->shapes_.push_back(shape);
        row++;
      }
    }
  }
  this->load_plots();
  emit update_view();
}

//---------------------------------------------------------------------------
QVector<QSharedPointer<Shape>> DeepSSMTool::get_shapes()
{
  return this->shapes_;
}

//---------------------------------------------------------------------------
void DeepSSMTool::load_plots()
{
  this->violin_plot_ = this->load_plot("deepssm/Augmentation/violin.png");
  this->training_plot_ = this->load_plot("deepssm/model/training_plot.png");
  this->resize_plots();
}

//---------------------------------------------------------------------------
QPixmap DeepSSMTool::load_plot(QString filename)
{
  if (QFile(filename).exists()) {
    return QPixmap(filename);
  }
  return QPixmap();
}

//---------------------------------------------------------------------------
void DeepSSMTool::resize_plots()
{
  this->set_plot(this->ui_->violin_plot, this->violin_plot_);
  this->set_plot(this->ui_->training_plot, this->training_plot_);
}

//---------------------------------------------------------------------------
void DeepSSMTool::set_plot(QLabel* qlabel, QPixmap pixmap)
{
  if (!pixmap.isNull()) {
    QPixmap resized = pixmap.scaledToWidth(this->width() * 0.95,
                                           Qt::SmoothTransformation);
    qlabel->setPixmap(resized);
  }
  else {
    qlabel->setPixmap(QPixmap{});
  }
}

//---------------------------------------------------------------------------
void DeepSSMTool::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);
  this->resize_plots();
}

//---------------------------------------------------------------------------
string DeepSSMTool::get_display_feature()
{
  if (this->current_tool_ == PythonWorker::JobType::DeepSSM_TrainingType ||
      this->current_tool_ == PythonWorker::JobType::DeepSSM_TestingType) {
    return "deepssm_error";
  }
  return "";
}

//---------------------------------------------------------------------------
void DeepSSMTool::restore_defaults()
{
  auto params = DeepSSMParameters(this->session_->get_project());

  switch (this->current_tool_) {
  case PythonWorker::JobType::DeepSSM_SplitType:
    params.restore_split_defaults();
    break;
  case PythonWorker::JobType::DeepSSM_AugmentationType:
    params.restore_augmentation_defaults();
    break;
  case PythonWorker::JobType::DeepSSM_TrainingType:
    params.restore_training_defaults();
    break;
  case PythonWorker::JobType::DeepSSM_TestingType:
    //params.restore_inference_defaults();
    break;
  }

  params.save_to_project();
  this->load_params();
}

//---------------------------------------------------------------------------
void DeepSSMTool::run_tool(PythonWorker::JobType type)
{
  this->current_tool_ = type;
  emit progress(-1);
  if (type == PythonWorker::JobType::DeepSSM_AugmentationType) {
    emit message("Please Wait: Running Data Augmentation...");
    // clean
    QFile("deepssm/Augmentation/TotalData.csv").remove();
  }
  else if (type == PythonWorker::JobType::DeepSSM_TrainingType) {
    emit message("Please Wait: Running Training...");
    // clean
    QDir dir("deepssm/model");
    dir.removeRecursively();

    this->show_training_meshes();
  }
  else {
    emit message("Please Wait: Running Testing...");
  }

  this->load_plots();
  this->update_tables();

  this->timer_.start();

  this->tool_is_running_ = true;
  this->update_panels();

  this->store_params();
  this->deep_ssm_ = QSharedPointer<QDeepSSM>::create(session_->get_project());

  connect(this->deep_ssm_.data(), &QDeepSSM::message, this, &DeepSSMTool::message);
  connect(this->deep_ssm_.data(), &QDeepSSM::error, this, &DeepSSMTool::error);
  connect(this->deep_ssm_.data(), &QDeepSSM::progress, this, &DeepSSMTool::handle_progress);

  this->py_worker->set_deep_ssm(this->deep_ssm_);

  this->py_worker->run_job(type);

  // ensure someone doesn't accidentally abort right after clicking RUN
  this->ui_->run_button->setEnabled(false);
  QTimer::singleShot(1000, [ = ]() {
      this->ui_->run_button->setEnabled(true);
    });
}

//---------------------------------------------------------------------------
}
