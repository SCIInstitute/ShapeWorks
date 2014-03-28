#include <iostream>

#include <Groom/GroomTool.h>

#include <ui_GroomTool.h>

GroomTool::GroomTool()
{

  this->ui_ = new Ui_GroomTool;
  this->ui_->setupUi( this );
}

GroomTool::~GroomTool()
{}

void GroomTool::on_antialias_checkbox_stateChanged( int state )
{
  this->ui_->antialias_groupbox->setEnabled( state );
}

void GroomTool::on_blur_checkbox_stateChanged( int state )
{
  this->ui_->blur_groupbox->setEnabled( state );
}
