/*
 * Shapeworks license
 */

/**
 * @file MeshGenerator.h
 * @brief Mesh generation
 *
 * The MeshGenerator performs the actual work of reconstructing
 * a mesh from a shape (list of points).
 */

#ifndef MESH_GENERATOR_H
#define MESH_GENERATOR_H

#include "vnl/vnl_vector.h"

#include <vtkSmartPointer.h>
#include <vtkSurfaceReconstructionFilter.h>
#include <vtkPoints.h>
#include <vtkContourFilter.h>
#include <vtkReverseSense.h>
#include <vtkSmoothPolyDataFilter.h>
#include <Data/Preferences.h>

class MeshGenerator
{
  public:
    MeshGenerator(Preferences& prefs_);
    ~MeshGenerator();
    vtkSmartPointer<vtkPolyData> buildMesh( const vnl_vector<double>& shape );

  private:
    vtkSmartPointer<vtkPolyData> transform_back(
        vtkSmartPointer<vtkPoints> pt,
        vtkSmartPointer<vtkPolyData> pd);
    vtkSmartPointer<vtkSurfaceReconstructionFilter>  surfaceReconstruction;
    vtkSmartPointer<vtkPoints>            points;
    vtkSmartPointer<vtkPolyData>            pointSet;
    vtkSmartPointer<vtkContourFilter>          contourFilter;
    vtkSmartPointer<vtkReverseSense>          reverseSense;
    vtkSmartPointer<vtkSmoothPolyDataFilter>      smoothFilter;

    Preferences& prefs_;
};

#endif // ifndef MESH_GENERATOR_H
