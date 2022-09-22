#include <Data/DataTool.h>
#include <Data/LandmarkItemDelegate.h>
#include <Data/LandmarkTableModel.h>
#include <Data/Session.h>
#include <Shape.h>
#include <Data/ShapeWorksWorker.h>
#include <Logging.h>
#include <StudioMesh.h>
#include <ui_DataTool.h>

#include <QDebug>
#include <QMessageBox>
#include <QThread>
#include <iostream>

#ifdef __APPLE__
static QString click_message = "⌘+click";
#else
static QString click_message = "ctrl+click";
#endif

namespace shapeworks {

//---------------------------------------------------------------------------
DataTool::DataTool(Preferences& prefs) : preferences_(prefs) {
  ui_ = new Ui_DataTool;
  ui_->setupUi(this);

  connect(ui_->add_button, &QPushButton::clicked, this, &DataTool::import_button_clicked);
  connect(ui_->delete_button, &QPushButton::clicked, this, &DataTool::delete_button_clicked);

  connect(ui_->table_open_button, &QPushButton::toggled, ui_->table_content, &QWidget::setVisible);
  connect(ui_->table_header, &QPushButton::clicked, ui_->table_open_button, &QPushButton::toggle);
  connect(ui_->landmarks_open_button, &QPushButton::toggled, ui_->landmarks_content, &QWidget::setVisible);
  connect(ui_->landmarks_open_button, &QPushButton::toggled, this, &DataTool::handle_landmark_mode_changed);
  connect(ui_->landmarks_header, &QPushButton::clicked, ui_->landmarks_open_button, &QPushButton::toggle);
  connect(ui_->constraints_open_button, &QPushButton::toggled, ui_->constraints_content, &QWidget::setVisible);
  connect(ui_->constraints_open_button, &QPushButton::toggled, this, &DataTool::handle_constraints_mode_changed);
  connect(ui_->constraints_header, &QPushButton::clicked, ui_->constraints_open_button, &QPushButton::toggle);
  connect(ui_->notes_open_button, &QPushButton::toggled, ui_->notes_content, &QWidget::setVisible);
  connect(ui_->notes_header, &QPushButton::clicked, ui_->notes_open_button, &QPushButton::toggle);

  connect(ui_->landmark_domain_box_, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &DataTool::landmark_domain_changed);
  connect(ui_->ffc_active_, &QCheckBox::toggled, this, &DataTool::handle_ffc_active_changed);

  connect(ui_->delete_plane_, &QPushButton::clicked, this, &DataTool::delete_planes_clicked);
  connect(ui_->delete_ffc_, &QPushButton::clicked, this, &DataTool::delete_ffc_clicked);

  ui_->table_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->landmarks_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->constraints_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->notes_label->setAttribute(Qt::WA_TransparentForMouseEvents);

  ui_->landmark_help->setText("Place landmarks using " + click_message);
  ui_->plane_contraints_instruction_->setText("• Place 3 points to define a plane on a shape using " + click_message +
                                              "\n" + "• Slide plane along normal with shift+click\n" +
                                              "• Right click plane point to flip normal");

  // start with these off
  ui_->landmarks_open_button->toggle();
  ui_->constraints_open_button->toggle();
  ui_->notes_open_button->toggle();

  // table on
  // ui_->table_open_button->toggle();

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
  connect(ui_->landmark_drag_mode, &QCheckBox::stateChanged, session_.data(), &Session::set_landmark_drag_mode);
  connect(ui_->show_landmark_labels, &QCheckBox::stateChanged, session_.data(), &Session::set_show_landmark_labels);
  connect(session.data(), &Session::planes_changed, this, &DataTool::update_plane_table);
  connect(session.data(), &Session::ffc_changed, this, &DataTool::update_ffc_table);
  landmark_table_model_->set_session(session);

  connect(ui_->ffc_brush_size_, &QSlider::valueChanged, session.data(), &Session::set_ffc_paint_size);
  connect(ui_->ffc_included_mode_, &QRadioButton::toggled, session.data(), &Session::set_ffc_paint_mode_inclusive);

  update_table();
  handle_landmark_mode_changed();
  handle_constraints_mode_changed();
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

  auto shapes = session_->get_shapes();

  auto project = session_->get_project();
  auto headers = project->get_headers();
  auto& subjects = project->get_subjects();

  QStringList table_headers;
  for (const std::string& header : headers) {
    table_headers << QString::fromStdString(header);
  }

  ui_->table->clear();
  ui_->table->setRowCount(shapes.size());
  ui_->table->setColumnCount(table_headers.size());

  ui_->table->setHorizontalHeaderLabels(table_headers);
  ui_->table->verticalHeader()->setVisible(true);

  auto contains = [](std::map<std::string, std::string> map, std::string key) -> bool {
    return map.find(key) != map.end();
  };

  for (int row = 0; row < subjects.size(); row++) {
    auto values = subjects[row]->get_table_values();
    for (int h = 0; h < table_headers.size(); h++) {
      std::string value;
      if (contains(values, headers[h])) {
        value = values[headers[h]];
      }
      QTableWidgetItem* new_item = new QTableWidgetItem(QString::fromStdString(value));
      ui_->table->setItem(row, h, new_item);
    }
  }

  ui_->table->resizeColumnsToContents();
  ui_->table->horizontalHeader()->setStretchLastSection(false);
  ui_->table->setSelectionBehavior(QAbstractItemView::SelectRows);

  update_landmark_table();
  update_plane_table();
  update_ffc_table();
}

//---------------------------------------------------------------------------
void DataTool::update_landmark_table() {
  update_domain_box(ui_->landmark_domain_box_);

  auto domain_names = session_->get_project()->get_domain_names();
  ui_->landmark_domain_widget_->setVisible(domain_names.size() > 1);
  ui_->constraints_domain_widget_->setVisible(domain_names.size() > 1);

  landmark_table_model_->update_table();
  ui_->landmark_table->resizeColumnsToContents();
}

//---------------------------------------------------------------------------
void DataTool::landmark_domain_changed() {
  landmark_table_model_->set_active_domain(ui_->landmark_domain_box_->currentIndex());
  landmark_table_model_->update_table();
}

//---------------------------------------------------------------------------
void DataTool::constraints_domain_changed() {}

//---------------------------------------------------------------------------
void DataTool::delete_planes_clicked() {
  QModelIndexList list = ui_->plane_table_->selectionModel()->selectedRows();

  auto shapes = session_->get_shapes();

  QList<int> index_list;
  for (int i = list.size() - 1; i >= 0; i--) {
    int row = list[i].row();
    index_list << list[i].row();

    int shape_id = ui_->plane_table_->item(row, 0)->data(Qt::UserRole).toInt();
    int domain_id = ui_->plane_table_->item(row, 1)->data(Qt::UserRole).toInt();
    int plane_id = ui_->plane_table_->item(row, 2)->data(Qt::UserRole).toInt();
    auto& planes = shapes[shape_id]->get_constraints(domain_id).getPlaneConstraints();
    planes.erase(planes.begin() + plane_id);
  }
  session_->trigger_planes_changed();
}

//---------------------------------------------------------------------------
void DataTool::delete_ffc_clicked() {
  QModelIndexList list = ui_->ffc_table_->selectionModel()->selectedRows();

  auto shapes = session_->get_shapes();

  QList<int> index_list;
  for (int i = list.size() - 1; i >= 0; i--) {
    int row = list[i].row();
    index_list << list[i].row();

    int shape_id = ui_->ffc_table_->item(row, 0)->data(Qt::UserRole).toInt();
    int domain_id = ui_->ffc_table_->item(row, 1)->data(Qt::UserRole).toInt();
    auto& ffc = shapes[shape_id]->get_constraints(domain_id).getFreeformConstraint();
    ffc.reset();
  }
  session_->trigger_ffc_changed();
  session_->trigger_repaint();
}

//---------------------------------------------------------------------------
void DataTool::update_domain_box(QComboBox* box) {
  auto domain_names = session_->get_project()->get_domain_names();

  int currentIndex = box->currentIndex();
  if (domain_names.size() != box->count()) {
    box->clear();
    for (auto&& item : domain_names) {
      box->addItem(QString::fromStdString(item));
    }
  }
  if (currentIndex < 0) {
    currentIndex = 0;
  }
  if (currentIndex < box->count()) {
    box->setCurrentIndex(currentIndex);
  }
}

//---------------------------------------------------------------------------
void DataTool::update_plane_table() {
  auto shapes = session_->get_shapes();
  auto domain_names = session_->get_project()->get_domain_names();

  int plane_count = 0;
  for (int i = 0; i < shapes.size(); i++) {
    auto shape = shapes[i];
    for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
      if (domain_id < shape->constraints().size()) {
        auto& planes = shape->constraints()[domain_id].getPlaneConstraints();
        plane_count += planes.size();
      }
    }
  }

