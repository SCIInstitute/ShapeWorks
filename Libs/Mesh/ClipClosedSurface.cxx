
#include "tinyxml.h"
#include <sstream>
#include <iostream>
#include <string>
#include <vector>

//------------- VTK ---------------------
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

using namespace std;

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_cross.h>

// Convenient typedef for storing cutting plane information
const int Dimension = 3;
struct CuttingPlaneType
{
    // three points defining the plane
    vnl_vector_fixed<double,Dimension> a;
    vnl_vector_fixed<double,Dimension> b;
    vnl_vector_fixed<double,Dimension> c;
};

int main(int argc, char *argv[])
{
    // preliminaries
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "ClipClosedSurface " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Given a set of meshes (vtk) and corresponding cutting planes, this tool clip each mesh with its cutting plane and result in a " << std::endl;
        std::cerr << "closed surface. A buffer distance can be used to move the cutting plane in the opposite direction of its normal to leave a buffer " << std::endl;
        std::cerr << "region below the cutting plane." << std::endl;

        std::cerr << "It uses a parameter file with the following tags:" << std::endl;
        std::cerr << "\t - input_meshes: a list of vtk file names for meshes to be processed" << std::endl;
        std::cerr << "\t - output_meshes: the vtk filenames of the output to be produced " << std::endl;
        std::cerr << "\t - cutting_planes: a list of cutting planes in three-point form ( x1 y1 z1 x2 y2 z2 x3 y3 z3 )" << std::endl;
        std::cerr << "\t - buffer_distance: phyical distance of the buffer region" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    // variables
    std::vector< std::string >      inFilenames; inFilenames.clear();
    std::vector< std::string >      outFilenames; outFilenames.clear();
    std::vector< CuttingPlaneType > cuttingPlanes; cuttingPlanes.clear();
    double buffer_distance = 0.0f;

    int numShapes = 0;

    // read parameters
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string filename("/dev/null\0");

        // Compile the list of input files.
        elem = docHandle.FirstChild( "input_meshes" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                inFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
            numShapes = inFilenames.size();
        }
        else
        {
            std::cerr << "No inputs to process!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "output_meshes" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                outFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No outputs specified!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "buffer_distance" ).Element();
        if (elem)
        {
            buffer_distance = atof( elem->GetText() );
        }
        else
        {
            std::cout << "No buffer_distance specific!!! there will be no buffer region below the cutting plane ..." << std::endl;
        }

        elem = docHandle.FirstChild( "cutting_planes" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());

            std::vector<double> cpVals;
            double pt;

            while (inputsBuffer >> pt)
            {
                cpVals.push_back(pt);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");

            if (cpVals.size() < (9*numShapes) )
            {
                std::cerr << "ERROR: Incomplete cutting plane data for all provided meshes! No cutting planes will be loaded!!" << std::endl;
                return EXIT_FAILURE;
            }
            else
            {
                vnl_vector_fixed<double,Dimension> a,b,c;
                int ctr = 0;

                // we need to read a cutting plane for mesh
                for (unsigned int shapeNo = 0 ; shapeNo < numShapes; shapeNo++)
                {
                    a[0] = cpVals[ctr++];
                    a[1] = cpVals[ctr++];
                    a[2] = cpVals[ctr++];

                    b[0] = cpVals[ctr++];
                    b[1] = cpVals[ctr++];
                    b[2] = cpVals[ctr++];

                    c[0] = cpVals[ctr++];
                    c[1] = cpVals[ctr++];
                    c[2] = cpVals[ctr++];

                    cuttingPlanes.resize(shapeNo+1);
                    cuttingPlanes[shapeNo].a = a;
                    cuttingPlanes[shapeNo].b = b;
                    cuttingPlanes[shapeNo].c = c;
                }
            }
        }
        else
        {
            std::cerr << "No cutting planes specificed ...!!!" << std::endl;
            return EXIT_FAILURE;
        }

    }

    for (unsigned int shapeNo = 0; shapeNo < numShapes; shapeNo++)
    {
        std::cout << "Reading mesh for " << inFilenames[shapeNo].c_str()  << std::endl ;

        vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();
        reader->SetFileName(inFilenames[shapeNo].c_str());
        reader->Update();

        vtkSmartPointer<vtkPolyData> mesh = reader->GetOutput();

        // compute cutting plane normal
        vnl_vector_fixed<double,Dimension> a,b,c;
        a = cuttingPlanes[shapeNo].a;
        b = cuttingPlanes[shapeNo].b;
        c = cuttingPlanes[shapeNo].c;

        vnl_vector<double> q = vnl_cross_3d((b-a),(c-a));

        vnl_vector_fixed<double, Dimension> normal;
        vnl_vector_fixed<double, Dimension> origin;

        normal = q / (q.magnitude() + 1e-20);
        origin = a - buffer_distance*normal;

        // Define the cutting plane
        vtkSmartPointer<vtkPlane> plane = vtkSmartPointer<vtkPlane>::New();
        plane->SetOrigin(origin[0], origin[1], origin[2]);
        plane->SetNormal(normal[0], normal[1], normal[2]);

        // Need a plane collection for clipping
        vtkSmartPointer<vtkPlaneCollection> planeCollection = vtkSmartPointer<vtkPlaneCollection>::New();
        planeCollection->AddItem(plane);

        // The clipper generates a clipped polygonial model
        vtkSmartPointer<vtkClipClosedSurface> clipper = vtkSmartPointer<vtkClipClosedSurface>::New();
        clipper->SetClippingPlanes(planeCollection);
        clipper->SetInputData(mesh);
        clipper->SetGenerateFaces(1);
        //clipper->SetScalarModeToLabels();
        clipper->Update();

        std::cout << "Wrting output mesh:" << outFilenames[shapeNo].c_str()  << std::endl ;
        vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
        writer->SetInputData(clipper->GetOutput());
        writer->SetFileName(outFilenames[shapeNo].c_str());
        writer->Update();
    }


    return EXIT_SUCCESS;
}


