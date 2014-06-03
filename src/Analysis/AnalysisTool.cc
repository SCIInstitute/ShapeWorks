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
void AnalysisTool::set_shapes( ShapeList shapes )
{
  this->shapes_ = shapes;
}

//---------------------------------------------------------------------------
void AnalysisTool::update_shape_stats()
{

  this->set_shapes( this->project_->get_shapes() );

  std::vector < vnl_vector < double >> points;
  foreach( ShapeHandle shape, this->shapes_ ) {
    points.push_back( shape->get_correspondence_points() );
  }

  this->stats.ImportPoints( points );

  this->stats.ComputeModes();

  vnl_vector<double> mean_shape = this->stats.Mean();

  MeshHandle mesh = MeshHandle( new Mesh() );
  mesh->create_from_pointset( mean_shape );

  DisplayObjectHandle object = DisplayObjectHandle( new DisplayObject() );

  object->set_mesh( mesh );
  object->set_correspondence_points( mean_shape );

  QStringList annotations;
  annotations << "mean";
  annotations << "";
  annotations << "";
  annotations << "";
  object->set_annotations( annotations );

  QVector<DisplayObjectHandle> list;
  list << object;
  this->lightbox_->set_display_objects(list);

}

//---------------------------------------------------------------------------
void AnalysisTool::on_mean_overall_clicked()
{
  std::cerr << "mean overall!\n";
  this->update_shape_stats();
}

//---------------------------------------------------------------------------
void AnalysisTool::set_lightbox(QSharedPointer<Lightbox> lightbox)
{
  this->lightbox_ = lightbox;
}
