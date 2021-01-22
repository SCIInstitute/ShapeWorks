// Copyright (c) 2011 LTSI INSERM U642
// All rights reserved.
#include "swHausdorffDistancePointSetFilter.h"
#include "vtkXMLPolyDataReader.h"
#include "vtkPolyDataReader.h"
#include "vtkAlgorithm.h"
#include "vtkPolyDataWriter.h"
#include "vtkSmartPointer.h"
#include "vtkPointSet.h"
#include "vtkFieldData.h"
#include "vtkPolyData.h"
#include "vtkPointData.h"


int main(int argc,char** argv)
{
    int inputAParam = 0;
    int inputBParam = 0;
    int outputAParam = 0;
    int outputBParam = 0;
    int targetParam = 0;
    int printOnlyParam = 0;
    int xmlReader = 0;

    for(int i=1;i<argc;i++)
    {
        if(strcmp(argv[i],"-a") == 0 || strcmp(argv[i],"--inputA") == 0)
        {
            inputAParam = i + 1;
        }
        else if(strcmp(argv[i],"-b") == 0 || strcmp(argv[i],"--inputB") == 0)
        {
            inputBParam = i + 1;
        }
        else if(strcmp(argv[i],"-A") == 0 || strcmp(argv[i],"--outputA") == 0)
        {
            outputAParam = i + 1;
        }
        else if(strcmp(argv[i],"-B") == 0 || strcmp(argv[i],"--outputB") == 0)
        {
            outputBParam = i + 1;
        }
        else if(strcmp(argv[i],"-t") == 0 || strcmp(argv[i],"--target") == 0)
        {
            targetParam = i + 1;
        }
        else if(strcmp(argv[i],"-p") == 0 || strcmp(argv[i],"--print-only") == 0)
        {
            printOnlyParam = 1;
        }
        else if(strcmp(argv[i],"-x") == 0 || strcmp(argv[i],"--xml-reader") == 0)
        {
            xmlReader = 1;
        }
    }

    if(argc<5)
    {
        cout<<"Missing Parameters!"<<endl;
        cout<<"\nUsage:"<<argv[0]<<endl;
        cout<<"\n(-a , --inputA) InputAFileName"<<endl;
        cout<<"\n(-b , --inputB) InputBFileName"<<endl;
        cout<<"\n(-A , --outputA) OutputAFileName"<<endl;
        cout<<"\n(-B , --outputB) OutputBFileName"<<endl;
        cout<<"\n(-t , --target) <0 point-to-point/1 point-to-cell> (Optional)"<<endl;
        cout<<"\n(-p , --print-only) print on stdout the distances (Optional) Hausdorff AtoB BtoA (space-separated)"<<endl;
        cout<<"\n(-x , --xml-reader) use vtkXMLPolyDataReader as input reader"<<endl;
        return EXIT_FAILURE;
    }


    vtkAlgorithm* readerA;
    vtkAlgorithm* readerB;

    // if( xmlReader )
    // {
    //     vtkXMLPolyDataReader* xmlReaderA = vtkXMLPolyDataReader::New();
    //     readerA = xmlReaderA;

    //     vtkXMLPolyDataReader* xmlReaderB = vtkXMLPolyDataReader::New();
    //     readerB = xmlReaderB;

    //     xmlReaderA->SetFileName(argv[inputAParam]);
    //     xmlReaderB->SetFileName(argv[inputBParam]);
    // }
    // else
    // {
        vtkPolyDataReader* legacyReaderA = vtkPolyDataReader::New();
        readerA = legacyReaderA;

        vtkPolyDataReader* legacyReaderB = vtkPolyDataReader::New();
        readerB = legacyReaderB;

        legacyReaderA->SetFileName(argv[inputAParam]);
        legacyReaderB->SetFileName(argv[inputBParam]);
    // }

    vtkSmartPointer<swHausdorffDistancePointSetFilter> filter = vtkSmartPointer<swHausdorffDistancePointSetFilter>::New();
    filter->SetInputConnection(readerA->GetOutputPort());
    filter->SetInputConnection(1,readerB->GetOutputPort());

    if( atoi(argv[targetParam]) )
        filter->SetTargetDistanceMethod( 1 );
    filter->Update();

    if( !printOnlyParam )
    {
        auto data = filter->GetOutput(0);
        data->GetPointData()->SetActiveScalars("Distance");
        vtkSmartPointer<vtkPolyDataWriter> polyDataWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
        polyDataWriter->SetInputData(data);
        polyDataWriter->SetFileName(argv[outputAParam]);
        polyDataWriter->Update();

        data = filter->GetOutput(1);
        data->GetPointData()->SetActiveScalars("Distance");
        polyDataWriter->SetInputData(data);
        polyDataWriter->SetFileName(argv[outputBParam]);
        polyDataWriter->Update();
    }
    // else
    // {
      auto hdist = static_cast<vtkPointSet*>(filter->GetOutput(0))->GetFieldData()->GetArray("HausdorffDistance");
      auto a2b = filter->GetOutputDataObject(0)->GetFieldData()->GetArray("RelativeDistanceAtoB");
      auto b2a = filter->GetOutputDataObject(1)->GetFieldData()->GetArray("RelativeDistanceBtoA");

      // these should all be the same
      std::cout << "hdist.size: " << hdist->GetNumberOfTuples() << std::endl;
      std::cout << "a2b.size: " << a2b->GetNumberOfTuples() << std::endl;
      std::cout << "b2a.size: " << b2a->GetNumberOfTuples() << std::endl;

      // these should be the same for each (one component of each tuple)
      std::cout << "hdist.totsize: " << hdist->GetNumberOfValues() << std::endl;
      std::cout << "a2b.totsize: " << a2b->GetNumberOfValues() << std::endl;
      std::cout << "b2a.totsize: " << b2a->GetNumberOfValues() << std::endl;

      
      for (auto i=0; i<hdist->GetNumberOfValues(); i++) {
        std::cout << hdist->GetComponent(i,0) << " "
                  << a2b->GetComponent(i,0) << " "
                  << b2a->GetComponent(i,0) << " "
                  << std::endl;
      }
    // }

    readerA->Delete( );
    readerB->Delete( );

    return( EXIT_SUCCESS );
}
