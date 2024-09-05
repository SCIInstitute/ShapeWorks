#include <Data/DataTool.h>
#include <Data/LandmarkItemDelegate.h>
#include <Data/LandmarkTableModel.h>
#include <Data/Session.h>
#include <Data/ShapeWorksWorker.h>
#include <Interface/Style.h>
#include <Logging.h>
#include <Shape.h>
#include <StudioMesh.h>
#include <Utils/StudioUtils.h>
#include <ui_DataTool.h>
#include <vtkPointData.h>
#include <vtkTransformPolyDataFilter.h>

#include <QDebug>
#include <QMenu>
#include <QMessageBox>
#include <QThread>

#include "SharedBoundaryPanel.h"

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

  connect(ui_->delete_plane_, &QPushButton::clicked, this, &DataTool::delete_planes_clicked);
  connect(ui_->delete_ffc_, &QPushButton::clicked, this, &DataTool::delete_ffc_clicked);

  ui_->table_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->landmarks_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->constraints_label->setAttribute(Qt::WA_TransparentForMouseEvents);
  ui_->notes_label->setAttribute(Qt::WA_TransparentForMouseEvents);

  ui_->landmark_help->setText("Place landmarks using " + click_message);
  ui_->plane_contraints_instruction_->setText("• Place 3 points to define a plane on a shape using " + click_message +
                                              "\n" + "• Slide plane along normal with shift+click\n" +
                                              "• Right click plane point to flip normal or apply to other shapes");

  ui_->ffc_instruction->setText(QString("• Enable painting and draw on the surface with left+click\n") +
                                "• Right click on a constraint in the table to apply to other shapes");

  // start with these off
  ui_->landmarks_open_button->toggle();
  ui_->constraints_open_button->toggle();
  ui_->notes_open_button->toggle();

  // table on
  // ui_->table_open_button->toggle();

  // when the table selection changes
  connect(ui_->table, &QTableWidget::itemSelectionChanged, this, &DataTool::table_selection_changed);

  // when the subject_notes changes
  connect(ui_->subject_notes, &QTextEdit::textChanged, this, &DataTool::subject_notes_changed);

  // when the table data changes
  connect(ui_->table, &QTableWidget::itemChanged, this, &DataTool::table_data_edited);

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

  // handle right click on constraints QTableWidget (note that the CustomContextMenu property must be set for this table
  // in the UI file)
  connect(ui_->ffc_table_, &QTableView::customContextMenuRequested, this, &DataTool::constraints_table_right_click);

  // handle right click on data table (note that the CustomContextMenu property must be set for this table in the UI
  // file)
  connect(ui_->table, &QTableView::customContextMenuRequested, this, &DataTool::data_table_right_click);

  auto delegate = new LandmarkItemDelegate(ui_->landmark_table);
  delegate->set_model(landmark_table_model_);

  connect(delegate, &LandmarkItemDelegate::button_clicked, this, &DataTool::set_placing_button_clicked);

  ui_->landmark_table->setItemDelegate(delegate);

  ui_->landmark_table->verticalHeader()->setVisible(true);
  ui_->landmark_table->horizontalHeader()->setVisible(true);
  ui_->landmark_table->resizeColumnsToContents();

  shared_boundary_panel_ = new SharedBoundaryPanel(this);
  layout()->addWidget(shared_boundary_panel_);

  auto spacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
  layout()->addItem(spacer);
}

//---------------------------------------------------------------------------
DataTool::~DataTool() {}

