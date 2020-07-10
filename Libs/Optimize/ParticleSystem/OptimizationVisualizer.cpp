#pragma once

#include "OptimizationVisualizer.h"

#include <vtkProperty.h>
#include <vtkLine.h>
#include <vtkCellData.h>
#include <vtkCamera.h>

#include <vtkSmartPointer.h>
#include <vtkCubeSource.h>
#include <vtkGlyph3D.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkGenericRenderWindowInteractor.h>
#include <vtkWindowToImageFilter.h>
#include <vtkPNGWriter.h>

#include <vtkExtractEdges.h>

namespace shapeworks {

  void OptimizationVisualizer::SetSaveScreenshots(bool enabled, std::string path) {
    this->saveScreenshots = enabled;
    if (this->saveScreenshots) {
      screenshotDirectory = path;
    }
  }

  void OptimizationVisualizer::SetWireFrame(bool enabled) {
    this->wireFrame = enabled;
  }

  void OptimizationVisualizer::AddMesh(vtkPolyData* mesh) {
    if (this->wireFrame) {
      vtkSmartPointer<vtkExtractEdges> edges = vtkSmartPointer<vtkExtractEdges>::New();
      edges->SetInputData(mesh);
      edges->Update();
      vtkPolyData * wireframemesh = edges->GetOutput();
      meshes.push_back(wireframemesh);
    }
    else {
      meshes.push_back(mesh);
    }
  }

  void OptimizationVisualizer::IterationCallback(itk::ParticleSystem<3> * particleSystem) {
    if (!initialized) {
      initialize();
    }

    int numShapes = particleSystem->GetNumberOfDomains();
    int numParticles = particleSystem->GetNumberOfParticles(0);

    int count = 0;
    for (int j = 0; j < numParticles; j++) {
      for (int i = 0; i < numShapes; i++) {
        itk::Point<double, 3> p = particleSystem->GetPosition(j, i);
        if (count >= points->GetNumberOfPoints()) {
          points->InsertNextPoint(p[0], p[1], p[2]);
          if (i > 0) {
            vtkSmartPointer<vtkLine> line2 = vtkSmartPointer<vtkLine>::New();
            line2->GetPointIds()->SetId(0, count);
            line2->GetPointIds()->SetId(1, count - 1);
            if (count >= lines->GetNumberOfCells()) {
              lines->InsertNextCell(line2);
            }
          }
        }
        else {
          points->SetPoint(count, p[0], p[1], p[2]);
        }
        count++;
        if (j >= samplePoints[i]->GetNumberOfPoints()) {
          samplePoints[i]->InsertNextPoint(p[0], p[1], p[2]);
        }
        else {
          samplePoints[i]->SetPoint(j, p[0], p[1], p[2]);
        }
      }
    }

    polydata->SetPoints(points);
    polydata->SetLines(lines);
    polydata->Modified();

    vtkSmartPointer<vtkPolyData> polydata2 = vtkSmartPointer<vtkPolyData>::New();
    polydata2->DeepCopy(polydata);
    lineMapper->SetInputData(polydata2);

    vtkSmartPointer<vtkGlyph3D> glyph3D = vtkSmartPointer<vtkGlyph3D>::New();
    glyph3D->SetSourceConnection(cubeSource->GetOutputPort());
    glyph3D->SetInputData(polydata);
    glyph3D->SetScaleFactor(0.15);
    glyph3D->Update();
    mapper->SetInputConnection(glyph3D->GetOutputPort());

    for (int i = 0; i < sampleMappers.size(); i++) {
      vtkSmartPointer<vtkGlyph3D> sampleGlyph = vtkSmartPointer<vtkGlyph3D>::New();
      sampleGlyph->SetSourceConnection(cubeSource->GetOutputPort());
      sampleGlyph->SetInputData(samplePolyData[i]);
      sampleGlyph->SetScaleFactor(1);
      sampleGlyph->Update();
      sampleMappers[i]->SetInputConnection(sampleGlyph->GetOutputPort());
    }

    vtkSmartPointer<vtkCamera> cam = mainRenderer->GetActiveCamera();

    int rotationSpeed = 10;
    double rotation = double(iteration % (360 * rotationSpeed)) / double(rotationSpeed);
    double PI = 3.14159;
    double z = sin(rotation * 2 * PI / 360);
    double x = cos(rotation * 2 * PI / 360);

    cam->SetPosition(focalPoint[0] + radius * x, focalPoint[1], focalPoint[2] + radius * z);
    cam->SetClippingRange(1, radius * 2);
    cam->Modified();

    for (int i = 0; i < sampleRenderers.size(); i++) {
      sampleRenderers[i]->GetActiveCamera()->SetPosition(focalPoint[0] + radius * x, focalPoint[1], focalPoint[2] + radius * z);
      sampleRenderers[i]->GetActiveCamera()->SetClippingRange(1, 50);
      sampleRenderers[i]->GetActiveCamera()->Modified();
    }

    renderWindow->Render();

    if (this->saveScreenshots) {
      vtkSmartPointer<vtkWindowToImageFilter> windowToImageFilter =
        vtkSmartPointer<vtkWindowToImageFilter>::New();
      windowToImageFilter->SetInput(renderWindow);
      //windowToImageFilter->SetMagnification(3); //set the resolution of the output image (3 times the current resolution of vtk render window)
      //windowToImageFilter->SetInputBufferTypeToRGBA(); //also record the alpha (transparency) channel
      windowToImageFilter->ReadFrontBufferOff(); // read from the back buffer
      windowToImageFilter->Update();

      vtkSmartPointer<vtkPNGWriter> writer = vtkSmartPointer<vtkPNGWriter>::New(); 
      char buffer[100];
      int n = sprintf(buffer, "%s/screenshot_%08d.png", this->screenshotDirectory, iteration);
      writer->SetFileName(buffer);
      writer->SetInputConnection(windowToImageFilter->GetOutputPort());
      writer->Write();
    }

    iteration++;
  }

