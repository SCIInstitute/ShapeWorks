#include <iostream>

#include <QFileDialog>

#include <Interface/ShapeWorksStudioApp.h>
#include <ui_ShapeWorksStudioApp.h>

#include <vtkRenderWindow.h>

ShapeWorksStudioApp::ShapeWorksStudioApp( int argc, char** argv )
{
  this->ui = new Ui_ShapeWorksStudioApp;
  this->ui->setupUi( this );

  this->ui->actionImportMode->setChecked( true );

  this->action_group_ = new QActionGroup( this );
  this->action_group_->addAction( this->ui->actionImportMode );
  this->action_group_->addAction( this->ui->actionGroomMode );
  this->action_group_->addAction( this->ui->actionActionOptimizeMode );
  this->action_group_->addAction( this->ui->actionAnalysisMode );

  this->ui->statusbar->showMessage( "All Systems Nominal!" );

  this->viewer_ = new Viewer();
  this->data_manager_ = new DataManager();

  this->data_manager_->set_table_widget( this->ui->tableWidget );
  this->data_manager_->set_viewer( this->viewer_ );

  this->viewer_->set_render_window( this->ui->qvtkWidget->GetRenderWindow() );

  QStringList files;
  files << "z:\\shared\\laatee\\laa_0_DT.nrrd";
  files << "z:\\shared\\laatee\\laa_1_DT.nrrd";
  files << "z:\\shared\\laatee\\laa_2_DT.nrrd";
  files << "z:\\shared\\laatee\\laa_3_DT.nrrd";
  files << "z:\\shared\\laatee\\laa_4_DT.nrrd";
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
