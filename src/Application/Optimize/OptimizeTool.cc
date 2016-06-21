#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QThread>
#include <QMessageBox>
#include <Optimize/OptimizeTool.h>
#include <Visualization/ShapeworksWorker.h>
#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <ui_OptimizeTool.h>

//---------------------------------------------------------------------------
OptimizeTool::OptimizeTool(Preferences& prefs) : preferences_(prefs) {
  this->ui_ = new Ui_OptimizeTool;
  this->ui_->setupUi( this );
  this->setupTable(this->ui_->number_of_scales->value());
}

//---------------------------------------------------------------------------
OptimizeTool::~OptimizeTool()
{}

//---------------------------------------------------------------------------
void OptimizeTool::handle_error(std::string msg) {
  emit error_message(msg);
  this->ui_->run_optimize_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_progress(int val) {
  emit progress(static_cast<size_t>(val));
}

//---------------------------------------------------------------------------
void OptimizeTool::handle_thread_complete() {
  auto local = this->optimize_->localPoints();
  auto global = this->optimize_->globalPoints();
  this->project_->load_points(local, true);
  this->project_->load_points(global, false);
  this->project_->set_reconstructed_present(
    local.size() == global.size() && global.size() > 1);
  this->project_->calculate_reconstructed_samples();
  emit progress(100);
  emit message("Optimize Complete");
  emit optimize_complete();
  this->ui_->run_optimize_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void OptimizeTool::on_run_optimize_button_clicked() {
  this->update_preferences();
  emit message("Please wait: running optimize step...");
  emit progress(1);
  this->ui_->run_optimize_button->setEnabled(false);
  auto shapes = this->project_->get_shapes();
  std::vector<ImageType::Pointer> imgs;
  for (auto s : shapes) {
    imgs.push_back(s->get_groomed_image());
  }
  auto scales = this->ui_->number_of_scales->value();
  this->optimize_ = new QOptimize(this, imgs, scales,
    this->getStartRegs(), this->getEndRegs(), this->getIters(),
    this->getDecaySpans(), this->ui_->weight->value(), true);

  QThread *thread = new QThread;
  ShapeworksWorker *worker = new ShapeworksWorker(
    ShapeworksWorker::Optimize, NULL, this->optimize_, this->project_,
    std::vector<std::vector<itk::Point<float> > >(),
    std::vector<std::vector<itk::Point<float> > >(),
    std::vector<ImageType::Pointer>(),
    this->ui_->maxAngle->value(),
    this->ui_->meshDecimation->value());
  worker->moveToThread(thread);
  connect(thread, SIGNAL(started()), worker, SLOT(process()));
  connect(worker, SIGNAL(result_ready()), this, SLOT(handle_thread_complete()));
  connect(this->optimize_, SIGNAL(progress(int)), this, SLOT(handle_progress(int)));
  connect(worker, SIGNAL(error_message(std::string)), this, SLOT(handle_error(std::string)));
  connect(worker, SIGNAL(message(std::string)), this, SLOT(handle_message(std::string)));
  connect(worker, SIGNAL(finished()), worker, SLOT(deleteLater()));
  thread->start();
}

void OptimizeTool::on_meshDecimation_valueChanged(double v) {
  this->preferences_.set_preference("optimize_decimation", v);
}

void OptimizeTool::handle_message(std::string s) {
  emit message(s);
}

void OptimizeTool::on_restoreDefaults_clicked() {
  this->preferences_.delete_entry("optimize_decimation");
  this->preferences_.delete_entry("optimize_scales");
  this->preferences_.delete_entry("optimize_start_reg");
  this->preferences_.delete_entry("optimize_end_reg");
  this->preferences_.delete_entry("optimize_iters");
  this->preferences_.delete_entry("optimize_weight");
  this->preferences_.delete_entry("optimize_maxAngle");
  this->preferences_.delete_entry("optimize_decay_span");
  this->preferences_.restore_defaults();
  this->set_preferences();
  auto scales = this->preferences_.get_preference("optimize_scales", 8);
  this->ui_->number_of_scales->setValue(scales);
  qApp->processEvents();
  this->preferences_.set_saved(false);
}

void OptimizeTool::on_number_of_scales_valueChanged(int val) {
  this->setupTable(val);
}

//---------------------------------------------------------------------------
void OptimizeTool::set_project( QSharedPointer<Project> project )
{
  this->project_ = project;
}

void OptimizeTool::setupTable(int rows) {
  auto table = this->ui_->parameterTable;
  table->setRowCount(rows);
  table->setColumnCount(4);
  QStringList Hheader, Vheader;
  Hheader << "StartReg" << "EndReg"
    << "Iters" << "DecaySpan";
  table->setHorizontalHeaderLabels(Hheader);
  for (size_t i = 0; i < rows; i++) {
    Vheader << QString::number(i + 1);
  }
  this->set_preferences(false);
  table->verticalHeader()->setVisible(true);
  table->setVerticalHeaderLabels(Vheader);
  table->setEditTriggers(QAbstractItemView::DoubleClicked);
  table->setSelectionMode(QAbstractItemView::SingleSelection);
  table->setShowGrid(true);
  table->setStyleSheet("QTableView {selection-background-color: 0x1111;}");
  QString styleSheet = "::section {"
    "spacing: 0px;"
    "background-color: 0xEEEE;"
    "color: black;"
    "border: 1px solid black;"
    "margin: 0px;"
    "text-align: left;"
    "font-family: arial;"
    "font-size: 11px; }";
  table->horizontalHeader()->setStyleSheet(styleSheet);
  table->verticalHeader()->setStyleSheet(styleSheet);
  table->resizeColumnsToContents();
  table->resizeRowsToContents();
}

std::vector<double> OptimizeTool::getStartRegs() {
  auto ans =  std::vector<double>();
  auto table = this->ui_->parameterTable;
  for (size_t i = 0; i < table->rowCount(); i++) {
    ans.push_back(table->item(i, 0)->text().toDouble());
  }
  return ans;
}

std::vector<double> OptimizeTool::getEndRegs() {
  auto ans = std::vector<double>();
  auto table = this->ui_->parameterTable;
  for (size_t i = 0; i < table->rowCount(); i++) {
    ans.push_back(table->item(i, 1)->text().toDouble());
  }
  return ans;
}

std::vector<double> OptimizeTool::getDecaySpans() {
  auto ans = std::vector<double>();
  auto table = this->ui_->parameterTable;
  for (size_t i = 0; i < table->rowCount(); i++) {
    ans.push_back(table->item(i, 3)->text().toDouble());
  }
  return ans;
}

std::vector<unsigned int> OptimizeTool::getIters() {
  auto ans = std::vector<unsigned int>();
  auto table = this->ui_->parameterTable;
  for (size_t i = 0; i < table->rowCount(); i++) {
    ans.push_back(table->item(i, 2)->text().toUInt());
  }
  return ans;
}

void OptimizeTool::set_preferences(bool setScales) {
  this->ui_->meshDecimation->setValue(
    this->preferences_.get_preference("optimize_decimation",
      this->ui_->meshDecimation->value()));
  if (setScales) {
    this->ui_->number_of_scales->setValue(
      this->preferences_.get_preference("optimize_scales",
        this->ui_->number_of_scales->value()));
  }
  auto table = this->ui_->parameterTable;
  auto rows = this->ui_->number_of_scales->value();
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < 4; j++) {
      QString cellname;
      switch (j) {
      case 0:
        cellname = "optimize_start_reg";
        break;
      case 1:
        cellname = "optimize_end_reg";
        break;
      case 2:
        cellname = "optimize_iters";
        break;
      case 3:
        cellname = "optimize_decay_span";
        break;
      }
      auto defaultVal = this->preferences_.get_preference(cellname.toStdString(), j == 2 ? 0 : 0.);
      auto cellTest = cellname + QString::number(i);
      auto cellVal = this->preferences_.get_preference(cellTest.toStdString(), defaultVal);
      table->setItem(i, j, new QTableWidgetItem(QString::number(cellVal)));
    }
  }
}

void OptimizeTool::update_preferences() {
  this->preferences_.set_preference("optimize_decimation",
      this->ui_->meshDecimation->value());
  this->preferences_.set_preference("optimize_scales",
    this->ui_->number_of_scales->value());
  auto table = this->ui_->parameterTable;
  auto rows = this->ui_->number_of_scales->value();
  for (size_t i = 0; i < rows; i++) {
    for (size_t j = 0; j < 4; j++) {
      QString cellname;
      switch (j) {
      case 0:
        cellname = "optimize_start_reg";
        break;
      case 1:
        cellname = "optimize_end_reg";
        break;
      case 2:
        cellname = "optimize_iters";
        break;
      case 3:
        cellname = "optimize_decay_span";
        break;
      }
      auto cellTest = cellname + QString::number(i);
      this->preferences_.set_preference(cellTest.toStdString(),
        j == 2 ? table->item(i, j)->text().toUInt() :
        table->item(i, j)->text().toDouble());
    }
  }
}

void OptimizeTool::enableActions() {
  this->ui_->run_optimize_button->setEnabled(true);
}

void OptimizeTool::disableActions() {
  this->ui_->run_optimize_button->setEnabled(false);
}