#include "TriMesh.h"
#include "meshFIM.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "itkImage.h"
#include "itkImageFileReader.h"


int main(int argc, char *argv[])
{
    if (argc != 6)
    {
        std::cout << "Usage: " << argv[0] << " plyMeshFilePath fidsFilePath DTFilePath lptsFilePath outFilePath" << std::endl;
        return EXIT_FAILURE;
    }
    std::string plyMesh  = std::string(argv[1]);
    std::string fidsFile = std::string(argv[2]);
    std::string ptFile   = std::string(argv[4]);
    std::string outFile  = std::string(argv[5]);
    std::string dtFile   = std::string(argv[3]);

    typedef  float  PixelType;
    typedef itk::Image< PixelType,  3 >   ImageType;
    typedef itk::ImageFileReader< ImageType >  ReaderType;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( dtFile.c_str() );
    reader->Update();
    reader->UpdateLargestPossibleRegion();

    ImageType::PointType origin    = reader->GetOutput()->GetOrigin();
    ImageType::SpacingType spacing = reader->GetOutput()->GetSpacing();
    ImageType::SizeType size       = reader->GetOutput()->GetLargestPossibleRegion().GetSize();


    TriMesh *mesh = TriMesh::read( plyMesh.c_str() );
    orient(mesh);

    mesh->imageOrigin[0] = origin[0];
    mesh->imageOrigin[1] = origin[1];
    mesh->imageOrigin[2] = origin[2];

    mesh->imageSpacing[0] = spacing[0];
    mesh->imageSpacing[1] = spacing[1];
    mesh->imageSpacing[2] = spacing[2];

    mesh->imageSize[0] = size[0] ;
    mesh->imageSize[1] = size[1] ;
    mesh->imageSize[2] = size[2] ;

    mesh->ReadFaceIndexMap(fidsFile.c_str());
    std::ifstream pointsFile(ptFile.c_str());
    std::ofstream outfile(outFile.c_str());
    int count = 0;
    if (!pointsFile)
    {
        std::cout << "point file does not exist... " << pointsFile << std::endl;
        return EXIT_FAILURE;
    }
    while(pointsFile)
    {
        count++;
        point tmpPt; tmpPt.clear();
        pointsFile >> tmpPt[0];
        if (!pointsFile)
        {
            count--;
            break;
        }
        pointsFile >> tmpPt[1];
        pointsFile >> tmpPt[2];
        TriMesh::Face ptface;
        std::cout << count << " " << std::endl;
        std::vector<float> baryC;
        baryC.resize(3);
        mesh->GetTriangleInfoForPoint(tmpPt, ptface, baryC[0], baryC[1], baryC[2]);
        point newPt;
        for (int d = 0; d < 3; d++)
        {
            newPt[d] = 0.0;
            for (int g = 0; g < 3; g++)
            {
                newPt[d] += baryC[g]*mesh->vertices[ptface.v[g]][d];
            }
        }
        outfile << newPt[0] << " " << newPt[1] << " " << newPt[2] << std::endl;
    }
    pointsFile.close();
    outfile.close();
    std::cout << "Total number of points: " << count << std::endl;
    return 0;
}
