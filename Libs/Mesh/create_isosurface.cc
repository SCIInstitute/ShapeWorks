
#include <iostream>
#include <sstream>

#include <vtkIterativeClosestPointTransform.h>
#include <vtkPolyDataReader.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkFileOutputWindow.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkDataSetWriter.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkImageImport.h>
#include <vtkMarchingCubes.h>
#include <vtkImageConstantPad.h>
#include <vtkPolyDataNormals.h>

#include <itkImage.h>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkImageRegionConstIterator.h>
#include <itkRegionOfInterestImageFilter.h>
#include <itkImageRegionIteratorWithIndex.h>
//#include <itkConstantPadImageFilter.h>
//#include <itkReinitializeLevelSetImageFilter.h>
#include <itkLinearInterpolateImageFunction.h>
#include <itkNearestNeighborInterpolateImageFunction.h>
#include <itkSignedMaurerDistanceMapImageFilter.h>
#include <itkGradientImageFilter.h>
#include <itkVTKImageExport.h>
#include <vtkPLYWriter.h>


// setup ITK types...
typedef float PixelType;
typedef itk::Image< PixelType,  3 >   ImageType;
typedef itk::ImageFileReader< ImageType  >  ReaderType;
typedef itk::ImageFileWriter< ImageType  >  WriterType;


/**
 * This function will connect the given itk::VTKImageExport filter to
 * the given vtkImageImport filter.
 */
template <typename ITK_Exporter, typename VTK_Importer>
void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

/**
 * This function will connect the given vtkImageExport filter to
 * the given itk::VTKImageImport filter.
 */
template <typename VTK_Exporter, typename ITK_Importer>
void ConnectPipelines(VTK_Exporter* exporter, ITK_Importer importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}


int main(int argc, char ** argv) {

  if (argc < 3) {
    std::cerr << "usage: " << argv[0] << " mask.nrrd output_surface.vtk" << std::endl;
    return 1;
  }


  std::string input_filename = argv[1];
  std::string output_filename = argv[2];



  // set vtk output window to go to a file
  vtkSmartPointer<vtkFileOutputWindow> fileOutputWindow = 
    vtkSmartPointer<vtkFileOutputWindow>::New();
  fileOutputWindow->SetFileName( "vtk_output.txt" );
  vtkOutputWindow* outputWindow = vtkOutputWindow::GetInstance();
  if (outputWindow) {
    outputWindow->SetInstance( fileOutputWindow );
  }





  // read wall
  ReaderType::Pointer mask_reader = ReaderType::New();
  mask_reader->SetFileName(input_filename);
  ImageType::Pointer wall_image = mask_reader->GetOutput();
  mask_reader->Update();




  // Hook up the ITK->VTK conversion pipeline.
  vtkImageImport *image_import = vtkImageImport::New();
  itk::VTKImageExport<ImageType>::Pointer image_export = itk::VTKImageExport<ImageType>::New();
  image_export->SetInput(mask_reader->GetOutput());
  ConnectPipelines(image_export, image_import);

  image_import->Update();



  // constant pad (to ensure capping)
  vtkImageConstantPad *constant_pad = vtkImageConstantPad::New();
  constant_pad->SetInputConnection(image_import->GetOutputPort());
  constant_pad->SetConstant(0); 

  int volextent[6];

  image_import->GetWholeExtent(volextent);
  volextent[0]-=1; volextent[1]+=1; volextent[2]-=1;
  volextent[3]+=1; volextent[4]-=1; volextent[5]+=1;
  constant_pad->SetOutputWholeExtent(volextent);
  constant_pad->Update();



  vtkMarchingCubes *marching_cubes = vtkMarchingCubes::New();
  marching_cubes->SetValue(0,0.5);
  marching_cubes->SetNumberOfContours(1);
  marching_cubes->SetInputConnection(constant_pad->GetOutputPort());
  marching_cubes->Update();


  vtkPolyData *poly_data = marching_cubes->GetOutput();

 
  vtkMatrix4x4 *matrix = vtkMatrix4x4::New();
  matrix->Identity();
  matrix->SetElement(0, 0, -1);
  matrix->SetElement(1, 1, -1);



  vtkTransform *transform = vtkTransform::New();
  transform->SetMatrix(matrix);

  vtkTransformPolyDataFilter *filter = vtkTransformPolyDataFilter::New();
  filter->SetTransform(transform);
  filter->SetInputData(poly_data);
  filter->Update();

  if(output_filename.substr(output_filename.size()-4, 4) == ".ply") {
    auto writer = vtkPLYWriter::New();
    writer->SetFileName(output_filename.c_str());
    writer->SetInputData(filter->GetOutput());
    writer->Update();
  } else {
    vtkDataSetWriter *writer = vtkDataSetWriter::New();
    writer->SetFileName(output_filename.c_str());
    writer->SetInputData(filter->GetOutput());
    writer->Update();
  }




  return 0;
}   