  QStringList table_headers;
  table_headers << "Shape";
  table_headers << "Domain";
  table_headers << "Center";
  table_headers << "Normal";

  QTableWidget* table = ui_->plane_table_;
  table->clear();
  table->setRowCount(plane_count);
  table->setColumnCount(table_headers.size());

  table->setHorizontalHeaderLabels(table_headers);
  table->verticalHeader()->setVisible(true);

  int row = 0;
  for (int i = 0; i < shapes.size(); i++) {
    auto shape = shapes[i];
    for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
      if (domain_id < shape->constraints().size()) {
        auto& planes = shape->constraints()[domain_id].getPlaneConstraints();
        int plane_id = 0;
        for (auto& plane : planes) {
          plane.updatePlaneFromPoints();
          // shape
          auto* new_item = new QTableWidgetItem(QString::fromStdString(shape->get_display_name()));
          new_item->setData(Qt::UserRole, i);  // shape id
          table->setItem(row, 0, new_item);
          // domain
          new_item = new QTableWidgetItem(QString::fromStdString(domain_names[domain_id]));
          new_item->setData(Qt::UserRole, domain_id);
          table->setItem(row, 1, new_item);

          auto center = plane.getPlanePoint();
          auto normal = plane.getPlaneNormal();
          auto center_string =
              QString::number(center[0]) + "," + QString::number(center[1]) + "," + QString::number(center[2]);
          auto normal_string =
              QString::number(normal[0]) + "," + QString::number(normal[1]) + "," + QString::number(normal[2]);
          if (plane.points().size() != 3) {
            normal_string = "N/A";
            center_string = "N/A";
          }
          // center
          new_item = new QTableWidgetItem(center_string);
          new_item->setData(Qt::UserRole, plane_id++);
          table->setItem(row, 2, new_item);

          // normal
          new_item = new QTableWidgetItem(normal_string);
          table->setItem(row, 3, new_item);
          row++;
        }
      }
    }
  }

  if (table->rowCount() < 1) {
    table->setMaximumHeight(ui_->table_label->height() * 2);
  } else {
    table->setMaximumHeight(16777215);  // default
  }
  table->resizeColumnsToContents();
  table->horizontalHeader()->setStretchLastSection(false);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
}

