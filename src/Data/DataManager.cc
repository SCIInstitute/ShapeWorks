#include <QtGui>

#include <Data/DataManager.h>
#include <Visualization/Viewer.h>

DataManager::DataManager()
{}

DataManager::~DataManager()
{}

void DataManager::import_file( QString file )
{

  QSharedPointer<Shape> new_shape = QSharedPointer<Shape>( new Shape );
  new_shape->import_initial_file( file );
  this->shapes_.push_back( new_shape );
}

void DataManager::set_table_widget( QTableWidget* table_widget )
{
  this->table_widget_ = table_widget;
}

void DataManager::set_viewer( QSharedPointer<Viewer> viewer )
{
  this->viewer_ = viewer;
}

void DataManager::import_files( QStringList file_names )
{

  for ( int i = 0; i < file_names.size(); i++ )
  {
    std::cerr << file_names[i].toStdString() << "\n";

    this->import_file( file_names[i] );
  }
}

std::vector<QSharedPointer<Shape> > DataManager::get_shapes()
{
  return this->shapes_;
}

void DataManager::remove_shape( int i )
{
  this->shapes_.erase( this->shapes_.begin() + i );
}

void DataManager::update_shapes()
{
  std::vector<QSharedPointer<Mesh> > meshes;

  for ( int i = 0; i < this->shapes_.size(); i++ )
  {
    meshes.push_back( this->shapes_[i]->get_initial_mesh() );
  }

  this->viewer_->set_meshes( meshes );
}
