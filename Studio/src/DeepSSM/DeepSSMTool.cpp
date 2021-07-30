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

#include <ui_DeepSSMTool.h>

namespace shapeworks {

const QString normal_button_ss = R"(
QPushButton{
	background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:0.960227, stop:0 rgba(221, 221, 221, 255), stop:0.155779 rgba(238, 238, 238, 255), stop:1 rgba(192, 194, 194, 255));
	border-radius: 4px;
	border: 1px solid rgb(90, 90, 90);
}

QPushButton:hover{
	background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(195, 195, 195, 255), stop:0.253769 rgba(206, 206, 206, 255), stop:1 rgba(185, 185, 185, 255));
	border-radius: 4px;
	border: 1px solid rgb(90, 90, 90);
}

QPushButton:pressed{
	background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(150, 150, 150, 255), stop:0.753769 rgba(206, 206, 206, 255), stop:1 rgba(185, 185, 185, 255));
	border-radius: 4px;
	border: 1px solid rgb(90, 90, 90);
}
)";

const QString abort_button_ss = R"(
QPushButton#run_button{
	background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(98, 0, 0, 255), stop:0.299435 rgba(128, 0, 0, 255), stop:0.491525 rgba(128, 0, 0, 255), stop:1 rgba(98, 0, 0, 255));
	border-radius: 4px;
	border: 1px solid rgb(90, 90, 90);
	color: white;
}

QPushButton#run_button:disabled{
	background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:0.960227, stop:0 rgba(128, 128, 128, 255), stop:0.155779 rgba(96, 96, 96, 255), stop:1 rgba(96, 96, 96, 255));
	border-radius: 4px;
	border: 1px solid rgb(90, 90, 90);
}

QPushButton#run_button:hover{
	background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(80, 0, 0, 255), stop:0.299435 rgba(110, 0, 0, 255), stop:0.491525 rgba(110, 0, 0, 255), stop:1 rgba(80, 0, 0, 255));
	border-radius: 4px;
	border: 1px solid rgb(90, 90, 90);
	color: white;
}

QPushButton#run_button:pressed{
	background-color: qlineargradient(spread:pad, x1:0.5, y1:0, x2:0.5, y2:1, stop:0 rgba(50, 0, 0, 255), stop:0.299435 rgba(80, 0, 0, 255), stop:0.491525 rgba(80, 0, 0, 255), stop:1 rgba(50, 0, 0, 255));
	border-radius: 4px;
	border: 1px solid rgb(90, 90, 90);
}
)";

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
          this, &DeepSSMTool::update_data);
  connect(this->ui_->original_data_checkbox, &QCheckBox::stateChanged,
          this, &DeepSSMTool::update_data);

  this->py_worker = QSharedPointer<PythonWorker>::create();

  connect(this->py_worker.data(), &PythonWorker::job_finished, this,
          &DeepSSMTool::handle_thread_complete);
  connect(this->py_worker.data(), &PythonWorker::message,
          this, &DeepSSMTool::message);
  connect(this->py_worker.data(), &PythonWorker::error_message,
          this, &DeepSSMTool::error);
  connect(this->py_worker.data(), &PythonWorker::progress,
          this, &DeepSSMTool::handle_progress);

  connect(this->ui_->tab_widget, &QTabWidget::currentChanged, this, &DeepSSMTool::tab_changed);

  this->ui_->restore_defaults->setStyleSheet(normal_button_ss);
  this->ui_->violin_plot->setText("");
  this->ui_->training_plot->setText("");
  this->update_panels();
}

//---------------------------------------------------------------------------
void DeepSSMTool::tab_changed(int tab)
{
  switch (tab) {
    case 0 :
      this->current_tool_ = PythonWorker::JobType::DeepSSM_AugmentationType;
      break;
    case 1 :
      this->current_tool_ = PythonWorker::JobType::DeepSSM_TrainingType;
      break;
    case 2 :
      this->current_tool_ = PythonWorker::JobType::DeepSSM_InferenceType;
      break;
  }
  this->update_panels();
}

//---------------------------------------------------------------------------
DeepSSMTool::~DeepSSMTool()
{}

//---------------------------------------------------------------------------
void DeepSSMTool::set_session(QSharedPointer<Session> session)
{
  this->session_ = session;
  this->load_params();
  this->update_data();
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

  this->ui_->num_samples->setText(QString::number(params.get_aug_num_samples()));
  this->ui_->num_pca_dims->setText(QString::number(params.get_aug_num_dims()));
  this->ui_->percent_variability->setText(QString::number(params.get_aug_percent_variability()));
  this->ui_->sampler_type->setCurrentText(QString::fromStdString(params.get_aug_sampler_type()));

  this->ui_->training_epochs->setText(QString::number(params.get_training_epochs()));
  this->ui_->training_learning_rate->setText(QString::number(params.get_training_learning_rate()));
  this->ui_->training_decay_learning->setChecked(params.get_training_decay_learning_rate());
  this->ui_->training_fine_tuning->setChecked(params.get_training_fine_tuning());

  this->update_data();
}

