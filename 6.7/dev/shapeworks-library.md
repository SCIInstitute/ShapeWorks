# Using ShapeWorks as a library

ShapeWorks can be used as a library, for example as part of an ITK based application.  To do so, when building ShapeWorks, specify CMAKE_INSTALL_PREFIX and use `make install`:

```
$ ccmake -DCMAKE_INSTALL_PREFIX=</path/to/sw/install>
$ make install
```

An example using ShapeWorks as a library in an ITK application is given in `Examples/C++`

CMakeLists.txt:

```
cmake_minimum_required(VERSION 3.10.2)

project(HelloShapeWorksItk)

set(CMAKE_CXX_STANDARD 17)

# Find ShapeWorks
find_package(ShapeWorks REQUIRED)

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--disable-new-dtags")

# Find ITK
#find_package(ITK REQUIRED)
#include(${ITK_USE_FILE})

add_executable(HelloShapeWorksItk HelloShapeWorksItk.cpp)

target_link_libraries(HelloShapeWorksItk
  ${ITK_LIBRARIES}
  shapeworks::Optimize
  shapeworks::Groom
  shapeworks::Analyze
  pybind11::embed
  )
```

HelloShapeWorksItk.cpp:
```
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
```

Configure using `-DShapeWorks_DIR` and the prefix module path used for building ShapeWorks. For example:

```
cd /path/to/shapeworks/Examples/C++
mkdir build
cd build
cmake .. -DShapeWorks_DIR=</path/to/sw/install>/lib/cmake/ShapeWorks -DCMAKE_PREFIX_PATH=</path/to/shapeworks/dependencies/install>
```

Build:
```
make
```

Run:
```
./HelloShapeWorksItk
```

Output:
```
Hello ShapeWorks ITK World!
Step 1: Use ITK to generate input data
Creating input file: mesh5.vtk
Creating input file: mesh6.vtk
Creating input file: mesh7.vtk
Creating input file: mesh8.vtk
Step 2: Groom Inputs
Step 3: Creating Shape Model
ShapeWorks: TBB using 16 threads
Verbosity 0: This will be the only output on your screen, unless there are any errors. Increase the verbosity if needed.
Step 4: Analyze
[2022-10-02 23:31:39.663] [info] ShapeWorks Offline Analysis
[2022-10-02 23:31:39.663] [info] number of subjects: 4
[2022-10-02 23:31:39.665] [info] Computing stats...
[2022-10-02 23:31:39.665] [info] Computed stats successfully
[2022-10-02 23:31:39.665] [info] number of modes: 3
[2022-10-02 23:31:39.676] [info] eigen value [0]: 213.72417259473602
[2022-10-02 23:31:39.676] [info] explained_variance [0]: 99.12
[2022-10-02 23:31:39.676] [info] cumulative_explained_variance [0]: 99.12
[2022-10-02 23:31:39.793] [info] eigen value [1]: 1.8944878261819007
[2022-10-02 23:31:39.793] [info] explained_variance [1]: 0.88
[2022-10-02 23:31:39.793] [info] cumulative_explained_variance [1]: 100.00
[2022-10-02 23:31:39.902] [info] eigen value [2]: 0.0016418139547566765
[2022-10-02 23:31:39.902] [info] explained_variance [2]: 0.00
[2022-10-02 23:31:39.902] [info] cumulative_explained_variance [2]: 100.00
```
