#include <Data/DataTool.h>
#include <Data/LandmarkItemDelegate.h>
#include <Data/LandmarkTableModel.h>
#include <Data/Session.h>
#include <Data/Shape.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/StudioLog.h>
#include <Data/StudioMesh.h>
#include <ui_DataTool.h>

#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <iostream>

namespace shapeworks {

//---------------------------------------------------------------------------
DataTool::DataTool(Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_DataTool;
  ui_->setupUi(this);

  connect(ui_->add_button, &QPushButton::clicked, this, &DataTool::import_button_clicked);
  connect(ui_->delete_button, &QPushButton::clicked, this, &DataTool::delete_button_clicked);

  connect(ui_->table_open_button, &QPushButton::toggled, ui_->table_content, &QWidget::setVisible);
  connect(ui_->landmarks_open_button, &QPushButton::toggled, ui_->landmarks_content, &QWidget::setVisible);
  connect(ui_->constraints_open_button, &QPushButton::toggled, ui_->constraints_content, &QWidget::setVisible);
  connect(ui_->notes_open_button, &QPushButton::toggled, ui_->notes_content, &QWidget::setVisible);

  // start with these off
  // ui_->landmarks_open_button->toggle();
  ui_->table_open_button->toggle();
  ui_->constraints_open_button->toggle();
  ui_->notes_open_button->toggle();

  landmark_table_model_ = std::make_shared<LandmarkTableModel>(this);
  connect(ui_->new_landmark_button, &QPushButton::clicked, landmark_table_model_.get(),
          &LandmarkTableModel::new_landmark);
  connect(ui_->delete_landmark_button, &QPushButton::clicked, this, &DataTool::delete_landmarks_clicked);

  ui_->landmark_table->setModel(landmark_table_model_.get());
  ui_->landmark_table->horizontalHeader()->setStretchLastSection(true);
  connect(ui_->landmark_table, &QTableView::clicked, landmark_table_model_.get(), &LandmarkTableModel::handle_click);
  connect(ui_->landmark_table, &QTableView::doubleClicked, landmark_table_model_.get(),
          &LandmarkTableModel::handle_double_click);
  connect(ui_->landmark_table->horizontalHeader(), &QHeaderView::sectionClicked, landmark_table_model_.get(),
          &LandmarkTableModel::handle_header_click);

  auto delegate = new LandmarkItemDelegate(ui_->landmark_table);
  delegate->set_model(landmark_table_model_);

  connect(delegate, &LandmarkItemDelegate::button_clicked, this, &DataTool::set_placing_button_clicked);

  ui_->landmark_table->setItemDelegate(delegate);

  ui_->landmark_table->verticalHeader()->setVisible(true);
  ui_->landmark_table->horizontalHeader()->setVisible(true);
  ui_->landmark_table->resizeColumnsToContents();
}

//---------------------------------------------------------------------------
DataTool::~DataTool() {}

//---------------------------------------------------------------------------
void DataTool::set_session(QSharedPointer<Session> session) {
  session_ = session;
  landmark_table_model_->set_project(session->get_project());
  landmark_table_model_->set_session(session);
  update_table();
}

//---------------------------------------------------------------------------
void DataTool::disable_actions() {
  ui_->add_button->setEnabled(false);
  ui_->delete_button->setEnabled(false);
}

//---------------------------------------------------------------------------
void DataTool::enable_actions() {
  ui_->add_button->setEnabled(true);
  ui_->delete_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void DataTool::update_table() {
  if (!session_) {
    return;
  }

  QVector<QSharedPointer<Shape>> shapes = session_->get_shapes();

  auto project = session_->get_project();
  auto headers = project->get_headers();

  QStringList table_headers;
  for (const std::string& header : headers) {
    table_headers << QString::fromStdString(header);
  }

  ui_->table->clear();
  ui_->table->setRowCount(shapes.size());
  ui_->table->setColumnCount(table_headers.size());

  ui_->table->setHorizontalHeaderLabels(table_headers);
  ui_->table->verticalHeader()->setVisible(true);

  for (int h = 0; h < table_headers.size(); h++) {
    auto rows = project->get_string_column(table_headers[h].toStdString());
    for (int row = 0; row < shapes.size() && row < rows.size(); row++) {
      QTableWidgetItem* new_item = new QTableWidgetItem(QString::fromStdString(rows[row]));
      ui_->table->setItem(row, h, new_item);
    }
  }

  ui_->table->resizeColumnsToContents();
  ui_->table->horizontalHeader()->setStretchLastSection(false);
  ui_->table->setSelectionBehavior(QAbstractItemView::SelectRows);

  landmark_table_model_->update_table();
  ui_->landmark_table->resizeColumnsToContents();
}

//---------------------------------------------------------------------------
void DataTool::update_notes() {
  if (session_) {
    ui_->notes->setText(QString::fromStdString(session_->parameters().get("notes", "")));
  }
}

//---------------------------------------------------------------------------
std::string DataTool::get_notes() { return ui_->notes->toHtml().toStdString(); }

//---------------------------------------------------------------------------
void DataTool::store_data() { landmark_table_model_->store_landmarks(); }

//---------------------------------------------------------------------------
void DataTool::delete_landmarks_clicked() {
  landmark_table_model_->delete_landmarks(ui_->landmark_table->selectionModel()->selectedRows());
}

//---------------------------------------------------------------------------
void DataTool::set_placing_button_clicked(int id) {
  if (id == session_->get_plaing_landmark()) {
    id = -1;
  }
  std::cerr << "now placing " << id << "\n";
  session_->set_placing_landmark(id);
}

//---------------------------------------------------------------------------
void DataTool::delete_button_clicked() {
  QModelIndexList list = ui_->table->selectionModel()->selectedRows();

  QList<int> index_list;
  for (int i = list.size() - 1; i >= 0; i--) {
    index_list << list[i].row();
  }

  session_->remove_shapes(index_list);
}

}  // namespace shapeworks
