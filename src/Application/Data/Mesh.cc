#include <QMessageBox>
#include <QTextStream>

#include <vtkTriangleFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyDataWriter.h>

#include <itkImageRegionIteratorWithIndex.h>
#include <itkVTKImageExport.h>

#include <Data/Mesh.h>
#include <Data/ItkToVtk.h>

#include <vtkSurfaceReconstructionFilter.h>
#include "itkNRRDImageIOFactory.h"
#include "itkMetaImageIOFactory.h"
#include <vtkMarchingCubes.h>

//---------------------------------------------------------------------------
Mesh::Mesh()
{}

//---------------------------------------------------------------------------
Mesh::~Mesh()
{}

void Mesh::set_poly_data(vtkSmartPointer<vtkPolyData> poly_data) {
	this->poly_data_ = poly_data;
}

//---------------------------------------------------------------------------
QString Mesh::get_dimension_string()
{
  QString str = "[" + QString::number( this->dimensions_[0] ) +
                ", " + QString::number( this->dimensions_[1] ) +
                ", " + QString::number( this->dimensions_[2] ) + "]";
  return str;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Mesh::get_poly_data()
{
  return this->poly_data_;
}

//---------------------------------------------------------------------------
ImageType::Pointer Mesh::create_from_file(QString filename, double iso_value)
{
  auto fname = filename.toStdString();
  if (fname.find(".nrrd") != std::string::npos) {
    itk::NrrdImageIOFactory::RegisterOneFactory();
  } else if (fname.find(".mha") != std::string::npos) {
    itk::MetaImageIOFactory::RegisterOneFactory();
  }

  // read file using ITK
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename.toStdString());
  reader->Update();
  ImageType::Pointer image = reader->GetOutput();
  this->create_from_image(image, iso_value);
  return image;
}

//---------------------------------------------------------------------------
void Mesh::create_from_image(ImageType::Pointer image, double iso_value)
{
  try {
    // get image dimensions
    ImageType::RegionType region = image->GetLargestPossibleRegion();
    ImageType::SizeType size = region.GetSize();
    this->dimensions_[0] = size[0];
    this->dimensions_[1] = size[1];
    this->dimensions_[2] = size[2];

    // find the center of mass
    itk::Array<double> params(3);
    params.Fill(0.0);
    double count = 0.0;
    itk::Point<double, 3> point;
    itk::ImageRegionIteratorWithIndex<ImageType> oit(image, image->GetLargestPossibleRegion());
    for (oit.GoToBegin(); !oit.IsAtEnd(); ++oit) {
      if (oit.Get() > 0) {
        // Get the physical index from the image index.
        image->TransformIndexToPhysicalPoint(oit.GetIndex(), point);
        for (unsigned int i = 0; i < 3; i++) {
          params[i] += point[i];
        }
        count += 1.0;
      }
    }

    // compute center of mass
    this->center_transform_.set_size(3);
    for (unsigned int i = 0; i < 3; i++)  {
      this->center_transform_[i] = params[i] / count;
    }
    // connect to VTK
    vtkSmartPointer<vtkImageImport> vtk_image = vtkSmartPointer<vtkImageImport>::New();
    itk::VTKImageExport<ImageType>::Pointer itk_exporter = itk::VTKImageExport<ImageType>::New();
    itk_exporter->SetInput(image);
    ConnectPipelines(itk_exporter, vtk_image.GetPointer());
    vtk_image->Update();

    // create isosurface
    auto marching = vtkSmartPointer<vtkMarchingCubes>::New();
    marching->SetInputConnection(vtk_image->GetOutputPort());
    marching->SetNumberOfContours(1);
    marching->SetValue(0, iso_value);
    marching->Update();

    // store isosurface polydata
    this->poly_data_ = marching->GetOutput();

  } catch (itk::ExceptionObject & excep) {
    std::cerr << "Exception caught!" << std::endl;
    std::cerr << excep << std::endl;
  }
}

//---------------------------------------------------------------------------
vnl_vector<double> Mesh::get_center_transform()
{
  return this->center_transform_;
}