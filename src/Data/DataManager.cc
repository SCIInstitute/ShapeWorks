#include <QtGui>


#include <vtkMarchingCubes.h>

#include <itkImageToVTKImageFilter.h>
#include <itkImageFileReader.h>

#include <Data/DataManager.h>
#include <Visualization/Viewer.h>

typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;

DataManager::DataManager()
{}

DataManager::~DataManager()
{}

void DataManager::import_file( std::string file )
{

  Shape new_shape;
  new_shape.import_initial_file( file );
  this->shapes_.push_back( new_shape );
}

std::vector<vtkSmartPointer<vtkPolyData> > DataManager::get_meshes()
{
  return this->meshes_;
}

void DataManager::set_table_widget( QTableWidget* table_widget )
{
  this->table_widget_ = table_widget;
}

void DataManager::set_viewer( Viewer* viewer )
{
  this->viewer_ = viewer;
}

void DataManager::import_files( QStringList file_names )
{
  int oldCount = this->shapes_.size();

  for ( int i = 0; i < file_names.size(); i++ )
  {
    std::cerr << file_names[i].toStdString() << "\n";

    this->import_file( file_names[i].toStdString() );
  }

  for ( int i = oldCount; i < this->shapes_.size(); i++ )
  {
    this->viewer_->add_input( this->shapes_[i].get_initial_mesh().get_poly_data() );
  }

  this->update_table();
}

void DataManager::update_table()
{
  this->table_widget_->clear();

  this->table_widget_->setRowCount( this->shapes_.size() );
  this->table_widget_->setColumnCount( 3 );

  QStringList table_header;
  table_header << "#" << "Name" << "Size";
  this->table_widget_->setHorizontalHeaderLabels( table_header );

  this->table_widget_->verticalHeader()->setVisible( false );

  for ( int i = 0; i < this->shapes_.size(); i++ )
  {
    Mesh initial_mesh = this->shapes_[i].get_initial_mesh();

    QString name = QString::fromStdString( initial_mesh.get_filename() );
    QFileInfo qfi( name );

    QTableWidgetItem* new_item = new QTableWidgetItem( QString::number( i ) );
    this->table_widget_->setItem( i, 0, new_item );

    new_item = new QTableWidgetItem( qfi.fileName() );
    this->table_widget_->setItem( i, 1, new_item );

    new_item = new QTableWidgetItem( QString::fromStdString( initial_mesh.get_dimension_string() ) );
    this->table_widget_->setItem( i, 2, new_item );
  }

  this->table_widget_->resizeColumnsToContents();
  //this->table_widget_->horizontalHeader()->setResizeMode(QHeaderView::Stretch);
  this->table_widget_->horizontalHeader()->setStretchLastSection( true );

  this->table_widget_->setSelectionBehavior( QAbstractItemView::SelectRows );
}
