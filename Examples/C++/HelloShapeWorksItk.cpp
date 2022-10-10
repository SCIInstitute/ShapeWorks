#include <iostream>

// itk includes
#include "itkImage.h"
#include "itkMesh.h"
#include "itkRegularSphereMeshSource.h"
#include "itkMeshFileWriter.h"
#include "itkMeshIOFactory.h"
#include "itkVTKPolyDataMeshIOFactory.h"

// shapeworks includes
#include <Project/Project.h>
#include <Groom/Groom.h>
#include <Optimize/OptimizeParameters.h>
#include <Optimize/Optimize.h>
#include <Analyze/Analyze.h>

constexpr unsigned int Dimension = 3;
using TCoordinate = float;
using TMesh = itk::Mesh<TCoordinate, Dimension>;
using TSphere = itk::RegularSphereMeshSource<TMesh>;
using TMeshWriter = itk::MeshFileWriter<TMesh>;

void create_sphere(double radius, std::string name) {
  // Create the sphere source.
  auto sphere = TSphere::New();

  TSphere::VectorType scale;
  scale.Fill( radius );
  sphere->SetScale( scale );
  sphere->SetResolution( 5 );
  sphere->Update();

  // We now assign it to a mesh pointer.
  TMesh::Pointer mesh = sphere->GetOutput();

  // It is necessary to disconnect the mesh from the pipeline;
  // otherwise, the point and cell data will be deallocated
  // when we call "Update()" on the writer later in the program.
  mesh->DisconnectPipeline();

  auto mesh_writer = TMeshWriter::New();
  mesh_writer->SetFileName(name);
  mesh_writer->SetInput(mesh);
  mesh_writer->Update();
}

int main() {

  itk::VTKPolyDataMeshIOFactory::RegisterOneFactory();
  // auto registeredIOs = itk::ObjectFactoryBase::CreateAllInstance( "itkMeshIOBase" );
  
  typedef itk::Image< unsigned short, 3 > ImageType;
  ImageType::Pointer image = ImageType::New();

  std::cout << "Hello ShapeWorks ITK World!" << std::endl;

  // create a ShapeWorks Project
  auto project = std::make_shared<shapeworks::Project>();

  std::cout << "Step 1: Use ITK to generate input data\n";
  for (int i=5;i<9;i++) {
    auto filename = "mesh" + std::to_string(i) + ".vtk";
    std::cout << "Creating input file: " << filename << "\n";
    create_sphere(i, filename);
    auto subject = std::make_shared<shapeworks::Subject>();
    subject->set_original_filenames({filename});
    project->get_subjects().push_back(subject);
  }

  // groom the inputs
  std::cout << "Step 2: Groom Inputs\n";
  shapeworks::Groom groom{project};
  groom.run();

  // create a ShapeWorks Optimizer
  std::cout << "Step 3: Creating Shape Model\n";
  shapeworks::Optimize optimize;
  shapeworks::OptimizeParameters params(project);
  params.set_up_optimize(&optimize);
  optimize.SetProject(project);

  // run the optimizer
  bool success = optimize.Run();
  project->save("HelloShapeWorksItk.swproj");

  // perform analysis
  std::cout << "Step 4: Analyze\n";
  shapeworks::Analyze analyze{project};
  analyze.run_offline_analysis("analysis.json");
  
  return 0;
}
