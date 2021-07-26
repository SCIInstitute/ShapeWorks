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

//---------------------------------------------------------------------------
DeepSSMTool::DeepSSMTool(Preferences& prefs) : preferences_(prefs)
{
  this->ui_ = new Ui_DeepSSMTool;
  this->ui_->setupUi(this);

  connect(this->ui_->run_augmentation_button, &QPushButton::clicked,
          this, &DeepSSMTool::run_augmentation_clicked);
  connect(this->ui_->restore_augmentation_defaults, &QPushButton::clicked,
          this, &DeepSSMTool::restore_augmentation_defaults);

  connect(this->ui_->run_training_button, &QPushButton::clicked,
          this, &DeepSSMTool::run_training_clicked);
  connect(this->ui_->restore_training_defaults, &QPushButton::clicked,
          this, &DeepSSMTool::restore_training_defaults);

  connect(this->ui_->data_open_button, &QPushButton::clicked,
          this, &DeepSSMTool::update_panels);
  connect(this->ui_->controls_open_button, &QPushButton::clicked,
          this, &DeepSSMTool::update_panels);

  connect(this->ui_->generated_data_checkbox, &QCheckBox::stateChanged,
          this, &DeepSSMTool::update_data);
  connect(this->ui_->original_data_checkbox, &QCheckBox::stateChanged,
          this, &DeepSSMTool::update_data);

  this->ui_->violin_plot->setText("");
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
void DeepSSMTool::run_augmentation_clicked()
{
  this->run_tool(ShapeworksWorker::ThreadType::DeepSSM_AugmentationType);
}

//---------------------------------------------------------------------------
void DeepSSMTool::run_training_clicked()
{
  this->run_tool(ShapeworksWorker::ThreadType::DeepSSM_TrainingType);
}

//---------------------------------------------------------------------------
void DeepSSMTool::handle_thread_complete()
{
  emit progress(100);
  QString duration = QString::number(this->timer_.elapsed() / 1000.0, 'f', 1);

  if (this->current_tool_ == ShapeworksWorker::DeepSSM_AugmentationType) {
    emit message("Data Augmentation Complete.  Duration: " + duration + " seconds");
  }
  else if (this->current_tool_ == ShapeworksWorker::DeepSSM_TrainingType) {
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
  this->ui_->controls_content->setEnabled(!this->tool_is_running_);
}

//---------------------------------------------------------------------------
void DeepSSMTool::update_data()
{
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

    QStringList wordList;
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
  this->load_violin_plot();
  emit update_view();
}

//---------------------------------------------------------------------------
QVector<QSharedPointer<Shape>> DeepSSMTool::get_shapes()
{
  return this->shapes_;
}

//---------------------------------------------------------------------------
void DeepSSMTool::load_violin_plot()
{
  QString filename = "deepssm/Augmentation/violin.png";
  if (QFile(filename).exists()) {
    this->violin_plot_ = QPixmap(filename);
  }
  else {
    this->violin_plot_ = QPixmap{};
  }
  this->resize_plot();
}

//---------------------------------------------------------------------------
void DeepSSMTool::resize_plot()
{
  if (!this->violin_plot_.isNull()) {
    QPixmap resized = this->violin_plot_.scaledToWidth(this->ui_->violin_plot->width(),
                                                       Qt::SmoothTransformation);
    this->ui_->violin_plot->setPixmap(resized);
  }
  else {
    this->ui_->violin_plot->setPixmap(QPixmap{});
  }
}

//---------------------------------------------------------------------------
void DeepSSMTool::resizeEvent(QResizeEvent* event)
{
  QWidget::resizeEvent(event);
  this->resize_plot();
}

//---------------------------------------------------------------------------
void DeepSSMTool::restore_augmentation_defaults()
{
  auto params = DeepSSMParameters(this->session_->get_project());
  params.restore_augmentation_defaults();
  params.save_to_project();
  this->load_params();
}

//---------------------------------------------------------------------------
void DeepSSMTool::restore_training_defaults()
{
  auto params = DeepSSMParameters(this->session_->get_project());
  params.restore_training_defaults();
  params.save_to_project();
  this->load_params();
}

//---------------------------------------------------------------------------
void DeepSSMTool::run_tool(ShapeworksWorker::ThreadType type)
{
  this->current_tool_ = type;
  emit progress(-1);
  if (type == ShapeworksWorker::DeepSSM_AugmentationType) {
    emit message("Please Wait: Running Data Augmentation...");
  }
  else if (type == ShapeworksWorker::DeepSSM_TrainingType) {
    emit message("Please Wait: Running Training...");
  }
  else {
    emit message("Please Wait: Running Inference...");
  }
  this->timer_.start();

  this->tool_is_running_ = true;
  this->update_panels();

  this->store_params();
  this->deep_ssm_ = QSharedPointer<QDeepSSM>::create(session_->get_project());

  connect(this->deep_ssm_.data(), &QDeepSSM::message, this, &DeepSSMTool::message);
  connect(this->deep_ssm_.data(), &QDeepSSM::error, this, &DeepSSMTool::error);

  ShapeworksWorker* worker = new ShapeworksWorker(
    this->current_tool_, nullptr, nullptr,
    nullptr, session_);
  worker->set_deep_ssm(this->deep_ssm_);

  QThread* thread = new QThread;
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, &ShapeworksWorker::finished, this, &DeepSSMTool::handle_thread_complete);
  connect(this->deep_ssm_.data(), &QDeepSSM::progress, this, &DeepSSMTool::handle_progress);
  connect(worker, &ShapeworksWorker::error_message, this, &DeepSSMTool::handle_error);
  connect(worker, &ShapeworksWorker::message, this, &DeepSSMTool::message);
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();

}


//---------------------------------------------------------------------------


}
