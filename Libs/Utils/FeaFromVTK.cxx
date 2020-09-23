#include <vtkPolyDataReader.h>
#include <vtkPolyDataWriter.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkPointData.h>
#include <vtkInformation.h>
#include <vtkDataSetAttributes.h>
#include <fstream>
#include <stdio.h>
#include <sstream>
#include <string>
#include "tinyxml.h"

int main(int argc, char *argv[])
{
    if( argc < 2 )
    {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }
//    std::vector< std::string > feature_name;
    std::vector< std::string > inFileName;
    std::vector< std::string > feaFilename;
//    int fea_per_mesh;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    if(loadOkay)
    {
        TiXmlHandle docHandle(&doc);
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string filename;

        elem = docHandle.FirstChild( "input" ).Element();
        if(elem)
        {
            inFileName.clear();
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
                inFileName.push_back(filename);
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No inputs to process!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "output" ).Element();
        if(elem)
        {
            feaFilename.clear();
            filename.clear();
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
                feaFilename.push_back(filename);
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

//        elem = docHandle.FirstChild( "feature_name" ).Element();
//        if(elem)
//        {
//            feature_name.clear();
//            filename.clear();
//            inputsBuffer.str(elem->GetText());
//            while (inputsBuffer >> filename)
//                feature_name.push_back(filename);
//            inputsBuffer.clear();
//            inputsBuffer.str("");
//            fea_per_mesh = feature_name.size();
//        }
//        else
//        {
//            std::cerr << "feature name is not provided!!!\n";
//            return EXIT_FAILURE;
//        }

        if (feaFilename.size() != inFileName.size())
        {
            std::cerr << "size mismatch..." << std::endl;
            return EXIT_FAILURE;
        }

    }
    else
    {
        std::cout<<"xml load failure"<<std::endl;
        return EXIT_FAILURE;
    }

    for (int i = 0; i < inFileName.size(); i++)
    {
        vtkSmartPointer<vtkPolyDataReader> reader_pd = vtkSmartPointer<vtkPolyDataReader>::New();

        vtkSmartPointer<vtkPolyData> inp = vtkSmartPointer<vtkPolyData>::New();

        reader_pd->SetFileName( inFileName[i].c_str() );
        reader_pd->Update();
        inp->DeepCopy( reader_pd->GetOutput() );
        //inp->Update();
        unsigned int numVert = inp->GetNumberOfPoints();
        vtkDataArray *scalars = inp->GetPointData()->GetScalars();
	std::ofstream outfile(feaFilename[i].c_str(), std::ios::binary);

        outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

        for (int i1 = 0; i1 < numVert; i1++)
        {
            float feaValue = (float)scalars->GetComponent(i1, 0);
            outfile.write( reinterpret_cast<char *>(&feaValue), sizeof(float) );
        }

        outfile.close();
        std::cout << "Done\n";

    }
    return EXIT_SUCCESS;
}
