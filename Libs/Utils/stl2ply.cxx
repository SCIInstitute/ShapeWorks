#include <vtkPolyData.h>
#include <vtkSTLReader.h>
#include <vtkSmartPointer.h>
#include <vtkPolyDataMapper.h>

#include <vtkActor.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>

#include <vtkPLYWriter.h>

int main ( int argc, char *argv[] )
{
    if(argc != 3)
    {
	std::cout << "Commandline tool to convert stl file format to ply file format ... "<< std::endl;
        std::cout << "Usage: " << argv[0] << "  inFilename(.stl) outFilename(.ply)" << std::endl;
        return EXIT_FAILURE;
    }

    std::string inputFilename = argv[1];
    std::string outputFilename = argv[2];

    vtkSmartPointer<vtkSTLReader> reader =
            vtkSmartPointer<vtkSTLReader>::New();
    reader->SetFileName ( inputFilename.c_str() );

    vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
    writer->SetInputConnection(reader->GetOutputPort());
    writer->SetFileTypeToASCII();
    writer->SetFileName( outputFilename.c_str() );
    writer->Update();


    if(0){
        // Visualize
        vtkSmartPointer<vtkPolyDataMapper> mapper =
                vtkSmartPointer<vtkPolyDataMapper>::New();
        mapper->SetInputConnection(reader->GetOutputPort());

        vtkSmartPointer<vtkActor> actor =
                vtkSmartPointer<vtkActor>::New();
        actor->SetMapper(mapper);

        vtkSmartPointer<vtkRenderer> renderer =
                vtkSmartPointer<vtkRenderer>::New();
        vtkSmartPointer<vtkRenderWindow> renderWindow =
                vtkSmartPointer<vtkRenderWindow>::New();
        renderWindow->AddRenderer(renderer);
        vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor =
                vtkSmartPointer<vtkRenderWindowInteractor>::New();
        renderWindowInteractor->SetRenderWindow(renderWindow);

        renderer->AddActor(actor);
        renderer->SetBackground(0.1804,0.5451,0.3412); // Sea green

        renderWindow->Render();
        renderWindowInteractor->Start();
    }

    return EXIT_SUCCESS;
}
