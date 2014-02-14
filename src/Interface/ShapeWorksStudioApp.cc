#include <iostream>

#include <Interface/ShapeWorksStudioApp.h>
#include <ui_ShapeWorksStudioApp.h>

ShapeWorksStudioApp::ShapeWorksStudioApp( int argc, char** argv )
{
  this->ui = new Ui_ShapeWorksStudioApp;
  this->ui->setupUi( this );

}

ShapeWorksStudioApp::~ShapeWorksStudioApp()
{}

void ShapeWorksStudioApp::on_actionQuit_triggered()
{
  this->close();
}

