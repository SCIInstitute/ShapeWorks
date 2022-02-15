// std
#include <iostream>

// qt
#include <QCloseEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <QMimeData>
#include <QProcess>
#include <QPushButton>
#include <QTextStream>
#include <QWidgetAction>

// vtk
#include <vtkPolyDataWriter.h>
#include <vtkRenderWindow.h>

// shapeworks
#include <Applications/Configuration.h>
#include <Libs/Mesh/Mesh.h>
#include <Libs/Utils/StringUtils.h>

// studio
#include <Analysis/AnalysisTool.h>
#include <Data/DataTool.h>
#include <Data/Preferences.h>
#include <Data/Session.h>
#include <Data/Shape.h>
#include <Data/StudioLog.h>
#include <DeepSSM/DeepSSMTool.h>
#include <Groom/GroomTool.h>
#include <Interface/KeyboardShortcuts.h>
#include <Interface/ShapeWorksStudioApp.h>
#include <Interface/SplashScreen.h>
#include <Interface/StatusBarWidget.h>
#include <Interface/WheelEventForwarder.h>
#include <Optimize/OptimizeTool.h>
#include <Utils/StudioUtils.h>
#include <Visualization/Lightbox.h>
#include <Visualization/Visualizer.h>

// ui
#include <ui_ShapeWorksStudioApp.h>

namespace shapeworks {

static QVariant ITEM_DISABLE(0);
static QVariant ITEM_ENABLE(1 | 32);
static int ITEM_ROLE = Qt::UserRole - 1;

const std::string ShapeWorksStudioApp::SETTING_ZOOM_C("zoom_state");

//---------------------------------------------------------------------------
ShapeWorksStudioApp::ShapeWorksStudioApp() {
  ui_ = new Ui_ShapeWorksStudioApp;
  ui_->setupUi(this);
  setAcceptDrops(true);

  status_bar_ = new StatusBarWidget(this);
  connect(status_bar_, &StatusBarWidget::toggle_log_window, this, &ShapeWorksStudioApp::toggle_log_window);

  studio_vtk_output_window_ = vtkSmartPointer<StudioVtkOutputWindow>::New();
  vtkOutputWindow::SetInstance(studio_vtk_output_window_);

  connect(studio_vtk_output_window_.Get(), &StudioVtkOutputWindow::warning, this,
          &ShapeWorksStudioApp::handle_message);
  connect(studio_vtk_output_window_.Get(), &StudioVtkOutputWindow::error, this,
          &ShapeWorksStudioApp::handle_error);
  connect(&(StudioLog::Instance()), &StudioLog::error, this, &ShapeWorksStudioApp::handle_error);

  // default hide
  ui_->feature_widget->hide();
  recent_file_actions_.append(ui_->action_recent1);
  recent_file_actions_.append(ui_->action_recent2);
  recent_file_actions_.append(ui_->action_recent3);
  recent_file_actions_.append(ui_->action_recent4);
  recent_file_actions_.append(ui_->action_recent5);
  recent_file_actions_.append(ui_->action_recent6);
  recent_file_actions_.append(ui_->action_recent7);
  recent_file_actions_.append(ui_->action_recent8);

  for (int i = 0; i < 8; i++) {
    connect(recent_file_actions_[i], SIGNAL(triggered()), this, SLOT(handle_open_recent()));
  }
  update_recent_files();

  py_worker_ = QSharedPointer<PythonWorker>::create();
  py_worker_->set_vtk_output_window(studio_vtk_output_window_);
  connect(py_worker_.data(), &PythonWorker::error_message, this, &ShapeWorksStudioApp::handle_error);

#if defined(Q_OS_LINUX)
  ui_->action_show_project_folder->setVisible(false);
#endif

  splash_screen_ = QSharedPointer<SplashScreen>(new SplashScreen(this, preferences_));
  connect(splash_screen_.data(), &SplashScreen::open_project, this, &ShapeWorksStudioApp::open_project);

  wheel_event_forwarder_ =
      QSharedPointer<WheelEventForwarder>(new WheelEventForwarder(ui_->vertical_scroll_bar));
  ui_->qvtkWidget->installEventFilter(wheel_event_forwarder_.data());

  create_glyph_submenu();
  // analysis tool initializations
  analysis_tool_ = QSharedPointer<AnalysisTool>::create(preferences_);
  analysis_tool_->set_app(this);
  ui_->stacked_widget->addWidget(analysis_tool_.data());
  connect(analysis_tool_.data(), SIGNAL(update_view()), this, SLOT(handle_display_setting_changed()));
  connect(analysis_tool_.data(), SIGNAL(pca_update()), this, SLOT(handle_pca_update()));
  connect(analysis_tool_.data(), &AnalysisTool::progress, this, &ShapeWorksStudioApp::handle_progress);
  connect(analysis_tool_.data(), SIGNAL(reconstruction_complete()), this, SLOT(handle_reconstruction_complete()));

  connect(analysis_tool_.data(), &AnalysisTool::message, this, &ShapeWorksStudioApp::handle_message);
  connect(analysis_tool_.data(), &AnalysisTool::warning, this, &ShapeWorksStudioApp::handle_warning);
  connect(analysis_tool_.data(), &AnalysisTool::error, this, &ShapeWorksStudioApp::handle_error);

  // DeepSSM tool init
  deepssm_tool_ = QSharedPointer<DeepSSMTool>::create(preferences_);
  deepssm_tool_->set_app(this);
  ui_->stacked_widget->addWidget(deepssm_tool_.data());
  connect(deepssm_tool_.data(), &DeepSSMTool::message, this, &ShapeWorksStudioApp::handle_message);
  connect(deepssm_tool_.data(), &DeepSSMTool::warning, this, &ShapeWorksStudioApp::handle_warning);
  connect(deepssm_tool_.data(), &DeepSSMTool::error, this, &ShapeWorksStudioApp::handle_error);
  connect(deepssm_tool_.data(), &DeepSSMTool::progress, this, &ShapeWorksStudioApp::handle_progress);
  connect(deepssm_tool_.data(), &DeepSSMTool::update_view, this,
          &ShapeWorksStudioApp::handle_display_setting_changed);

  // resize from preferences
  if (!preferences_.get_window_geometry().isEmpty()) {
    restoreGeometry(preferences_.get_window_geometry());
  }
  if (!preferences_.get_window_state().isEmpty()) {
    restoreState(preferences_.get_window_state());
  }

  // set to import
  ui_->action_import_mode->setChecked(true);
  ui_->stacked_widget->setCurrentIndex(VIEW_MODE::ORIGINAL);

  action_group_ = new QActionGroup(this);
  action_group_->addAction(ui_->action_import_mode);
  action_group_->addAction(ui_->action_groom_mode);
  action_group_->addAction(ui_->action_optimize_mode);
  action_group_->addAction(ui_->action_analysis_mode);
  action_group_->addAction(ui_->action_deepssm_mode);

  lightbox_ = LightboxHandle(new Lightbox());

  // visualizer initializations
  visualizer_ = QSharedPointer<Visualizer>::create(preferences_);
  visualizer_->set_lightbox(lightbox_);

  // data tool initialization
  data_tool_ = QSharedPointer<DataTool>::create(preferences_);
  connect(data_tool_.data(), &DataTool::import_button_clicked, this, &ShapeWorksStudioApp::on_action_import_triggered);
  ui_->stacked_widget->addWidget(data_tool_.data());

  // groom tool initialization
  groom_tool_ = QSharedPointer<GroomTool>::create(preferences_);
  ui_->stacked_widget->addWidget(groom_tool_.data());
  connect(groom_tool_.data(), &GroomTool::groom_start, this, &ShapeWorksStudioApp::handle_groom_start);
  connect(groom_tool_.data(), &GroomTool::groom_complete, this, &ShapeWorksStudioApp::handle_groom_complete);
  connect(groom_tool_.data(), &GroomTool::error_message, this, &ShapeWorksStudioApp::handle_error);
  connect(groom_tool_.data(), &GroomTool::message, this, &ShapeWorksStudioApp::handle_message);
  connect(groom_tool_.data(), &GroomTool::progress, this, &ShapeWorksStudioApp::handle_progress);

  // optimize tool initialization
  optimize_tool_ = QSharedPointer<OptimizeTool>::create(preferences_);

  ui_->stacked_widget->addWidget(optimize_tool_.data());
  connect(optimize_tool_.data(), SIGNAL(optimize_complete()), this, SLOT(handle_optimize_complete()));

  connect(optimize_tool_.data(), &OptimizeTool::optimize_start, this,
          &ShapeWorksStudioApp::handle_optimize_start);

  connect(optimize_tool_.data(), &OptimizeTool::error_message, this, &ShapeWorksStudioApp::handle_error);
  connect(optimize_tool_.data(), &OptimizeTool::warning_message, this, &ShapeWorksStudioApp::handle_warning);
  connect(optimize_tool_.data(), &OptimizeTool::message, this, &ShapeWorksStudioApp::handle_message);
  connect(optimize_tool_.data(), &OptimizeTool::status, this, &ShapeWorksStudioApp::handle_status);
  connect(optimize_tool_.data(), &OptimizeTool::progress, this, &ShapeWorksStudioApp::handle_progress);

  // set up preferences window
  preferences_window_ = QSharedPointer<PreferencesWindow>(new PreferencesWindow(this, preferences_));
  preferences_window_->set_values_from_preferences();

  connect(preferences_window_.data(), SIGNAL(clear_cache()), this, SLOT(handle_clear_cache()));
  connect(preferences_window_.data(), SIGNAL(update_view()), this, SLOT(handle_color_scheme()));
  connect(preferences_window_.data(), SIGNAL(slider_update()), this, SLOT(handle_slider_update()));

  connect(ui_->alignment_combo, qOverload<int>(&QComboBox::currentIndexChanged), this,
          &ShapeWorksStudioApp::handle_alignment_changed);
  // regression tool TODO
  /*analysis_tool_ = QSharedPointer<AnalysisTool> (new AnalysisTool());
     analysis_tool_->set_project( session_ );
     analysis_tool_->set_app( this );
     ui_->lower_stacked_widget->addWidget( analysis_tool_.data() );*/

  new_session();

  update_from_preferences();
  update_display();

  // setup modes
  ui_->view_mode_combobox->addItem(Visualizer::MODE_ORIGINAL_C.c_str());
  ui_->view_mode_combobox->addItem(Visualizer::MODE_GROOMED_C.c_str());
  ui_->view_mode_combobox->addItem(Visualizer::MODE_RECONSTRUCTION_C.c_str());
  ui_->view_mode_combobox->setCurrentIndex(VIEW_MODE::ORIGINAL);
  set_view_combo_item_enabled(VIEW_MODE::ORIGINAL, true);
  set_view_combo_item_enabled(VIEW_MODE::GROOMED, false);
  set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED, false);

