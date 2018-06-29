
#include <iostream>
#include <vtkPolyDataReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyData.h>
#include <vtkGradientFilter.h>
#include <vtkAssignAttribute.h>
#include <vtkPolyDataWriter.h>
#include <vtkFloatArray.h>

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "Usage: " << argv[0] << " vtkFilename(.vtk)" << std::endl;
        return EXIT_FAILURE;
    }

    std::string inputFilename = argv[1];

    vtkSmartPointer<vtkPolyDataReader> reader = vtkSmartPointer<vtkPolyDataReader>::New();

    reader->SetFileName ( inputFilename.c_str() );

    vtkSmartPointer<vtkGradientFilter> gradientFilter = vtkSmartPointer<vtkGradientFilter>::New();
    gradientFilter->SetInputConnection(reader->GetOutputPort());

    vtkSmartPointer<vtkFloatArray> gradData = gradientFilter->GetOutput();
    gradData->SetName("gradient");

//    vtkSmartPointer<vtkAssignAttribute> feaData = vtkSmartPointer<vtkAssignAttribute>::New();
//    feaData->SetInputConnection();

    vtkSmartPointer<vtkPolyData> polyData = reader->GetOutput();

    polyData->GetPointData()->SetVectors(gradData);

    vtkSmartPointer<vtkPolyDataWriter> writer = vtkSmartPointer<vtkPolyDataWriter>::New();
    writer->SetInput(polyData);
    writer->SetFileName("gradients.vtk");
    writer->Update();

    return 0;
}
