#ifndef STUDIO_DATA_MESH_H
#define STUDIO_DATA_MESH_H

#include <QString>
#include <QSharedPointer>

#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkImageImport.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImage.h>

#include <vnl/vnl_vector.h>

typedef float PixelType;
typedef itk::Image< PixelType, 3 > ImageType;
typedef itk::ImageFileReader< ImageType > ReaderType;
typedef itk::ImageFileWriter< ImageType > WriterType;

class Mesh;
typedef QSharedPointer< Mesh > MeshHandle;
//! Representation of a single mesh.
/*!
 * The Mesh class represents a single mesh generated from an image file.
 * It is responsible for loading the image and generating a mesh from it.
 *
 */
class Mesh
{
public:

  /// Constructor
  Mesh();

  /// Destructor
  ~Mesh();

  /// Create a mesh from an image
  ImageType::Pointer create_from_file(QString filename, double iso_value);
  void create_from_image(ImageType::Pointer img, double iso_value);
  
  /// Get the dimensions as a string for display (if loaded from an image)
  QString get_dimension_string();

  /// Get the mesh polydata
  vtkSmartPointer<vtkPolyData> get_poly_data();

  /// Get the center transform
  vnl_vector<double> get_center_transform();

  void set_poly_data(vtkSmartPointer<vtkPolyData> poly_data);

private:

  vtkSmartPointer<vtkPolyData> poly_data_;

  int dimensions_[3];

  vnl_vector<double> center_transform_;
};

#endif /* STUDIO_DATA_MESH_H */
