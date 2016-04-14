// std
#include <iostream>

// qt
#include <QFileDialog>
#include <QWidgetAction>
#include <QMessageBox>
#include <QCloseEvent>
// vtk
#include <vtkRenderWindow.h>
#include <vtkPolyDataWriter.h>

// studio
#include <Visualization/ShapeWorksStudioApp.h>
#include <Data/Preferences.h>
#include <Groom/GroomTool.h>
#include <Optimize/OptimizeTool.h>
#include <Analysis/AnalysisTool.h>
#include <Data/Project.h>
#include <Data/Shape.h>
#include <Data/Mesh.h>
#include <Visualization/Lightbox.h>
#include <Visualization/DisplayObject.h>
#include <Visualization/Visualizer.h>
#include <Visualization/WheelEventForwarder.h>

// ui
#include <ui_ShapeWorksStudioApp.h>

//---------------------------------------------------------------------------
ShapeWorksStudioApp::ShapeWorksStudioApp(int argc, char** argv)
{
  this->ui_ = new Ui_ShapeWorksStudioApp;
  this->ui_->setupUi(this);

  this->recent_file_actions_.append(this->ui_->action_recent1);
  this->recent_file_actions_.append(this->ui_->action_recent2);
  this->recent_file_actions_.append(this->ui_->action_recent3);
  this->recent_file_actions_.append(this->ui_->action_recent4);

  for (int i = 0; i < 4; i++)
  {
    connect(this->recent_file_actions_[i], SIGNAL(triggered()),
      this, SLOT(handle_open_recent()));
  }
  this->update_recent_files();

  this->wheel_event_forwarder_ = QSharedPointer<WheelEventForwarder>
    (new WheelEventForwarder(this->ui_->vertical_scroll_bar));
  this->ui_->qvtkWidget->installEventFilter(this->wheel_event_forwarder_.data());

  // Glyph options in the render window.
  QMenu* menu = new QMenu();
  QWidget* widget = new QWidget();
  QGridLayout* layout = new QGridLayout(widget);

  QLabel* size_label = new QLabel("Glyph Size: ");
  layout->addWidget(size_label, 0, 0, 1, 1);
  size_label = new QLabel("Glyph Detail: ");
  layout->addWidget(size_label, 1, 0, 1, 1);

  this->glyph_quality_label_ = new QLabel("....");
  this->glyph_quality_label_->setMinimumWidth(50);
  this->glyph_size_label_ = new QLabel("....");
  this->glyph_size_label_->setMinimumWidth(50);
  layout->addWidget(this->glyph_size_label_, 0, 1, 1, 1);
  layout->addWidget(this->glyph_quality_label_, 1, 1, 1, 1);

  this->glyph_size_slider_ = new QSlider(widget);
  this->glyph_size_slider_->setOrientation(Qt::Horizontal);
  this->glyph_size_slider_->setMinimum(1);
  this->glyph_size_slider_->setMaximum(100);
  this->glyph_size_slider_->setPageStep(10);
  this->glyph_size_slider_->setTickPosition(QSlider::TicksBelow);
  this->glyph_size_slider_->setTickInterval(10);
  this->glyph_size_slider_->setMinimumWidth(200);

  this->glyph_quality_slider_ = new QSlider(widget);
  this->glyph_quality_slider_->setMinimum(1);
  this->glyph_quality_slider_->setMaximum(20);
  this->glyph_quality_slider_->setPageStep(3);
  this->glyph_quality_slider_->setOrientation(Qt::Horizontal);
  this->glyph_quality_slider_->setTickPosition(QSlider::TicksBelow);
  this->glyph_quality_slider_->setTickInterval(1);
  this->glyph_quality_slider_->setMinimumWidth(200);
  this->ui_->glyphs_visible_button->setMenu(menu);

  layout->addWidget(this->glyph_size_slider_, 0, 2, 1, 1);
  layout->addWidget(this->glyph_quality_slider_, 1, 2, 1, 1);
  widget->setLayout(layout);

  QWidgetAction* widget_action = new QWidgetAction(widget);
  widget_action->setDefaultWidget(widget);
  menu->addAction(widget_action);

  // Isosurface options in the render window.
  menu = new QMenu();
  widget = new QWidget();
  layout = new QGridLayout(widget);
  //column 1 (labels)
  size_label = new QLabel("Neighborhood Size: ");
  layout->addWidget(size_label, 0, 0, 1, 1);
  size_label = new QLabel("Spacing: ");
  layout->addWidget(size_label, 1, 0, 1, 1);
  size_label = new QLabel("Smoothing iterations: ");
  layout->addWidget(size_label, 2, 0, 1, 1);
  //column 2 (number labels)
  size_label = new QLabel("");
  layout->addWidget(size_label, 0, 1, 1, 1);
  layout->addWidget(size_label, 1, 1, 1, 1);
  this->iso_smoothing_label_ = new QLabel("0");
  this->iso_smoothing_label_->setMinimumWidth(50);
  layout->addWidget(this->iso_smoothing_label_, 2, 1, 1, 1);
  //column 3 (the text/slider widgets)
  //        neighborhood
  this->iso_neighborhood_spinner_ = new QSpinBox(widget);
  this->iso_neighborhood_spinner_->setMinimumWidth(200);
  this->iso_neighborhood_spinner_->setMinimum(1);
  this->iso_neighborhood_spinner_->setMaximum(99);
  this->iso_neighborhood_spinner_->setSingleStep(1);
  layout->addWidget(this->iso_neighborhood_spinner_, 0, 2, 1, 1);
  //        spacing
  this->iso_spacing_spinner_ = new QDoubleSpinBox(widget);
  this->iso_spacing_spinner_->setMinimumWidth(200);
  this->iso_spacing_spinner_->setMinimum(0.00);
  this->iso_spacing_spinner_->setMaximum(99.99);
  this->iso_spacing_spinner_->setSingleStep(1.0);
  layout->addWidget(this->iso_spacing_spinner_, 1, 2, 1, 1);
  //        smoothing
  this->iso_smoothing_slider_ = new QSlider(widget);
  this->iso_smoothing_slider_->setOrientation(Qt::Horizontal);
  this->iso_smoothing_slider_->setMinimum(0);
  this->iso_smoothing_slider_->setMaximum(5);
  this->iso_smoothing_slider_->setSingleStep(1);
  this->iso_smoothing_slider_->setTickPosition(QSlider::TicksBelow);
  this->iso_smoothing_slider_->setTickInterval(1);
  this->iso_smoothing_slider_->setMinimumWidth(200);
  layout->addWidget(this->iso_smoothing_slider_, 2, 2, 1, 1);
  // add to the tool button
  this->ui_->surface_visible_button->setMenu(menu);
  widget->setLayout(layout);

  widget_action = new QWidgetAction(widget);
  widget_action->setDefaultWidget(widget);
  menu->addAction(widget_action);

  /*
     QToolButton* tool_button = new QToolButton();
     tool_button->setMaximumWidth( 15 );
     tool_button->setMenu( menu );
     tool_button->setPopupMode( QToolButton::InstantPopup );
     this->ui_->glyph_layout->addWidget( tool_button );
     */

  //project initializations
  this->project_ = QSharedPointer<Project>(new Project(preferences_));
  this->project_->set_parent(this);
  connect(this->project_.data(), SIGNAL(data_changed()), this, SLOT(handle_project_changed()));
  connect(this->project_.data(), SIGNAL(update_display()), this, SLOT(handle_display_setting_changed()));

  // setup modes
  this->ui_->view_mode_combobox->addItem(Visualizer::MODE_ORIGINAL_C.c_str());
  this->ui_->view_mode_combobox->addItem(Visualizer::MODE_GROOMED_C.c_str());
  this->ui_->view_mode_combobox->addItem(Visualizer::MODE_RECONSTRUCTION_C.c_str());
  this->ui_->view_mode_combobox->setCurrentIndex(0);
  this->ui_->view_mode_combobox->setItemData(1, 0, Qt::UserRole - 1);
  this->ui_->view_mode_combobox->setItemData(2, 0, Qt::UserRole - 1);

  // resize from preferences
  this->resize(preferences_.get_preference("main_window_size", QSize()));

  // set to import
  this->ui_->action_import_mode->setChecked(true);
  this->ui_->stacked_widget->setCurrentIndex(0);

  this->action_group_ = new QActionGroup(this);
  this->action_group_->addAction(this->ui_->action_import_mode);
  this->action_group_->addAction(this->ui_->action_groom_mode);
  this->action_group_->addAction(this->ui_->action_optimize_mode);
  this->action_group_->addAction(this->ui_->action_analysis_mode);
  this->ui_->action_import_mode->setEnabled(true);
  this->ui_->action_groom_mode->setEnabled(false);
  this->ui_->action_optimize_mode->setEnabled(false);
  this->ui_->action_analysis_mode->setEnabled(false);

  this->ui_->statusbar->showMessage("ShapeWorksStudio");
  this->lightbox_ = LightboxHandle(new Lightbox());

  //visualizer initializations
  this->visualizer_ = QSharedPointer<Visualizer>(new Visualizer(preferences_));
  this->visualizer_->set_lightbox(this->lightbox_);
  this->visualizer_->set_project(this->project_);

  this->reset();

  //groom tool initializations
  this->groom_tool_ = QSharedPointer<GroomTool>(new GroomTool(preferences_, this->originalFilenames_));
  this->groom_tool_->set_project(this->project_);
  this->groom_tool_->set_app(this);
  this->ui_->stacked_widget->addWidget(this->groom_tool_.data());
  connect(this->groom_tool_.data(), SIGNAL(groom_complete()), this, SLOT(handle_groom_complete()));

  //optimize tool initializations
  this->optimize_tool_ = QSharedPointer<OptimizeTool>(new OptimizeTool(preferences_));
  this->optimize_tool_->set_project(this->project_);
  this->optimize_tool_->set_app(this);
  this->ui_->stacked_widget->addWidget(this->optimize_tool_.data());
  connect(this->optimize_tool_.data(), SIGNAL(optimize_complete()), this, SLOT(handle_optimize_complete()));

  //set up preferences window
  this->preferences_window_ = QSharedPointer<PreferencesWindow>(new PreferencesWindow(this, preferences_));
  connect(this->preferences_window_.data(), SIGNAL(clear_cache()), this->project_.data(), SLOT(handle_clear_cache()));
  connect(this->preferences_window_.data(), SIGNAL(clear_cache()), this, SLOT(handle_pca_changed()));
  connect(this->preferences_window_.data(), SIGNAL(update_view()), this, SLOT(handle_color_scheme()));

  //analysis tool initializations
  this->analysis_tool_ = QSharedPointer<AnalysisTool>(new AnalysisTool(preferences_));
  this->analysis_tool_->set_project(this->project_);
  this->analysis_tool_->set_app(this);
  this->ui_->stacked_widget->addWidget(this->analysis_tool_.data());
  connect(this->analysis_tool_.data(), SIGNAL(update_view()), this, SLOT(handle_display_setting_changed()));
  connect(this->analysis_tool_.data(), SIGNAL(pca_update()), this, SLOT(handle_new_mesh()));


  //regression tool TODO
  /*this->analysis_tool_ = QSharedPointer<AnalysisTool> (new AnalysisTool());
    this->analysis_tool_->set_project( this->project_ );
    this->analysis_tool_->set_app( this );
    this->ui_->lower_stacked_widget->addWidget( this->analysis_tool_.data() );*/


  this->update_from_preferences();
  this->update_display();

  //glyph options signals/slots
  connect(this->ui_->glyphs_visible_button, SIGNAL(clicked()), this, SLOT(handle_glyph_changed()));
  connect(this->ui_->surface_visible_button, SIGNAL(clicked()), this, SLOT(handle_glyph_changed()));
  connect(this->glyph_size_slider_, SIGNAL(valueChanged(int)), this, SLOT(handle_glyph_changed()));
  connect(this->glyph_quality_slider_, SIGNAL(valueChanged(int)), this, SLOT(handle_glyph_changed()));
  //isosurface options signals/slots
  connect(this->iso_neighborhood_spinner_, SIGNAL(valueChanged(int)), this, SLOT(handle_pca_changed()));
  connect(this->iso_spacing_spinner_, SIGNAL(valueChanged(double)), this, SLOT(handle_pca_changed()));
  connect(this->iso_smoothing_slider_, SIGNAL(valueChanged(int)), this, SLOT(handle_pca_changed()));
}

