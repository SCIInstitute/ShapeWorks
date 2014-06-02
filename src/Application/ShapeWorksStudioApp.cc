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

// ui
#include <ui_ShapeWorksStudioApp.h>

//---------------------------------------------------------------------------
ShapeWorksStudioApp::ShapeWorksStudioApp( int argc, char** argv )
{
  this->ui_ = new Ui_ShapeWorksStudioApp;
  this->ui_->setupUi( this );

  // resize from preferences
  this->resize( Preferences::Instance().get_main_window_size() );

  // set to import
  this->ui_->action_import_mode->setChecked( true );
  this->ui_->stacked_widget->setCurrentIndex( 0 );

  this->action_group_ = new QActionGroup( this );
  this->action_group_->addAction( this->ui_->action_import_mode );
  this->action_group_->addAction( this->ui_->action_groom_mode );
  this->action_group_->addAction( this->ui_->action_optimize_mode );
  this->action_group_->addAction( this->ui_->action_analysis_mode );

  this->ui_->statusbar->showMessage( "ShapeWorksStudio" );

  this->project_ = QSharedPointer<Project>( new Project() );

  connect( this->project_.data(), SIGNAL( data_changed() ), this, SLOT( handle_project_changed() ) );

  this->lightbox_ = QSharedPointer<Lightbox>( new Lightbox() );

  this->groom_tool_ = QSharedPointer<GroomTool>( new GroomTool() );
  this->groom_tool_->set_project( this->project_ );
  this->groom_tool_->set_app( this );
  this->ui_->stacked_widget->addWidget( this->groom_tool_.data() );

  this->optimize_tool_ = QSharedPointer<OptimizeTool>( new OptimizeTool() );
  this->optimize_tool_->set_project( this->project_ );
  this->optimize_tool_->set_app( this );
  this->ui_->stacked_widget->addWidget( this->optimize_tool_.data() );

  this->analysis_tool_ = QSharedPointer<AnalysisTool>( new AnalysisTool() );
  this->analysis_tool_->set_project( this->project_ );
  this->analysis_tool_->set_app( this );
  this->ui_->stacked_widget->addWidget( this->analysis_tool_.data() );

  this->ui_->view_mode_combobox->setItemData( 1, 0, Qt::UserRole - 1 );
  this->ui_->view_mode_combobox->setItemData( 2, 0, Qt::UserRole - 1 );
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
void ShapeWorksStudioApp::on_action_save_project_as_triggered()
{
  QString filename = QFileDialog::getSaveFileName( this, tr( "Save Project As..." ),
                                                   QString(), tr( "XML files (*.xml)" ) );
  if ( filename.isEmpty() )
  {
    return;
  }

  this->project_->save_project( filename );
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
                                             QString(), tr( "NRRD files (*.nrrd)" ) );

  if ( filenames.size() == 0 )
  {
    return;
  }

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
  int value = this->ui_->thumbnail_size_slider->maximum() - this->ui_->thumbnail_size_slider->value() + 1;

  this->lightbox_->set_tile_layout( value, value );

  this->update_scrollbar();

  this->ui_->qvtkWidget->GetRenderWindow()->Render();
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
  this->ui_->stacked_widget->setCurrentWidget( this->groom_tool_.data() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_import_mode_triggered()
{
  this->ui_->stacked_widget->setCurrentIndex( 0 );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_optimize_mode_triggered()
{
  this->ui_->stacked_widget->setCurrentWidget( this->optimize_tool_.data() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_action_analysis_mode_triggered()
{
  this->ui_->stacked_widget->setCurrentWidget( this->analysis_tool_.data() );
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

  this->update_display_objects();
  this->update_table();
  this->update_scrollbar();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_center_checkbox_stateChanged()
{
  this->lightbox_->set_auto_center( this->ui_->center_checkbox->isChecked() );
  this->handle_project_changed();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_display_objects()
{
  QVector < QSharedPointer < DisplayObject > > display_objects;

  QVector < QSharedPointer < Shape > > shapes = this->project_->get_shapes();

  QString mode = this->ui_->view_mode_combobox->currentText();

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QSharedPointer<DisplayObject> object = QSharedPointer<DisplayObject>( new DisplayObject() );

    QSharedPointer<Mesh> mesh;
    QString filename;
    if ( mode == "Original" )
    {
      mesh = shapes[i]->get_initial_mesh();
      filename = shapes[i]->get_initial_filename();
    }
    else if ( mode == "Groomed" )
    {
      mesh = shapes[i]->get_groomed_mesh();
      filename = shapes[i]->get_groomed_filename();
    }
    else if ( mode == "Reconstruction" )
    {
      mesh = shapes[i]->get_reconstructed_mesh();
      filename = shapes[i]->get_point_filename();
    }
    object->set_mesh( mesh );
    object->set_correspondence_points( shapes[i]->get_correspondence_points() );

    QStringList annotations;
    annotations << filename;
    annotations << "";
    annotations << QString::number( shapes[i]->get_id() );
    annotations << "";
    object->set_annotations( annotations );

    display_objects << object;
  }

  this->lightbox_->set_display_objects( display_objects );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_view_mode_combobox_currentIndexChanged()
{
  this->update_display_objects();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::open_project( QString filename )
{
  this->project_->load_project( filename );
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