  connect(ui_->features, qOverload<const QString&>(&QComboBox::currentIndexChanged), this,
          &ShapeWorksStudioApp::update_feature_map_selection);

  connect(ui_->image_combo_, qOverload<const QString&>(&QComboBox::currentIndexChanged), this,
          &ShapeWorksStudioApp::image_combo_changed);

  connect(ui_->feature_uniform_scale, &QCheckBox::toggled, this, &ShapeWorksStudioApp::set_feature_uniform_scale);

  // glyph options signals/slots
  connect(ui_->glyphs_visible_button, SIGNAL(clicked()), this, SLOT(handle_glyph_changed()));
  connect(ui_->landmarks_visible_button, &QToolButton::clicked, this, &ShapeWorksStudioApp::handle_glyph_changed);
  connect(ui_->surface_visible_button, SIGNAL(clicked()), this, SLOT(handle_glyph_changed()));
  connect(glyph_size_slider_, SIGNAL(valueChanged(int)), this, SLOT(handle_glyph_changed()));
  connect(glyph_quality_slider_, SIGNAL(valueChanged(int)), this, SLOT(handle_glyph_changed()));
  connect(glyph_auto_size_, &QCheckBox::clicked, this, &ShapeWorksStudioApp::handle_glyph_changed);
  preferences_.set_saved();
  enable_possible_actions();

  connect(ui_->actionAbout, &QAction::triggered, this, &ShapeWorksStudioApp::about);
  connect(ui_->actionKeyboard_Shortcuts, &QAction::triggered, this, &ShapeWorksStudioApp::keyboard_shortcuts);

  update_feature_map_scale();
  handle_message("ShapeWorks Studio Initialized");
}