//---------------------------------------------------------------------------
void DeepSSMTool::store_params()
{
  auto params = DeepSSMParameters(this->session_->get_project());

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
{
}

//---------------------------------------------------------------------------
bool DeepSSMTool::get_active()
{
  return false;
}

//---------------------------------------------------------------------------
void DeepSSMTool::run_clicked()
{
  if (this->tool_is_running_) {
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
    emit message("Inference Complete.  Duration: " + duration + " seconds");
  }

  this->update_data();
  this->tool_is_running_ = false;
  this->update_panels();
}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_progress(int val)
{
  this->load_plots();
  this->update_training_table();
  emit progress(val);
}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_error(QString msg)
{

}

//---------------------------------------------------------------------------
void DeepSSMTool::update_panels()
{
  this->ui_->data_content->setVisible(this->ui_->data_open_button->isChecked());
  this->ui_->controls_content->setVisible(this->ui_->controls_open_button->isChecked());
  this->ui_->training_content->setVisible(this->ui_->training_open_button->isChecked());
  this->ui_->tab_widget->setEnabled(!this->tool_is_running_);
  this->ui_->restore_defaults->setEnabled(!this->tool_is_running_);

  QString string = "";
  switch (this->current_tool_) {
    case PythonWorker::JobType::DeepSSM_AugmentationType:
      string = "Data Augmentation";
      break;
    case PythonWorker::JobType::DeepSSM_TrainingType:
      string = "Training";
      break;
    case PythonWorker::JobType::DeepSSM_InferenceType:
      string = "Inference";
      break;
  }

  if (this->tool_is_running_) {
    this->ui_->run_button->setStyleSheet(abort_button_ss);
    this->ui_->run_button->setText("Abort");
  }
  else {
    this->ui_->run_button->setStyleSheet(normal_button_ss);

    this->ui_->run_button->setText("Run " + string);
  }

}

//---------------------------------------------------------------------------
void DeepSSMTool::update_training_table()
{
  this->ui_->training_table->clear();
  QString filename = "deepssm/model/train_log.csv";
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
  lines.pop_front();
  this->ui_->training_table->setRowCount(lines.size());
  this->ui_->training_table->setColumnCount(headers.size());
  this->ui_->training_table->setHorizontalHeaderLabels(headers);
  this->ui_->training_table->verticalHeader()->setVisible(false);
  this->ui_->training_table->horizontalHeader()->setVisible(true);

  int row = 0;
  for (auto line : lines) {
    int col = 0;
    for (auto item : line.split(',')) {
      item = item.trimmed();
      if (item != "") {
        QTableWidgetItem* new_item = new QTableWidgetItem(QString(item));
        this->ui_->training_table->setItem(row, col++, new_item);
      }
    }
    row++;
  }
}

//---------------------------------------------------------------------------
void DeepSSMTool::update_data()
{
  this->update_training_table();

  this->ui_->table->clear();

  QString filename = "deepssm/Augmentation/TotalData.csv";
  if (QFile(filename).exists()) {

    QFile file(filename);

    if (!file.open(QIODevice::ReadOnly)) {
      emit error("Unable to open file: " + filename);
      return;
    }

    this->ui_->table->setRowCount(0);
    this->ui_->table->setColumnCount(5);

    //this->ui_->table->verticalHeader()->setVisible(true);
    //this->ui_->table->horizontalHeader()->setVisible(true);

    this->shapes_.clear();

    int row = 0;
    while (!file.atEnd()) {
      QByteArray line = file.readLine();
      int col = 0;
      bool show_generated = this->ui_->generated_data_checkbox->isChecked();
      bool show_original = this->ui_->original_data_checkbox->isChecked();
      // this needs to be replaced with it's own column (in all the python code as well)
      bool is_generated = line.contains("Generated");
      if ((is_generated && show_generated) || (!is_generated && show_original)) {
        this->ui_->table->insertRow(this->ui_->table->rowCount());
        for (auto item : line.split(',')) {
          QTableWidgetItem* new_item = new QTableWidgetItem(QString(item));
          this->ui_->table->setItem(row, col++, new_item);
        }

        auto groom_file = line.split(',')[0].toStdString();
        auto particle_file = line.split(',')[1].toStdString();

        auto subject = std::make_shared<Subject>();
        // nevermind, these are CT/MRI images
        //subject->set_groomed_filenames({groom_file});
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
void DeepSSMTool::restore_defaults()
{
  auto params = DeepSSMParameters(this->session_->get_project());

  switch (this->current_tool_) {
    case PythonWorker::JobType::DeepSSM_AugmentationType:
      params.restore_augmentation_defaults();
      break;
    case PythonWorker::JobType::DeepSSM_TrainingType:
      params.restore_training_defaults();
      break;
    case PythonWorker::JobType::DeepSSM_InferenceType:
      //params.restore_inference_defaults();
      break;
  }

  params.save_to_project();
  this->load_params();
}

//---------------------------------------------------------------------------
void DeepSSMTool::run_tool(PythonWorker::JobType type)
{

  // ensure someone doesn't accidental abort right after clicking RUN
  this->ui_->run_button->setEnabled(false);
  QTimer::singleShot(1000, [=]() {
    this->ui_->run_button->setEnabled(true);
  });

  this->current_tool_ = type;
  emit progress(-1);
  if (type == PythonWorker::JobType::DeepSSM_AugmentationType) {
    emit message("Please Wait: Running Data Augmentation...");
  }
  else if (type == PythonWorker::JobType::DeepSSM_TrainingType) {
    emit message("Please Wait: Running Training...");
    // clean
    QDir dir("deepssm/model");
    dir.removeRecursively();
  }
  else {
    emit message("Please Wait: Running Inference...");
  }

  this->load_plots();
  this->update_training_table();


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

}





//---------------------------------------------------------------------------


}
