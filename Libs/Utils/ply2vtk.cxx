
#include <vtkPolyData.h>
#include <vtkPLYReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataWriter.h>

int main ( int argc, char *argv[] )
{
    if(argc != 3)
    {
        std::cout << "Usage: " << argv[0] << "  inFilename(.ply) outFilename(.vtk)" << std::endl;
        return EXIT_FAILURE;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];

    vtkSmartPointer<vtkPLYReader> reader =
      vtkSmartPointer<vtkPLYReader>::New();

    reader->SetFileName ( inputFilename.c_str() );

    vtkSmartPointer<vtkPolyDataWriter> writer =
            vtkSmartPointer<vtkPolyDataWriter>::New();
    writer->SetInputConnection(reader->GetOutputPort());
    writer->SetFileTypeToASCII();
    writer->SetFileName( outputFilename.c_str() );
    writer->Update();

    return EXIT_SUCCESS;
}
