#include <iostream>

#include <Interface/ShapeWorksStudioApp.h>
#include <ui_ShapeWorksStudioApp.h>
                                                                               
#include <vtkDataObjectToTable.h>                                                     
#include <vtkElevationFilter.h>                                                       
#include <vtkPolyDataMapper.h>                                                        
#include <vtkQtTableView.h>                                                           
#include <vtkRenderer.h>                                                              
#include <vtkRenderWindow.h>                                                          
#include <vtkVectorText.h>                                                            
                                                                                      
#include "vtkSmartPointer.h"                                                          
#define VTK_CREATE(type, name) \
vtkSmartPointer<type> name = vtkSmartPointer<type>::New()                           
                                      

ShapeWorksStudioApp::ShapeWorksStudioApp( int argc, char** argv )
{
  this->ui = new Ui_ShapeWorksStudioApp;
  this->ui->setupUi( this );


  // Qt Table View                                                                    
  this->TableView = vtkSmartPointer<vtkQtTableView>::New();                           
                                                                                      
  // Place the table view in the designer form                                        
  this->ui->tableFrame->layout()->addWidget(this->TableView->GetWidget());            
                                                                                      
  // Geometry                                                                         
  VTK_CREATE(vtkVectorText, text);                                                    
  text->SetText("VTK and Qt!");                                                       
  VTK_CREATE(vtkElevationFilter, elevation);                                          
  elevation->SetInputConnection(text->GetOutputPort());                               
  elevation->SetLowPoint(0,0,0);                                                      
  elevation->SetHighPoint(10,0,0);                                                    
                                                                                      
  // Mapper                                                                           
  VTK_CREATE(vtkPolyDataMapper, mapper);                                              
  mapper->ImmediateModeRenderingOn();                                                 
  mapper->SetInputConnection(elevation->GetOutputPort());                             
                                                                                      
  // Actor in scene                                                                   
  VTK_CREATE(vtkActor, actor);                                                        
  actor->SetMapper(mapper);                                                           
                                                                                      
  // VTK Renderer                                                                     
  VTK_CREATE(vtkRenderer, ren);                                                       
                                                                                      
  // Add Actor to renderer                                                            
  ren->AddActor(actor);                                                               
                                                                                      
  // VTK/Qt wedded                                                                    
  this->ui->qvtkWidget->GetRenderWindow()->AddRenderer(ren);                          
                                                                                      
  // Just a bit of Qt interest: Culling off the                                       
  // point data and handing it to a vtkQtTableView                                    
  VTK_CREATE(vtkDataObjectToTable, toTable);                                          
  toTable->SetInputConnection(elevation->GetOutputPort());                            
  toTable->SetFieldType(vtkDataObjectToTable::POINT_DATA);                            
                                                                                      
  // Here we take the end of the VTK pipeline and give it to a Qt View                
  this->TableView->SetRepresentationFromInputConnection(toTable->GetOutputPort());    
                        
  // Set up action signals and slots                                                  
  //connect(this->ui->actionOpenFile, SIGNAL(triggered()), this, SLOT(slotOpenFile())); 
  //connect(this->ui->actionExit, SIGNAL(triggered()), this, SLOT(slotExit()));         
            

}

ShapeWorksStudioApp::~ShapeWorksStudioApp()
{}

void ShapeWorksStudioApp::on_actionQuit_triggered()
{
  this->close();
}