//---------------------------------------------------------------------------
void DataTool::update_ffc_table() {
  auto shapes = session_->get_shapes();
  auto domain_names = session_->get_project()->get_domain_names();

  int ffc_count = 0;
  for (int i = 0; i < shapes.size(); i++) {
    auto shape = shapes[i];
    for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
      if (shape->get_constraints(domain_id).getFreeformConstraint().isSet()) {
        ffc_count++;
      }
    }
  }

  QStringList table_headers;
  table_headers << "Shape";
  table_headers << "Domain";

  QTableWidget* table = ui_->ffc_table_;
  table->clear();
  table->setRowCount(ffc_count);
  table->setColumnCount(table_headers.size());

  table->setHorizontalHeaderLabels(table_headers);
  table->verticalHeader()->setVisible(true);

  int row = 0;
  for (int i = 0; i < shapes.size(); i++) {
    auto shape = shapes[i];
    for (int domain_id = 0; domain_id < domain_names.size(); domain_id++) {
      if (shape->get_constraints(domain_id).getFreeformConstraint().isSet()) {
        // shape
        auto* new_item = new QTableWidgetItem(QString::fromStdString(shape->get_display_name()));
        new_item->setData(Qt::UserRole, i);  // shape id
        table->setItem(row, 0, new_item);
        // domain
        new_item = new QTableWidgetItem(QString::fromStdString(domain_names[domain_id]));
        new_item->setData(Qt::UserRole, domain_id);
        table->setItem(row, 1, new_item);
        row++;
      }
    }
  }

  if (table->rowCount() < 1) {
    table->setMaximumHeight(ui_->table_label->height() * 2);
  } else {
    table->setMaximumHeight(16777215);  // default
  }
  table->resizeColumnsToContents();
  table->horizontalHeader()->setStretchLastSection(false);
  table->setSelectionBehavior(QAbstractItemView::SelectRows);
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
  if (id == session_->get_placing_landmark()) {
    id = -1;
  }
  session_->set_placing_landmark(id);
}

//---------------------------------------------------------------------------
void DataTool::handle_landmark_mode_changed() {
  if (session_) {
    session_->set_landmarks_active(ui_->landmarks_open_button->isChecked());
  }
  if (ui_->landmarks_open_button->isChecked() && ui_->constraints_open_button->isChecked()) {
    ui_->constraints_open_button->setChecked(false);
  }
}

//---------------------------------------------------------------------------
void DataTool::handle_constraints_mode_changed() {
  if (session_) {
    session_->set_planes_active(ui_->constraints_open_button->isChecked());
  }
  if (ui_->landmarks_open_button->isChecked() && ui_->constraints_open_button->isChecked()) {
    ui_->landmarks_open_button->setChecked(false);
  }
}

//---------------------------------------------------------------------------
void DataTool::handle_ffc_active_changed() {
  // maybe just direct connect unless we add something else?
  session_->set_ffc_paint_active(ui_->ffc_active_->isChecked());
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