  void OptimizationVisualizer::initialize() {

    polydata->SetPoints(points);
    polydata->SetLines(lines);
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(1.0, 0, 0);
    renderWindow->SetFullScreen(1);
    renderWindow->AddRenderer(mainRenderer);
    mainRenderer->SetBackground(.5, .5, .5);
    mainRenderer->AddActor(actor);

    lineActor->SetMapper(lineMapper);
    lineActor->GetProperty()->SetColor(0, 1, 0);
    lineActor->GetProperty()->SetLineWidth(1);
    mainRenderer->AddActor(lineActor);

    double width = 1.0 / meshes.size();
    double height = (width < 0.1) ? width : 0.1;
    for (int i = 0; i < meshes.size(); i++) {
      vtkSmartPointer<vtkPolyDataMapper> meshMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      vtkSmartPointer<vtkActor> meshActor = vtkSmartPointer<vtkActor>::New();
      meshMapper->SetInputData(meshes[i]);
      meshActor->SetMapper(meshMapper);
      meshActor->GetProperty()->SetColor(0.0, 0.0, 0.0);
      meshActor->GetProperty()->SetEdgeVisibility(false);
      double alpha = 1.0 / meshes.size();
      alpha /= 2;
      alpha = alpha > 1 ? 1 : alpha;
      meshActor->GetProperty()->SetOpacity(alpha);
      meshActor->GetProperty()->SetLineWidth(0.01);
      mainRenderer->AddActor(meshActor);

      vtkSmartPointer<vtkRenderer> meshRenderer = vtkSmartPointer<vtkRenderer>::New();
      meshRenderer->AddActor(meshActor);
      meshRenderer->SetBackground(.5, .5, .5);
      meshRenderer->SetViewport(width * i, 0, width * (i + 1), height);

      vtkSmartPointer<vtkPolyDataMapper> sampleMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
      vtkSmartPointer<vtkActor> sampleActor = vtkSmartPointer<vtkActor>::New();
      sampleActor->SetMapper(sampleMapper);
      sampleActor->GetProperty()->SetColor(1.0, 0, 0);

      meshRenderer->AddActor(sampleActor);

      renderWindow->AddRenderer(meshRenderer);

      vtkSmartPointer<vtkPolyData> samplepolydata = vtkSmartPointer<vtkPolyData>::New();
      vtkSmartPointer<vtkPoints> samplepoints = vtkSmartPointer<vtkPoints>::New();
      samplepolydata->SetPoints(samplepoints);

      sampleRenderers.push_back(meshRenderer);
      sampleMappers.push_back(sampleMapper);
      samplePoints.push_back(samplepoints);
      samplePolyData.push_back(samplepolydata);
    }
    mainRenderer->ResetCamera();
    focalPoint = mainRenderer->GetActiveCamera()->GetFocalPoint();

    double *pos = mainRenderer->GetActiveCamera()->GetPosition();
    radius = pos[2] - focalPoint[2];

    for (int i = 0; i < sampleRenderers.size(); i++) {
      sampleRenderers[i]->ResetCamera();
    }
    initialized = true;

  }
}
