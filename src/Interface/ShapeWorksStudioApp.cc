#include <iostream>

#include <QFileDialog>

#include <Interface/ShapeWorksStudioApp.h>
#include <ui_ShapeWorksStudioApp.h>

#include <vtkRenderWindow.h>

#include <Visualization/Viewer.h>
#include <Groom/GroomTool.h>
#include <Data/Project.h>
#include <Data/Shape.h>
#include <Data/Mesh.h>

//---------------------------------------------------------------------------
ShapeWorksStudioApp::ShapeWorksStudioApp( int argc, char** argv )
{
  this->ui_ = new Ui_ShapeWorksStudioApp;
  this->ui_->setupUi( this );

  // set to import
  this->ui_->actionImportMode->setChecked( true );
  this->ui_->stackedWidget->setCurrentIndex( 0 );

  this->action_group_ = new QActionGroup( this );
  this->action_group_->addAction( this->ui_->actionImportMode );
  this->action_group_->addAction( this->ui_->actionGroomMode );
  this->action_group_->addAction( this->ui_->actionActionOptimizeMode );
  this->action_group_->addAction( this->ui_->actionAnalysisMode );

  this->ui_->statusbar->showMessage( "ShapeWorksStudio" );

  this->project_ = QSharedPointer<Project>( new Project() );

  connect( this->project_.data(), SIGNAL( data_changed() ), this, SLOT( handle_project_changed() ) );

  this->viewer_ = QSharedPointer<Viewer>( new Viewer() );
  this->groom_tool_ = QSharedPointer<GroomTool>( new GroomTool() );
  this->ui_->stackedWidget->addWidget( this->groom_tool_.data() );

  this->groom_tool_->set_project( this->project_ );


  QStringList files;

/*
   files << "z:/shared/laatee/laa_0_DT.nrrd";
   files << "z:/shared/laatee/laa_1_DT.nrrd";
   files << "z:/shared/laatee/laa_2_DT.nrrd";
   files << "z:/shared/laatee/laa_3_DT.nrrd";
   files << "z:/shared/laatee/laa_4_DT.nrrd";
 */

/*
   files << "h:/projects/laa_tee/groomed/interface_0_DT.nrrd";
   files << "h:/projects/laa_tee/groomed/interface_1_DT.nrrd";
   files << "h:/projects/laa_tee/groomed/interface_2_DT.nrrd";
 */
  files << "h:/projects/laa_tee/data/interface_0.nrrd";
  files << "h:/projects/laa_tee/data/interface_1.nrrd";
  files << "h:/projects/laa_tee/data/interface_2.nrrd";
  files << "h:/projects/laa_tee/data/interface_3.nrrd";
  files << "h:/projects/laa_tee/data/interface_4.nrrd";
  files << "h:/projects/laa_tee/data/interface_5.nrrd";

  this->ui_->view_mode_combobox->setItemData( 1, 0, Qt::UserRole - 1 );
  this->ui_->view_mode_combobox->setItemData( 2, 0, Qt::UserRole - 1 );

  this->import_files( files );
}

ShapeWorksStudioApp::~ShapeWorksStudioApp()
{}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionOpenProject_triggered()
{
  QString filename = QFileDialog::getOpenFileName( this, tr( "Open Project..." ),
                                                   QString(), tr( "XML files (*.xml)" ) );
  if ( filename.isEmpty() )
  {
    return;
  }

  this->project_->open_project( filename );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionSaveProjectAs_triggered()
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
void ShapeWorksStudioApp::on_actionQuit_triggered()
{
  this->close();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionImport_triggered()
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

  this->viewer_->set_tile_layout( value, value );

  this->update_scrollbar();

  this->ui_->qvtkWidget->GetRenderWindow()->Render();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::update_scrollbar()
{
  int num_rows = this->viewer_->get_num_rows();
  int num_visible = this->viewer_->get_num_rows_visible();
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
  this->viewer_->set_start_row( value );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_addButton_clicked()
{
  this->on_actionImport_triggered();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_deleteButton_clicked()
{

  QModelIndexList list = this->ui_->tableWidget->selectionModel()->selectedRows();

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

  std::vector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();

  this->ui_->tableWidget->clear();

  this->ui_->tableWidget->setRowCount( shapes.size() );
  this->ui_->tableWidget->setColumnCount( 3 );

  QStringList table_header;
  table_header << "#" << "Name" << "Size";
  this->ui_->tableWidget->setHorizontalHeaderLabels( table_header );

  this->ui_->tableWidget->verticalHeader()->setVisible( false );

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QSharedPointer<Mesh> initial_mesh = shapes[i]->get_initial_mesh();

    QTableWidgetItem* new_item = new QTableWidgetItem( QString::number( i + 1 ) );
    this->ui_->tableWidget->setItem( i, 0, new_item );

    new_item = new QTableWidgetItem( initial_mesh->get_filename() );
    this->ui_->tableWidget->setItem( i, 1, new_item );

    new_item = new QTableWidgetItem( initial_mesh->get_dimension_string() );
    this->ui_->tableWidget->setItem( i, 2, new_item );
  }

  this->ui_->tableWidget->resizeColumnsToContents();
  //this->ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  this->ui_->tableWidget->horizontalHeader()->setStretchLastSection( true );

  this->ui_->tableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::mode_changed()
{}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionGroomMode_triggered()
{
  std::cerr << "groom!\n";
  //this->ui->stackedWidget->setCurrentIndex( 1 );
  this->ui_->stackedWidget->setCurrentWidget( this->groom_tool_.data() );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_actionImportMode_triggered()
{
  this->ui_->stackedWidget->setCurrentIndex( 0 );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::handle_project_changed()
{
  std::vector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();

  if ( this->project_->get_pipeline_state() == Project::GROOMED_C )
  {
    this->ui_->view_mode_combobox->setItemData( 1, 33, Qt::UserRole - 1 );
  }

  this->update_meshes();
  this->update_table();
  this->update_scrollbar();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_center_checkbox_stateChanged()
{
  this->viewer_->set_auto_center( this->ui_->center_checkbox->isChecked() );
  this->handle_project_changed();
}

//---------------------------------------------------------------------------
<<<<<<< HEAD
void ShapeWorksStudioApp::update_meshes()
{

  QString mode = this->ui_->view_mode_combobox->currentText();

  std::vector<QSharedPointer<Shape> > shapes = this->project_->get_shapes();
  std::vector<QSharedPointer<Mesh> > meshes;

  for ( int i = 0; i < shapes.size(); i++ )
  {
    if ( mode == "Original" )
    {
      meshes.push_back( shapes[i]->get_initial_mesh() );
    }
    else if ( mode == "Groomed" )
    {
      meshes.push_back( shapes[i]->get_groomed_mesh() );
    }
  }

  this->viewer_->set_meshes( meshes );
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::on_view_mode_combobox_currentIndexChanged()
{
  this->update_meshes();
}

//---------------------------------------------------------------------------
void ShapeWorksStudioApp::init_vtk()
{
  this->viewer_->set_render_window( this->ui_->qvtkWidget->GetRenderWindow() );
}