//---------------------------------------------------------------------------
void DataTool::set_session(QSharedPointer<Session> session) {
  session_ = session;

  // set (reset) values
  ui_->ffc_brush_size_->setValue(static_cast<int>(session->get_ffc_paint_size()));
  ui_->ffc_brush_size_->setSliderPosition(static_cast<int>(session->get_ffc_paint_size()));
  ui_->ffc_included_mode_->setChecked(session->get_ffc_paint_mode_inclusive());
  ui_->ffc_excluded_mode_->setChecked(!session->get_ffc_paint_mode_inclusive());
  ui_->ffc_active_->setChecked(session->get_ffc_paint_active());

  connect(ui_->landmark_drag_mode, &QCheckBox::stateChanged, session_.data(), &Session::set_landmark_drag_mode);
  connect(ui_->show_landmark_labels, &QCheckBox::stateChanged, session_.data(), &Session::set_show_landmark_labels);
  connect(session.data(), &Session::planes_changed, this, &DataTool::update_plane_table);
  connect(session.data(), &Session::ffc_changed, this, &DataTool::update_ffc_table);
  landmark_table_model_->set_session(session);

  connect(ui_->ffc_brush_size_, &CustomSlider::valueChanged, session.data(), &Session::set_ffc_paint_size);
  connect(ui_->ffc_included_mode_, &QRadioButton::toggled, session.data(), &Session::set_ffc_paint_mode_inclusive);
  connect(ui_->ffc_active_, &QCheckBox::toggled, session.data(), &Session::set_ffc_paint_active);

  update_table();
  handle_landmark_mode_changed();
  handle_constraints_mode_changed();
  shared_boundary_panel_->set_session(session);
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
void DataTool::update_table(bool clean) {
  if (!session_) {
    return;
  }

  block_table_update_ = true;
  auto shapes = session_->get_shapes();
  auto project = session_->get_project();
  auto headers = project->get_headers();
  auto& subjects = project->get_subjects();

  QStringList table_headers;
  for (const std::string& header : headers) {
    table_headers << QString::fromStdString(header);
  }

  if (clean) {
    ui_->table->clear();
    ui_->table->setRowCount(shapes.size());
    ui_->table->setColumnCount(table_headers.size());
    ui_->table->setHorizontalHeaderLabels(table_headers);
    ui_->table->verticalHeader()->setVisible(true);
  }

  for (int row = 0; row < subjects.size(); row++) {
    auto values = subjects[row]->get_table_values();
    for (int h = 0; h < table_headers.size(); h++) {
      std::string value;
      if (values.contains(headers[h])) {
        value = values[headers[h]];
      }
      if (clean) {
        QTableWidgetItem* new_item = new QTableWidgetItem(QString::fromStdString(value));
        ui_->table->setItem(row, h, new_item);
      } else {
        // just update the text
        ui_->table->item(row, h)->setText(QString::fromStdString(value));
      }
    }
  }

  if (clean) {
    ui_->table->resizeColumnsToContents();
    ui_->table->horizontalHeader()->setStretchLastSection(false);
    ui_->table->setSelectionBehavior(QAbstractItemView::SelectRows);

    update_landmark_table();
    update_plane_table();
    update_ffc_table();
    shared_boundary_panel_->update_panel();
  }

  block_table_update_ = false;
}

//---------------------------------------------------------------------------
void DataTool::update_landmark_table() {
  StudioUtils::update_domain_combobox(ui_->landmark_domain_box_, session_);

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
  session_->trigger_reinsert_shapes();
  session_->trigger_repaint();
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
void DataTool::constraints_table_right_click(const QPoint& point) {
  // get the item at the position
  auto item = ui_->ffc_table_->itemAt(point);
  int row = ui_->ffc_table_->row(item);
  ui_->ffc_table_->selectRow(row);

  if (!item) {
    return;
  }

  QMenu menu(this);
  QAction* copy_action = new QAction("Copy constraint to other shapes", this);
  connect(copy_action, &QAction::triggered, this, &DataTool::copy_ffc_clicked);
  menu.addAction(copy_action);
  menu.exec(QCursor::pos());
}

//---------------------------------------------------------------------------
void DataTool::data_table_right_click(const QPoint& point) {
  auto selected = ui_->table->selectionModel()->selectedRows();
  if (selected.size() < 1) {
    auto item = ui_->table->itemAt(point);
    if (!item) {
      return;
    }
    int row = ui_->table->row(item);
    selected.push_back(ui_->table->model()->index(row, 0));
  }

  auto apply_to_selected = [=](std::function<void(std::shared_ptr<Subject>)> f) {
    auto shapes = session_->get_shapes();
    for (auto& index : selected) {
      int id = index.row();
      f(shapes[id]->get_subject());
    }
    session_->get_project()->update_subjects();
    update_table();
    session_->trigger_reinsert_shapes();
  };

  QMenu menu(this);

  QAction* mark_excluded = new QAction("Mark as excluded", this);
  connect(mark_excluded, &QAction::triggered, this,
          [=] { apply_to_selected([](std::shared_ptr<Subject> s) { s->set_excluded(true); }); });
  menu.addAction(mark_excluded);

  QAction* unmark_excluded = new QAction("Unmark as excluded", this);
  connect(unmark_excluded, &QAction::triggered, this,
          [=] { apply_to_selected([](std::shared_ptr<Subject> s) { s->set_excluded(false); }); });
  menu.addAction(unmark_excluded);

  QAction* mark_fixed_action = new QAction("Mark as fixed", this);
  connect(mark_fixed_action, &QAction::triggered, this,
          [=] { apply_to_selected([](std::shared_ptr<Subject> s) { s->set_fixed(true); }); });
  menu.addAction(mark_fixed_action);

  QAction* unmark_fixed_action = new QAction("Unmark as fixed", this);
  connect(unmark_fixed_action, &QAction::triggered, this,
          [=] { apply_to_selected([](std::shared_ptr<Subject> s) { s->set_fixed(false); }); });
  menu.addAction(unmark_fixed_action);

  menu.exec(QCursor::pos());
}

//---------------------------------------------------------------------------
void DataTool::copy_ffc_clicked() {
  // get the selected row
  QModelIndexList list = ui_->ffc_table_->selectionModel()->selectedRows();
  if (list.size() < 1) {
    return;
  }
  int shape_id = list[0].row();

  // for each domain, copy the constraints from the selected shape to all other shapes
  auto shapes = session_->get_shapes();
  auto domain_names = session_->get_project()->get_domain_names();

  // get current display mode
  auto display_mode = session_->get_display_mode();

  for (int d = 0; d < domain_names.size(); d++) {
    auto constraints = shapes[shape_id]->get_constraints(d);
    auto ffc = constraints.getFreeformConstraint();

    // transform from source shape to common space
    auto base_transform = shapes[shape_id]->get_transform(d);

    for (int i = 0; i < shapes.size(); i++) {
      if (i == shape_id) {
        continue;
      }

      // transform from common space to destination space
      auto inverse = shapes[i]->get_inverse_transform(d);

      // get the polydata, transform and set to the new ffc
      ffc.createInoutPolyData();
      auto ffc_poly_data = ffc.getInoutPolyData();

      // apply vtk transforms to poly data
      auto transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
      transform_filter->SetInputData(ffc_poly_data);
      transform_filter->SetTransform(base_transform);
      transform_filter->Update();
      ffc_poly_data = transform_filter->GetOutput();
      transform_filter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
      transform_filter->SetInputData(ffc_poly_data);
      transform_filter->SetTransform(inverse);
      transform_filter->Update();
      ffc_poly_data = transform_filter->GetOutput();

      // apply the new ffc to the destination shape
      auto meshes = shapes[i]->get_meshes(display_mode, true);
      MeshHandle mesh = meshes.meshes()[d];
      auto poly_data = mesh->get_poly_data();

      // create a new FreeFormConstraint with the transformed source definition
      FreeFormConstraint new_ffc;
      new_ffc.setInoutPolyData(ffc_poly_data);
      new_ffc.applyToPolyData(poly_data);

      // store the new ffc
      auto& this_ffc = shapes[i]->get_constraints(d).getFreeformConstraint();
      this_ffc.setDefinition(poly_data);
      this_ffc.setInoutPolyData(poly_data);
      this_ffc.setPainted(true);
    }
  }
  session_->trigger_ffc_changed();
  session_->trigger_repaint();
}

//---------------------------------------------------------------------------
void DataTool::table_selection_changed() {
  // if only one row is selected, update the subject_notes, otherwise disable it
  QModelIndexList list = ui_->table->selectionModel()->selectedRows();
  if (list.size() == 1) {
    int row = list[0].row();
    if (row >= session_->get_shapes().size()) {
      ui_->subject_notes->setText("");
      ui_->subject_notes->setEnabled(false);
      return;
    }
    auto shape = session_->get_shapes()[row];
    ui_->subject_notes->setText(QString::fromStdString(shape->get_subject()->get_notes()));
    ui_->subject_notes->setEnabled(true);
  } else {
    ui_->subject_notes->setText("");
    ui_->subject_notes->setEnabled(false);
  }
}

//---------------------------------------------------------------------------
void DataTool::subject_notes_changed() {
  // update the notes for the selected subject
  QModelIndexList list = ui_->table->selectionModel()->selectedRows();
  if (list.size() == 1) {
    int row = list[0].row();
    auto shape = session_->get_shapes()[row];
    auto old_notes = shape->get_subject()->get_notes();
    auto new_notes = ui_->subject_notes->toPlainText().toStdString();
    if (old_notes != new_notes) {
      shape->get_subject()->set_notes(new_notes);
      // update the table
      update_table(false);
    }
  }
}

//---------------------------------------------------------------------------
void DataTool::table_data_edited() {
  if (block_table_update_) {
    return;
  }

  // store all the changes

  if (ui_->table->rowCount() != session_->get_shapes().size() || ui_->table->columnCount() < 2) {
    return;
  }

  bool change = false;

  // find name and notes columns
  int name_column = -1;
  int notes_column = -1;
  for (int i = 0; i < ui_->table->columnCount(); i++) {
    if (ui_->table->horizontalHeaderItem(i)->text() == "name") {
      name_column = i;
    } else if (ui_->table->horizontalHeaderItem(i)->text() == "notes") {
      notes_column = i;
    }
  }

  // iterate over all rows, not just selected
  for (int row = 0; row < ui_->table->rowCount(); row++) {
    auto shape = session_->get_shapes()[row];
    auto old_name = shape->get_subject()->get_display_name();
    auto old_notes = shape->get_subject()->get_notes();

    std::string new_name;
    if (name_column != -1) {
      new_name = ui_->table->item(row, name_column)->text().toStdString();
    }
    std::string new_notes;
    if (notes_column != -1) {
      new_notes = ui_->table->item(row, notes_column)->text().toStdString();
    }
    if (old_name != new_name) {
      shape->get_subject()->set_display_name(new_name);
      shape->update_annotations();
      session_->trigger_annotations_changed();
      change = true;
    }
    if (old_notes != new_notes) {
      shape->get_subject()->set_notes(new_notes);
      change = true;
    }
  }

  if (change) {
    table_selection_changed();
  }
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
