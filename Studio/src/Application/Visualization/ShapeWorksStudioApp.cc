// std
#include <iostream>

// qt
#include <QFileDialog>
#include <QWidgetAction>
#include <QMessageBox>
#include <QCloseEvent>
#include <QXmlStreamWriter>
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
  this->progressBar_ = new QProgressBar(this);
  this->ui_->statusbar->addPermanentWidget(this->progressBar_);
  this->progressBar_->setVisible(false);

  this->recent_file_actions_.append(this->ui_->action_recent1);
  this->recent_file_actions_.append(this->ui_->action_recent2);
  this->recent_file_actions_.append(this->ui_->action_recent3);
  this->recent_file_actions_.append(this->ui_->action_recent4);

  for (int i = 0; i < 4; i++) {
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

  //project initializations
  this->project_ = QSharedPointer<Project>(new Project(this, preferences_));
  this->project_->set_parent(this);
  connect(this->project_.data(), SIGNAL(data_changed()), this, SLOT(handle_project_changed()));
  connect(this->project_.data(), SIGNAL(points_changed()), this, SLOT(handle_points_changed()));
  connect(this->project_.data(), SIGNAL(update_display()), this,
          SLOT(handle_display_setting_changed()));
  connect(this->project_.data(), SIGNAL(message(std::string)), this,
          SLOT(handle_message(std::string)));

  // setup modes
  this->ui_->view_mode_combobox->addItem(Visualizer::MODE_ORIGINAL_C.c_str());
  this->ui_->view_mode_combobox->addItem(Visualizer::MODE_GROOMED_C.c_str());
  this->ui_->view_mode_combobox->addItem(Visualizer::MODE_RECONSTRUCTION_C.c_str());
  this->ui_->view_mode_combobox->setCurrentIndex(0);
  this->ui_->view_mode_combobox->setItemData(1, 0, Qt::UserRole - 1);
  this->ui_->view_mode_combobox->setItemData(2, 0, Qt::UserRole - 1);

  // resize from preferences
  if (this->preferences_.has_entry("StudioWindow/geometry")) {
    this->restoreGeometry(this->preferences_.get_preference("StudioWindow/geometry", QByteArray()));
  }
  if (this->preferences_.has_entry("StudioWindow/windowState")) {
    this->restoreState(this->preferences_.get_preference("StudioWindow/windowState", QByteArray()));
  }

  // set to import
  this->ui_->action_import_mode->setChecked(true);
  this->ui_->stacked_widget->setCurrentIndex(0);

  this->action_group_ = new QActionGroup(this);
  this->action_group_->addAction(this->ui_->action_import_mode);
  this->action_group_->addAction(this->ui_->action_groom_mode);
  this->action_group_->addAction(this->ui_->action_optimize_mode);
  this->action_group_->addAction(this->ui_->action_analysis_mode);

  this->ui_->statusbar->showMessage("ShapeWorksStudio");
  this->lightbox_ = LightboxHandle(new Lightbox());

  //visualizer initializations
  this->visualizer_ = QSharedPointer<Visualizer>(new Visualizer(preferences_));
  this->visualizer_->set_lightbox(this->lightbox_);
  this->visualizer_->set_project(this->project_);

  //groom tool initializations
  this->groom_tool_ = QSharedPointer<GroomTool>(new GroomTool(preferences_,
                                                              this->originalFilenames_));
  this->groom_tool_->set_project(this->project_);
  this->ui_->stacked_widget->addWidget(this->groom_tool_.data());
  connect(this->groom_tool_.data(), SIGNAL(groom_complete()), this, SLOT(handle_groom_complete()));
  connect(this->groom_tool_.data(), SIGNAL(error_message(std::string)),
          this, SLOT(handle_error(std::string)));
  connect(this->groom_tool_.data(), SIGNAL(message(std::string)),
          this, SLOT(handle_message(std::string)));
  connect(this->groom_tool_.data(), SIGNAL(progress(size_t)),
          this, SLOT(handle_progress(size_t)));

  //optimize tool initializations
  this->optimize_tool_ = QSharedPointer<OptimizeTool>(new OptimizeTool(preferences_));
  this->optimize_tool_->set_project(this->project_);
  this->ui_->stacked_widget->addWidget(this->optimize_tool_.data());
  connect(this->optimize_tool_.data(), SIGNAL(optimize_complete()),
          this, SLOT(handle_optimize_complete()));

  connect(this->optimize_tool_.data(), &OptimizeTool::optimize_start, this,
          &ShapeWorksStudioApp::handle_optimize_start);

  connect(this->optimize_tool_.data(), SIGNAL(error_message(std::string)),
          this, SLOT(handle_error(std::string)));
  connect(this->optimize_tool_.data(), SIGNAL(warning_message(std::string)),
          this, SLOT(handle_warning(std::string)));
  connect(this->optimize_tool_.data(), SIGNAL(message(std::string)),
          this, SLOT(handle_message(std::string)));
  connect(this->optimize_tool_.data(), SIGNAL(progress(size_t)),
          this, SLOT(handle_progress(size_t)));

  //set up preferences window
  this->preferences_window_ =
    QSharedPointer<PreferencesWindow>(new PreferencesWindow(this, preferences_));
  this->preferences_window_->set_values_from_preferences();
  connect(this->preferences_window_.data(), SIGNAL(clear_cache()), this->project_.data(),
          SLOT(handle_clear_cache()));
  connect(this->preferences_window_.data(), SIGNAL(clear_cache()), this,
          SLOT(handle_pca_changed()));
  connect(this->preferences_window_.data(), SIGNAL(update_view()), this,
          SLOT(handle_color_scheme()));
  connect(this->preferences_window_.data(), SIGNAL(slider_update()), this,
          SLOT(handle_slider_update()));

  //analysis tool initializations
  this->analysis_tool_ = QSharedPointer<AnalysisTool>(new AnalysisTool(preferences_));
  this->analysis_tool_->set_project(this->project_);
  this->analysis_tool_->set_app(this);
  this->ui_->stacked_widget->addWidget(this->analysis_tool_.data());
  connect(this->analysis_tool_.data(), SIGNAL(update_view()), this,
          SLOT(handle_display_setting_changed()));
  connect(this->analysis_tool_.data(), SIGNAL(pca_update()), this, SLOT(handle_new_mesh()));
  connect(this->analysis_tool_.data(), SIGNAL(progress(size_t)),
          this, SLOT(handle_progress(size_t)));
  connect(this->analysis_tool_.data(), SIGNAL(reconstruction_complete()),
          this, SLOT(handle_reconstruction_complete()));

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
  connect(this->glyph_quality_slider_, SIGNAL(valueChanged(int)), this,
          SLOT(handle_glyph_changed()));
  this->preferences_.set_saved();
  this->enablePossibleActions();
}

