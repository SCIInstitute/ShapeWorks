#include <iostream>

#include <QFileDialog>

#include <Interface/ShapeWorksStudioApp.h>
#include <ui_ShapeWorksStudioApp.h>

#include <vtkRenderWindow.h>

#include <Visualization/Viewer.h>
#include <Data/DataManager.h>

ShapeWorksStudioApp::ShapeWorksStudioApp( int argc, char** argv )
{
  this->ui = new Ui_ShapeWorksStudioApp;
  this->ui->setupUi( this );

  // set to import
  this->ui->actionImportMode->setChecked( true );
  this->ui->stackedWidget->setCurrentIndex( 0 );
  this->ui->toolListWidget->setCurrentRow(0);

  this->action_group_ = new QActionGroup( this );
  this->action_group_->addAction( this->ui->actionImportMode );
  this->action_group_->addAction( this->ui->actionGroomMode );
  this->action_group_->addAction( this->ui->actionActionOptimizeMode );
  this->action_group_->addAction( this->ui->actionAnalysisMode );

  this->ui->statusbar->showMessage( "ShapeWorksStudio" );

  this->viewer_ = QSharedPointer<Viewer>( new Viewer() );
  this->data_manager_ = QSharedPointer<DataManager>( new DataManager() );

  this->data_manager_->set_table_widget( this->ui->tableWidget );
  this->data_manager_->set_viewer( this->viewer_ );

  this->viewer_->set_render_window( this->ui->qvtkWidget->GetRenderWindow() );

/*
  QStringList files;
  files << "z:\\shared\\laatee\\laa_0_DT.nrrd";
  files << "z:\\shared\\laatee\\laa_1_DT.nrrd";
  files << "z:\\shared\\laatee\\laa_2_DT.nrrd";
  files << "z:\\shared\\laatee\\laa_3_DT.nrrd";
  files << "z:\\shared\\laatee\\laa_4_DT.nrrd";
  this->import_files( files );
*/
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
  int value = this->ui->thumbnail_size_slider->maximum() - this->ui->thumbnail_size_slider->value() + 1;

  this->viewer_->set_tile_layout( value, value );

  this->update_scrollbar();

  this->ui->qvtkWidget->GetRenderWindow()->Render();
}

void ShapeWorksStudioApp::update_scrollbar()
{
  int num_rows = this->viewer_->get_num_rows();
  int num_visible = this->viewer_->get_num_rows_visible();
  std::cerr << "num_rows = " << num_rows << "\n";
  std::cerr << "num_visible = " << num_visible << "\n";
  if ( num_visible >= num_rows )
  {
    this->ui->vertical_scroll_bar->setMaximum( 0 );
    this->ui->vertical_scroll_bar->setEnabled( false );
  }
  else
  {
    this->ui->vertical_scroll_bar->setEnabled( true );
    this->ui->vertical_scroll_bar->setMaximum( num_rows - num_visible );
    this->ui->vertical_scroll_bar->setPageStep( num_visible );
  }
}

void ShapeWorksStudioApp::on_vertical_scroll_bar_valueChanged()
{
  int value = this->ui->vertical_scroll_bar->value();

  std::cerr << "vertical scrollbar value = " << value << "\n";
  this->viewer_->set_start_row( value );
}

void ShapeWorksStudioApp::on_addButton_clicked()
{
  this->on_actionImport_triggered();
}

void ShapeWorksStudioApp::on_deleteButton_clicked()
{

  QModelIndexList list = this->ui->tableWidget->selectionModel()->selectedRows();

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

  this->ui->tableWidget->clear();

  this->ui->tableWidget->setRowCount( shapes.size() );
  this->ui->tableWidget->setColumnCount( 3 );

  QStringList table_header;
  table_header << "#" << "Name" << "Size";
  this->ui->tableWidget->setHorizontalHeaderLabels( table_header );

  this->ui->tableWidget->verticalHeader()->setVisible( false );

  for ( int i = 0; i < shapes.size(); i++ )
  {
    QSharedPointer<Mesh> initial_mesh = shapes[i]->get_initial_mesh();

    QString name = QString::fromStdString( initial_mesh->get_filename() );
    QFileInfo qfi( name );

    QTableWidgetItem* new_item = new QTableWidgetItem( QString::number( i + 1 ) );
    this->ui->tableWidget->setItem( i, 0, new_item );

    new_item = new QTableWidgetItem( qfi.fileName() );
    this->ui->tableWidget->setItem( i, 1, new_item );

    new_item = new QTableWidgetItem( QString::fromStdString( initial_mesh->get_dimension_string() ) );
    this->ui->tableWidget->setItem( i, 2, new_item );
  }

  this->ui->tableWidget->resizeColumnsToContents();
  //this->ui->tableWidget->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  this->ui->tableWidget->horizontalHeader()->setStretchLastSection( true );

  this->ui->tableWidget->setSelectionBehavior( QAbstractItemView::SelectRows );
}

void ShapeWorksStudioApp::mode_changed()
{}

void ShapeWorksStudioApp::on_actionGroomMode_triggered()
{
  std::cerr << "groom!\n";

  //this->ui->stackedWidget->setCurrentIndex( 1 );
}

void ShapeWorksStudioApp::on_actionImportMode_triggered()
{
  std::cerr << "import!\n";

  //this->ui->stackedWidget->setCurrentIndex( 0 );
}

void ShapeWorksStudioApp::on_toolListWidget_currentRowChanged( int row )
{
  std::cerr << "row: " << row << "\n";
  if ( row < 3 )
  {
    this->ui->toolStackedWidget->setCurrentIndex( row );
  }
}

void ShapeWorksStudioApp::on_addToolButton_clicked()
{

  QListWidgetItem* selected_tool = this->ui->toolListWidget->currentItem();
  QString tool_string = selected_tool->text();

  if (selected_tool->text() == "Antialias")
  {
    tool_string = tool_string + " (iterations: " + QString::number(this->ui->antialiasIterations->value()) + ")";
  }

  if (selected_tool->text() == "Blur")
  {
    tool_string = tool_string + " (sigma: " + QString::number(this->ui->blurSigma->value()) + ")";
  }



  QListWidgetItem* item = new QListWidgetItem( tool_string );

  this->ui->groomPipelineListWidget->addItem( item );
}

void ShapeWorksStudioApp::on_deleteToolButton_clicked()
{
  //this->ui->groomPipelineListWidget->removeItemWidget(this->ui->groomPipelineListWidget->selectedItems());

  qDeleteAll(this->ui->groomPipelineListWidget->selectedItems());
}
