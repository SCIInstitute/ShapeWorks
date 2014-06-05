#include <iostream>

#include <QXmlStreamWriter>
#include <QTemporaryFile>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

#include <Application/ShapeWorksStudioApp.h>
#include <Application/Preferences.h>

#include <Data/Project.h>
#include <Data/Mesh.h>
#include <Data/Shape.h>

#include <Analysis/AnalysisTool.h>

#include <Analysis/itkParticleShapeStatistics.h>

#include <Visualization/Lightbox.h>
#include <Visualization/DisplayObject.h>

#include <ui_AnalysisTool.h>

//---------------------------------------------------------------------------
AnalysisTool::AnalysisTool()
{

  this->ui_ = new Ui_AnalysisTool;
  this->ui_->setupUi( this );
  this->update_from_preferences();
}

//---------------------------------------------------------------------------
AnalysisTool::~AnalysisTool()
{}

//---------------------------------------------------------------------------
void AnalysisTool::set_project( QSharedPointer<Project> project )
{
  this->project_ = project;
}

//---------------------------------------------------------------------------
void AnalysisTool::set_app( ShapeWorksStudioApp* app )
{
  this->app_ = app;
}

//---------------------------------------------------------------------------
void AnalysisTool::on_mean_overall_clicked()
{
  this->visualizer_->display_mean();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_show_surface_stateChanged()
{
  this->visualizer_->set_show_surface(this->ui_->show_surface->isChecked());
}

//---------------------------------------------------------------------------
void AnalysisTool::on_show_glyphs_stateChanged()
{
  this->visualizer_->set_show_glyphs(this->ui_->show_glyphs->isChecked());
}

//---------------------------------------------------------------------------
void AnalysisTool::on_glyph_size_valueChanged( int value )
{
  Preferences::Instance().set_glyph_size( value / 10.0 );
  this->update_from_preferences();
}

//---------------------------------------------------------------------------
void AnalysisTool::on_glyph_quality_valueChanged( int value )
{
  Preferences::Instance().set_glyph_quality( value );
  this->update_from_preferences();
}

//---------------------------------------------------------------------------
void AnalysisTool::set_visualizer( VisualizerHandle visualizer )
{
  this->visualizer_ = visualizer;
}

//---------------------------------------------------------------------------
void AnalysisTool::update_from_preferences()
{
  this->ui_->glyph_quality->setValue(Preferences::Instance().get_glyph_quality());
  this->ui_->glyph_quality_label->setText( QString::number( Preferences::Instance().get_glyph_quality() ) );
  this->ui_->glyph_size->setValue(Preferences::Instance().get_glyph_size() * 10.0);
  this->ui_->glyph_size_label->setText( QString::number( Preferences::Instance().get_glyph_size() ) );
}
