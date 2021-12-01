#include <iostream>

#include <QMessageBox>
#include <QThread>

#include <Data/Session.h>
#include <Data/StudioMesh.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <Data/ShapeWorksWorker.h>
#include <Data/DataTool.h>

#include <Libs/Groom/GroomParameters.h>

#include <ui_DataTool.h>


namespace shapeworks {

//---------------------------------------------------------------------------
DataTool::DataTool(Preferences& prefs) : preferences_(prefs)
{
  ui_ = new Ui_DataTool;
  ui_->setupUi(this);

  connect(ui_->add_button, &QPushButton::clicked, this, &DataTool::import_button_clicked);
  connect(ui_->delete_button, &QPushButton::clicked, this, &DataTool::delete_button_clicked);

  connect(ui_->table_open_button, &QPushButton::toggled, ui_->table_content, &QWidget::setVisible);
  connect(ui_->landmarks_open_button, &QPushButton::toggled, ui_->landmarks_content, &QWidget::setVisible);
  connect(ui_->constraints_open_button, &QPushButton::toggled, ui_->constraints_content, &QWidget::setVisible);
  connect(ui_->notes_open_button, &QPushButton::toggled, ui_->notes_content, &QWidget::setVisible);

  // start with these off
  ui_->landmarks_open_button->toggle();
  ui_->constraints_open_button->toggle();
  ui_->notes_open_button->toggle();


}

//---------------------------------------------------------------------------
DataTool::~DataTool()
{}

//---------------------------------------------------------------------------
void DataTool::set_session(QSharedPointer<Session> session)
{
  this->session_ = session;
  this->update_table();
}

//---------------------------------------------------------------------------
void DataTool::disable_actions()
{
  this->ui_->add_button->setEnabled(false);
  this->ui_->delete_button->setEnabled(false);
}

//---------------------------------------------------------------------------
void DataTool::enable_actions()
{
  this->ui_->add_button->setEnabled(true);
  this->ui_->delete_button->setEnabled(true);
}

//---------------------------------------------------------------------------
void DataTool::update_table()
{
  if (!this->session_) {
    return;
  }

  QVector<QSharedPointer<Shape>> shapes = this->session_->get_shapes();

  auto project = this->session_->get_project();
  auto headers = project->get_headers();

  QStringList table_headers;
  for (const std::string& header : headers) {
    //std::cerr << "header: " << header << "\n";
    table_headers << QString::fromStdString(header);
  }

  this->ui_->table->clear();
  this->ui_->table->setRowCount(shapes.size());
  this->ui_->table->setColumnCount(table_headers.size());

  this->ui_->table->setHorizontalHeaderLabels(table_headers);
  this->ui_->table->verticalHeader()->setVisible(true);

  for (int h = 0; h < table_headers.size(); h++) {
    auto rows = project->get_string_column(table_headers[h].toStdString());
    for (int row = 0; row < shapes.size() && row < rows.size(); row++) {
      QTableWidgetItem* new_item = new QTableWidgetItem(QString::fromStdString(rows[row]));
      this->ui_->table->setItem(row, h, new_item);
    }
  }

  this->ui_->table->resizeColumnsToContents();
  this->ui_->table->horizontalHeader()->setStretchLastSection(false);
  this->ui_->table->setSelectionBehavior(QAbstractItemView::SelectRows);
}

//---------------------------------------------------------------------------
void DataTool::update_notes()
{
  if (this->session_) {
    this->ui_->notes->setText(QString::fromStdString(
                                this->session_->parameters().get("notes", "")));
  }
}

//---------------------------------------------------------------------------
std::string DataTool::get_notes()
{
  return this->ui_->notes->toHtml().toStdString();
}

//---------------------------------------------------------------------------
void DataTool::delete_button_clicked()
{
  QModelIndexList list = this->ui_->table->selectionModel()->selectedRows();

  QList<int> index_list;
  for (int i = list.size() - 1; i >= 0; i--) {
    index_list << list[i].row();
  }

  this->session_->remove_shapes(index_list);
}

//---------------------------------------------------------------------------
}
