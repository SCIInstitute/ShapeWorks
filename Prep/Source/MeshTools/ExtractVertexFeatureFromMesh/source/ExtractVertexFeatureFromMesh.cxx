#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPLYWriter.h>
#include <vtkPLYReader.h>
#include <vtkPointData.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include "vtkUnstructuredGrid.h"
#include "vtkUnstructuredGridReader.h"
#include "tinyxml.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>
// if input is in .ply file, turn this off.
//#define READ_AS_PLY 0


// ------------------- string manipulation ------------------------------------

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '\\' || ch == '/';
    }
};
#else
struct MatchPathSeparator
{
    bool operator()( char ch ) const
    {
        return ch == '/';
    }
};
#endif

std::string removeExtension( std::string const& filename )
{
    std::string::const_reverse_iterator
            pivot
            = std::find( filename.rbegin(), filename.rend(), '.' );
    return pivot == filename.rend()
            ? filename
            : std::string( filename.begin(), pivot.base() - 1 );
}

std::string getPath( std::string const& filename )
{
    std::string::const_reverse_iterator
            pivot
            = std::find( filename.rbegin(), filename.rend(), '/' );
    return pivot == filename.rend()
            ? filename
            : std::string( filename.begin(), pivot.base() - 1 );
}

std::string getFilename( std::string const& pathname )
{
    return std::string(
                std::find_if( pathname.rbegin(), pathname.rend(),
                              MatchPathSeparator() ).base(),
                pathname.end() );
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

int main(int argc, char *argv[])
{
    if( argc < 2 )
    {

        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "ExtractVertexFeatureFromMesh " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Extract scalar values associated to mesh vertices to txt file" << std::endl;

        std::cerr << "It uses a parameter file with the following tags:" << std::endl;
        std::cerr << "\t - inputs: a list of vtk file names for meshes to be processed" << std::endl;
        std::cerr << "\t - out_path: a directory (otional) to save output files, it not indicated, out files will be saved in the same directory as the mesh files" << std::endl;
        std::cerr << "\t - vtk_feature_name: feature name in the vtk file" << std::endl;
        std::cerr << "\t - out_feature_name: feature name to be used in the output file names, if not indicated, mesh filename will be used as is" << std::endl;
        std::cerr << "\t - read_as_ply: to read ply rather than vtk files" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    std::vector< std::string > inFileNames; inFileNames.clear();
    std::string out_path ;
    bool useSameInPath = false;
    std::string vtk_feature_name;
    std::string out_feature_name;
    bool useFilename = false;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    int type;
    bool read_as_ply = false;
    std::vector<int> is_unstructured_grid;

    if(loadOkay)
    {
        TiXmlHandle docHandle(&doc);
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string filename("/dev/null\0");

        elem = docHandle.FirstChild( "inputs" ).Element();
        if(elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                inFileNames.push_back( filename );
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No inputs to process!" << std::endl;
            return EXIT_FAILURE;
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

            if(is_unstructured_grid.size() != inFileNames.size())
            {
                std::cerr << "Some file_types not provided!!!\n";
            }
        }

        elem = docHandle.FirstChild( "out_path" ).Element();
        if(elem) {
            out_path = elem->GetText();
        }
        else {
            useSameInPath = true;
        }

        elem = docHandle.FirstChild( "vtk_feature_name" ).Element();
        if(elem) {
            vtk_feature_name = elem->GetText();
        }
        else
        {
            std::cerr << "vtk feature name is not provided!!!\n";
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "out_feature_name" ).Element();
        if(elem) {
            out_feature_name = elem->GetText();
        }
        else
        {
            useFilename = true;
        }

        elem = docHandle.FirstChild( "read_as_ply" ).Element();
        if(elem) {
            read_as_ply = atoi( elem->GetText() ) ? true : false;
        }
    }
    else
    {
        return EXIT_FAILURE;
    }

    if(is_unstructured_grid.size() == 0) {
        for(unsigned int i = 0; i < inFileNames.size(); i++) is_unstructured_grid.push_back(0);
    }

    vtkSmartPointer<vtkPLYReader> reader_pl = vtkSmartPointer<vtkPLYReader>::New();
    vtkSmartPointer<vtkUnstructuredGridReader> reader_ug = vtkSmartPointer<vtkUnstructuredGridReader>::New();
    vtkSmartPointer<vtkPolyDataReader> reader_pd = vtkSmartPointer<vtkPolyDataReader>::New();

    for(size_t s = 0; s < inFileNames.size(); s++)
    {
        size_t fileIndex = s;
        std::cout << "processing " << inFileNames[fileIndex] << "...";
        std::cout << ",is_unstructured_grid(" << is_unstructured_grid[fileIndex] << ")...";

        if(read_as_ply) {
            vtkSmartPointer<vtkPLYReader> reader_pl = vtkSmartPointer<vtkPLYReader>::New();
            reader_pl->SetFileName( inFileNames[fileIndex].c_str() );
            reader_pl->Update();
        } else {
            if(is_unstructured_grid[fileIndex] == 1)
            {
                reader_ug->SetFileName( inFileNames[fileIndex].c_str() );
                reader_ug->Update();
            } else {
                reader_pd->SetFileName( inFileNames[fileIndex].c_str() );
                reader_pd->Update();
            }
        }
        vtkSmartPointer<vtkPolyData> inp = vtkSmartPointer<vtkPolyData>::New();
        if(is_unstructured_grid[fileIndex] == 1) {
            convertUnstructuredGridToPolyData( reader_ug->GetOutput(), inp );
        } else if(read_as_ply) {
            inp->DeepCopy( reader_pl->GetOutput() );
        } else {
            inp->DeepCopy( reader_pd->GetOutput() );
        }

        std::string verticesfilename;
        std::string feafilename;

        if (useSameInPath){
            verticesfilename = removeExtension(inFileNames[fileIndex]) + ".vertices.txt";
            if (useFilename)
                feafilename      = removeExtension(inFileNames[fileIndex]) + ".txt";
            else
                feafilename      = removeExtension(inFileNames[fileIndex]) + "." + out_feature_name + ".txt";
        }
        else {
            verticesfilename = out_path + '/'+ removeExtension(getFilename(inFileNames[fileIndex])) + ".vertices.txt";

            if (useFilename)
                feafilename      = out_path + '/'+ removeExtension(getFilename(inFileNames[fileIndex]))  + ".txt";
            else
                feafilename      = out_path + '/'+ removeExtension(getFilename(inFileNames[fileIndex]))  + "." + out_feature_name + ".txt";
        }

        std::ofstream vopt(verticesfilename.c_str() );
        for(vtkIdType i = 0; i < inp->GetNumberOfPoints(); i++) {
            double p[3];
            inp->GetPoint(i,p);

            vopt << p[0] << ' ' << p[1] << ' ' << p[2] << "\n";

        }
        vopt.close();

        // Write Fea File
        unsigned int numVert = inp->GetNumberOfPoints();

        vtkDataArray *scalars = inp->GetPointData()->GetScalars(vtk_feature_name.c_str());
        if(scalars->GetNumberOfTuples() != numVert)
        {
            std::cerr << "Number of vertices != number of scalar values\n";
            throw 1;
        }

        std::ofstream fout(feafilename.c_str());
        for(int i = 0; i < scalars->GetNumberOfTuples(); i++)
        {
            float feaValue = scalars->GetComponent(i, 0);
            fout << feaValue << std::endl;
        }
        fout.close();

        std::cout << "Done\n";

    }
    return EXIT_SUCCESS;
}