//---------------------------------------------------------------------------
ShapeWorksStudioApp::~ShapeWorksStudioApp()
{

}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::initialize_vtk()
{
  this->lightbox_->set_render_window(this->ui_->qvtkWidget->GetRenderWindow());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_new_project_triggered()
{
  this->originalFilenames_.clear();
  QList<int> index_list;

  for (int i = this->project_->get_num_shapes() - 1; i >= 0; i--)
  {
    index_list << i;
  }

  this->ui_->action_groom_mode->setEnabled(false);
  this->ui_->action_optimize_mode->setEnabled(false);
  this->ui_->action_analysis_mode->setEnabled(false);
  this->project_->remove_shapes(index_list);
  this->project_->reset();
  this->lightbox_->clear_renderers();
  this->reset();
  this->analysis_tool_->reset_stats();
  this->update_display();
  this->ui_->action_import_mode->setChecked(true);
  this->ui_->action_groom_mode->setChecked(false);
  this->ui_->action_optimize_mode->setChecked(false);
  this->ui_->action_analysis_mode->setChecked(false);
  this->project_->set_tool_state(Project::DATA_C);
  this->ui_->stacked_widget->setCurrentWidget(this->ui_->import_page);
  this->ui_->controlsDock->setWindowTitle("Data");
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionShow_Tool_Window_triggered() {
  this->ui_->controlsDock->setVisible(true);
  this->ui_->controlsDock->show();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_open_project_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Open Project..."),
    this->preferences_.get_preference("last_directory", QString()),
    tr("XML files (*.xml)"));
  if (filename.isEmpty())
  {
    return;
  }

  this->preferences_.set_preference("last_directory", QDir().absoluteFilePath(filename));

  this->open_project(filename);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_save_project_triggered()
{
  if (this->project_->get_filename() == "")
  {
    this->on_action_save_project_as_triggered();
  } else
  {
    if (this->project_->save_project())
    {
      this->set_status_bar("Project Saved");
    }
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_save_project_as_triggered()
{
  QString fname("Untitiled.xml");
  if (this->project_->get_shapes().size() > 0) {
    fname = this->project_->get_shapes()[0]->get_original_filename();
    std::string tmp = fname.toStdString();
    tmp = tmp.substr(0, tmp.size() - 5);
    fname = QString::fromStdString(tmp);
  }
  QString direct = this->preferences_.get_preference("last_directory", QString());
  auto dir = direct.toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Project As..."),
    QString::fromStdString(dir) + fname,
    tr("XML files (*.xml)"));
  if (filename.isEmpty()) {
    return;
  }
  this->preferences_.add_recent_file(filename);
  this->update_recent_files();

  this->preferences_.set_preference("last_directory", QDir().absoluteFilePath(filename));

  if (this->project_->save_project(filename))
  {
    this->set_status_bar("Project Saved");
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_quit_triggered()
{
  this->close();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_import_triggered()
{
  QStringList filenames;

  filenames = QFileDialog::getOpenFileNames(this, tr("Import Files..."),
    preferences_.get_preference("last_directory", QString()),
    tr("NRRD files (*.nrrd);;MHA files (*.mha)"));

  if (filenames.size() == 0) {
    return;
  }
  for (size_t i = 0; i < filenames.size(); i++) {
    this->originalFilenames_.push_back(filenames.at(i).toStdString());
  }

  preferences_.set_preference("last_directory", QDir().absoluteFilePath(filenames[0]));
  //need to re-run everything if something new is added.
  this->ui_->view_mode_combobox->setCurrentIndex(0);
  this->ui_->view_mode_combobox->setItemData(1, 0, Qt::UserRole - 1);
  this->ui_->view_mode_combobox->setItemData(2, 0, Qt::UserRole - 1);
  this->project_->set_display_state(this->ui_->view_mode_combobox->currentText().toStdString());
  this->visualizer_->set_display_mode(this->ui_->view_mode_combobox->currentText().toStdString());
  this->import_files(filenames);
  this->visualizer_->update_lut();
  this->ui_->action_groom_mode->setEnabled(true);
  this->ui_->action_optimize_mode->setEnabled(false);
  this->ui_->action_analysis_mode->setEnabled(false);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::import_files(QStringList file_names)
{
  this->project_->load_original_files(file_names);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_thumbnail_size_slider_valueChanged()
{
  if (!this->lightbox_->render_window_ready()) return;
  this->project_->set_zoom_state(this->ui_->thumbnail_size_slider->value());

  int value = this->ui_->thumbnail_size_slider->maximum() - this->ui_->thumbnail_size_slider->value() + 1;

  this->lightbox_->set_tile_layout(value, value);
  this->visualizer_->update_viewer_properties();

  this->update_scrollbar();

  this->ui_->qvtkWidget->GetRenderWindow()->Render();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_from_preferences()
{
  this->glyph_quality_slider_->setValue(preferences_.get_preference("glyph_quality", 5.));
  this->glyph_quality_label_->setText(QString::number(preferences_.get_preference("glyph_quality", 5.)));
  this->glyph_size_slider_->setValue(preferences_.get_preference("glyph_size", 1.) * 10.0);
  this->glyph_size_label_->setText(QString::number(preferences_.get_preference("glyph_size", 1.)));
  this->iso_smoothing_label_->setText(QString::number(preferences_.get_preference("smoothing_amount", 0.)));

  this->iso_smoothing_slider_->setValue(preferences_.get_preference("smoothing_amount", 0.) / 100);
  this->iso_neighborhood_spinner_->setValue(preferences_.get_preference("neighborhood", 5));
  this->iso_spacing_spinner_->setValue(preferences_.get_preference("spacing", 1.f));

}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_scrollbar()
{
  int num_rows = this->lightbox_->get_num_rows();
  int num_visible = this->lightbox_->get_num_rows_visible();
  std::cerr << "num_rows = " << num_rows << "\n";
  std::cerr << "num_visible = " << num_visible << "\n";
  if (num_visible >= num_rows)
  {
    this->ui_->vertical_scroll_bar->setMaximum(0);
    this->ui_->vertical_scroll_bar->setEnabled(false);
  } else
  {
    this->ui_->vertical_scroll_bar->setEnabled(true);
    this->ui_->vertical_scroll_bar->setMaximum(num_rows - num_visible);
    this->ui_->vertical_scroll_bar->setPageStep(num_visible);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_vertical_scroll_bar_valueChanged()
{
  int value = this->ui_->vertical_scroll_bar->value();

  std::cerr << "vertical scrollbar value = " << value << "\n";
  this->lightbox_->set_start_row(value);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_add_button_clicked()
{
  this->on_action_import_triggered();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_delete_button_clicked()
{

  QModelIndexList list = this->ui_->table_widget->selectionModel()->selectedRows();

  QList<int> index_list;
  for (int i = list.size() - 1; i >= 0; i--) {
    index_list << list[i].row();
  }
  std::vector<std::string> newList;
  for (size_t i = 0; i < this->originalFilenames_.size(); i++) {
    if (!index_list.contains(i)) {
      newList.push_back(this->originalFilenames_[i]);
    }
  }
  this->originalFilenames_ = newList;

  this->project_->remove_shapes(index_list);
  if (this->project_->get_shapes().size() == 0) {
    this->project_->reset();
    this->reset();
    this->analysis_tool_->reset_stats();
    this->project_->set_tool_state(Project::DATA_C);
    this->ui_->action_groom_mode->setEnabled(false);
    this->ui_->action_optimize_mode->setEnabled(false);
    this->ui_->action_analysis_mode->setEnabled(false);
    this->lightbox_->clear_renderers();
    this->update_display();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_table()
{

  QVector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();

  this->ui_->table_widget->clear();

  this->ui_->table_widget->setRowCount(shapes.size());
  this->ui_->table_widget->setColumnCount(3);

  QStringList table_header;
  table_header << "#" << "Name" << "Size";
  this->ui_->table_widget->setHorizontalHeaderLabels(table_header);

  this->ui_->table_widget->verticalHeader()->setVisible(false);

  for (int i = 0; i < shapes.size(); i++)
  {
    QSharedPointer<Mesh> original_mesh = shapes[i]->get_original_mesh();

    QTableWidgetItem* new_item = new QTableWidgetItem(QString::number(i + 1));
    this->ui_->table_widget->setItem(i, 0, new_item);

    new_item = new QTableWidgetItem(shapes[i]->get_original_filename());
    this->ui_->table_widget->setItem(i, 1, new_item);

    if (original_mesh)
    {
      new_item = new QTableWidgetItem(original_mesh->get_dimension_string());
      this->ui_->table_widget->setItem(i, 2, new_item);
    }
  }

  this->ui_->table_widget->resizeColumnsToContents();
  //this->ui_->table_widget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  this->ui_->table_widget->horizontalHeader()->setStretchLastSection(true);

  this->ui_->table_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_pca_changed() {
  preferences_.set_preference("neighborhood", this->iso_neighborhood_spinner_->value());
  preferences_.set_preference("spacing", this->iso_spacing_spinner_->value());
  preferences_.set_preference("smoothing_amount", this->iso_smoothing_slider_->value() * 100);
  this->update_from_preferences();

  if (!this->project_->reconstructed_present()) return;
  this->project_->handle_clear_cache();
  this->visualizer_->update_lut();
  this->compute_mode_shape();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_new_mesh() {
  this->compute_mode_shape();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_groom_mode_triggered()
{
  this->project_->set_tool_state(Project::GROOM_C);
  this->ui_->stacked_widget->setCurrentWidget(this->groom_tool_.data());
  this->ui_->controlsDock->setWindowTitle("Groom");
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_import_mode_triggered()
{
  this->project_->set_tool_state(Project::DATA_C);
  this->ui_->stacked_widget->setCurrentIndex(0);
  this->ui_->controlsDock->setWindowTitle("Data");
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_optimize_mode_triggered()
{
  this->project_->set_tool_state(Project::OPTIMIZE_C);
  this->ui_->stacked_widget->setCurrentWidget(this->optimize_tool_.data());
  this->ui_->controlsDock->setWindowTitle("Optimize");
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_analysis_mode_triggered()
{
  this->project_->set_tool_state(Project::ANALYSIS_C);
  this->ui_->stacked_widget->setCurrentWidget(this->analysis_tool_.data());
  this->ui_->controlsDock->setWindowTitle("Analysis");
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_project_changed()
{
  QVector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();
  if (shapes.size() < 1)
    this->ui_->action_groom_mode->setEnabled(false);
  else
    this->ui_->action_groom_mode->setEnabled(true);

  if (this->project_->groomed_present())
  {
    this->ui_->view_mode_combobox->setItemData(1, 33, Qt::UserRole - 1);
    this->ui_->action_optimize_mode->setEnabled(true);
  } else
  {
    this->ui_->view_mode_combobox->setItemData(1, 0, Qt::UserRole - 1);
    this->ui_->action_optimize_mode->setEnabled(false);
  }

  if (this->project_->reconstructed_present())
  {
    this->ui_->view_mode_combobox->setItemData(2, 33, Qt::UserRole - 1);
    this->ui_->action_analysis_mode->setEnabled(true);
    this->project_->handle_clear_cache();
  } else
  {
    this->ui_->view_mode_combobox->setItemData(2, 0, Qt::UserRole - 1);
    this->ui_->action_analysis_mode->setEnabled(false);
  }
  this->update_table();
  this->update_scrollbar();
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_optimize_complete()
{
  this->analysis_tool_->reset_stats();
  this->project_->handle_clear_cache();
  this->ui_->action_analysis_mode->setEnabled(true);
  this->set_status_bar("Optimize complete");
  this->ui_->view_mode_combobox->setItemData(2, 33, Qt::UserRole - 1);
  this->ui_->view_mode_combobox->setCurrentIndex(2);
  this->project_->set_display_state(this->ui_->view_mode_combobox->currentText().toStdString());
  this->visualizer_->set_display_mode(this->ui_->view_mode_combobox->currentText().toStdString());
  this->visualizer_->setMean(this->analysis_tool_->getMean());
  this->visualizer_->update_lut();
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_groom_complete()
{
  this->set_status_bar("Groom complete");
  this->ui_->view_mode_combobox->setItemData(1, 33, Qt::UserRole - 1);
  this->ui_->view_mode_combobox->setCurrentIndex(1);
  this->ui_->action_optimize_mode->setEnabled(true);
  this->ui_->action_analysis_mode->setEnabled(false);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_display_setting_changed()
{
  if (this->analysis_tool_->pcaAnimate()) return;
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_glyph_changed()
{
  this->visualizer_->set_show_surface(this->ui_->surface_visible_button->isChecked());
  this->visualizer_->set_show_glyphs(this->ui_->glyphs_visible_button->isChecked());
  preferences_.set_preference("glyph_size", this->glyph_size_slider_->value() / 10.0);
  preferences_.set_preference("glyph_quality", this->glyph_quality_slider_->value());
  this->update_from_preferences();
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_center_checkbox_stateChanged()
{
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_display()
{
  if (!this->visualizer_ || this->project_->get_num_shapes() <= 0) return;

  this->visualizer_->set_center(this->ui_->center_checkbox->isChecked());
  this->project_->set_display_state(this->ui_->view_mode_combobox->currentText().toStdString());
  std::string mode = this->analysis_tool_->getAnalysisMode();
  if (mode == "all samples") {
    this->visualizer_->display_samples();
    size_t num_samples = this->project_->get_shapes().size();
    if (num_samples == 0) num_samples = 9;
    double root = std::sqrt(static_cast<double>(num_samples));
    if (std::fmod(root, 1.0) > 1e-6) root += 1.;
    size_t value = static_cast<size_t>(root);
    size_t zoom_val = this->ui_->thumbnail_size_slider->maximum() + 1 - value;
    if (zoom_val != this->ui_->thumbnail_size_slider->value())
      this->ui_->thumbnail_size_slider->setValue(zoom_val);
  } else {
    if (this->ui_->thumbnail_size_slider->maximum() !=
      this->ui_->thumbnail_size_slider->value())
      this->ui_->thumbnail_size_slider->setValue(this->ui_->thumbnail_size_slider->maximum());
    if (mode == "mean") {
      this->ui_->view_mode_combobox->setCurrentIndex(2);
      this->visualizer_->display_shape(this->analysis_tool_->getMean());
    } else if (mode == "pca") {
      this->ui_->view_mode_combobox->setCurrentIndex(2);
      this->compute_mode_shape();
    } else if (mode == "single sample") {
      this->visualizer_->display_sample(this->analysis_tool_->getSampleNumber());
    } //TODO regression
  }
  this->project_->set_zoom_state(this->ui_->thumbnail_size_slider->value());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_view_mode_combobox_currentIndexChanged()
{
  if (this->visualizer_) {
    auto disp_mode = this->ui_->view_mode_combobox->currentText().toStdString();
    this->visualizer_->set_display_mode(disp_mode);
    this->update_display();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::reset() {
  this->ui_->view_mode_combobox->setCurrentIndex(0);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::open_project(QString filename)
{
  this->reset();
  this->project_->load_project(filename);
  this->groom_tool_->set_preferences();
  this->project_->calculate_reconstructed_samples();
  this->visualizer_->setMean(this->analysis_tool_->getMean());
  this->analysis_tool_->setAnalysisMode("all samples");
  this->analysis_tool_->reset_stats();

  preferences_.add_recent_file(filename);
  this->update_recent_files();

  // set UI state based on project
  if (this->project_->get_tool_state() == Project::DATA_C)
  {
    this->ui_->action_import_mode->trigger();
  } else if (this->project_->get_tool_state() == Project::GROOM_C)
  {
    this->ui_->action_groom_mode->trigger();
  } else if (this->project_->get_tool_state() == Project::OPTIMIZE_C)
  {
    this->ui_->action_optimize_mode->trigger();
  } else if (this->project_->get_tool_state() == Project::ANALYSIS_C)
  {
    this->ui_->action_analysis_mode->trigger();
  }

  // load display mode
  if (this->project_->get_display_state()
    == Visualizer::MODE_ORIGINAL_C)
  {
    this->ui_->view_mode_combobox->setCurrentIndex(0);
  } else if (this->project_->get_display_state()
    == Visualizer::MODE_GROOMED_C)
  {
    this->ui_->view_mode_combobox->setCurrentIndex(1);
  } else if (this->project_->get_display_state()
    == Visualizer::MODE_RECONSTRUCTION_C)
  {
    this->ui_->view_mode_combobox->setCurrentIndex(2);
  }

  // set the zoom state
  this->ui_->thumbnail_size_slider->setValue(this->project_->get_zoom_state());
  this->analysis_tool_->reset_stats();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::set_status_bar(QString status)
{
  this->ui_->statusbar->showMessage(status);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_preferences_triggered()
{
  this->preferences_window_->set_values_from_preferences();
  this->preferences_window_->show();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::closeEvent(QCloseEvent* event) {
  // close the preferences window in case it is open
  this->preferences_window_->close();
  if (this->preferences_.not_saved()) {
    // save the size of the window to preferences
    preferences_.set_preference("main_window_size", this->size());
    QMessageBox msgBox;
    msgBox.setText("Do you want to save your changes as a project file?");
    msgBox.setInformativeText("This will reload generated files and changed settings.");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Save) {
      this->on_action_save_project_as_triggered();
    } else if (ret == QMessageBox::Cancel) {
      event->ignore();
    }
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::compute_mode_shape()
{
  this->visualizer_->display_shape(this->analysis_tool_->getShape());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_recent_files()
{
  QStringList recent_files = preferences_.get_recent_files();

  int num_recent_files = qMin(recent_files.size(), (int)Preferences::MAX_RECENT_FILES);

  for (int i = 0; i < num_recent_files; i++)
  {

    QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(recent_files[i]).fileName());
    this->recent_file_actions_[i]->setText(text);
    this->recent_file_actions_[i]->setData(recent_files[i]);
    this->recent_file_actions_[i]->setVisible(true);
  }

  for (int j = num_recent_files; j < Preferences::MAX_RECENT_FILES; ++j)
  {
    this->recent_file_actions_[j]->setVisible(false);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_open_recent()
{
  QAction* action = qobject_cast<QAction*>(sender());
  if (action)
  {
    this->open_project(action->data().toString());
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_color_scheme()
{
  this->visualizer_->update_viewer_properties();
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_auto_view_button_clicked() {
  this->visualizer_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionExport_PCA_Mesh_triggered() {
  QString direct = preferences_.get_preference("last_directory", QString());
  auto dir = direct.toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Project As..."),
    QString::fromStdString(dir) + "newMesh",
    tr("VTK files (*.vtk)"));
  if (filename.isEmpty())
  {
    return;
  }
  auto shape = this->visualizer_->getCurrentShape();
  MeshGenerator g(this->preferences_);
  auto meshFilter = g.buildMeshOutputFilter(shape);
  vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
  writer->SetFileName(filename.toStdString().c_str());
  writer->SetInputConnection(meshFilter->GetOutputPort());
  writer->Write();
}