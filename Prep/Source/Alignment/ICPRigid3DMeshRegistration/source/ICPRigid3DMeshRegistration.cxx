 
/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    IcpRegid3DRegistration.cxx
  Language:  C++
  Date:      $Date: 2011/03/23 22:40:15 $
  Version:   $Revision: 1.1 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/



#include "vtkImageImport.h"
#include "vtkImageExport.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkContourFilter.h"
#include "vtkImageData.h"
#include "vtkDataSet.h"
#include "vtkProperty.h"
#include "vtkPolyDataWriter.h"
#include "vtkPolyDataReader.h"

#include "vtkRenderer.h"
#include "vtkRenderWindow.h"
#include "vtkRenderWindowInteractor.h"
#include "vtkActor.h"
#include "vtkImagePlaneWidget.h"
#include "vtkCellPicker.h"

#include "vtkSmartPointer.h"
#include "vtkTransform.h"
#include "vtkVertexGlyphFilter.h"
#include "vtkPoints.h"
#include "vtkCellArray.h"
#include "vtkIterativeClosestPointTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkLandmarkTransform.h"
#include "vtkMath.h"

#include "tinyxml.h"

#include <sstream>
#include <iostream>
#include <string>
#include <vector>


int main(int argc, char * argv [] )
{  

    // check input
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "ICPRigid3DMeshRegistration " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Performs iterative closed point (ICP) rigid registration on a pair of vtk meshes." << std::endl << std::endl;

        std::cerr << "It uses a parameter file that would enable to specify the source mesh (moving) and the target mesh (fixed)" << std::endl;
        std::cerr << "to be used to estimated the rigid transformation matrix then apply the same transformation on other meshes defined " << std::endl;
        std::cerr << "in the source mesh domain to be mapped to the target domain." << std::endl;

        std::cerr << "parameter file tags are as follows:" << std::endl;
        std::cerr << "\t - source_mesh: vtk filename of the moving mesh" << std::endl;
        std::cerr << "\t - target_mesh: vtk filename of the fixed mesh" << std::endl;
        std::cerr << "\t - out_mesh : vtk filename of the aligned moving mesh to be save" << std::endl;
        std::cerr << "\t - out_transform : txt filename to save the estimated transformation" << std::endl;
        std::cerr << "\t - mode : Registration mode rigid, similarity, affine (default: similarity) " << std::endl;
        std::cerr << "\t - source_meshes: (optional) a list of vtk filenames for meshes defined in the source mesh domain " << std::endl;
        std::cerr << "\t \t to be mapped to the target domain using the same transformation matrix estimated." << std::endl;
        std::cerr << "\t - out_meshes : a list vtk filenames to save source_meshes after applying the transformation matrix." << std::endl;
        std::cerr << "\t - icp_iterations: number of iterations" << std::endl;
        std::cerr << "\t - debug: verbose debugging information" << std::endl;
        std::cerr << "\t - visualize: display the resulting alignment" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;

        return EXIT_FAILURE;
    }

    std::string sourceMeshFilename;
    std::string targetMeshFilename;
    std::string outMeshFilename;
    std::vector< std::string >  sourceMeshesFilenames; sourceMeshesFilenames.clear();
    std::vector< std::string >  outMeshesFilenames;    outMeshesFilenames.clear();
    unsigned int icpIterations = 10;
    bool debug = false;
    bool visualize = false;
    std::string mode = "similarity";
    std::string out_transform;

    // read parameters
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string filename("/dev/null\0");

        elem = docHandle.FirstChild( "source_mesh" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            inputsBuffer >> sourceMeshFilename;
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No source mesh to process!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "target_mesh" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            inputsBuffer >> targetMeshFilename;
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No target mesh to process!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "out_mesh" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            inputsBuffer >> outMeshFilename;
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No output mesh specified to save output!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "out_transform" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            inputsBuffer >> out_transform;
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No output transform file specified to save output!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "source_meshes" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                sourceMeshesFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }


        elem = docHandle.FirstChild( "out_meshes" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                outMeshesFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        elem = docHandle.FirstChild( "mode" ).Element();
        if (elem)
        {
            mode = elem->GetText();
        }
        elem = docHandle.FirstChild( "icp_iterations" ).Element();
        if (elem)
        {
            icpIterations = atoi(elem->GetText());
        }

        elem = docHandle.FirstChild( "debug" ).Element();
        if (elem)
        {
            debug = atoi(elem->GetText()) == 1 ? true : false;
        }

        elem = docHandle.FirstChild( "visualize" ).Element();
        if (elem)
        {
            visualize = atoi(elem->GetText()) == 1 ? true : false;
        }
    }


    vtkSmartPointer<vtkPolyDataReader> targetReader = vtkSmartPointer<vtkPolyDataReader>::New();
    targetReader->SetFileName( targetMeshFilename.c_str() );
    targetReader->Update();

    vtkSmartPointer<vtkPolyDataReader> movingReader = vtkSmartPointer<vtkPolyDataReader>::New();
    movingReader->SetFileName(sourceMeshFilename.c_str());
    movingReader->Update();

    vtkSmartPointer<vtkPolyData> target = targetReader->GetOutput();
    vtkSmartPointer<vtkPolyData> moving = movingReader->GetOutput();

    // Setup ICP transform
    vtkSmartPointer<vtkIterativeClosestPointTransform> icp =
            vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
    icp->SetSource(moving);
    icp->SetTarget(target);

    if(mode.compare("rigid") == 0)
        icp->GetLandmarkTransform()->SetModeToRigidBody();
    if(mode.compare("similarity") == 0)
        icp->GetLandmarkTransform()->SetModeToSimilarity();
    if(mode.compare("affine") == 0)
        icp->GetLandmarkTransform()->SetModeToAffine();

    icp->SetMaximumNumberOfIterations(icpIterations);
    icp->StartByMatchingCentroidsOn();
    if(debug)
    {
        icp->SetDebug(1);
        icp->SetMaximumMeanDistance(1e-5);
        icp->CheckMeanDistanceOn();
    }
    icp->Modified();
    icp->Update();

    std::cout << "Mean dist : " << icp->GetMaximumMeanDistance() << std::endl;

    vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter =
            vtkSmartPointer<vtkTransformPolyDataFilter>::New();
    icpTransformFilter->SetInput(moving);
    icpTransformFilter->SetTransform(icp);
    icpTransformFilter->Update();

    //Transform Segmentation
    // Get the resulting transformation matrix (this matrix takes the source points to the target points)
    vtkSmartPointer<vtkMatrix4x4> transformationMatrix = icp->GetMatrix();

    std::ofstream ofs(out_transform.c_str());
    for(unsigned int i = 0 ; i < 4 ; i++)
    {
        for(unsigned int j = 0 ; j < 4 ; j++)
            ofs << transformationMatrix->GetElement(i,j) << " ";
        ofs << "\n";
    }
    ofs.close();

    std::cout << "The resulting transformation matrix is: " << *transformationMatrix << std::endl;

    vtkTransform* transform = vtkTransform::New();
    transform->SetMatrix(transformationMatrix);

    vtkTransformPolyDataFilter* transformer = vtkTransformPolyDataFilter::New();
    transformer->SetInputConnection( movingReader->GetOutputPort());
    transformer->SetTransform( transform );
    transformer->Update();

    vtkSmartPointer<vtkPolyData> solution = vtkSmartPointer<vtkPolyData>::New();
    solution = transformer->GetOutput();

    vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
    writer->SetFileName(outMeshFilename.c_str());
    writer->SetInputConnection(transformer->GetOutputPort());
    writer->Update();

    //---------------------------------------------------
    // VTK Render pipeline.
    //---------------------------------------------------
    if(visualize)
    {
        vtkRenderer* renderer = vtkRenderer::New();
        vtkRenderWindow* renWin = vtkRenderWindow::New();
        vtkRenderWindowInteractor* iren = vtkRenderWindowInteractor::New();

        renWin->SetSize(500,500);
        renWin->AddRenderer(renderer);
        iren->SetRenderWindow(renWin);

        renderer->SetBackground(0.4392, 0.5020, 0.5647);

        vtkPolyDataMapper* tarMapper = vtkPolyDataMapper::New();
        vtkActor* tarActor = vtkActor::New();

        tarActor->SetMapper( tarMapper );
        tarMapper->SetInput( target );
        tarMapper->ScalarVisibilityOff();

        vtkPolyDataMapper* movMapper = vtkPolyDataMapper::New();
        vtkActor* movActor = vtkActor::New();

        movActor->SetMapper(movMapper);
        movMapper->SetInput(moving);
        movMapper->ScalarVisibilityOff();

        vtkPolyDataMapper* solMapper = vtkPolyDataMapper::New();
        vtkActor* solActor = vtkActor::New();

        solActor->SetMapper(solMapper);
        solMapper->SetInput(solution);
        solMapper->ScalarVisibilityOff();

        vtkProperty *targetProperty = vtkProperty::New();
        targetProperty->SetAmbient(0.1);
        targetProperty->SetDiffuse(0.1);
        targetProperty->SetSpecular(0.5);
        targetProperty->SetColor(0.0,0.0,1.0);
        targetProperty->SetLineWidth(2.0);
        targetProperty->SetRepresentationToSurface();

        vtkProperty * movingProperty = vtkProperty::New();
        movingProperty->SetAmbient(0.1);
        movingProperty->SetDiffuse(0.1);
        movingProperty->SetSpecular(0.5);
        movingProperty->SetColor(1.0,0.0,0.0);
        movingProperty->SetLineWidth(2.0);
        movingProperty->SetRepresentationToSurface();

        vtkProperty * solutionProperty = vtkProperty::New();
        solutionProperty->SetAmbient(0.1);
        solutionProperty->SetDiffuse(0.1);
        solutionProperty->SetSpecular(0.5);
        solutionProperty->SetColor(0.0,1.0,0.0);
        solutionProperty->SetLineWidth(2.0);
        solutionProperty->SetRepresentationToSurface();

        movActor->SetProperty( movingProperty);
        tarActor->SetProperty( targetProperty);
        solActor->SetProperty( solutionProperty);

        renderer->AddActor( movActor);
        renderer->AddActor( tarActor);
        renderer->AddActor( solActor);

        renderer->ResetCamera();
        renWin->Render();
        iren->Start();

        // Release VTK components
        tarActor->Delete();
        movActor->Delete();
        solActor->Delete();
        targetProperty->Delete();
        movingProperty->Delete();
        solutionProperty->Delete();
        tarMapper->Delete();
        movMapper->Delete();
        solMapper->Delete();
        renWin->Delete();
        renderer->Delete();
        iren->Delete();
    }

    if(sourceMeshesFilenames.size() > 0)
    {
        for (unsigned meshNo = 0; meshNo < sourceMeshesFilenames.size(); meshNo++)
        {
            vtkSmartPointer<vtkPolyDataReader> movingReader2 = vtkSmartPointer<vtkPolyDataReader>::New();
            movingReader2->SetFileName(sourceMeshesFilenames[meshNo].c_str());
            movingReader2->Update();

            vtkTransformPolyDataFilter* transformer2 = vtkTransformPolyDataFilter::New();
            transformer2->SetInputConnection( movingReader2->GetOutputPort());
            transformer2->SetTransform( transform );
            transformer2->Update();

            vtkSmartPointer<vtkPolyData> solution2 = vtkSmartPointer<vtkPolyData>::New();
            solution2 = transformer2->GetOutput();

            vtkSmartPointer<vtkPolyDataWriter> writer2 = vtkSmartPointer<vtkPolyDataWriter>::New();
            writer2->SetFileName(outMeshesFilenames[meshNo].c_str());
            writer2->SetInputConnection(transformer2->GetOutputPort());
            writer2->Update();
        }
    }


    return 0;
}
