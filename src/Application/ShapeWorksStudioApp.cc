// std
#include <iostream>

// qt
#include <QFileDialog>

// vtk
#include <vtkRenderWindow.h>

// studio
#include <Application/ShapeWorksStudioApp.h>
#include <Application/Preferences.h>
#include <Groom/GroomTool.h>
#include <Optimize/OptimizeTool.h>
#include <Analysis/AnalysisTool.h>
#include <Data/Project.h>
#include <Data/Shape.h>
#include <Data/Mesh.h>
#include <Visualization/Lightbox.h>
#include <Visualization/DisplayObject.h>
#include <Visualization/Visualizer.h>

// ui
#include <ui_ShapeWorksStudioApp.h>

//---------------------------------------------------------------------------
ShapeWorksStudioApp::ShapeWorksStudioApp( int argc, char** argv )
{
  this->ui_ = new Ui_ShapeWorksStudioApp;
  this->ui_->setupUi( this );

  this->project_ = QSharedPointer<Project>( new Project() );

  connect( this->project_.data(), SIGNAL( data_changed() ), this, SLOT( handle_project_changed() ) );

  // setup modes
  this->ui_->view_mode_combobox->addItem( Visualizer::MODE_ORIGINAL_C );
  this->ui_->view_mode_combobox->addItem( Visualizer::MODE_GROOMED_C );
  this->ui_->view_mode_combobox->addItem( Visualizer::MODE_RECONSTRUCTION_C );
  this->ui_->view_mode_combobox->setCurrentIndex( 0 );
  this->ui_->view_mode_combobox->setItemData( 1, 0, Qt::UserRole - 1 );
  this->ui_->view_mode_combobox->setItemData( 2, 0, Qt::UserRole - 1 );

  // resize from preferences
  this->resize( Preferences::Instance().get_main_window_size() );

  // set to import
  this->ui_->action_import_mode->setChecked( true );
  this->ui_->stacked_widget->setCurrentIndex( 0 );

  this->action_group_ = new QActionGroup( this );
  this->action_group_->addAction( this->ui_->action_import_mode );
  this->action_group_->addAction( this->ui_->action_groom_mode );
  this->action_group_->addAction( this->ui_->action_optimize_mode );

  this->ui_->statusbar->showMessage( "ShapeWorksStudio" );

  this->lightbox_ = LightboxHandle( new Lightbox() );

  this->visualizer_ = QSharedPointer<Visualizer> ( new Visualizer() );
  this->visualizer_->set_lightbox( this->lightbox_ );
  this->visualizer_->set_project( this->project_ );

  this->groom_tool_ = QSharedPointer<GroomTool>( new GroomTool() );
  this->groom_tool_->set_project( this->project_ );
  this->groom_tool_->set_app( this );
  this->ui_->stacked_widget->addWidget( this->groom_tool_.data() );

  connect( this->groom_tool_.data(), SIGNAL( groom_complete() ), this, SLOT( handle_groom_complete() ) );

  this->optimize_tool_ = QSharedPointer<OptimizeTool>( new OptimizeTool() );
  this->optimize_tool_->set_project( this->project_ );
  this->optimize_tool_->set_app( this );
  this->ui_->stacked_widget->addWidget( this->optimize_tool_.data() );

  this->update_from_preferences();
}

