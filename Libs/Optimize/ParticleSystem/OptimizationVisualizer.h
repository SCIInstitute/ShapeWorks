#pragma once

#include <vector>
#include <string>

#include "TriMesh.h"

#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkPoints.h>
#include <vtkSphereSource.h>
#include <vtkCellArray.h>

#include "ParticleSystem/itkParticleSystem.h"

namespace shapeworks
{
  class OptimizationVisualizer
  {
  public:

    void AddMesh(vtkPolyData *mesh, trimesh::TriMesh *tmesh);
    void IterationCallback(itk::ParticleSystem<3> * particleSystem);

    void SetWireFrame(bool enabled);
    void SetSaveScreenshots(bool enabled, std::string path);

    OptimizationVisualizer() { }
    ~OptimizationVisualizer() {}

  private:

    void initialize();

    std::string screenshotDirectory;
    bool saveScreenshots = false;
    bool wireFrame = false;
    bool initialized = false;

    double *focalPoint;
    int iteration = 0;
    double radius;

    std::vector<vtkSmartPointer<vtkPolyData>> meshes;
    std::vector<trimesh::TriMesh *> tmeshes;

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    vtkSmartPointer<vtkRenderer> mainRenderer = vtkSmartPointer<vtkRenderer>::New();
    vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
    vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
    vtkSmartPointer<vtkSphereSource> cubeSource = vtkSmartPointer<vtkSphereSource>::New();

    vtkSmartPointer<vtkPolyDataMapper> lineMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkSmartPointer<vtkActor> lineActor = vtkSmartPointer<vtkActor>::New();
    vtkSmartPointer<vtkCellArray> lines = vtkSmartPointer<vtkCellArray>::New();

    std::vector<vtkSmartPointer<vtkRenderer>> sampleRenderers;
    std::vector<vtkSmartPointer<vtkPolyDataMapper>> sampleMappers;
    std::vector<vtkSmartPointer<vtkPoints>> samplePoints;
    std::vector<vtkSmartPointer<vtkPolyData>> samplePolyData;

  };

}