//---------------------------------------------------------------------------
ShapeWorksStudioApp::~ShapeWorksStudioApp()
{}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::initialize_vtk()
{
  this->lightbox_->set_render_window(this->ui_->qvtkWidget->GetRenderWindow());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_new_project_triggered()
{
  if (this->preferences_.not_saved() && this->ui_->action_save_project->isEnabled()) {
    // save the size of the window to preferences
    QMessageBox msgBox;
    msgBox.setText("Do you want to save your changes as a project file?");
    msgBox.setInformativeText("This will reload generated files and changed settings.");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Save) {
      if (!this->on_action_save_project_triggered()) {
        return;
      }
    }
    else if (ret == QMessageBox::Cancel) {
      return;
    }
  }
  this->update_from_preferences();
  this->originalFilenames_.clear();
  QList<int> index_list;

  for (int i = this->project_->get_num_shapes() - 1; i >= 0; i--) {
    index_list << i;
  }

  this->project_->remove_shapes(index_list);
  this->project_->reset();
  this->lightbox_->clear_renderers();
  this->analysis_tool_->reset_stats();
  this->update_display();
  this->ui_->action_import_mode->setChecked(true);
  this->ui_->action_groom_mode->setChecked(false);
  this->ui_->action_optimize_mode->setChecked(false);
  this->ui_->action_analysis_mode->setChecked(false);
  this->preferences_.set_preference("tool_state", QString::fromStdString(Project::DATA_C));
  this->ui_->stacked_widget->setCurrentWidget(this->ui_->import_page);
  this->ui_->controlsDock->setWindowTitle("Data");
  this->preferences_.set_saved();
  this->enablePossibleActions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionShow_Tool_Window_triggered()
{
  this->ui_->controlsDock->setVisible(true);
  this->ui_->controlsDock->show();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_open_project_triggered()
{
  QString filename = QFileDialog::getOpenFileName(this, tr("Open Project..."),
                                                  this->preferences_.get_preference(
                                                    "Main/last_directory", QString()),
                                                  tr("XML files (*.xml)"));
  if (filename.isEmpty()) {
    return;
  }
  this->preferences_.set_preference("Main/last_directory", QDir().absoluteFilePath(filename));
  this->open_project(filename);
  this->enablePossibleActions();
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::on_action_save_project_triggered()
{
  if (this->project_->get_filename() == "") {
    return this->on_action_save_project_as_triggered();
  }
  else {
    if (this->project_->save_project(
          this->project_->get_filename().toStdString(), this->data_dir_,
          this->optimize_tool_->getCutPlanesFile())) {
      this->handle_message("Project Saved");
    }
  }
  return true;
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::on_action_save_project_as_triggered()
{
  QString fname("Untitled.xml");
  if (this->project_->get_shapes().size() > 0) {
    fname = this->project_->get_shapes()[0]->get_original_filename();
    std::string tmp = fname.toStdString();
    tmp = tmp.substr(0, tmp.size() - 5);
    fname = QString::fromStdString(tmp);
  }
  QString direct = this->preferences_.get_preference("Main/last_directory", QString());
  auto dir = direct.toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Project As..."),
                                                  QString::fromStdString(dir) + fname,
                                                  tr("XML files (*.xml)"));
  if (filename.isEmpty()) {
    return false;
  }
  this->preferences_.add_recent_file(filename);
  this->update_recent_files();

  this->preferences_.set_preference("Main/last_directory", QDir().absoluteFilePath(filename));

  if (this->project_->save_project(filename.toStdString(), this->data_dir_,
                                   this->optimize_tool_->getCutPlanesFile())) {
    this->handle_message("Project Saved");
    return true;
  }
  return false;
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
                                            preferences_.get_preference("Main/last_directory",
                                                                        QString()),
                                            tr("NRRD files (*.nrrd);;MHA files (*.mha)"));

  if (filenames.size() == 0) {
    return;
  }
  for (size_t i = 0; i < filenames.size(); i++) {
    this->originalFilenames_.push_back(filenames.at(i).toStdString());
  }

  preferences_.set_preference("Main/last_directory", QDir().absoluteFilePath(filenames[0]));
  //need to re-run everything if something new is added.
  this->ui_->view_mode_combobox->setCurrentIndex(0);
  this->ui_->view_mode_combobox->setItemData(1, 0, Qt::UserRole - 1);
  this->ui_->view_mode_combobox->setItemData(2, 0, Qt::UserRole - 1);
  this->preferences_.set_preference("display_state", this->ui_->view_mode_combobox->currentText());
  this->visualizer_->set_display_mode(this->ui_->view_mode_combobox->currentText().toStdString());
  this->import_files(filenames);
  this->visualizer_->update_lut();
  this->enablePossibleActions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::import_files(QStringList file_names)
{
  std::vector<std::string> list;
  for (auto &a : file_names) {
    list.push_back(a.toStdString());
  }
  try {
    this->project_->load_original_files(list);
  } catch (std::runtime_error e) {
    this->handle_error(e.what());
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_thumbnail_size_slider_valueChanged()
{
  if (!this->lightbox_->render_window_ready()) {return;}
  this->preferences_.set_preference("zoom_state", this->ui_->thumbnail_size_slider->value());

  int value = this->ui_->thumbnail_size_slider->maximum() -
              this->ui_->thumbnail_size_slider->value() + 1;

  this->lightbox_->set_tile_layout(value, value);
  this->visualizer_->update_viewer_properties();

  this->update_scrollbar();

  this->ui_->qvtkWidget->GetRenderWindow()->Render();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::disableAllActions()
{
  // export / save / new / open
  this->ui_->action_save_project->setEnabled(false);
  this->ui_->action_save_project_as->setEnabled(false);
  this->ui_->actionExport_PCA_Mesh->setEnabled(false);
  this->ui_->actionExport_Eigenvalues->setEnabled(false);
  this->ui_->actionExport_Eigenvectors->setEnabled(false);
  this->ui_->actionExport_Parameter_XML->setEnabled(false);
  this->ui_->actionExport_PCA_Mode_Points->setEnabled(false);
  this->ui_->action_new_project->setEnabled(false);
  this->ui_->action_open_project->setEnabled(false);
  this->ui_->action_import->setEnabled(false);
  this->ui_->add_button->setEnabled(false);
  this->ui_->delete_button->setEnabled(false);
  this->ui_->actionSet_Data_Directory->setEnabled(false);
  //subtools
  this->groom_tool_->disableActions();
  this->optimize_tool_->disableActions();
  //recent
  QStringList recent_files = preferences_.get_recent_files();
  int num_recent_files = qMin(recent_files.size(), (int)Preferences::MAX_RECENT_FILES);
  for (int i = 0; i < num_recent_files; i++) {
    this->recent_file_actions_[i]->setEnabled(false);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::enablePossibleActions()
{
  // export / save / new / open
  bool reconstructed = this->project_->reconstructed_present();
  bool original = this->project_->original_present();
  this->ui_->action_save_project->setEnabled(original);
  this->ui_->action_save_project_as->setEnabled(original);
  this->ui_->actionExport_PCA_Mesh->setEnabled(reconstructed);
  this->ui_->actionExport_Eigenvalues->setEnabled(reconstructed);
  this->ui_->actionExport_Eigenvectors->setEnabled(reconstructed);
  this->ui_->actionExport_Parameter_XML->setEnabled(reconstructed);
  this->ui_->actionExport_PCA_Mode_Points->setEnabled(reconstructed);
  this->ui_->action_new_project->setEnabled(true);
  this->ui_->action_open_project->setEnabled(true);
  this->ui_->action_import->setEnabled(true);
  this->ui_->add_button->setEnabled(true);
  this->ui_->delete_button->setEnabled(true);
  this->ui_->actionSet_Data_Directory->setEnabled(true);
  //available modes
  this->ui_->action_import_mode->setEnabled(true);
  this->ui_->action_groom_mode->setEnabled(original);
  this->ui_->action_optimize_mode->setEnabled(this->project_->groomed_present());
  this->ui_->action_analysis_mode->setEnabled(reconstructed);
  //subtools
  this->groom_tool_->enableActions();
  this->optimize_tool_->enableActions();
  this->analysis_tool_->enableActions();
  //recent
  QStringList recent_files = preferences_.get_recent_files();
  int num_recent_files = qMin(recent_files.size(), (int)Preferences::MAX_RECENT_FILES);
  for (int i = 0; i < num_recent_files; i++) {
    this->recent_file_actions_[i]->setEnabled(true);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_from_preferences()
{
  this->glyph_quality_slider_->setValue(preferences_.get_preference("glyph_quality", 5.));
  this->glyph_size_slider_->setValue(preferences_.get_preference("glyph_size", 1.) * 10.0);

  this->glyph_quality_label_->setText(QString::number(preferences_.get_preference("glyph_quality",
                                                                                  5.)));
  this->glyph_size_label_->setText(QString::number(preferences_.get_preference("glyph_size", 1.)));

  this->ui_->center_checkbox->setChecked(preferences_.get_preference("center_option", true));
  this->groom_tool_->set_preferences();
  this->optimize_tool_->set_preferences();
  this->analysis_tool_->load_from_preferences();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_scrollbar()
{
  int num_rows = this->lightbox_->get_num_rows();
  int num_visible = this->lightbox_->get_num_rows_visible();
  if (num_visible >= num_rows) {
    this->ui_->vertical_scroll_bar->setMaximum(0);
    this->ui_->vertical_scroll_bar->setEnabled(false);
  }
  else {
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
    this->analysis_tool_->reset_stats();
    this->preferences_.set_preference("tool_state", QString::fromStdString(Project::DATA_C));
    this->lightbox_->clear_renderers();
    this->update_display();
  }
  this->enablePossibleActions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_table()
{

  QVector<QSharedPointer<Shape>> shapes = this->project_->get_shapes();

  this->ui_->table_widget->clear();

  this->ui_->table_widget->setRowCount(shapes.size());
  this->ui_->table_widget->setColumnCount(3);

  QStringList table_header;
  table_header << "#" << "Name" << "Size";
  this->ui_->table_widget->setHorizontalHeaderLabels(table_header);

  this->ui_->table_widget->verticalHeader()->setVisible(false);

  for (int i = 0; i < shapes.size(); i++) {
    QSharedPointer<Mesh> original_mesh = shapes[i]->get_original_mesh();

    QTableWidgetItem* new_item = new QTableWidgetItem(QString::number(i + 1));
    this->ui_->table_widget->setItem(i, 0, new_item);

    new_item = new QTableWidgetItem(shapes[i]->get_original_filename());
    this->ui_->table_widget->setItem(i, 1, new_item);

    if (original_mesh) {
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
void ShapeWorksStudioApp::handle_pca_changed()
{
  if (!this->project_->reconstructed_present()) {return;}
  this->project_->handle_clear_cache();
  this->visualizer_->update_lut();
  this->compute_mode_shape();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_slider_update()
{
  this->analysis_tool_->updateSlider();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_new_mesh()
{
  this->compute_mode_shape();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_message(std::string str)
{
  this->ui_->statusbar->showMessage(QString::fromStdString(str));
  this->currentMessage_ = str;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_error(std::string str)
{
  QMessageBox::critical(this, "Critical Error", str.c_str());
  this->handle_message(str);
  this->handle_progress(100);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_warning(std::string str)
{
  QMessageBox::warning(this, "Warning!", str.c_str());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_progress(size_t value)
{
  if (value < 100) {
    this->progressBar_->setVisible(true);
    this->progressBar_->setValue(static_cast<int>(value));
    this->disableAllActions();
  }
  else {
    this->progressBar_->setValue(100);
    this->progressBar_->setVisible(false);
    this->enablePossibleActions();
  }
  this->handle_message(this->currentMessage_);
  qApp->processEvents();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_groom_mode_triggered()
{
  this->preferences_.set_preference("tool_state", QString::fromStdString(Project::GROOM_C));
  this->ui_->stacked_widget->setCurrentWidget(this->groom_tool_.data());
  this->ui_->controlsDock->setWindowTitle("Groom");
  this->on_actionShow_Tool_Window_triggered();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_import_mode_triggered()
{
  this->preferences_.set_preference("tool_state", QString::fromStdString(Project::DATA_C));
  this->ui_->stacked_widget->setCurrentIndex(0);
  this->ui_->controlsDock->setWindowTitle("Data");
  this->on_actionShow_Tool_Window_triggered();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_optimize_mode_triggered()
{
  this->preferences_.set_preference("tool_state", QString::fromStdString(Project::OPTIMIZE_C));
  this->ui_->stacked_widget->setCurrentWidget(this->optimize_tool_.data());
  this->ui_->controlsDock->setWindowTitle("Optimize");
  this->on_actionShow_Tool_Window_triggered();

  this->visualizer_->set_display_mode(Visualizer::MODE_GROOMED_C.c_str());
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_analysis_mode_triggered()
{
  this->preferences_.set_preference("tool_state", QString::fromStdString(Project::ANALYSIS_C));
  this->ui_->stacked_widget->setCurrentWidget(this->analysis_tool_.data());
  this->ui_->controlsDock->setWindowTitle("Analysis");
  this->on_actionShow_Tool_Window_triggered();
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_project_changed()
{
  if (this->project_->original_present()) {
    this->ui_->view_mode_combobox->setItemData(0, 33, Qt::UserRole - 1);
  }
  else {
    this->ui_->view_mode_combobox->setCurrentIndex(1);
    this->visualizer_->set_display_mode(Visualizer::MODE_GROOMED_C.c_str());
    this->ui_->view_mode_combobox->setItemData(0, 0, Qt::UserRole - 1);
  }

  if (this->project_->groomed_present()) {
    this->ui_->view_mode_combobox->setItemData(1, 33, Qt::UserRole - 1);
  }
  else {
    this->ui_->view_mode_combobox->setItemData(1, 0, Qt::UserRole - 1);
  }

  if (this->project_->reconstructed_present()) {
    this->ui_->view_mode_combobox->setItemData(2, 33, Qt::UserRole - 1);
    this->project_->handle_clear_cache();
  }
  else {
    this->ui_->view_mode_combobox->setItemData(2, 0, Qt::UserRole - 1);
  }
  this->update_table();
  this->update_scrollbar();
  this->update_display();
  this->visualizer_->update_lut();

  this->enablePossibleActions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_points_changed()
{
  this->visualizer_->update_samples();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_optimize_complete()
{
  this->project_->get_mesh_manager()->getSurfaceReconstructor()->resetReconstruct();
  this->analysis_tool_->reset_stats();
  this->project_->handle_clear_cache();
  this->ui_->view_mode_combobox->setItemData(2, 0, Qt::UserRole - 1);
  this->ui_->view_mode_combobox->setCurrentIndex(1);
  this->preferences_.set_preference("display_state",
                                    this->ui_->view_mode_combobox->currentText());
  this->visualizer_->set_display_mode(this->ui_->view_mode_combobox->currentText().toStdString());
  this->visualizer_->setMean(this->analysis_tool_->getMean());
  this->visualizer_->update_lut();
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_reconstruction_complete()
{
  this->project_->handle_clear_cache();
  this->ui_->view_mode_combobox->setItemData(2, 33, Qt::UserRole - 1);
  this->ui_->view_mode_combobox->setCurrentIndex(2);
  this->preferences_.set_preference("display_state",
                                    this->ui_->view_mode_combobox->currentText());
  this->visualizer_->set_display_mode(this->ui_->view_mode_combobox->currentText().toStdString());
  this->visualizer_->setMean(this->analysis_tool_->getMean());
  this->visualizer_->update_lut();
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_groom_complete()
{
  this->ui_->view_mode_combobox->setItemData(1, 33, Qt::UserRole - 1);
  this->ui_->view_mode_combobox->setCurrentIndex(1);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_optimize_start()
{
  this->ui_->action_analysis_mode->setEnabled(false);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_display_setting_changed()
{
  if (this->analysis_tool_->pcaAnimate()) {return;}
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_glyph_changed()
{
  this->visualizer_->set_show_surface(this->ui_->surface_visible_button->isChecked());
  this->visualizer_->set_show_glyphs(this->ui_->glyphs_visible_button->isChecked());
  this->preferences_.set_preference("glyph_size", this->glyph_size_slider_->value() / 10.);
  this->preferences_.set_preference("glyph_quality", this->glyph_quality_slider_->value());
  this->glyph_quality_label_->setText(QString::number(preferences_.get_preference("glyph_quality",
                                                                                  5.)));
  this->glyph_size_label_->setText(QString::number(preferences_.get_preference("glyph_size", 1.)));
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_center_checkbox_stateChanged()
{
  this->preferences_.set_preference("center_option", this->ui_->center_checkbox->isChecked());
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_display()
{
  if (!this->visualizer_ || this->project_->get_num_shapes() <= 0) {
    return;
  }

  this->visualizer_->set_center(this->ui_->center_checkbox->isChecked());
  this->preferences_.set_preference("display_state",
                                    this->ui_->view_mode_combobox->currentText());

  bool reconstruct_ready =
    this->project_->get_mesh_manager()->getSurfaceReconstructor()->hasDenseMean();

  std::string mode = "all samples";

  if (this->ui_->action_analysis_mode->isChecked()) {
    mode = this->analysis_tool_->getAnalysisMode();
  }

  if (mode == "all samples") {

    this->project_->calculate_reconstructed_samples();
    this->visualizer_->display_samples();

    size_t num_samples = this->project_->get_shapes().size();
    if (num_samples == 0) {
      num_samples = 9;
    }

    double root = std::sqrt(static_cast<double>(num_samples));
    if (std::fmod(root, 1.0) > 1e-6) {
      root += 1.;
    }

    size_t value = static_cast<size_t>(root);
    size_t zoom_val = this->ui_->thumbnail_size_slider->maximum() + 1 - value;
    if (zoom_val != this->ui_->thumbnail_size_slider->value()) {
      this->ui_->thumbnail_size_slider->setValue(zoom_val);
    }
    this->ui_->view_mode_combobox->setItemData(0, 33, Qt::UserRole - 1);
    this->ui_->view_mode_combobox->setItemData(1,
                                               this->project_->groomed_present() ? 33 : 0,
                                               Qt::UserRole - 1);
    this->ui_->view_mode_combobox->setItemData(2,
                                               (this->project_->reconstructed_present() &&
                                                reconstruct_ready) ?
                                               33 : 0, Qt::UserRole - 1);
  }
  else {
    if (mode == "mean") {
      this->ui_->view_mode_combobox->setItemData(0, 0, Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setItemData(1, reconstruct_ready ? 0 : 33, Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setItemData(2, reconstruct_ready ? 33 : 0, Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setCurrentIndex(reconstruct_ready ? 2 : 1);
      this->visualizer_->display_shape(this->analysis_tool_->getMean());
      this->visualizer_->reset_camera();
    }
    else if (mode == "pca") {
      this->ui_->view_mode_combobox->setItemData(0, 0, Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setItemData(1, reconstruct_ready ? 0 : 33, Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setItemData(2, reconstruct_ready ? 33 : 0, Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setCurrentIndex(reconstruct_ready ? 2 : 1);
      this->compute_mode_shape();
      this->visualizer_->reset_camera();
    }
    else if (mode == "single sample") {
      this->ui_->view_mode_combobox->setItemData(0, 33, Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setItemData(1, 33, Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setItemData(2, reconstruct_ready ? 33 : 0, Qt::UserRole - 1);
      this->visualizer_->display_sample(this->analysis_tool_->getSampleNumber());
      this->visualizer_->reset_camera();
    }
    else {
      this->ui_->view_mode_combobox->setItemData(0, 33, Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setItemData(1,
                                                 this->project_->groomed_present() ? 33 : 0,
                                                 Qt::UserRole - 1);
      this->ui_->view_mode_combobox->setItemData(2,
                                                 (this->project_->reconstructed_present() &&
                                                  reconstruct_ready) ?
                                                 33 : 0, Qt::UserRole - 1);
    } //TODO regression?

    if (this->ui_->thumbnail_size_slider->maximum() !=
        this->ui_->thumbnail_size_slider->value()) {
      this->ui_->thumbnail_size_slider->setValue(this->ui_->thumbnail_size_slider->maximum());
    }
  }
  this->preferences_.set_preference("zoom_state", this->ui_->thumbnail_size_slider->value());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_view_mode_combobox_currentIndexChanged(QString disp_mode)
{
  if (this->visualizer_) {
    this->preferences_.set_preference("display_state", disp_mode);
    this->visualizer_->set_display_mode(disp_mode.toStdString());
    this->update_display();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::open_project(QString filename)
{
  this->analysis_tool_->reset_stats();

  std::string planesFile;
  try
  {
    if (!this->project_->load_project(filename, planesFile)) {
      return;
    }
  } catch (std::runtime_error e) {
    this->handle_error(e.what());
  }

  auto display_state = this->preferences_.get_preference(
    "display_state", QString::fromStdString(Visualizer::MODE_ORIGINAL_C)).toStdString();
  auto tool_state = this->preferences_.get_preference(
    "tool_state", QString::fromStdString(Project::DATA_C)).toStdString();
  this->groom_tool_->set_preferences();
  this->optimize_tool_->set_preferences();
  this->preferences_window_->set_values_from_preferences();
  this->update_from_preferences();
  //this->project_->calculate_reconstructed_samples();
  this->visualizer_->setMean(this->analysis_tool_->getMean());

  /*
     this->analysis_tool_->reset_stats();

     if (this->project_->is_light_project()) {
     this->analysis_tool_->setAnalysisMode("mean");
     }
     else {
     this->analysis_tool_->setAnalysisMode("all samples");
     }
   */

  preferences_.add_recent_file(filename);
  this->update_recent_files();
  if (!planesFile.empty() && planesFile != "None Selected") {
    this->optimize_tool_->setCutPlanesFile(planesFile);
  }
  // set UI state based on project
  if (tool_state == Project::DATA_C) {
    this->ui_->action_import_mode->trigger();
  }
  else if (tool_state == Project::GROOM_C) {
    this->ui_->action_groom_mode->trigger();
  }
  else if (tool_state == Project::OPTIMIZE_C) {
    this->ui_->action_optimize_mode->trigger();
  }
  else if (tool_state == Project::ANALYSIS_C) {
    this->ui_->action_analysis_mode->trigger();
  }

  // load display mode
  if (display_state == Visualizer::MODE_ORIGINAL_C) {
    this->ui_->view_mode_combobox->setCurrentIndex(0);
  }
  else if (display_state == Visualizer::MODE_GROOMED_C) {
    this->ui_->view_mode_combobox->setCurrentIndex(1);
  }
  else if (display_state == Visualizer::MODE_RECONSTRUCTION_C) {
    this->ui_->view_mode_combobox->setCurrentIndex(2);
  }

  // set the zoom state
  this->ui_->thumbnail_size_slider->setValue(
    this->preferences_.get_preference("zoom_state", 1));
  this->analysis_tool_->reset_stats();

  if (this->project_->is_light_project()) {
    this->analysis_tool_->setAnalysisMode("mean");
  }
  else {
    this->analysis_tool_->setAnalysisMode("all samples");
  }

  this->visualizer_->update_lut();
  this->preferences_.set_saved();
  this->enablePossibleActions();
  this->visualizer_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_preferences_triggered()
{
  this->preferences_window_->show();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::closeEvent(QCloseEvent* event)
{
  // close the preferences window in case it is open
  this->preferences_window_->close();
  if (this->preferences_.not_saved() && this->ui_->action_save_project->isEnabled()) {
    // save the size of the window to preferences
    QMessageBox msgBox;
    msgBox.setText("Do you want to save your changes as a project file?");
    msgBox.setInformativeText("This will reload generated files and changed settings.");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Save) {
      if (!this->on_action_save_project_triggered()) {
        event->ignore();
        return;
      }
    }
    else if (ret == QMessageBox::Cancel) {
      event->ignore();
      return;
    }
  }
  this->analysis_tool_->shutdown();
  this->preferences_.set_preference("StudioWindow/geometry", this->saveGeometry());
  this->preferences_.set_preference("StudioWindow/windowState", this->saveState());

  this->hide();
  this->optimize_tool_->shutdown_threads();
  QCoreApplication::processEvents();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::compute_mode_shape()
{
  int mode = this->analysis_tool_->getPCAMode();
  double value = this->analysis_tool_->get_pca_value();
  this->visualizer_->display_shape(this->analysis_tool_->getShape(mode, value));
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_recent_files()
{
  QStringList recent_files = preferences_.get_recent_files();

  int num_recent_files = qMin(recent_files.size(), (int)Preferences::MAX_RECENT_FILES);

  for (int i = 0; i < num_recent_files; i++) {

    QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(recent_files[i]).fileName());
    this->recent_file_actions_[i]->setText(text);
    this->recent_file_actions_[i]->setData(recent_files[i]);
    this->recent_file_actions_[i]->setVisible(true);
  }

  for (int j = num_recent_files; j < Preferences::MAX_RECENT_FILES; ++j) {
    this->recent_file_actions_[j]->setVisible(false);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_open_recent()
{
  QAction* action = qobject_cast<QAction*>(sender());
  if (action) {
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
void ShapeWorksStudioApp::on_auto_view_button_clicked()
{
  this->visualizer_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionExport_PCA_Mesh_triggered()
{
  QString direct = preferences_.get_preference("Main/last_directory", QString());
  auto dir = direct.toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Project As..."),
                                                  QString::fromStdString(dir) + "newMesh",
                                                  tr("VTK files (*.vtk)"));
  if (filename.isEmpty()) {
    return;
  }
  this->preferences_.set_preference("Main/last_directory", QDir().absoluteFilePath(filename));
  if (this->analysis_tool_->getAnalysisMode() == "all samples") {
    auto shapes = this->project_->get_shapes();
    for (size_t i = 0; i < shapes.size(); i++) {
      auto msh = shapes[i]->get_reconstructed_mesh()->get_poly_data();
      vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
      auto name = filename.toStdString();
      name = name.substr(0, name.find_last_of(".")) + std::to_string(i) + ".vtk";
      writer->SetFileName(name.c_str());
      writer->SetInputData(msh);
      writer->Write();
    }
    this->handle_message("Successfully exported PCA Mesh files: " + filename.toStdString());
    return;
  }
  auto shape = this->visualizer_->getCurrentShape();
  auto msh = this->project_->get_mesh_manager()->getMesh(shape);
  vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
  writer->SetFileName(filename.toStdString().c_str());
  writer->SetInputData(msh);
  writer->Write();
  this->handle_message("Successfully exported PCA Mesh file: " + filename.toStdString());
}

void ShapeWorksStudioApp::on_actionSet_Data_Directory_triggered()
{

  this->data_dir_ = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                      preferences_.get_preference(
                                                        "Main/last_directory", QString()),
                                                      QFileDialog::ShowDirsOnly
                                                      | QFileDialog::DontResolveSymlinks).
                    toStdString();
  this->handle_message("Data directory now set to " + this->data_dir_);
  this->preferences_.set_preference("Main/last_directory",
                                    QDir().absoluteFilePath(QString::fromStdString(this->data_dir_)));
}

void ShapeWorksStudioApp::on_actionExport_Parameter_XML_triggered()
{
  QString fname("Untitled.xml");
  QString direct = this->preferences_.get_preference("Main/last_directory", QString());
  auto dir = direct.toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Save XML Parameter file..."),
                                                  QString::fromStdString(dir) + fname,
                                                  tr("XML files (*.xml)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_preference("Main/last_directory", QDir().absoluteFilePath(filename));
  // open file
  QFile file(filename);

  if (!file.open(QIODevice::WriteOnly)) {
    QMessageBox::warning(0, "Read only", "Failed to write XML file.");
    return;
  }
  // setup XML
  QSharedPointer<QXmlStreamWriter> xml = QSharedPointer<QXmlStreamWriter>(new QXmlStreamWriter());
  xml->setAutoFormatting(true);
  xml->setDevice(&file);
  xml->writeStartElement("shapeworks_parameter_file");
  xml->writeAttribute("version", "1");
  auto prefs = this->preferences_.getAllPreferences();
  for (auto &a : prefs) {
    if (a.first.find("optimize") != std::string::npos ||
        a.first.find("groom") != std::string::npos) {
      xml->writeTextElement(
        QString::fromStdString(a.first), a.second.toString());
    }
  }
  xml->writeEndElement();
  this->handle_message("Successfully exported XML parameter file: " + filename.toStdString());
}

void ShapeWorksStudioApp::on_actionExport_Eigenvalues_triggered()
{
  auto stats = this->analysis_tool_->getStats();
  auto values = stats.Eigenvalues();
  QString fname("Untitled.eval");
  QString direct = this->preferences_.get_preference("Main/last_directory", QString());
  auto dir = direct.toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Eigenvalue EVAL file..."),
                                                  QString::fromStdString(dir) + fname,
                                                  tr("EVAL files (*.eval)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_preference("Main/last_directory", QDir().absoluteFilePath(filename));
  std::ofstream out(filename.toStdString().c_str());
  for (size_t i = values.size() - 1; i > 0; i--) {
    out << values[i] << std::endl;
  }
  out.close();
  this->handle_message("Successfully exported eigenvalue EVAL file: " + filename.toStdString());
}

void ShapeWorksStudioApp::on_actionExport_Eigenvectors_triggered()
{
  auto stats = this->analysis_tool_->getStats();
  auto values = stats.Eigenvectors();
  QString fname("Untitled.eval");
  QString direct = this->preferences_.get_preference("Main/last_directory", QString());
  auto dir = direct.toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Save Eigenvector EVAL files..."),
                                                  QString::fromStdString(dir) + fname,
                                                  tr("EVAL files (*.eval)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_preference("Main/last_directory", QDir().absoluteFilePath(filename));
  auto basename =
    filename.toStdString().substr(0, filename.toStdString().find_last_of(".eval") - 4);
  for (size_t i = values.columns() - 1, ii = 0; i > 0; i--, ii++) {
    auto col = values.get_column(i);
    std::ofstream out(basename + std::to_string(ii) + ".eval");
    size_t newline = 1;
    for (auto &a : col) {
      out << a << (newline % 3 == 0 ? "\n" : "    ");
      newline++;
    }
    out.close();
  }
  this->handle_message("Successfully exported eigenvalue EVAL file: " + filename.toStdString());
}

void ShapeWorksStudioApp::on_actionExport_PCA_Mode_Points_triggered()
{
  QString fname("Untitled.pts");
  QString direct = this->preferences_.get_preference("Main/last_directory", QString());
  auto dir = direct.toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Save PCA Mode PCA files..."),
                                                  QString::fromStdString(dir) + fname,
                                                  tr("PTS files (*.pts)"));
  auto basename = filename.toStdString().substr(0, filename.toStdString().find_last_of(".pts") - 3);
  if (filename.isEmpty()) {
    return;
  }
  float range = preferences_.get_preference("pca_range", 2.f);
  float steps = static_cast<float>(preferences_.get_preference("pca_steps", 20));
  int mode = this->analysis_tool_->getPCAMode();
  auto increment = range * 2.f / steps;
  size_t i = 0;
  for (float pca = -range; pca <= range; pca += increment, i++) {
    auto pts = this->analysis_tool_->getShape(mode, pca);
    std::ofstream out(basename + std::to_string(mode) + "-" + std::to_string(i) + ".pts");
    size_t newline = 1;
    for (auto &a : pts) {
      out << a << (newline % 3 == 0 ? "\n" : "    ");
      newline++;
    }
    out.close();
  }
  this->handle_message("Successfully exported PCA Mode PTS files: " + filename.toStdString());
}
