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
#include <vector>
#include <string>
#include <float.h>
#include <fstream>
#include <sstream>

#include <vtkPolyDataNormals.h>
#include <vtkReverseSense.h>

#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that reflect meshes with respect to a specified center and specific axis....";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("Mesh file to be reflected.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output reflection mesh.");
    parser.add_option("--reflectCenterFilename").action("store").type("string").set_default("empty").help("The filename for origin about which reflection occurs.");
    parser.add_option("--inputDirection").action("store").type("int").set_default(0).help("Direction along which it needs to be reflected");
    parser.add_option("--meshFormat").action("store").type("string").set_default("vtk").help("The IO mesh format");

    return parser;
}

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
    // read the command line parameters using the option parser
    optparse::OptionParser parser = buildParser();
    optparse::Values & options = parser.parse_args(argc,argv);
    std::vector<std::string> args = parser.args();
    
    if(argc < 2)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }

    std::string inFilename    = (std::string) options.get("inFilename");
    std::string outFilename   = (std::string) options.get("outFilename");
    std::string refCFilename  = (std::string) options.get("reflectCenterFilename");
    int      inputDirection   = (int) options.get("inputDirection");
    std::string pref          = (std::string) options.get("meshFormat");

    vtkSmartPointer<vtkPolyData> mesh = vtkSmartPointer<vtkPolyData>::New();
    

    if (pref == "ply")
    {
        vtkSmartPointer<vtkPLYReader> reader = vtkSmartPointer<vtkPLYReader>::New();
        reader->SetFileName( inFilename.c_str());
        reader->Update();
        mesh->DeepCopy( reader->GetOutput() );
    }
    else {
        vtkSmartPointer<vtkPolyDataReader> reader_pd = vtkSmartPointer<vtkPolyDataReader>::New();
        reader_pd->SetFileName( inFilename.c_str() );
        reader_pd->Update();
        mesh->DeepCopy( reader_pd->GetOutput() );
    }

    double cx, cy, cz;
    if(refCFilename == "empty"){
        // find the center of the bounding box
        double max_x = -1*FLT_MAX, min_x = FLT_MAX;
        double max_y = -1*FLT_MAX, min_y = FLT_MAX;
        double max_z = -1*FLT_MAX, min_z = FLT_MAX;

        for(vtkIdType i = 0; i < mesh->GetNumberOfPoints(); i++){
            double p[3];
            mesh->GetPoint(i, p);
            if(max_x < p[0]) max_x = p[0];
            if(max_y < p[1]) max_y = p[1];
            if(max_z < p[2]) max_z = p[2];
            if(min_x > p[0]) min_x = p[0];
            if(min_y > p[1]) min_y = p[1];
            if(min_z > p[2]) min_z = p[2];                
        }

        cx = -1*(max_x + min_x) / 2;
        cy = -1*(max_y + min_y) / 2;
        cz = -1*(max_z + min_z) / 2;
        std::cout << max_x << ", " << max_y << ", " << max_z <<std::endl;
        std::cout << min_x << ", " << min_y << ", " << min_z <<std::endl;
    }
    else{
        std::ifstream infile;
        infile.open( refCFilename.c_str());
        infile >> cx >> cy >> cz;
    }

    std::cout << cx << ", " << cy << ", " << cz <<std::endl;

    vtkSmartPointer<vtkTransform> tr = vtkSmartPointer<vtkTransform>::New();
    double dir[3] = {1.0, 1.0, 1.0};
    if(inputDirection == 0) {
        dir[0] = -1.0;
    } else if(inputDirection == 1) {
        dir[1] = -1.0;
    } else if(inputDirection == 2) {
        dir[2] = -1.0;
    } else {
        std::cerr << "Unknown direction\n";
        return EXIT_FAILURE;
    }
    tr->Translate(-cx, -cy, -cz);
    tr->Scale(dir[0], dir[1], dir[2]);
    tr->Translate(cx, cy, cz);
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

    vtkSmartPointer<vtkPolyData> inp = vtkSmartPointer<vtkPolyData>::New();
    //        inp->ShallowCopy( normalGenerator->GetOutput() );
    inp->ShallowCopy( transform->GetOutput() );

    if (pref == "ply") {
        vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
        writer->SetInputData( inp );
        writer->SetFileName( outFilename.c_str() );
        writer->Update();
    } else {
        vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
        writer->SetInputData( inp );
        writer->SetFileName( outFilename.c_str() );
        writer->Update();
    }
    return EXIT_SUCCESS;
}