//---------------------------------------------------------------------------
ShapeWorksStudioApp::~ShapeWorksStudioApp() {}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::initialize_vtk() {
  lightbox_->set_render_window(ui_->qvtkWidget->GetRenderWindow());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_new_project_triggered() {
  if (preferences_.not_saved() && ui_->action_save_project->isEnabled()) {
    // save the size of the window to preferences
    QMessageBox msgBox;
    msgBox.setText("Do you want to save your changes as a project file?");
    msgBox.setInformativeText("This will reload generated files and changed settings.");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Save) {
      if (!on_action_save_project_triggered()) {
        return;
      }
    } else if (ret == QMessageBox::Cancel) {
      return;
    }
  }

  new_session();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionShow_Tool_Window_triggered() {
  ui_->controlsDock->setVisible(true);
  ui_->controlsDock->show();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_open_project_triggered() {
  QString filename = QFileDialog::getOpenFileName(this, tr("Open Project..."), preferences_.get_last_directory(),
                                                  tr("XLSX files (*.xlsx)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());
  open_project(filename);
  enable_possible_actions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_show_project_folder_triggered() {
  auto filename = session_->get_filename();
  if (filename == "") {
    handle_message("No project");
  }

  auto qstring_path = QFileInfo(filename).absoluteDir().absolutePath();

  QProcess process;
  process.setReadChannelMode(QProcess::MergedChannels);

#ifdef _WIN32
  qstring_path = qstring_path.replace(QString("/"), QString("\\"));
  process.start("explorer.exe", QStringList() << qstring_path);
#else
  process.start("open", QStringList() << "-R" << filename);
#endif

  if (!process.waitForFinished()) {
    QString error = QString("Could not open project: ") + process.errorString() + ".";
    STUDIO_LOG_ERROR(error);
  }
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::on_action_save_project_triggered() {
  if (session_->get_filename() == "") {
    return on_action_save_project_as_triggered();
  } else {
    save_project(session_->get_filename().toStdString());
  }
  return true;
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::on_action_save_project_as_triggered() {
  QString last_directory = preferences_.get_last_directory();
  QString filename =
      QFileDialog::getSaveFileName(this, tr("Save Project As..."), last_directory, tr("XLSX files (*.xlsx)"));
  if (filename.isEmpty()) {
    return false;
  }

  if (!StringUtils::hasSuffix(filename.toStdString(), ".xlsx")) {
    filename = filename + ".xlsx";
  }

  preferences_.set_last_directory(QFileInfo(filename).absolutePath());

  save_project(filename.toStdString());

  preferences_.add_recent_file(filename, QDir::currentPath());
  update_recent_files();

  enable_possible_actions();
  return false;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_quit_triggered() { qApp->closeAllWindows(); }

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_import_triggered() {
  auto filenames =
      QFileDialog::getOpenFileNames(this, tr("Import Files..."), preferences_.get_last_directory(),
                                    tr("Supported types (*.nrrd *.nii *.nii.gz *.mha *.vtk *.ply *.vtp *.obj *.stl)"));

  if (filenames.size() == 0) {
    // was cancelled
    return;
  }

  preferences_.set_last_directory(QFileInfo(filenames[0]).absolutePath());

  // need to re-run everything if something new is added.
  ui_->view_mode_combobox->setCurrentIndex(VIEW_MODE::ORIGINAL);
  set_view_combo_item_enabled(VIEW_MODE::ORIGINAL, true);
  set_view_combo_item_enabled(VIEW_MODE::GROOMED, false);
  set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED, false);

  visualizer_->set_display_mode(ui_->view_mode_combobox->currentText().toStdString());
  import_files(filenames);

  visualizer_->update_lut();
  enable_possible_actions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::import_files(QStringList file_names) {
  handle_message("Loading Files...");
  handle_progress(-1);
  QCoreApplication::processEvents();

  std::vector<std::string> list;
  for (auto& a : file_names) {
    list.push_back(a.toStdString());
  }
  try {
    bool first_load = false;

    if (session_->get_num_shapes() == 0 && file_names.size() > 0) {
      // if nothing is loaded, take the path of the first one as the relative path
      first_load = true;
      session_->set_project_path(QFileInfo(file_names[0]).absolutePath());
    }

    session_->load_original_files(list);

    session_->get_project()->store_subjects();

    update_table();
    enable_possible_actions();
    update_display(true);

    reset_num_viewers();

    if (first_load) {
      // On first load, we can check if there was an active scalar on loaded meshes
      set_feature_map(session_->get_default_feature_map());
    }
  } catch (std::runtime_error& e) {
    handle_error(e.what());
  }
  handle_message("Files loaded");
  handle_progress(100);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_zoom_slider_valueChanged() {
  if (!lightbox_->render_window_ready()) {
    return;
  }

  int value = ui_->zoom_slider->value();
  if (value == 0) {
    lightbox_->set_tile_layout(1, 1);
  } else if (value == 1) {
    lightbox_->set_tile_layout(2, 1);
  } else {
    lightbox_->set_tile_layout(value, value);
  }

  visualizer_->update_viewer_properties();

  update_scrollbar();

  ui_->qvtkWidget->GetRenderWindow()->Render();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::disable_all_actions() {
  // export / save / new / open
  ui_->action_save_project->setEnabled(false);
  ui_->action_save_project_as->setEnabled(false);
  ui_->actionExport_PCA_Mesh->setEnabled(false);
  ui_->actionExport_Eigenvalues->setEnabled(false);
  ui_->actionExport_Eigenvectors->setEnabled(false);
  ui_->actionExport_PCA_Mode_Points->setEnabled(false);
  ui_->action_new_project->setEnabled(false);
  ui_->action_open_project->setEnabled(false);
  ui_->action_import->setEnabled(false);
  ui_->menuExport->setEnabled(false);

  // subtools
  data_tool_->disable_actions();
  groom_tool_->disable_actions();
  optimize_tool_->disable_actions();
  // recent
  QStringList recent_files = preferences_.get_recent_files();
  int num_recent_files = qMin(recent_files.size(), 4);
  for (int i = 0; i < num_recent_files; i++) {
    recent_file_actions_[i]->setEnabled(false);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::enable_possible_actions() {
  // export / save / new / open
  bool reconstructed = session_->particles_present();

  bool original_present = session_->get_project()->get_segmentations_present();

  auto filename = session_->get_filename();
  bool save_enabled = filename == "" || filename.endsWith(".xlsx");
  ui_->action_save_project->setEnabled(save_enabled);
  // ui_->action_save_project_as->setEnabled(original_present);
  ui_->action_save_project_as->setEnabled(true);
  ui_->actionExport_PCA_Mesh->setEnabled(reconstructed);
  ui_->actionExport_Eigenvalues->setEnabled(reconstructed);
  ui_->actionExport_Eigenvectors->setEnabled(reconstructed);
  ui_->actionExport_PCA_Mode_Points->setEnabled(reconstructed);
  ui_->action_new_project->setEnabled(true);
  ui_->action_open_project->setEnabled(true);
  ui_->action_import->setEnabled(true);
  ui_->menuExport->setEnabled(true);

  // available modes
  ui_->action_import_mode->setEnabled(true);
  ui_->action_groom_mode->setEnabled(original_present);
  ui_->action_optimize_mode->setEnabled(original_present);
  bool new_analysis = false;
  if (!ui_->action_analysis_mode->isEnabled() && reconstructed) {
    new_analysis = true;
  }
  ui_->action_analysis_mode->setEnabled(reconstructed);
  ui_->action_deepssm_mode->setEnabled(reconstructed && session_->get_project()->get_images_present());
  // subtools
  data_tool_->enable_actions();
  groom_tool_->enable_actions();
  optimize_tool_->enable_actions();
  analysis_tool_->enable_actions(new_analysis);
  // recent
  QStringList recent_files = preferences_.get_recent_files();
  int num_recent_files = qMin(recent_files.size(), 8);
  for (int i = 0; i < num_recent_files; i++) {
    recent_file_actions_[i]->setEnabled(true);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_from_preferences() {
  glyph_quality_slider_->setValue(preferences_.get_glyph_quality());
  glyph_size_slider_->setValue(preferences_.get_glyph_size() * 10.0);
  glyph_auto_size_->setChecked(preferences_.get_glyph_auto_size());
  glyph_size_slider_->setEnabled(!glyph_auto_size_->isChecked());

  glyph_quality_label_->setText(QString::number(preferences_.get_glyph_quality()));
  glyph_size_label_->setText(QString::number(preferences_.get_glyph_size()));

  ui_->center_checkbox->setChecked(preferences_.get_center_checked());

  if (session_) {
    ui_->feature_uniform_scale->setChecked(get_feature_uniform_scale());
    ui_->feature_auto_scale->setChecked(session_->get_feature_auto_scale());
    ui_->feature_min->setValue(session_->get_feature_range_min());
    ui_->feature_max->setValue(session_->get_feature_range_max());
  }
  groom_tool_->load_params();
  optimize_tool_->load_params();
  analysis_tool_->load_settings();
  deepssm_tool_->load_params();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_scrollbar() {
  int num_rows = lightbox_->get_num_rows();
  int num_visible = lightbox_->get_num_rows_visible();
  if (num_visible >= num_rows) {
    ui_->vertical_scroll_bar->setMaximum(0);
    ui_->vertical_scroll_bar->setEnabled(false);
  } else {
    ui_->vertical_scroll_bar->setEnabled(true);
    ui_->vertical_scroll_bar->setMaximum(num_rows - num_visible);
    ui_->vertical_scroll_bar->setPageStep(num_visible);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_vertical_scroll_bar_valueChanged() {
  int value = ui_->vertical_scroll_bar->value();
  lightbox_->set_start_row(value);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_table() {
  auto project = session_->get_project();

  data_tool_->update_table();

  /// todo: check if the list has changed before changing
  auto current_feature = ui_->features->currentText();
  ui_->features->clear();
  ui_->features->addItem("-none-");
  auto feature_maps = project->get_feature_names();
  for (const std::string& feature : feature_maps) {
    QString item = QString::fromStdString(feature);
    item = item.replace("feature_", "");
    ui_->features->addItem(item);
  }
  ui_->features->setCurrentText(current_feature);
  ui_->feature_uniform_scale->setChecked(get_feature_uniform_scale());

  /// fill in image combo
  auto current_image = ui_->image_combo_->currentText();
  ui_->image_combo_->clear();
  ui_->image_combo_->addItem("-none-");
  auto image_names = project->get_image_names();
  for (const std::string& name : image_names) {
    QString item = QString::fromStdString(name);
    item = item.replace("feature_", "");
    ui_->image_combo_->addItem(item);
  }
  ui_->image_combo_->setCurrentText(current_image);
  ui_->image_combo_->setCurrentText(QString::fromStdString(session_->get_image_name()));

  ui_->image_axis_->setCurrentText(QString::fromStdString(axisToString(session_->get_image_axis())));
  ui_->image_3d_mode_->setChecked(session_->get_image_3d_mode());
  ui_->image_share_window_and_level_->setChecked(session_->get_image_share_window_and_level());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_pca_changed() {
  if (!session_->particles_present()) {
    return;
  }
  session_->handle_clear_cache();
  visualizer_->update_lut();
  compute_mode_shape();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_slider_update() { analysis_tool_->updateSlider(); }

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_pca_update() {
  if (analysis_tool_->get_active() && analysis_tool_->get_analysis_mode() == AnalysisTool::MODE_PCA_C) {
    compute_mode_shape();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_message(QString str) {
  if (str != current_message_) {
    STUDIO_LOG_MESSAGE(str);
    set_message(MessageType::normal, str);
  } else {
    status_bar_->set_message(MessageType::normal, str);
  }
  current_message_ = str;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_status(QString str) {
  status_bar_->set_message(MessageType::normal, str);
  current_message_ = str;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_error(QString str) {
  STUDIO_LOG_ERROR(str);
  set_message(MessageType::error, str);
  current_message_ = str;
  error_message_dialog_.showMessage("Error:\n" + str, "error");
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_warning(QString str) {
  STUDIO_LOG_MESSAGE(str);
  set_message(MessageType::warning, str);
  current_message_ = str;
  error_message_dialog_.showMessage("Warning:\n" + str, "warning");
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_progress(int value) {
  status_bar_->set_progress(value);
  handle_message(current_message_);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::set_message(MessageType message_type, QString message) {
  status_bar_->set_message(message_type, message);
  log_window_.add_message(message_type, message);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::create_glyph_submenu() {
  // Glyph options in the render window.
  QMenu* menu = new QMenu();
  QWidget* widget = new QWidget();
  QGridLayout* layout = new QGridLayout(widget);

  QLabel* size_label = new QLabel("Glyph Size: ");
  layout->addWidget(size_label, 0, 0, 1, 1);
  size_label = new QLabel("Glyph Detail: ");
  layout->addWidget(size_label, 1, 0, 1, 1);

  glyph_quality_label_ = new QLabel("....");
  glyph_quality_label_->setMinimumWidth(50);
  glyph_size_label_ = new QLabel("....");
  glyph_size_label_->setMinimumWidth(50);
  layout->addWidget(glyph_size_label_, 0, 1, 1, 1);
  layout->addWidget(glyph_quality_label_, 1, 1, 1, 1);

  glyph_size_slider_ = new QSlider(widget);
  glyph_size_slider_->setOrientation(Qt::Horizontal);
  glyph_size_slider_->setMinimum(1);
  glyph_size_slider_->setMaximum(100);
  glyph_size_slider_->setPageStep(10);
  glyph_size_slider_->setTickPosition(QSlider::TicksBelow);
  glyph_size_slider_->setTickInterval(10);
  glyph_size_slider_->setMinimumWidth(200);

  glyph_auto_size_ = new QCheckBox("Auto");

  glyph_quality_slider_ = new QSlider(widget);
  glyph_quality_slider_->setMinimum(1);
  glyph_quality_slider_->setMaximum(20);
  glyph_quality_slider_->setPageStep(3);
  glyph_quality_slider_->setOrientation(Qt::Horizontal);
  glyph_quality_slider_->setTickPosition(QSlider::TicksBelow);
  glyph_quality_slider_->setTickInterval(1);
  glyph_quality_slider_->setMinimumWidth(200);

  layout->addWidget(glyph_size_slider_, 0, 2, 1, 1);
  layout->addWidget(glyph_auto_size_, 0, 3, 1, 1);
  layout->addWidget(glyph_quality_slider_, 1, 2, 1, 1);
  widget->setLayout(layout);

  QWidgetAction* widget_action = new QWidgetAction(widget);
  widget_action->setDefaultWidget(widget);
  menu->addAction(widget_action);
  ui_->glyphs_visible_button->setMenu(menu);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::create_iso_submenu() {
  // Glyph options in the render window.
  QMenu* menu = new QMenu();
  QWidget* widget = new QWidget();
  QGridLayout* layout = new QGridLayout(widget);

  if (!session_) {
    return;
  }
  auto project = session_->get_project();

  auto names = project->get_domain_names();
  iso_opacity_sliders_.clear();
  for (size_t row = 0; row < names.size(); row++) {
    auto name = names[row];
    QString text = "Opacity";
    if (names.size() > 1) {
      text = QString::fromStdString(name) + " opacity: ";
    }
    QLabel* size_label = new QLabel(text);
    layout->addWidget(size_label, row, 0, 1, 1);

    QSlider* slider = new QSlider(widget);
    slider->setOrientation(Qt::Horizontal);
    slider->setMinimum(1);
    slider->setMaximum(100);
    slider->setPageStep(10);
    slider->setTickPosition(QSlider::TicksBelow);
    slider->setTickInterval(10);
    slider->setValue(100);
    slider->setMinimumWidth(200);
    connect(slider, &QSlider::valueChanged, this, &ShapeWorksStudioApp::handle_opacity_changed);

    layout->addWidget(slider, row, 1, 1, 1);
    widget->setLayout(layout);

    iso_opacity_sliders_.push_back(slider);
  }

  QWidgetAction* widget_action = new QWidgetAction(widget);
  widget_action->setDefaultWidget(widget);
  menu->addAction(widget_action);
  ui_->surface_visible_button->setMenu(menu);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_new_mesh() {
  visualizer_->handle_new_mesh();

  std::string mode = AnalysisTool::MODE_ALL_SAMPLES_C;
  if (ui_->action_analysis_mode->isChecked()) {
    mode = analysis_tool_->get_analysis_mode();
  }

  if (visualizer_->get_feature_map() != "" && mode == AnalysisTool::MODE_MEAN_C) {
    visualizer_->display_shape(analysis_tool_->get_mean_shape());
  }

  deepssm_tool_->handle_new_mesh();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_clear_cache() {
  handle_pca_changed();
  if (session_) {
    session_->handle_clear_cache();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::new_session() {
  // project initializations
  session_ = QSharedPointer<Session>::create(this, preferences_);
  session_->set_parent(this);
  setWindowTitle(session_->get_display_name());

  connect(session_->get_mesh_manager().data(), &MeshManager::error_encountered, this,
          &ShapeWorksStudioApp::handle_error);
  connect(session_->get_mesh_manager().data(), &MeshManager::progress, this, &ShapeWorksStudioApp::handle_progress);
  connect(session_->get_mesh_manager().data(), &MeshManager::status, this, &ShapeWorksStudioApp::handle_status);

  connect(session_.data(), &Session::data_changed, this, &ShapeWorksStudioApp::handle_project_changed);
  connect(session_.data(), &Session::points_changed, this, &ShapeWorksStudioApp::handle_points_changed);
  connect(session_.data(), &Session::update_display, this, &ShapeWorksStudioApp::handle_display_setting_changed);
  connect(session_.data(), &Session::update_view_mode, this, &ShapeWorksStudioApp::update_view_mode);
  connect(session_.data(), &Session::message, this, &ShapeWorksStudioApp::handle_message);
  connect(session_.data(), &Session::new_mesh, this, &ShapeWorksStudioApp::handle_new_mesh);
  connect(session_.data(), &Session::error, this, &ShapeWorksStudioApp::handle_error);

  connect(ui_->feature_auto_scale, &QCheckBox::toggled, this, &ShapeWorksStudioApp::update_feature_map_scale);
  connect(ui_->feature_auto_scale, &QCheckBox::toggled, session_.data(), &Session::set_feature_auto_scale);
  connect(ui_->feature_min, qOverload<double>(&QDoubleSpinBox::valueChanged), session_.data(),
          &Session::set_feature_range_min);
  connect(ui_->feature_max, qOverload<double>(&QDoubleSpinBox::valueChanged), session_.data(),
          &Session::set_feature_range_max);

  connect(ui_->image_axis_, qOverload<const QString&>(&QComboBox::currentIndexChanged), session_.data(),
          &Session::set_image_axis);
  connect(ui_->image_3d_mode_, &QCheckBox::clicked, session_.data(), &Session::set_image_3d_mode);
  connect(ui_->image_share_window_and_level_, &QCheckBox::clicked, session_.data(),
          &Session::set_image_share_window_and_level);

  data_tool_->update_notes();

  visualizer_->clear_viewers();

  data_tool_->set_session(session_);
  analysis_tool_->set_session(session_);
  visualizer_->set_session(session_);
  groom_tool_->set_session(session_);
  optimize_tool_->set_session(session_);
  deepssm_tool_->set_session(session_);
  create_iso_submenu();

  update_table();
  update_alignment_options();
  update_from_preferences();

  lightbox_->clear_renderers();
  analysis_tool_->reset_stats();
  ui_->action_import_mode->setChecked(true);
  ui_->action_groom_mode->setChecked(false);
  ui_->action_optimize_mode->setChecked(false);
  ui_->action_analysis_mode->setChecked(false);
  ui_->stacked_widget->setCurrentWidget(data_tool_.data());
  ui_->controlsDock->setWindowTitle("Data");
  preferences_.set_saved();
  enable_possible_actions();
  update_display(true);
  visualizer_->update_viewer_properties();

  ui_->view_mode_combobox->setCurrentIndex(VIEW_MODE::ORIGINAL);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_tool_mode() {
  std::string tool_state = session_->parameters().get("tool_state", Session::DATA_C);

  analysis_tool_->set_active(tool_state == Session::ANALYSIS_C);

  for (int i = 0; i < ui_->stacked_widget->count(); i++) {
    ui_->stacked_widget->widget(i)->setSizePolicy(QSizePolicy::Ignored, QSizePolicy::Ignored);
  }

  if (tool_state == Session::ANALYSIS_C) {
    ui_->stacked_widget->setCurrentWidget(analysis_tool_.data());
    ui_->controlsDock->setWindowTitle("Analysis");
    set_view_mode(Visualizer::MODE_RECONSTRUCTION_C);
    on_actionShow_Tool_Window_triggered();
    update_display(true);
    ui_->action_analysis_mode->setChecked(true);
  } else if (tool_state == Session::GROOM_C) {
    ui_->stacked_widget->setCurrentWidget(groom_tool_.data());
    groom_tool_->activate();
    ui_->controlsDock->setWindowTitle("Groom");
    set_view_mode(Visualizer::MODE_ORIGINAL_C);
    ui_->action_groom_mode->setChecked(true);
  } else if (tool_state == Session::OPTIMIZE_C) {
    ui_->stacked_widget->setCurrentWidget(optimize_tool_.data());
    optimize_tool_->activate();
    ui_->controlsDock->setWindowTitle("Optimize");
    if (session_->groomed_present()) {
      set_view_mode(Visualizer::MODE_GROOMED_C);
    }
    update_display();
    ui_->action_optimize_mode->setChecked(true);
  } else if (tool_state == Session::DEEPSSM_C) {
    ui_->stacked_widget->setCurrentWidget(deepssm_tool_.data());
    ui_->controlsDock->setWindowTitle("DeepSSM");
    update_display();
    ui_->action_deepssm_mode->setChecked(true);
    set_view_mode(Visualizer::MODE_RECONSTRUCTION_C);
  } else {  // DATA
    ui_->stacked_widget->setCurrentWidget(data_tool_.data());
    // ui_->stacked_widget->setCurrentIndex(VIEW_MODE::ORIGINAL);
    ui_->controlsDock->setWindowTitle("Data");
    ui_->action_import_mode->setChecked(true);
  }

  ui_->stacked_widget->widget(ui_->stacked_widget->currentIndex())
      ->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
  ui_->stacked_widget->adjustSize();

  on_actionShow_Tool_Window_triggered();
}

//---------------------------------------------------------------------------
std::string ShapeWorksStudioApp::get_tool_state() {
  std::string tool_state = session_->parameters().get("tool_state", Session::DATA_C);
  return tool_state;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_view_mode() {

  auto view_mode = get_view_mode();
  ui_->view_mode_combobox->setCurrentText(QString::fromStdString(view_mode));

  auto feature_map = get_feature_map();
  ui_->features->setCurrentText(QString::fromStdString(feature_map));

  if (visualizer_) {
    visualizer_->set_display_mode(view_mode);
    if (feature_map == "-none-") {
      feature_map = "";
    }
    analysis_tool_->set_feature_map(feature_map);

    auto image_volume_name = session_->get_image_name();
    if (image_volume_name == "-none-") {
      image_volume_name = "";
    }

    std::string feature_map_override = "";
    if (get_tool_state() == Session::DEEPSSM_C) {
      if (deepssm_tool_->get_display_feature() != "") {
        feature_map_override = deepssm_tool_->get_display_feature();
      }
    } else if (get_tool_state() == Session::ANALYSIS_C) {
      if (analysis_tool_->get_display_feature_map() != feature_map) {
        feature_map_override = analysis_tool_->get_display_feature_map();
      }
    }

    if (feature_map_override != "") {
      ui_->features->hide();
      ui_->feature_line->setText(QString::fromStdString(feature_map_override));
      ui_->feature_line->setVisible(true);
      feature_map = feature_map_override;
    } else {
      ui_->features->show();
      ui_->feature_line->hide();
    }

    auto feature_maps = session_->get_project()->get_feature_names();
    ui_->feature_widget->setVisible(feature_maps.size() > 0 || feature_map_override != "");

    visualizer_->set_feature_map(feature_map);
    visualizer_->set_uniform_feature_range(get_feature_uniform_scale());
    update_feature_map_scale();
    update_display(true);
  }
}

//---------------------------------------------------------------------------
std::string ShapeWorksStudioApp::get_view_mode() {
  return session_->parameters().get("view_state", Visualizer::MODE_ORIGINAL_C);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::set_view_combo_item_enabled(int item, bool value) {
  ui_->view_mode_combobox->setItemData(item, value ? ITEM_ENABLE : ITEM_DISABLE, ITEM_ROLE);
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::is_view_combo_item_enabled(int item) {
  return ui_->view_mode_combobox->itemData(item, ITEM_ROLE) == ITEM_ENABLE;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_import_mode_triggered() {
  session_->parameters().set("tool_state", Session::DATA_C);
  update_tool_mode();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_groom_mode_triggered() {
  session_->parameters().set("tool_state", Session::GROOM_C);
  update_tool_mode();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_optimize_mode_triggered() {
  session_->parameters().set("tool_state", Session::OPTIMIZE_C);
  update_tool_mode();
  visualizer_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_analysis_mode_triggered() {
  session_->parameters().set("tool_state", Session::ANALYSIS_C);
  update_tool_mode();
  visualizer_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_deepssm_mode_triggered() {
  session_->parameters().set("tool_state", Session::DEEPSSM_C);
  update_tool_mode();
  visualizer_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_project_changed() {
  if (session_->is_loading()) {
    return;
  }

  if (session_->get_shapes().size() == 0) {
    new_session();
    analysis_tool_->reset_stats();
    lightbox_->clear_renderers();
  }

  set_view_combo_item_enabled(VIEW_MODE::ORIGINAL, session_->original_present());
  set_view_combo_item_enabled(VIEW_MODE::GROOMED, session_->groomed_present());
  set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED, session_->particles_present());

  if (session_->particles_present()) {
    session_->handle_clear_cache();
  }
  update_table();
  update_scrollbar();
  update_display();
  visualizer_->update_lut();

  enable_possible_actions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_points_changed() {
  bool update = false;
  if (!time_since_last_update_.isValid()) {
    update = true;
  } else {
    auto time_since = time_since_last_update_.elapsed();
    if (time_since > 25 + (last_render_ * 2)) {
      update = true;
    }
  }

  if (update) {
    double cur_size = visualizer_->get_current_glyph_size();
    double new_size = session_->update_auto_glyph_size();
    double percent_diff = cur_size / new_size * 100.0;
    if (percent_diff < 90 || percent_diff > 110) {
      handle_glyph_changed();
    }

    QElapsedTimer render_time;
    render_time.start();
    visualizer_->update_samples();
    last_render_ = render_time.elapsed();
    time_since_last_update_.start();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_optimize_complete() {
  int num_domains = session_->get_domains_per_shape();
  for (int i = 0; i < num_domains; i++) {
    session_->get_mesh_manager()->get_surface_reconstructor(i)->resetReconstruct();
  }
  analysis_tool_->reset_stats();
  analysis_tool_->initialize_mesh_warper();
  session_->handle_clear_cache();
  set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED, true);
  ui_->view_mode_combobox->setCurrentIndex(VIEW_MODE::GROOMED);
  visualizer_->set_display_mode(ui_->view_mode_combobox->currentText().toStdString());
  visualizer_->set_mean(analysis_tool_->get_mean_shape_points().get_combined_global_particles());
  visualizer_->update_lut();
  update_display();

  visualizer_->update_samples();
  handle_glyph_changed();
  enable_possible_actions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_reconstruction_complete() {
  session_->handle_clear_cache();
  set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED, true);
  visualizer_->set_mean(analysis_tool_->get_mean_shape_points().get_combined_global_particles());
  visualizer_->update_lut();
  update_display(true);
  enable_possible_actions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_groom_start() {
  // clear out old points
  session_->clear_particles();
  ui_->action_analysis_mode->setEnabled(false);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_groom_complete() {
  set_view_combo_item_enabled(VIEW_MODE::GROOMED, true);
  ui_->view_mode_combobox->setCurrentIndex(VIEW_MODE::GROOMED);
  session_->handle_clear_cache();
  update_display(true);
  visualizer_->reset_camera();
  enable_possible_actions();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_optimize_start() {
  visualizer_->set_selected_point_one(-1);
  visualizer_->set_selected_point_two(-1);
  ui_->action_analysis_mode->setEnabled(false);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_display_setting_changed() {
  if (analysis_tool_->pcaAnimate()) {
    return;
  }
  update_display(true);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_glyph_changed() {
  visualizer_->set_show_surface(ui_->surface_visible_button->isChecked());
  visualizer_->set_show_glyphs(ui_->glyphs_visible_button->isChecked());
  visualizer_->set_show_landmarks(ui_->landmarks_visible_button->isChecked());
  preferences_.set_glyph_size(glyph_size_slider_->value() / 10.0);
  preferences_.set_glyph_quality(glyph_quality_slider_->value());
  preferences_.set_glyph_auto_size(glyph_auto_size_->isChecked());
  glyph_size_slider_->setEnabled(!glyph_auto_size_->isChecked());
  if (glyph_auto_size_->isChecked()) {
    auto glyph_size = session_->get_auto_glyph_size();
    if (glyph_size > 0) {
      glyph_size_slider_->setValue(glyph_size * 10.0);
    }
  }

  glyph_quality_label_->setText(QString::number(preferences_.get_glyph_quality()));
  glyph_size_label_->setText(QString::number(preferences_.get_glyph_size()));
  // update_display(true);
  visualizer_->update_viewer_properties();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_opacity_changed() {
  std::vector<float> opacities;
  for (auto& slider : iso_opacity_sliders_) {
    opacities.push_back(slider->value() / 100.0);
  }
  visualizer_->set_opacities(opacities);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_alignment_changed() {
  visualizer_->set_alignment_domain(ui_->alignment_combo->currentIndex() - 1);
  update_display(true);
  visualizer_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_center_checkbox_stateChanged() {
  preferences_.set_center_checked(ui_->center_checkbox->isChecked());
  update_display(true);
  visualizer_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_display(bool force) {
  if (!visualizer_) {
    return;
  }

  if (block_update_ || session_->is_loading()) {
    return;
  }

  block_update_ = true;

  visualizer_->set_center(ui_->center_checkbox->isChecked());

  bool reconstruct_ready = true;

  for (int i = 0; i < session_->get_domains_per_shape(); i++) {
    if (!session_->get_mesh_manager()->get_surface_reconstructor(i)->hasDenseMean()) {
      reconstruct_ready = false;
    }
  }

  if (!session_->groomed_present() && session_->particles_present()) {
    // legacy will be used
    reconstruct_ready = true;
  }

  if (session_->particles_present()) {
    reconstruct_ready = true;
  }

  std::string mode = AnalysisTool::MODE_ALL_SAMPLES_C;

  bool analysis_mode = ui_->action_analysis_mode->isChecked();

  int num_domains = session_->get_domains_per_shape();
  ui_->alignment_combo->setVisible(!analysis_mode && num_domains > 1);
  ui_->center_checkbox->setVisible(!analysis_mode);

  if (analysis_mode) {
    mode = analysis_tool_->get_analysis_mode();
  }

  bool change = false;
  if (current_display_mode_ != mode) {
    change = true;
  }

  if (current_display_mode_ == mode && !force) {
    block_update_ = false;
    return;
  }

  std::string tool_state = session_->parameters().get("tool_state", Session::DATA_C);

  update_view_mode();

  if (tool_state == Session::DEEPSSM_C) {
    visualizer_->display_shapes(deepssm_tool_->get_shapes());
    set_view_combo_item_enabled(VIEW_MODE::ORIGINAL, false);
    set_view_combo_item_enabled(VIEW_MODE::GROOMED, false);
    set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED, true);
  } else {
    current_display_mode_ = mode;

    visualizer_->set_mean(analysis_tool_->get_mean_shape_points().get_combined_global_particles());

    if (mode == AnalysisTool::MODE_ALL_SAMPLES_C) {
      set_view_combo_item_enabled(VIEW_MODE::ORIGINAL, session_->original_present());
      set_view_combo_item_enabled(VIEW_MODE::GROOMED, session_->groomed_present());
      set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED, session_->particles_present());

      session_->calculate_reconstructed_samples();
      visualizer_->display_samples();
    } else {
      if (mode == AnalysisTool::MODE_MEAN_C) {
        set_view_combo_item_enabled(VIEW_MODE::ORIGINAL, false);
        set_view_combo_item_enabled(VIEW_MODE::GROOMED, false);
        set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED, true);

        set_view_mode(Visualizer::MODE_RECONSTRUCTION_C);

        visualizer_->display_shape(analysis_tool_->get_mean_shape());
      } else if (mode == AnalysisTool::MODE_PCA_C) {
        set_view_combo_item_enabled(VIEW_MODE::ORIGINAL, false);
        set_view_combo_item_enabled(VIEW_MODE::GROOMED, false);
        set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED, true);
        set_view_mode(Visualizer::MODE_RECONSTRUCTION_C);
        compute_mode_shape();
        visualizer_->reset_camera();
      } else if (mode == AnalysisTool::MODE_SINGLE_SAMPLE_C) {
        set_view_combo_item_enabled(VIEW_MODE::ORIGINAL, session_->original_present());
        set_view_combo_item_enabled(VIEW_MODE::GROOMED, session_->groomed_present());
        set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED,
                                          session_->particles_present() && reconstruct_ready);
        visualizer_->display_sample(analysis_tool_->get_sample_number());
        visualizer_->reset_camera();
      } else {  //?
        set_view_combo_item_enabled(VIEW_MODE::ORIGINAL, session_->original_present());
        set_view_combo_item_enabled(VIEW_MODE::GROOMED, session_->groomed_present());
        set_view_combo_item_enabled(VIEW_MODE::RECONSTRUCTED,
                                          session_->particles_present() && reconstruct_ready);
      }  // TODO regression?
    }
  }

  lightbox_->update_interactor_style();

  if ((force || change) && !session_->is_loading()) {  // do not override if loading
    reset_num_viewers();
  }

  update_scrollbar();

  block_update_ = false;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_view_mode_combobox_currentIndexChanged(QString disp_mode) {
  set_view_mode(disp_mode.toStdString());
  visualizer_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::open_project(QString filename) {
  new_session();
  handle_message("Loading Project: " + filename);
  handle_progress(-1);  // busy
  QApplication::processEvents();

  session_->set_loading(true);

  try {
    if (!session_->load_project(filename)) {
      enable_possible_actions();
      handle_error("Project failed to load");
      handle_progress(100);
      return;
    }
  } catch (std::exception& e) {
    handle_error(QString("Project failed to load: ") + e.what());
    handle_progress(100);
    return;
  }

  auto project = session_->get_project();
  if (project->get_version() > project->get_supported_version()) {
    handle_warning(
        "Warning: The project you have opened was created in a newer version of ShapeWorks\n\n"
        "Some features may not work and some settings may be incorrect or missing");
  }

  analysis_tool_->reset_stats();
  analysis_tool_->initialize_mesh_warper();

  block_update_ = true;

  groom_tool_->load_params();
  optimize_tool_->load_params();
  preferences_window_->set_values_from_preferences();
  update_from_preferences();

  preferences_.add_recent_file(QFileInfo(filename).absoluteFilePath(), QDir::currentPath());
  update_recent_files();

  update_tool_mode();

  // set the zoom state
  // ui_->thumbnail_size_slider->setValue(
  //  preferences_.get_preference("zoom_state", 1));

  visualizer_->update_lut();
  preferences_.set_saved();
  enable_possible_actions();
  visualizer_->reset_camera();

  update_table();
  update_alignment_options();

  update_view_mode();

  // load analysis state
  std::string analysis_mode = session_->parameters().get("analysis_mode", "mean");
  analysis_tool_->set_analysis_mode(analysis_mode);

  int zoom_value = session_->parameters().get(ShapeWorksStudioApp::SETTING_ZOOM_C, "4");

  ui_->zoom_slider->setValue(zoom_value);

  data_tool_->update_notes();

  block_update_ = false;
  update_display(true);

  on_zoom_slider_valueChanged();

  //update_table();

  session_->set_loading(false);

  handle_project_changed();

  if (session_->is_light_project()) {
    reset_num_viewers();
  }

  session_->update_auto_glyph_size();
  handle_glyph_changed();

  setWindowTitle(session_->get_display_name());

  // final check after loading that the view mode isn't set to something invalid
  if (!is_view_combo_item_enabled(ui_->view_mode_combobox->currentIndex())) {
    set_view_mode(Visualizer::MODE_ORIGINAL_C);
    update_view_mode();
  }

  create_iso_submenu();
  handle_progress(100);
  handle_message("Project loaded: " + filename);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_preferences_triggered() { preferences_window_->show(); }

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_export_current_mesh_triggered() {
  bool single = StudioUtils::ask_multiple_domains_as_single(this, session_->get_project());

  auto dir = preferences_.get_last_directory() + "/";
  QString filename = QFileDialog::getSaveFileName(this, tr("Export Current Mesh"), dir + "mesh",
                                                  tr("Supported types (*.vtk *.ply *.vtp *.obj *.stl)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());

  if (single) {
    write_mesh(visualizer_->get_current_mesh(), filename);
    handle_message("Wrote: " + filename);
  } else {
    auto meshes = visualizer_->get_current_meshes_transformed();
    auto domain_names = session_->get_project()->get_domain_names();

    if (meshes.empty()) {
      handle_error("Error exporting mesh: not ready yet");
      return;
    }

    QFileInfo fi(filename);
    QString base = fi.path() + QDir::separator() + fi.completeBaseName();
    for (int domain = 0; domain < meshes.size(); domain++) {
      QString name = base + "_" + QString::fromStdString(domain_names[domain]) + "." + fi.completeSuffix();

      if (!write_mesh(meshes[domain], name)) {
        return;
      }
      handle_message("Wrote: " + name);
    }
  }
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::write_mesh(vtkSmartPointer<vtkPolyData> poly_data, QString filename) {
  if (!poly_data) {
    handle_error("Error exporting mesh: not ready yet");
  }
  try {
    Mesh mesh(poly_data);
    mesh.write(filename.toStdString());
  } catch (std::exception& e) {
    handle_error(e.what());
    return false;
  }
  return true;
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::write_scalars(vtkSmartPointer<vtkPolyData> poly_data, QString filename) {
  if (!poly_data || !poly_data->GetPointData()->GetScalars()) {
    handle_error("Error, no scalars to export");
    return false;
  }

  std::ofstream output;
  output.open(filename.toStdString().c_str());
  if (output.bad()) {
    handle_error("Error writing to file: " + filename);
    return false;
  }
  output << "point,x,y,z";

  int num_arrays = poly_data->GetPointData()->GetNumberOfArrays();
  std::cerr << "number of arrays = " << num_arrays << "\n";

  for (int i = 0; i < num_arrays; i++) {
    if (!poly_data->GetPointData()->GetArrayName(i)) {
      output << ","
             << "scalars";
    } else {
      output << "," << poly_data->GetPointData()->GetArrayName(i);
    }
  }

  output << "\n";

  // iterate over vertices
  vtkPoints* points = poly_data->GetPoints();
  int num_points = points->GetNumberOfPoints();

  for (int i = 0; i < num_points; i++) {
    output << i;
    output << "," << poly_data->GetPoint(i)[0];
    output << "," << poly_data->GetPoint(i)[1];
    output << "," << poly_data->GetPoint(i)[2];

    for (int j = 0; j < num_arrays; j++) {
      output << "," << poly_data->GetPointData()->GetArray(j)->GetTuple(i)[0];
    }
    output << "\n";
  }

  output.close();
  return true;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_export_current_particles_triggered() {
  bool single = StudioUtils::ask_multiple_domains_as_single(this, session_->get_project());

  auto dir = preferences_.get_last_directory() + "/";
  QString filename = QFileDialog::getSaveFileName(this, tr("Export Current Particles"), dir + "shape",
                                                  tr("Particle files (*.particles)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());

  if (single) {
    auto particles = visualizer_->get_current_shape().get_combined_global_particles();

    if (!ShapeWorksStudioApp::write_particle_file(filename.toStdString(), particles)) {
      handle_error("Error writing particle file: " + filename);
    }
    handle_message("Wrote: " + filename);
  } else {
    auto domain_names = session_->get_project()->get_domain_names();

    QFileInfo fi(filename);
    QString base = fi.path() + QDir::separator() + fi.completeBaseName();
    for (int domain = 0; domain < domain_names.size(); domain++) {
      QString name = base + "_" + QString::fromStdString(domain_names[domain]) + "." + fi.completeSuffix();

      auto shape = visualizer_->get_current_shape();
      auto particles = visualizer_->get_current_shape().get_world_particles(domain);
      if (!ShapeWorksStudioApp::write_particle_file(name.toStdString(), particles)) {
        handle_error("Error writing particle file: " + name);
      }

      handle_message("Wrote: " + name);
    }
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_export_mesh_scalars_triggered() {
  bool single = StudioUtils::ask_multiple_domains_as_single(this, session_->get_project());

  auto dir = preferences_.get_last_directory().toStdString() + "/";
  QString filename = QFileDialog::getSaveFileName(this, tr("Export Mesh Scalars"),
                                                  QString::fromStdString(dir) + "scalars", tr("CSV files (*.csv)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());

  if (single) {
    auto poly_data = visualizer_->get_current_mesh();
    write_scalars(poly_data, filename);
  } else {
    auto meshes = visualizer_->get_current_meshes_transformed();
    if (meshes.empty()) {
      handle_error("Error exporting mesh: not ready yet");
      return;
    }

    auto domain_names = session_->get_project()->get_domain_names();

    QFileInfo fi(filename);
    QString base = fi.path() + QDir::separator() + fi.completeBaseName();
    for (int domain = 0; domain < domain_names.size(); domain++) {
      QString name = base + "_" + QString::fromStdString(domain_names[domain]) + "." + fi.completeSuffix();

      auto poly_data = meshes[domain];
      if (!write_scalars(poly_data, name)) {
        return;
      }

      handle_message("Wrote: " + name);
    }
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_export_pca_scores_triggered() {
  auto dir = preferences_.get_last_directory().toStdString() + "/";
  QString filename = QFileDialog::getSaveFileName(this, tr("Export PCA Scores"), QString::fromStdString(dir) + "scores",
                                                  tr("CSV files (*.csv)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());

  auto stats = analysis_tool_->get_stats();
  stats.PrincipalComponentProjections();

  stats.WriteCSVFile2(filename.toStdString());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::closeEvent(QCloseEvent* event) {
  // close the preferences window in case it is open
  preferences_window_->close();
  if (preferences_.not_saved() && ui_->action_save_project->isEnabled()) {
    // save the size of the window to preferences
    QMessageBox msgBox;
    msgBox.setText("Do you want to save your changes as a project file?");
    msgBox.setInformativeText("This will reload generated files and changed settings.");
    msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    msgBox.setDefaultButton(QMessageBox::Save);
    int ret = msgBox.exec();
    if (ret == QMessageBox::Save) {
      if (!on_action_save_project_triggered()) {
        event->ignore();
        return;
      }
    } else if (ret == QMessageBox::Cancel) {
      event->ignore();
      return;
    }
  }
  analysis_tool_->shutdown();
  preferences_.set_window_geometry(saveGeometry());
  preferences_.set_window_state(saveState());

  optimize_tool_->shutdown_threads();
  deepssm_tool_->shutdown();
  STUDIO_CLOSE_LOG();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::compute_mode_shape() {
  int pca_mode = analysis_tool_->getPCAMode();
  double pca_value = analysis_tool_->get_pca_value();

  visualizer_->display_shape(analysis_tool_->get_mode_shape(pca_mode, pca_value));
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::set_view_mode(std::string view_mode) {
  if (view_mode != get_view_mode()) {
    if (!session_->is_loading()) {
      session_->parameters().set("view_state", view_mode);
    }
    update_view_mode();
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_recent_files() {
  QStringList recent_files = preferences_.get_recent_files();
  QStringList recent_paths = preferences_.get_recent_paths();

  int num_recent_files = qMin(recent_files.size(), 8);  // only 8 max in the file menu

  for (int i = 0; i < num_recent_files; i++) {
    QString text = tr("&%1 %2").arg(i + 1).arg(QFileInfo(recent_files[i]).fileName());
    recent_file_actions_[i]->setText(text);
    QStringList user_data = {recent_files[i], recent_paths[i]};
    recent_file_actions_[i]->setData(user_data);
    recent_file_actions_[i]->setVisible(true);
  }

  for (int j = num_recent_files; j < 8; ++j) {
    recent_file_actions_[j]->setVisible(false);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_alignment_options() {
  int num_domains = session_->get_domains_per_shape();
  ui_->alignment_combo->setVisible(num_domains > 1);
  ui_->alignment_combo->clear();
  ui_->alignment_combo->addItem("Global");
  auto domain_names = session_->get_project()->get_domain_names();
  for (const auto& name : domain_names) {
    ui_->alignment_combo->addItem(QString::fromStdString(name));
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::save_project(std::string filename) {
  session_->parameters().set(ShapeWorksStudioApp::SETTING_ZOOM_C,
                                   std::to_string(ui_->zoom_slider->value()));

  session_->parameters().set("notes", data_tool_->get_notes());

  session_->parameters().set("analysis_mode", analysis_tool_->get_analysis_mode());

  data_tool_->store_data();
  groom_tool_->store_params();
  optimize_tool_->store_params();
  analysis_tool_->store_settings();
  deepssm_tool_->store_params();

  if (session_->save_project(filename)) {
    handle_message("Project Saved");
  }

  update_table();
  setWindowTitle(session_->get_display_name());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_open_recent() {
  QAction* action = qobject_cast<QAction*>(sender());
  if (action) {
    auto user_data = action->data().toStringList();
    if (user_data.size() == 2) {
      QDir::setCurrent(user_data[1]);
      open_project(user_data[0]);
    }
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_color_scheme() {
  visualizer_->update_viewer_properties();
  update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_auto_view_button_clicked() {
  // visualizer_->reset_camera();
  lightbox_->reset_camera();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionExport_PCA_Mesh_triggered() {
  auto dir = preferences_.get_last_directory().toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Export PCA Mesh"), QString::fromStdString(dir) + "newMesh",
                                                  tr("VTK files (*.vtk)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());
  if (analysis_tool_->get_analysis_mode() == "all samples") {
    auto shapes = session_->get_shapes();
    for (size_t i = 0; i < shapes.size(); i++) {
      /// TODO: multiple-domain support
      auto msh = shapes[i]->get_reconstructed_meshes().meshes()[0]->get_poly_data();
      vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
      auto name = filename.toStdString();
      name = name.substr(0, name.find_last_of(".")) + std::to_string(i) + ".vtk";
      writer->SetFileName(name.c_str());
      writer->SetInputData(msh);
      writer->WriteArrayMetaDataOff();
      writer->Write();
    }
    handle_message("Successfully exported PCA Mesh files: " + filename);
    return;
  }
  auto shape = visualizer_->get_current_shape();
  /// TODO: fix
  // auto msh = session_->get_mesh_manager()->get_mesh(shape);

  vtkPolyDataWriter* writer = vtkPolyDataWriter::New();
  writer->SetFileName(filename.toStdString().c_str());
  /// TODO: fix
  // writer->SetInputData(msh);
  writer->WriteArrayMetaDataOff();
  writer->Write();
  handle_message("Successfully exported PCA Mesh file: " + filename);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionExport_Eigenvalues_triggered() {
  auto stats = analysis_tool_->get_stats();
  auto values = stats.Eigenvalues();
  QString fname("Untitled.eval");

  auto dir = preferences_.get_last_directory().toStdString() + "/";
  QString filename = QFileDialog::getSaveFileName(this, tr("Export Eigenvalue EVAL file..."),
                                                  QString::fromStdString(dir) + fname, tr("EVAL files (*.eval)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());
  std::ofstream out(filename.toStdString().c_str());
  for (size_t i = values.size() - 1; i > 0; i--) {
    out << values[i] << std::endl;
  }
  out.close();
  handle_message("Successfully exported eigenvalue EVAL file: " + filename);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionExport_Eigenvectors_triggered() {
  auto stats = analysis_tool_->get_stats();
  auto values = stats.Eigenvectors();
  QString fname("Untitled.eval");
  auto dir = preferences_.get_last_directory().toStdString() + "/";
  QString filename = QFileDialog::getSaveFileName(this, tr("Export Eigenvector EVAL files..."),
                                                  QString::fromStdString(dir) + fname, tr("EVAL files (*.eval)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());
  auto basename = filename.toStdString().substr(0, filename.toStdString().find_last_of(".eval") - 4);
  for (size_t i = values.cols() - 1, ii = 0; i > 0; i--, ii++) {
    auto col = values.col(i);
    std::ofstream out(basename + std::to_string(ii) + ".eval");
    size_t newline = 1;
    for (int i = 0; i < col.size(); i++) {
      out << col[i] << (newline % 3 == 0 ? "\n" : "    ");
      newline++;
    }
    out.close();
  }
  handle_message("Successfully exported eigenvalue EVAL file: " + filename);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionExport_PCA_Mode_Points_triggered() {
  QString fname("Untitled.particles");
  auto dir = preferences_.get_last_directory().toStdString() + "/";
  QString filename =
      QFileDialog::getSaveFileName(this, tr("Save PCA Mode Particle files..."), QString::fromStdString(dir) + fname,
                                   tr("Particle files (*.particles)"));
  auto basename = filename.toStdString().substr(0, filename.toStdString().find_last_of(".particles") - 9);
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());

  double range = preferences_.get_pca_range();
  double steps = static_cast<double>(preferences_.get_pca_steps());
  int mode = analysis_tool_->getPCAMode();
  int half_steps = (steps / 2.0);
  double increment = range / half_steps;

  auto mean_pts = analysis_tool_->get_shape_points(mode, 0).get_combined_global_particles();
  QString mean_name = QString::fromStdString(basename) + "_mean.particles";
  if (!ShapeWorksStudioApp::write_particle_file(mean_name.toStdString(), mean_pts)) {
    handle_error("Error writing particle file: " + mean_name);
    return;
  }

  for (int i = 1; i <= half_steps; i++) {
    double pca_value = increment * i;
    std::string pca_string = QString::number(pca_value, 'g', 2).toStdString();

    std::string minus_name = basename + "_mode_" + std::to_string(mode) + "_minus_" + pca_string + ".pts";
    auto pts = analysis_tool_->get_shape_points(mode, -pca_value).get_combined_global_particles();
    if (!ShapeWorksStudioApp::write_particle_file(minus_name, pts)) {
      handle_error("Error writing particle file: " + QString::fromStdString(minus_name));
      return;
    }

    std::string plus_name = basename + "_mode_" + std::to_string(mode) + "_plus_" + pca_string + ".pts";
    pts = analysis_tool_->get_shape_points(mode, pca_value).get_combined_global_particles();
    if (!ShapeWorksStudioApp::write_particle_file(plus_name, pts)) {
      handle_error("Error writing particle file: " + QString::fromStdString(plus_name));
      return;
    }
  }

  handle_message("Successfully exported PCA Mode particle files");
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::write_particle_file(std::string filename, Eigen::VectorXd particles) {
  std::ofstream out(filename);
  if (!out) {
    return false;
  }
  size_t newline = 1;
  for (int i = 0; i < particles.size(); i++) {
    out << particles[i] << (newline % 3 == 0 ? "\n" : "    ");
    newline++;
  }
  out.close();
  if (out.bad()) {
    return false;
  }

  return true;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionExport_Variance_Graph_triggered() {
  QString fname("Untitled.png");
  auto dir = preferences_.get_last_directory().toStdString();
  dir = dir.substr(0, dir.find_last_of("/") + 1);
  QString filename = QFileDialog::getSaveFileName(this, tr("Export Variance Graph"),
                                                  QString::fromStdString(dir) + fname, tr("PNG files (*.png)"));
  if (filename.isEmpty()) {
    return;
  }
  preferences_.set_last_directory(QFileInfo(filename).absolutePath());

  if (!analysis_tool_->export_variance_graph(filename)) {
    handle_error("Error writing variance graph");
  } else {
    handle_message("Successfully exported Variance Graph: " + filename);
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_feature_map_selection(const QString& feature_map) {
  set_feature_map(feature_map.toStdString());
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_feature_map_scale() {
  bool auto_mode = ui_->feature_auto_scale->isChecked();
  ui_->feature_min->setEnabled(!auto_mode);
  ui_->feature_max->setEnabled(!auto_mode);
  if (!auto_mode) {
    if (session_->get_feature_range_min() == 0 && session_->get_feature_range_max() == 0) {
      if (visualizer_->get_feature_range_valid()) {
        double* range = visualizer_->get_feature_raw_range();
        ui_->feature_min->setValue(range[0]);
        ui_->feature_max->setValue(range[1]);
      }
    }
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::image_combo_changed(const QString& image_name) {
  session_->set_image_name(image_name.toStdString());
}

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::set_feature_map(std::string feature_map) {
  if (feature_map != get_feature_map()) {
    if (!session_->is_loading()) {
      session_->parameters().set("feature_map", feature_map);
    }
    update_view_mode();
    return true;
  }
  return false;
}

//---------------------------------------------------------------------------
std::string ShapeWorksStudioApp::get_feature_map() { return session_->parameters().get("feature_map", ""); }

//---------------------------------------------------------------------------
bool ShapeWorksStudioApp::get_feature_uniform_scale() {
  return session_->parameters().get("feature_uniform_scale", true);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::set_feature_uniform_scale(bool value) {
  if (!session_->is_loading()) {
    session_->parameters().set("feature_uniform_scale", value);
    update_view_mode();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::show_splash_screen() { splash_screen_->show(); }

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::about() {
  std::string about = std::string("About ShapeWorksStudio");
  QMessageBox::about(this, QString::fromStdString(about),
                     "ShapeWorksStudio\n" SHAPEWORKS_VERSION
                     "\n\n"
                     "http://shapeworks.sci.utah.edu");
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::keyboard_shortcuts() {
  KeyboardShortcuts dialog(this);
  dialog.exec();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::reset_num_viewers() {
  std::string mode = AnalysisTool::MODE_ALL_SAMPLES_C;

  if (ui_->action_analysis_mode->isChecked()) {
    mode = analysis_tool_->get_analysis_mode();
  }

  if (mode == AnalysisTool::MODE_ALL_SAMPLES_C) {
    size_t num_samples = session_->get_shapes().size();
    int value = 4;
    if (num_samples == 1) {
      value = 0;  // single
    } else if (num_samples == 2) {
      value = 1;  // two side by side
    } else if (num_samples <= 4) {
      value = 2;  // 2x2
    } else if (num_samples <= 9) {
      value = 3;  // 3x3
    } else if (num_samples > 9) {
      value = 4;  // 4x4
    }
    if (value != ui_->zoom_slider->value()) {
      ui_->zoom_slider->setValue(value);
    }
  } else {
    if (0 != ui_->zoom_slider->value()) {
      ui_->zoom_slider->setValue(0);
    }
  }
  on_zoom_slider_valueChanged();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::dragEnterEvent(QDragEnterEvent* event) {
  bool accept = false;

  if (event->mimeData()->hasUrls()) {
    QList<QUrl> urls = event->mimeData()->urls();

    for (int i = 0; i < urls.size(); ++i) {
      std::string filename = urls[i].toLocalFile().toStdString();
      if (Session::is_supported_file_format(filename)) {
        accept = true;
      }
    }
  }

  if (accept) {
    setFocus();
    event->accept();
  } else {
    event->ignore();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::dragLeaveEvent(QDragLeaveEvent* event) {
  clearFocus();
  QWidget::dragLeaveEvent(event);
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::dropEvent(QDropEvent* event) {
  bool accept = false;

  QStringList files_to_load;

  if (event->mimeData()->hasUrls()) {
    QList<QUrl> urls = event->mimeData()->urls();

    for (int i = 0; i < urls.size(); ++i) {
      std::string filename = urls[i].toLocalFile().toStdString();

      if (Session::is_supported_file_format(filename)) {
        accept = true;
        files_to_load << QString::fromStdString(filename);
      }
    }
  }

  if (accept) {
    import_files(files_to_load);
    event->accept();
  } else {
    event->ignore();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::toggle_log_window() { log_window_.setVisible(!log_window_.isVisible()); }

//---------------------------------------------------------------------------
QSharedPointer<PythonWorker> ShapeWorksStudioApp::get_py_worker() { return py_worker_; }

//---------------------------------------------------------------------------
}  // namespace shapeworks
