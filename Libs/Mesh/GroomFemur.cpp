
#include <sstream>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>

//------------- VTK ---------------------
#include <vtkPLYWriter.h>
#include <vtkPLYReader.h>
#include <vtkReverseSense.h>
#include <vtkIterativeClosestPointTransform.h>

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>

#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkProbeFilter.h>
#include <vtkClipClosedSurface.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_cross.h>

#include <vtkSphereSource.h>
#include <vtkDecimatePro.h>
#include <vtkClipPolyData.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkDataSetMapper.h>
#include <vtkPolyData.h>
#include <vtkProperty.h>
#include <vtkClipClosedSurface.h>
#include <vtkPlane.h>
#include <vtkPlaneCollection.h>
#include <vtkXMLPolyDataReader.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>

std::string printPoint(double *a) {
  std::string s = "(";
  for (int i = 0; i < 3; i++) {
    s += a[i];
    if (i != 3 - 1) {
      s += ", ";
    }
  }
  s += ")";
  return s;
}

std::string printArray(double *a, int length) {
  std::string s = "(";
  for (int i = 0; i < length; i++) {
    s += a[i];
    if (i != length - 1) {
      s += ", ";
    }
  }
  s += ")";
  return s;
}

void alignMeshes(std::vector<vtkSmartPointer<vtkPolyData>> &inmeshes, std::vector<vtkSmartPointer<vtkPolyData>> &outmeshes, bool rigid) {

  vtkSmartPointer<vtkPolyData> targetMesh = inmeshes[0];
  outmeshes.push_back(targetMesh);
  for (int i = 1; i < inmeshes.size(); i++) {
    vtkSmartPointer<vtkPolyData> mesh = inmeshes[i];
    vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
    icp->SetSource(mesh);
    icp->SetTarget(targetMesh);
    if (rigid) {
      icp->GetLandmarkTransform()->SetModeToRigidBody();
    }
    else {
      icp->GetLandmarkTransform()->SetModeToAffine();
    }

    icp->SetMaximumNumberOfIterations(20);
    icp->StartByMatchingCentroidsOn();
    icp->Modified();
    icp->Update();

    vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    icpTransformFilter->SetInputData(mesh);
    icpTransformFilter->SetTransform(icp);
    icpTransformFilter->Update();
    mesh = icpTransformFilter->GetOutput();

    outmeshes.push_back(mesh);
  }
}

void cropExtra(std::vector<vtkSmartPointer<vtkPolyData>> &inmeshes, std::vector<vtkSmartPointer<vtkPolyData>> &outmeshes, double buffer) {
  // First figure out the minimum z length so that we can crop at the length of the shortest femur + buffer
  double minZLength = 999999;
  double minCrop = -999999;
  for (int i = 0; i < inmeshes.size(); i++) {
    vtkSmartPointer<vtkPolyData> mesh = inmeshes[i];
    double *center = mesh->GetCenter();
    double *bounds = mesh->GetBounds();
    double zlength = bounds[5] - bounds[4];
    if (zlength < minZLength) {
      minZLength = zlength;
    }
    if (bounds[4] > minCrop) {
      minCrop = bounds[4];
    }
  }
  minCrop -= buffer;

  // crop all of the femur to z length of shortest one + buffer
  for (int i = 0; i < inmeshes.size(); i++) {
    vtkSmartPointer<vtkPolyData> mesh = inmeshes[i];
    double *center = mesh->GetCenter();
    double *bounds = mesh->GetBounds();
    double zlength = bounds[5] - bounds[4];
    vtkSmartPointer<vtkPlane> clipPlane = vtkSmartPointer<vtkPlane>::New();
    clipPlane->SetNormal(0, 0, 1);
    clipPlane->SetOrigin(0.0, 0.0, minCrop);

    vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
    clipper->SetInputData(mesh);
    clipper->SetClipFunction(clipPlane);
    clipper->Update();
    mesh = clipper->GetOutput();
    outmeshes.push_back(mesh);
  }
}

