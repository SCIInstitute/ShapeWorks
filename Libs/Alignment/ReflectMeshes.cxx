 
#include "vtkSmartPointer.h"
#include "vtkPolyDataReader.h"
#include "vtkPolyDataWriter.h"
#include "vtkReflectionFilter.h"
#include "vtkGeometryFilter.h"
#include "vtkDataSetSurfaceFilter.h"
#include "vtkMatrix4x4.h"
#include "vtkTransform.h"
#include "vtkTransformPolyDataFilter.h"
#include <vtkPLYReader.h>
#include <vtkPLYWriter.h>
#include "vtkUnstructuredGridReader.h"
#include "vtkUnstructuredGridWriter.h"
#include "vtkUnstructuredGrid.h"
#include "tinyxml.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include <vtkPolyDataNormals.h>
#include <vtkReverseSense.h>

void convertUnstructuredGridToPolyData( vtkSmartPointer<vtkUnstructuredGrid> data, vtkSmartPointer<vtkPolyData> &output)
{
    vtkSmartPointer<vtkPoints> verts = vtkSmartPointer<vtkPoints>::New();
    for(vtkIdType n = 0; n < data->GetNumberOfPoints(); n++) {
        double pt[3];
        data->GetPoint(n, pt);

        verts->InsertNextPoint(pt[0], pt[1], pt[2]);
    }

    output = vtkSmartPointer<vtkPolyData>::New();
    output->SetPoints( verts );

    vtkCellArray* cells = data->GetCells();
    output->SetPolys( cells );
}

