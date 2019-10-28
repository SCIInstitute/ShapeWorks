#ifndef VIS_H
#define VIS_H

#include <vtkVersion.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkPolyDataNormals.h>
#include <vtkPointData.h>
#include <vtkMath.h>

#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkIdTypeArray.h>
#include <vtkGlyph3D.h>
#include <vtkProperty.h>

#include <vtkConeSource.h>
#include <vtkTransform.h>

#include <vtkTransformPolyDataFilter.h>
#include <vtkMaskPoints.h>

class Vis{
public:
    static void visMesh(vtkSmartPointer<vtkPolyData> mesh, std::string window_title = "");
    static void visMeshWithNormals(vtkSmartPointer<vtkPolyData> mesh, int ratio = 10, std::string window_title = "");

    static void visMeshWithParticles(vtkSmartPointer<vtkPolyData> mesh,
                              vtkSmartPointer<vtkPoints> particles, float glyphRadius = 2, std::string window_title = "");
    static void visMeshWithParticles(vtkSmartPointer<vtkPolyData> mesh,
                                    vtkSmartPointer<vtkPoints> particles, std::vector<double> glyphRadii , std::string window_title = "");

    static void visMeshWithParticles(vtkSmartPointer<vtkPolyData> mesh,
                              vtkSmartPointer<vtkPoints> particles, vtkSmartPointer<vtkPoints> particles2, float glyphRadius = 2, std::string window_title = "");

    static void visMeshWithParticlesNormals(vtkSmartPointer<vtkPolyData> mesh, vtkSmartPointer<vtkPolyData> particlesData, std::string window_title = "");

    static void visParticles(vtkSmartPointer<vtkPoints> particles, float glyphRadius = 2, std::string window_title = "");
    static void visParticles(vtkSmartPointer<vtkPolyData> polydata, float glyphRadius = 2, std::string window_title = "");
    static void visParticles(vtkSmartPointer<vtkPoints> particles,
                      vtkSmartPointer<vtkPoints> particles2,
                      float glyphRadius = 2, std::string window_title = "");


};

#endif // VIS_H
