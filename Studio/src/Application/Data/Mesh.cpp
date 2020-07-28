#include <QMessageBox>
#include <QTextStream>

#include <itkNrrdImageIOFactory.h>
#include <itkMetaImageIOFactory.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkVTKImageExport.h>
#include <itkOrientImageFilter.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkLinearInterpolateImageFunction.h>

#include <vtkSurfaceReconstructionFilter.h>
#include <vtkMarchingCubes.h>
#include <vtkTriangleFilter.h>
#include <vtkPolyDataNormals.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkPolyDataWriter.h>

#include <Data/Mesh.h>
#include <Data/ItkToVtk.h>

using NearestNeighborInterpolatorType = itk::NearestNeighborInterpolateImageFunction<ImageType, double>;
using LinearInterpolatorType = itk::LinearInterpolateImageFunction<ImageType, double>;

//---------------------------------------------------------------------------
Mesh::Mesh()
{}

//---------------------------------------------------------------------------
Mesh::~Mesh()
{}

//---------------------------------------------------------------------------
void Mesh::set_poly_data(vtkSmartPointer<vtkPolyData> poly_data)
{
  this->poly_data_ = poly_data;
}

//---------------------------------------------------------------------------
void Mesh::set_error_message(std::string error_message)
{
  this->error_message_ = error_message;
}

//---------------------------------------------------------------------------
std::string Mesh::get_error_message()
{
  return this->error_message_;
}

//---------------------------------------------------------------------------
QString Mesh::get_dimension_string()
{
  QString str = "[" + QString::number(this->dimensions_[0]) +
                ", " + QString::number(this->dimensions_[1]) +
                ", " + QString::number(this->dimensions_[2]) + "]";
  return str;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> Mesh::get_poly_data()
{
  return this->poly_data_;
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
    for (unsigned int i = 0; i < 3; i++) {
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
  } catch (itk::ExceptionObject& excep) {
    std::cerr << "3Exception caught!" << std::endl;
    std::cerr << excep << std::endl;
  }
}

//---------------------------------------------------------------------------
vnl_vector<double> Mesh::get_center_transform()
{
  return this->center_transform_;
}

//---------------------------------------------------------------------------
void Mesh::apply_feature_map(std::string name, std::string filename)
{
  if (!this->poly_data_ || name == "") {
    return;
  }

  std::cerr << "being asked to apply feature map for feature '" << name << "' using filename " << filename << "\n";


  // read fibrosis_mask
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName(filename);
  reader->Update();
  ImageType::Pointer image = reader->GetOutput();
  LinearInterpolatorType::Pointer interpolator = LinearInterpolatorType::New();
  interpolator->SetInputImage(image);


  float radius = 5.0; // mm - need to expose as parameter

  auto points = this->poly_data_->GetPoints();

  vtkFloatArray* scalars = vtkFloatArray::New();
  scalars->SetNumberOfValues(points->GetNumberOfPoints());

  scalars->SetName(name.c_str());

  for (int i = 0; i < points->GetNumberOfPoints(); i++) {
    double pt[3];
    points->GetPoint(i, pt);

    ImageType::PointType pitk;
    pitk[0] = pt[0];
    pitk[1] = pt[1];
    pitk[2] = pt[2];
    ImageType::IndexType idx;



    LinearInterpolatorType::ContinuousIndexType index;
    image->TransformPhysicalPointToContinuousIndex(pitk, index);

    auto pixel = interpolator->EvaluateAtContinuousIndex(index);

    //image->TransformPhysicalPointToIndex(pitk, idx);

    //auto pixel = image->GetPixel(idx);
    std::cerr << pixel << " ";
    scalars->SetValue(i, pixel);
  }

  this->poly_data_->GetPointData()->AddArray(scalars);

  this->poly_data_->GetPointData()->SetScalars(scalars);

  //auto writer = vtkSmartPointer<vtkPolyDataWriter>::New();

  //writer->SetInputData(this->poly_data_);
  //writer->SetFileName("/tmp/foo.vtk");
  //writer->Update();


}
