#include <iostream>

#include <QFileDialog>

#include <Interface/ShapeWorksStudioApp.h>
#include <ui_ShapeWorksStudioApp.h>

#include <vtkRenderWindow.h>

#include <Visualization/Viewer.h>
#include <Data/DataManager.h>
#include <Groom/GroomTool.h>

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

  this->viewer_ = QSharedPointer<Viewer>( new Viewer() );
  this->data_manager_ = QSharedPointer<DataManager>( new DataManager() );
  this->groom_tool_ = QSharedPointer<GroomTool>( new GroomTool() );
  this->ui_->stackedWidget->addWidget( this->groom_tool_.data() );

  this->data_manager_->set_table_widget( this->ui_->tableWidget );
  this->data_manager_->set_viewer( this->viewer_ );
  this->groom_tool_->set_data_manager( this->data_manager_ );

  this->viewer_->set_render_window( this->ui_->qvtkWidget->GetRenderWindow() );

  QStringList files;
  files << "z:/shared/laatee/laa_0_DT.nrrd";
  files << "z:/shared/laatee/laa_1_DT.nrrd";
  files << "z:/shared/laatee/laa_2_DT.nrrd";
  files << "z:/shared/laatee/laa_3_DT.nrrd";
  files << "z:/shared/laatee/laa_4_DT.nrrd";
  this->import_files( files );
}

ShapeWorksStudioApp::~ShapeWorksStudioApp()
{}

void ShapeWorksStudioApp::on_actionQuit_triggered()
{
  this->close();
}

void ShapeWorksStudioApp::on_actionImport_triggered()
{
  QFileDialog dialog( this );
  //dialog.setDirectory(QDir::homePath());
  dialog.setFileMode( QFileDialog::ExistingFiles );
  dialog.setNameFilter( trUtf8( "NRRD (*.nrrd)" ) );
  QStringList fileNames;
  if ( dialog.exec() )
  {
    fileNames = dialog.selectedFiles();
  }

  this->import_files( fileNames );
}

void ShapeWorksStudioApp::import_files( QStringList file_names )
{
  this->data_manager_->import_files( file_names );
  this->data_manager_->update_shapes();
  this->update_table();
  this->update_scrollbar();
}

void ShapeWorksStudioApp::on_thumbnail_size_slider_valueChanged()
{
  int value = this->ui_->thumbnail_size_slider->maximum() - this->ui_->thumbnail_size_slider->value() + 1;

  this->viewer_->set_tile_layout( value, value );

  this->update_scrollbar();

  this->ui_->qvtkWidget->GetRenderWindow()->Render();
}

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

void ShapeWorksStudioApp::on_vertical_scroll_bar_valueChanged()
{
  int value = this->ui_->vertical_scroll_bar->value();

  std::cerr << "vertical scrollbar value = " << value << "\n";
  this->viewer_->set_start_row( value );
}

void ShapeWorksStudioApp::on_addButton_clicked()
{
  this->on_actionImport_triggered();
}

void ShapeWorksStudioApp::on_deleteButton_clicked()
{

  QModelIndexList list = this->ui_->tableWidget->selectionModel()->selectedRows();

  for ( int i = list.size() - 1; i >= 0; i-- )
  {
    std::cerr << list[i].row() << "\n";
    this->data_manager_->remove_shape( list[i].row() );
  }

  this->data_manager_->update_shapes();
  this->update_table();
  this->update_scrollbar();
}

void ShapeWorksStudioApp::update_table()
{

  std::vector<QSharedPointer<Shape> > shapes = this->data_manager_->get_shapes();

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

void ShapeWorksStudioApp::mode_changed()
{}

void ShapeWorksStudioApp::on_actionGroomMode_triggered()
{
  std::cerr << "groom!\n";
  //this->ui->stackedWidget->setCurrentIndex( 1 );
  this->ui_->stackedWidget->setCurrentWidget( this->groom_tool_.data() );
}

void ShapeWorksStudioApp::on_actionImportMode_triggered()
{
  this->ui_->stackedWidget->setCurrentIndex( 0 );
}
