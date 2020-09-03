// Copyright (c) 2011 LTSI INSERM U642
// All rights reserved.

#include "vtkHausdorffDistancePointSetFilter2.h"

#include "vtkXMLPolyDataReader.h"
#include "vtkPolyDataReader.h"
#include "vtkAlgorithm.h"
#include "vtkPolyDataWriter.h"
#include "vtkSmartPointer.h"
#include "vtkPointSet.h"
#include "vtkFieldData.h"

// SHIREEN
#include <fstream>

int main(int argc,char** argv)
{
    int inputAParam = 0;
    int inputBParam = 0;
    int outputAParam = 0;
    int outputBParam = 0;
    int outputAidx = 0;// SHIREEN
    int outputBidx = 0;// SHIREEN
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
        else if(strcmp(argv[i],"-Ax") == 0 || strcmp(argv[i],"--outputAidx") == 0)// SHIREEN
        {
            outputAidx = i + 1;
        }
        else if(strcmp(argv[i],"-Bx") == 0 || strcmp(argv[i],"--outputAidx") == 0)// SHIREEN
        {
            outputBidx = i + 1;
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
        cout<<"\n(-Ax , --outputAidx) OutputAIndicesFileName"<<endl;// SHIREEN
        cout<<"\n(-Bx , --outputBidx) OutputBIndicesFileName"<<endl;// SHIREEN
        cout<<"\n(-t , --target) <0 point-to-point/1 point-to-cell> (Optional)"<<endl;
        cout<<"\n(-p , --print-only) print on stdout the distances (Optional) Hausdorff AtoB BtoA (space-separated)"<<endl;
        cout<<"\n(-x , --xml-reader) use vtkXMLPolyDataReader as input reader"<<endl;
        return EXIT_FAILURE;
    }


    vtkAlgorithm* readerA;
    vtkAlgorithm* readerB;

    if( xmlReader )
    {
        vtkXMLPolyDataReader* xmlReaderA = vtkXMLPolyDataReader::New();
        readerA = xmlReaderA;

        vtkXMLPolyDataReader* xmlReaderB = vtkXMLPolyDataReader::New();
        readerB = xmlReaderB;

        xmlReaderA->SetFileName(argv[inputAParam]);
        xmlReaderB->SetFileName(argv[inputBParam]);
    }
    else
    {
        vtkPolyDataReader* legacyReaderA = vtkPolyDataReader::New();
        readerA = legacyReaderA;

        vtkPolyDataReader* legacyReaderB = vtkPolyDataReader::New();
        readerB = legacyReaderB;

        legacyReaderA->SetFileName(argv[inputAParam]);
        legacyReaderB->SetFileName(argv[inputBParam]);
    }

    vtkSmartPointer<vtkHausdorffDistancePointSetFilter2> filter = vtkSmartPointer<vtkHausdorffDistancePointSetFilter2>::New();
    filter->SetInputConnection(readerA->GetOutputPort());
    filter->SetInputConnection(1,readerB->GetOutputPort());

    if( atoi(argv[targetParam]) )
        filter->SetTargetDistanceMethod( 1 );
    filter->Update();

    // SHIREEN
    std::vector<int> AtoBindices = filter->GetAtoBindices();
    std::vector<int> BtoAindices = filter->GetBtoAindices();

    std::ofstream AtoBidxfile(argv[outputAidx]);
    for(unsigned int ii = 0 ; ii < AtoBindices.size(); ii++)
        AtoBidxfile << AtoBindices[ii] << std::endl;
    AtoBidxfile.close();

    std::ofstream BtoAidxfile(argv[outputBidx]);
    for(unsigned int ii = 0 ; ii < BtoAindices.size(); ii++)
        BtoAidxfile << BtoAindices[ii] << std::endl;
    BtoAidxfile.close();
    // end SHIREEN

    if( !printOnlyParam )
    {
        vtkSmartPointer<vtkPolyDataWriter> polyDataWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
        polyDataWriter->SetInputConnection(filter->GetOutputPort(0));
        polyDataWriter->SetFileName(argv[outputAParam]);
        polyDataWriter->Update();

        polyDataWriter->SetInputConnection(filter->GetOutputPort(1));
        polyDataWriter->SetFileName(argv[outputBParam]);
        polyDataWriter->Update();
    }
    else
    {
        std::cout << static_cast<vtkPointSet*>(filter->GetOutput(0))->GetFieldData()->GetArray("HausdorffDistance")->GetComponent(0,0) << " "
                  << filter->GetOutputDataObject(0)->GetFieldData()->GetArray("RelativeDistanceAtoB")->GetComponent(0,0) << " "
                  << filter->GetOutputDataObject(1)->GetFieldData()->GetArray("RelativeDistanceBtoA")->GetComponent(0,0) << " "
                  << std::endl;
    }

    readerA->Delete( );
    readerB->Delete( );

    return( EXIT_SUCCESS );
}