int main(int argc, char* argv[])
{
    // check input
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "ReflectMeshes " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Reflect meshes to make data in whole ensemble align in same direction. It uses a parameter file with the following tags:" << std::endl;
        std::cerr << "\t - inputs: a list of vtk/ply file names for meshes to be relfected" << std::endl;
        std::cerr << "\t - output: a list of output filenames" << std::endl;
        std::cerr << "\t - is_unstructured_grid: (a scalar 1/0 for each input file) indicated whether the input meshes are in vtk unstructured grid format (Default 0)" << std::endl;
        std::cerr << "\t - direction: which axis to reflect with respect to, 0 for x-axis, 1 fir y-axis, and 2 for z-axis" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;

        return EXIT_FAILURE;
    }
    
    // variables
    std::vector< std::string > inFilenames; inFilenames.clear();
    std::vector< std::string > outFilenames; outFilenames.clear();
    unsigned int direction = 0;
    int type;
    std::vector<int> is_unstructured_grid;

    // read parameters
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string filename("/dev/null\0");

        elem = docHandle.FirstChild( "inputs" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                inFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        elem = docHandle.FirstChild( "outputs" ).Element();
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

        elem = docHandle.FirstChild( "is_unstructured_grid" ).Element();
        if(elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> type)
            {
                is_unstructured_grid.push_back( type );
            }
            inputsBuffer.clear();
            inputsBuffer.str("");

            if(is_unstructured_grid.size() != inFilenames.size())
            {
                std::cerr << "Some file_types not provided!!!\n";
            }
        }

        elem = docHandle.FirstChild( "direction" ).Element();
        if(elem)
        {
            direction = atoi(elem->GetText());
        }
    }

    if(is_unstructured_grid.size() == 0) {
        for (unsigned int i = 0; i < inFilenames.size(); i++) is_unstructured_grid.push_back(0);
    }

    vtkSmartPointer<vtkUnstructuredGridReader> reader_ug = vtkSmartPointer<vtkUnstructuredGridReader>::New();
    vtkSmartPointer<vtkPolyDataReader> reader_pd = vtkSmartPointer<vtkPolyDataReader>::New();
    vtkSmartPointer<vtkPLYReader> reader_pl = vtkSmartPointer<vtkPLYReader>::New();
    for(unsigned int sh = 0; sh < inFilenames.size(); sh++)
    {
        std::cout << "processing " << inFilenames[sh] << std::endl;
        int ind = outFilenames[sh].rfind(".");
        std::string pref = outFilenames[sh].substr(ind+1);

        std::cout << "extension: " << pref << std::endl;

        vtkSmartPointer<vtkPolyData> mesh = vtkSmartPointer<vtkPolyData>::New();

        if (pref == "ply")
        {
            vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
            reader->SetFileName( inFilenames[sh].c_str());
            reader->Update();
            mesh->DeepCopy( reader->GetOutput() );
        }
        else if(is_unstructured_grid[sh] == 1)
        {
            reader_ug->SetFileName( inFilenames[sh].c_str() );
            reader_ug->Update();
            convertUnstructuredGridToPolyData( reader_ug->GetOutput(), mesh );
        } else {

            reader_pd->SetFileName( inFilenames[sh].c_str() );
            reader_pd->Update();
            mesh->DeepCopy( reader_pd->GetOutput() );
        }

        vtkSmartPointer<vtkTransform> tr = vtkSmartPointer<vtkTransform>::New();
        double dir[3] = {1.0, 1.0, 1.0};
        if(direction == 0) {
            dir[0] = -1.0;
        } else if(direction == 1) {
            dir[1] = -1.0;
        } else if(direction == 2) {
            dir[2] = -1.0;
        } else {
            std::cerr << "Unknown direction\n";
            return EXIT_FAILURE;
        }
        tr->Scale(dir[0], dir[1], dir[2]);

        // in order to handle flipping normals under negative scaling
        vtkSmartPointer<vtkReverseSense> reverseSense = vtkSmartPointer<vtkReverseSense>::New();
        reverseSense->SetInputData(mesh);
        reverseSense->ReverseNormalsOff();
        reverseSense->ReverseCellsOn();
        reverseSense->Update();

        vtkSmartPointer<vtkTransformPolyDataFilter> transform = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
        transform->SetTransform( tr );
        transform->SetInputData( reverseSense->GetOutput() );
        transform->Update();


        //        vtkSmartPointer<vtkPolyData> inp = vtkSmartPointer<vtkPolyData>::New();
        //        inp->ShallowCopy( transform->GetOutput() );

        // recomputing normals caused the introduction of non-manifold triangles !!!
        //        // to handle the normals issue
        //        // Generate normals
        //        vtkSmartPointer<vtkPolyDataNormals> normalGenerator = vtkSmartPointer<vtkPolyDataNormals>::New();
        //#if VTK_MAJOR_VERSION <= 5
        //        normalGenerator->SetInput(transform->GetOutput());
        //#else
        //        normalGenerator->SetInputData(transform->GetOutput());
        //#endif
        //        normalGenerator->ComputePointNormalsOn();
        //        normalGenerator->ComputeCellNormalsOff();
        //        normalGenerator->SetConsistency(1);
        //        normalGenerator->SetFlipNormals(0);
        //        normalGenerator->SetNonManifoldTraversal(1);
        //        normalGenerator->Update();

        //        /*
        //            // Optional settings
        //            normalGenerator->SetFeatureAngle(0.1);
        //            normalGenerator->SetSplitting(1);
        //            normalGenerator->SetConsistency(0);
        //            normalGenerator->SetAutoOrientNormals(0);
        //            normalGenerator->SetComputePointNormals(1);
        //            normalGenerator->SetComputeCellNormals(0);
        //            normalGenerator->SetFlipNormals(0);
        //            normalGenerator->SetNonManifoldTraversal(1);
        //            */

        vtkSmartPointer<vtkPolyData> inp = vtkSmartPointer<vtkPolyData>::New();
        //        inp->ShallowCopy( normalGenerator->GetOutput() );
        inp->ShallowCopy( transform->GetOutput() );

        if (pref == "ply") {
            vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
            writer->SetInputData( inp );
            writer->SetFileName( outFilenames[sh].c_str() );
            writer->Update();
        } else {
            vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
            writer->SetInputData( inp );
            writer->SetFileName( outFilenames[sh].c_str() );
            writer->Update();
        }
    }
    return EXIT_SUCCESS;
}