int main(int argc, char *argv[])
{
  std::cout << "Instructions: Place all of the femur mesh .ply files to be groomed in ./grooming/ and add a file_list.txt file which lists the filenames 1 per line\n";

  std::string directoryName = "grooming/";
  ifstream inFile;
  std::string infilename = directoryName + "file_list.txt";
  inFile.open(infilename);
  if (!inFile) {
    cerr << "Unable to open file " << infilename << "\n";
    return 0;
  }
  std::vector<std::string> inputFiles;
  std::string line;
  while (inFile >> line) {
    inputFiles.push_back(line);
  }
  inFile.close();

  std::vector<vtkSmartPointer<vtkPolyData>> meshes;
  for (int i = 0; i < inputFiles.size(); i++) {
    std::string filename = directoryName + inputFiles[i];
    std::cerr << filename << "\n";

    vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
    reader->SetFileName(filename.c_str());
    reader->Update();
    vtkSmartPointer<vtkPolyData> mesh = reader->GetOutput();


    //// Clip off the end of each of the meshes
    double *center = mesh->GetCenter();
    double *bounds = mesh->GetBounds();
    std::cerr << "center = " << "(" << center[0] << ", " << center[1] << ", " << center[2] << ")" << "\n";
    std::cerr << "bounds = " << "(" << bounds[0] << ", " << bounds[1] << ", " << bounds[2] << ", " << bounds[3] << ", " << bounds[4] << ", " << bounds[5] << ")" << "\n";
    double zlength = bounds[5] - bounds[4];

    vtkSmartPointer<vtkPlane> clipPlane = vtkSmartPointer<vtkPlane>::New();
    clipPlane->SetNormal(0, 0, 1);
    clipPlane->SetOrigin(0.0, 0.0, bounds[4] + 2);

    vtkSmartPointer<vtkClipPolyData> clipper = vtkSmartPointer<vtkClipPolyData>::New();
    clipper->SetInputData(mesh);
    clipper->SetClipFunction(clipPlane);
    clipper->Update();
    mesh = clipper->GetOutput();


    //// Reflect the right sided meshes
    if (inputFiles[i].find("R") != std::string::npos) {

      vtkSmartPointer<vtkTransform> tr = vtkSmartPointer<vtkTransform>::New();
      tr->Scale(-1, 1, 1);

      // in order to handle flipping normals under negative scaling
      vtkSmartPointer<vtkReverseSense> reverseSense = vtkSmartPointer<vtkReverseSense>::New();
      reverseSense->SetInputData(mesh);
      reverseSense->ReverseNormalsOff();
      reverseSense->ReverseCellsOn();
      reverseSense->Update();

      vtkSmartPointer<vtkTransformPolyDataFilter> transform = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
      transform->SetTransform(tr);
      transform->SetInputData(reverseSense->GetOutput());
      transform->Update();
      mesh = transform->GetOutput();
    }

    //// Scale the meshes to ~100 radius bounding sphere
    bounds = mesh->GetBounds();
    double xrange = bounds[1] - bounds[0];
    double yrange = bounds[3] - bounds[2];
    double avgRange = (xrange + yrange) / 2;
    double scaleFactor = 100.0 / avgRange;
    vtkSmartPointer<vtkTransform> tr = vtkSmartPointer<vtkTransform>::New();
    tr->Scale(scaleFactor, scaleFactor, scaleFactor);

    vtkSmartPointer<vtkTransformPolyDataFilter> transform2 = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transform2->SetTransform(tr);
    transform2->SetInputData(mesh);
    transform2->Update();
    mesh = transform2->GetOutput();


    //// Center meshes at origin
    center = mesh->GetCenter();

    vtkSmartPointer<vtkTransform> translate = vtkSmartPointer<vtkTransform>::New();
    translate->Translate(-center[0], -center[1], -center[2]);

    vtkSmartPointer<vtkTransformPolyDataFilter> transform = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    transform->SetTransform(translate);
    transform->SetInputData(mesh);
    transform->Update();
    mesh = transform->GetOutput();

    meshes.push_back(mesh);
  }


  std::vector< vtkSmartPointer<vtkPolyData>> cropped;
  std::vector< vtkSmartPointer<vtkPolyData>> aligned;

  //// 2 iterations of alignment and cropping gave me good results for femur
  // Note that the goal is to get all the meshes cropped so that all the shafts are cropped at the same cutting plane
  // initial alignment
  alignMeshes(meshes, aligned, true);
  meshes.clear();
  // initially crop with 3 unit buffer
  cropExtra(aligned, meshes, 3);
  aligned.clear();
  // align again
  alignMeshes(meshes, aligned, false);
  meshes.clear();
  // this time crop down to the target
  cropExtra(aligned, meshes, 0);
  aligned.clear();


  std::string output_dir = "groom_output/";
  for (int i = 0; i < meshes.size(); i++) {
    std::string outputFile = output_dir + inputFiles[i];
    std::cerr << "Writing " << outputFile << "\n";
    vtkSmartPointer<vtkPLYWriter> writer2 = vtkSmartPointer<vtkPLYWriter>::New();
    writer2->SetInputData(meshes[i]);
    writer2->SetFileTypeToASCII();
    writer2->SetFileName(outputFile.c_str());
    writer2->Update();
  }
  std::cout << "output files have been written to: " << output_dir << "\n";

  //std::string asdf;
  //std::cin >> asdf;

}