//---------------------------------------------------------------------------
ShapeWorksStudioApp::~ShapeWorksStudioApp()
{}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::initialize_vtk()
{
  this->lightbox_->set_render_window( this->ui_->qvtkWidget->GetRenderWindow() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_open_project_triggered()
{
  QString filename = QFileDialog::getOpenFileName( this, tr( "Open Project..." ),
                                                   QString(), tr( "XML files (*.xml)" ) );
  if ( filename.isEmpty() )
  {
    return;
  }

  this->open_project( filename );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_save_project_triggered()
{
  if ( this->project_->get_filename() == "" )
  {
    this->on_action_save_project_as_triggered();
  }
  else
  {
    if ( this->project_->save_project() )
    {
      this->set_status_bar( "Project Saved" );
    }
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_save_project_as_triggered()
{
  QString filename = QFileDialog::getSaveFileName( this, tr( "Save Project As..." ),
                                                   QString(), tr( "XML files (*.xml)" ) );
  if ( filename.isEmpty() )
  {
    return;
  }

  if ( this->project_->save_project( filename ) )
  {
    this->set_status_bar( "Project Saved" );
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

  filenames = QFileDialog::getOpenFileNames( this, tr( "Import Files..." ),
                                             Preferences::Instance().get_last_directory(),
                                             tr( "NRRD files (*.nrrd)" ) );

  if ( filenames.size() == 0 )
  {
    return;
  }

  Preferences::Instance().set_last_directory( QDir().absoluteFilePath( filenames[0] ) );

  this->import_files( filenames );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::import_files( QStringList file_names )
{
  this->project_->import_files( file_names );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_thumbnail_size_slider_valueChanged()
{
  this->project_->set_zoom_state( this->ui_->thumbnail_size_slider->value() );

  int value = this->ui_->thumbnail_size_slider->maximum() - this->ui_->thumbnail_size_slider->value() + 1;

  this->lightbox_->set_tile_layout( value, value );
  this->visualizer_->update_viewer_properties();

  this->update_scrollbar();

  this->ui_->qvtkWidget->GetRenderWindow()->Render();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_from_preferences()
{
  this->ui_->glyph_quality->setValue( Preferences::Instance().get_glyph_quality() );
  this->ui_->glyph_quality_label->setText( QString::number( Preferences::Instance().get_glyph_quality() ) );
  this->ui_->glyph_size->setValue( Preferences::Instance().get_glyph_size() * 10.0 );
  this->ui_->glyph_size_label->setText( QString::number( Preferences::Instance().get_glyph_size() ) );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_scrollbar()
{
  int num_rows = this->lightbox_->get_num_rows();
  int num_visible = this->lightbox_->get_num_rows_visible();
  std::cerr << "num_rows = " << num_rows << "\n";
  std::cerr << "num_visible = " << num_visible << "\n";
  if ( num_visible >= num_rows )
  {
    this->ui_->vertical_scroll_bar->setMaximum( 0 );
    this->ui_->vertical_scroll_bar->setEnabled( false );
  }
  else
  {
    this->ui_->vertical_scroll_bar->setEnabled( true );
    this->ui_->vertical_scroll_bar->setMaximum( num_rows - num_visible );
    this->ui_->vertical_scroll_bar->setPageStep( num_visible );
  }
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_vertical_scroll_bar_valueChanged()
{
  int value = this->ui_->vertical_scroll_bar->value();

  std::cerr << "vertical scrollbar value = " << value << "\n";
  this->lightbox_->set_start_row( value );
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

  for ( int i = list.size() - 1; i >= 0; i-- )
  {
    index_list << list[i].row();
  }

  this->project_->remove_shapes( index_list );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_table()
{

  QVector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();

  this->ui_->table_widget->clear();

  this->ui_->table_widget->setRowCount( shapes.size() );
  this->ui_->table_widget->setColumnCount( 3 );

  QStringList table_header;
  table_header << "#" << "Name" << "Size";
  this->ui_->table_widget->setHorizontalHeaderLabels( table_header );

  this->ui_->table_widget->verticalHeader()->setVisible( false );

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QSharedPointer<Mesh> initial_mesh = shapes[i]->get_initial_mesh();

    QTableWidgetItem* new_item = new QTableWidgetItem( QString::number( i + 1 ) );
    this->ui_->table_widget->setItem( i, 0, new_item );

    new_item = new QTableWidgetItem( shapes[i]->get_initial_filename() );
    this->ui_->table_widget->setItem( i, 1, new_item );

    new_item = new QTableWidgetItem( initial_mesh->get_dimension_string() );
    this->ui_->table_widget->setItem( i, 2, new_item );
  }

  this->ui_->table_widget->resizeColumnsToContents();
  //this->ui->table_widget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  this->ui_->table_widget->horizontalHeader()->setStretchLastSection( true );

  this->ui_->table_widget->setSelectionBehavior( QAbstractItemView::SelectRows );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::mode_changed()
{}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_groom_mode_triggered()
{
  this->project_->set_tool_state( Project::GROOM_C );
  this->ui_->stacked_widget->setCurrentWidget( this->groom_tool_.data() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_import_mode_triggered()
{
  this->project_->set_tool_state( Project::DATA_C );
  this->ui_->stacked_widget->setCurrentIndex( 0 );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_optimize_mode_triggered()
{
  this->project_->set_tool_state( Project::OPTIMIZE_C );
  this->ui_->stacked_widget->setCurrentWidget( this->optimize_tool_.data() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_project_changed()
{
  QVector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();

  if ( this->project_->groomed_present() )
  {
    this->ui_->view_mode_combobox->setItemData( 1, 33, Qt::UserRole - 1 );
  }
  else
  {
    this->ui_->view_mode_combobox->setItemData( 1, 0, Qt::UserRole - 1 );
  }

  if ( this->project_->reconstructed_present() )
  {
    this->ui_->view_mode_combobox->setItemData( 2, 33, Qt::UserRole - 1 );
  }
  else
  {
    this->ui_->view_mode_combobox->setItemData( 2, 0, Qt::UserRole - 1 );
  }

  this->update_display();
  this->update_table();
  this->update_scrollbar();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_groom_complete()
{
  this->set_status_bar( "Groom complete" );
  this->ui_->view_mode_combobox->setCurrentIndex( 1 );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_center_checkbox_stateChanged()
{
  this->handle_project_changed();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_display()
{
  if ( !this->visualizer_ )
  {
    return;
  }
  this->visualizer_->set_center( this->ui_->center_checkbox->isChecked() );
  this->visualizer_->set_display_mode( this->ui_->view_mode_combobox->currentText() );
  this->visualizer_->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_view_mode_combobox_currentIndexChanged()
{
  this->project_->set_display_state( this->ui_->view_mode_combobox->currentText() );
  this->update_display();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::open_project( QString filename )
{
  this->project_->load_project( filename );

  // set UI state based on project
  if ( this->project_->get_tool_state() == Project::DATA_C )
  {
    this->ui_->action_import_mode->trigger();
  }
  else if ( this->project_->get_tool_state() == Project::GROOM_C )
  {
    this->ui_->action_groom_mode->trigger();
  }
  else if ( this->project_->get_tool_state() == Project::OPTIMIZE_C )
  {
    this->ui_->action_optimize_mode->trigger();
  }

  // load display mode
  if ( this->project_->get_display_state() == Visualizer::MODE_ORIGINAL_C )
  {
    this->ui_->view_mode_combobox->setCurrentIndex( 0 );
  }
  else if ( this->project_->get_display_state() == Visualizer::MODE_GROOMED_C )
  {
    this->ui_->view_mode_combobox->setCurrentIndex( 1 );
  }
  else if ( this->project_->get_display_state() == Visualizer::MODE_RECONSTRUCTION_C )
  {
    this->ui_->view_mode_combobox->setCurrentIndex( 2 );
  }

  // set the zoom state
  this->ui_->thumbnail_size_slider->setValue( this->project_->get_zoom_state() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::set_status_bar( QString status )
{
  this->ui_->statusbar->showMessage( status );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_preferences_triggered()
{
  Preferences::Instance().show_window();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::closeEvent( QCloseEvent* event )
{
  // close the preferences window in case it is open
  Preferences::Instance().close_window();

  // save the size of the window to preferences
  Preferences::Instance().set_main_window_size( this->size() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_show_surface_stateChanged()
{
  this->visualizer_->set_show_surface( this->ui_->show_surface->isChecked() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_show_glyphs_stateChanged()
{
  this->visualizer_->set_show_glyphs( this->ui_->show_glyphs->isChecked() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_glyph_size_valueChanged( int value )
{
  Preferences::Instance().set_glyph_size( value / 10.0 );
  this->update_from_preferences();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_glyph_quality_valueChanged( int value )
{
  Preferences::Instance().set_glyph_quality( value );
  this->update_from_preferences();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_samples_button_clicked()
{
  this->ui_->pcaPanel->setVisible( false );
  this->update_tools();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_stats_button_clicked()
{
  this->visualizer_->display_mean();
  this->update_tools();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_pca_button_clicked()
{
  this->update_tools();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_tools()
{
  if ( this->ui_->samples_button->isChecked() )
  {
    this->ui_->pcaPanel->setVisible( false );
  }
  else if ( this->ui_->stats_button->isChecked() )
  {
    this->ui_->pcaPanel->setVisible( false );
  }
  else if ( this->ui_->pca_button->isChecked() )
  {
    this->compute_mode_shape();
    this->ui_->pcaPanel->setVisible( true );
  }
}

//---------------------------------------------------------------------------
double ShapeWorksStudioApp::get_pca_value( int slider_value )
{
  float range = Preferences::Instance().get_pca_range();
  int halfRange = this->ui_->pcaSlider->maximum();

  double value = (double)slider_value / (double)halfRange * range;
  return value;
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::compute_mode_shape()
{
  double pcaSliderValue = this->get_pca_value( this->ui_->pcaSlider->value() );
  int mode = this->ui_->pcaModeSpinBox->value() + 1;
  this->visualizer_->display_pca( mode, pcaSliderValue );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_pcaSlider_valueChanged()
{
  // this will make the slider handle redraw making the UI appear more responsive
  QCoreApplication::processEvents();

  this->compute_mode_shape();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_pcaModeSpinBox_valueChanged()
{
  this->compute_mode_shape();
}
