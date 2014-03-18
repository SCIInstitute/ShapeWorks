#include <iostream>

#include <QFileDialog>

#include <Interface/ShapeWorksStudioApp.h>
#include <ui_ShapeWorksStudioApp.h>

#include <vtkDataObjectToTable.h>
#include <vtkElevationFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkQtTableView.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkVectorText.h>

#include <itkImageToVTKImageFilter.h>

#include "vtkSmartPointer.h"
#define VTK_CREATE( type, name ) \
  vtkSmartPointer<type> name = vtkSmartPointer<type>::New()

ShapeWorksStudioApp::ShapeWorksStudioApp( int argc, char** argv )
{
  this->ui = new Ui_ShapeWorksStudioApp;
  this->ui->setupUi( this );

  // Geometry
  VTK_CREATE( vtkVectorText, text );
  text->SetText( "VTK and Qt!" );
  VTK_CREATE( vtkElevationFilter, elevation );
  elevation->SetInputConnection( text->GetOutputPort() );
  elevation->SetLowPoint( 0, 0, 0 );
  elevation->SetHighPoint( 10, 0, 0 );

  // Mapper
  VTK_CREATE( vtkPolyDataMapper, mapper );
  mapper->ImmediateModeRenderingOn();
  mapper->SetInputConnection( elevation->GetOutputPort() );

  // Actor in scene
  VTK_CREATE( vtkActor, actor );
  actor->SetMapper( mapper );

  // VTK Renderer
  //VTK_CREATE( vtkRenderer, ren );

  this->viewer = new Viewer();
  this->dataManager = new DataManager();

  // Add Actor to renderer
  //ren->AddActor( actor );

  // VTK/Qt wedded
  //this->ui->qvtkWidget->GetRenderWindow()->AddRenderer( ren );
  //

  this->viewer->set_render_window( this->ui->qvtkWidget->GetRenderWindow() );

  //this->ui->qvtkWidget->GetRenderWindow()->AddRenderer( this->viewer->getRenderer() );
  // this->viewer->setInteractor( this->ui->qvtkWidget->GetRenderWindow()->GetInteractor() );
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

  int oldCount = this->dataManager->getMeshes().size();

  for ( int i = 0; i < fileNames.size(); i++ )
  {
    std::cerr << fileNames[i].toStdString() << "\n";

    this->dataManager->importFile( fileNames[i].toStdString() );
  }

  std::vector<vtkSmartPointer<vtkPolyData> > meshes = this->dataManager->getMeshes();

  for ( int i = oldCount; i < meshes.size(); i++ )
  {
    this->viewer->add_input( meshes[i] );
  }
}
