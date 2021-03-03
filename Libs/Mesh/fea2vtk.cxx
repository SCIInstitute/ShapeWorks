#include "TriMesh.h"
#include "meshFIM.h"
#include "math.h"
#include "tinyxml.h"
#include <iostream>
#include <sstream>
#include <string>
#include "vnl/vnl_vector.h"

#include <vtkVersion.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkPointData.h>
#include <vtkCellArray.h>
#include <vtkTriangle.h>

#include <vtkPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkFloatArray.h>

#define _USE_MATH_DEFINES

#include <sstream>      // std::istringstream


#define _USE_MATH_DEFINES

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "fea2vtk " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "Read in feature files (.fea) and their corresponding triangular mesh (ply) and output a vtk file containing the feature values as scalar onto the mesh (colormap) " << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - mesh:      a list of ply file names for meshes to be processed" << std::endl;
        std::cout << "\t - fea_per_mesh:      number of fea files to be processed per mesh" << std::endl;
        std::cout << "\t - fea:      a list of fea files to be processed such that the first fea_per_mesh fea files correspond to the first mesh and so on." << std::endl;
        
        std::cout << "Usage: " << argv[0] << "  parameter file" << std::endl;
        return EXIT_FAILURE;
    }

    typedef vnl_vector< double > PointVectorType;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;

    // Collect a list of point file names and output file names
    std::vector< std::string > meshFilename;
    std::vector< std::string > feaFilename;
    int fea_per_mesh;
    
    
    std::string tmpString;

    elem = docHandle.FirstChild( "mesh" ).Element();
    if (!elem)
    {
        std::cerr << "No mesh files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        meshFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            meshFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "fea_per_mesh" ).Element();
    if (elem)
    {
        fea_per_mesh = atoi( elem->GetText() );
    }
    else
    {
        std::cerr << "No fea_per_mesh specified!" << std::endl;
        return EXIT_FAILURE;
    }

    elem = docHandle.FirstChild( "fea" ).Element();
    if (!elem)
    {
        std::cerr << "No fea files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        feaFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            feaFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    if (feaFilename.size() != (fea_per_mesh * meshFilename.size()))
    {
        std::cerr << "Number of fea files doesn't match number of given meshes x fea_per_mesh" << std::endl;
        throw 1;
    }


    int numShapes = meshFilename.size();
    for (unsigned int shCount = 0; shCount < numShapes; shCount++)
    {
        // read mesh
        std::cout << "Reading mesh file: " << meshFilename[shCount] << std::endl;
        TriMesh *mesh = TriMesh::read( meshFilename[shCount].c_str() );
        orient(mesh);
	
        for(unsigned int fId = 0 ; fId < fea_per_mesh; fId++)
        {
            std::string curfeaFilename = feaFilename[shCount*fea_per_mesh + fId];

            std::cout << "Reading fea File: " << curfeaFilename << std::endl;
            mesh->ReadFeatureFromFile( curfeaFilename.c_str() );

            // TODO: use the mesh data to construct the vtk mesh structure to be saves as vtk file

            // Setup points
            vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
            int numVert = (int) mesh->vertices.size();
            for(unsigned int ii = 0 ; ii < numVert; ii++)
            {
                point pt = mesh->vertices[ii];
                points->InsertNextPoint(pt[0], pt[1], pt[2]);
            }

            // Setup faces
            vtkSmartPointer<vtkCellArray> triangles = vtkSmartPointer<vtkCellArray>::New();
            int numFaces = (int) mesh->faces.size();
            for(unsigned int ii = 0; ii < numFaces; ii++)
            {
                TriMesh::Face face = mesh->faces[ii];

                vtkSmartPointer<vtkTriangle> triangle = vtkSmartPointer<vtkTriangle>::New();
                triangle->GetPointIds()->SetId(0, face.v[0]);
                triangle->GetPointIds()->SetId(1, face.v[1]);
                triangle->GetPointIds()->SetId(2, face.v[2]);
                triangles->InsertNextCell(triangle);
            }

            vtkSmartPointer<vtkFloatArray> feaData = vtkSmartPointer<vtkFloatArray>::New();
            feaData->SetName("FEA_DATA");
            for(unsigned int ii = 0 ; ii < numVert; ii++)
            {
                feaData->InsertNextValue(mesh->features[fId][ii]); // each fea file has only one feature for each vertex
            }

            // Create a polydata object and add everything to it
            vtkSmartPointer<vtkPolyData> polydata = vtkSmartPointer<vtkPolyData>::New();
            polydata->SetPoints(points);
            polydata->SetPolys(triangles);
            polydata->GetPointData()->SetScalars(feaData);

	    // get name of output file
            int index = curfeaFilename.rfind("/");
            std::string path  = curfeaFilename.substr(0, index+1);
            std::string file  = curfeaFilename.substr(index+1, curfeaFilename.length()-1);
            std::string vtkFileName;

            index = file.rfind(".");
            if (index == std::string::npos)
            {
                vtkFileName = path + file + ".vtk";
            }
            else
            {
                vtkFileName = path + file.substr(0, index) + ".vtk";
            }

            
            std::cout << "vtkfilename: " << vtkFileName << std::endl;

            // TODO: write output vtk file
            vtkSmartPointer<vtkPolyDataWriter> out_writer = vtkSmartPointer<vtkPolyDataWriter>::New();
            out_writer->SetInputData(polydata);
            out_writer->SetFileName(vtkFileName.c_str());
            out_writer->Update();
        }
    }

    return EXIT_SUCCESS;
}
