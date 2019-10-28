#include "TriMesh.h"
#include "TriMesh_algo.h"
#include <iostream>
#include <string>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkGradientImageFilter.h"
#include "itkPoint.h"
#include <sstream>
#include <fstream>
#include "itkFixedArray.h"
#include "vnl/vnl_vector_fixed.h"
#include "tinyxml.h"

using namespace std;

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "GenerateNormalFeaFiles " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "Probe normals at vertices and save as fea files " << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - DT:      a list of DT file names to be processed" << std::endl;
        std::cout << "\t - mesh:    a list of mesh file names to be processed" << std::endl;
        std::cout << "Usage: " << argv[0] << "  parameter_file" << std::endl;
        return EXIT_FAILURE;
    }

    std::vector< std::string > imgFnames;
    std::vector< std::string > meshFnames;
    typedef  float  PixelType;
    typedef itk::Point<double, 3> PointType;
    typedef itk::FixedArray<PixelType, 3> VectorType;
    typedef vnl_vector_fixed<PixelType, 3> VnlVectorType;
    typedef itk::Image< PixelType,  3 >   ImageType;
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typedef itk::GradientImageFilter<ImageType> GradientImageFilterType;
    typedef typename GradientImageFilterType::OutputImageType GradientImageType;
    typedef itk::VectorLinearInterpolateImageFunction<GradientImageType, typename PointType::CoordRepType>
    GradientInterpolatorType;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string tmpString;

        elem = docHandle.FirstChild( "DT" ).Element();
        if (!elem)
        {
            std::cerr << "No DT files have been specified" << std::endl;
            throw 1;
        }
        else
        {
            imgFnames.clear();
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                imgFnames.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        elem = docHandle.FirstChild( "mesh" ).Element();
        if (!elem)
        {
            std::cerr << "No mesh files have been specified" << std::endl;
            throw 1;
        }
        else
        {
            meshFnames.clear();
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                meshFnames.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        if (meshFnames.size() != imgFnames.size())
        {
            std::cerr << "Inconsistent number of files..." << std::endl;
            throw 1;
        }
    }
    else
    {
        std::cerr << "Failed at loading parameter file: " << argv[1] <<std::endl;
        throw 1;
    }
    int numShapes = imgFnames.size();

    for (int i = 0; i < numShapes; i++)
    {
        int index = meshFnames[i].rfind("/");
        std::string path  = meshFnames[i].substr(0, index+1);
        std::string file  = meshFnames[i].substr(index+1, meshFnames[i].length()-1);
        std::string normalFileNameX;
        std::string normalFileNameY;
        std::string normalFileNameZ;
        index = file.rfind(".");
        if (index == std::string::npos)
        {
            normalFileNameX = path + file + ".NormalX.fea";
            normalFileNameY = path + file + ".NormalY.fea";
            normalFileNameZ = path + file + ".NormalZ.fea";
        }
        else
        {
            normalFileNameX = path + file.substr(0, index) + ".NormalX.fea";
            normalFileNameY = path + file.substr(0, index) + ".NormalY.fea";
            normalFileNameZ = path + file.substr(0, index) + ".NormalZ.fea";
        }

        std::cout << "Normal fea file names: " << std::endl;
        std::cout << normalFileNameX << std::endl;
        std::cout << normalFileNameY << std::endl;
        std::cout << normalFileNameZ << std::endl;

        std::ofstream outfileX(normalFileNameX.c_str(), std::ios::binary);
        std::ofstream outfileY(normalFileNameY.c_str(), std::ios::binary);
        std::ofstream outfileZ(normalFileNameZ.c_str(), std::ios::binary);

        std::cout << "Reading DT: " << imgFnames[i] << std::endl;

        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName( imgFnames[i].c_str() );
        reader->Update();
        reader->UpdateLargestPossibleRegion();

        typename GradientImageFilterType::Pointer filter = GradientImageFilterType::New();
        filter->SetInput(reader->GetOutput());
        filter->SetUseImageSpacingOn();
        filter->Update();

        typename GradientInterpolatorType::Pointer gradientInterpolator = GradientInterpolatorType::New();
        gradientInterpolator->SetInputImage(filter->GetOutput());

        std::cout << "Reading mesh file: " << meshFnames[i] << std::endl;
        TriMesh *mesh = TriMesh::read( meshFnames[i].c_str() );
        orient(mesh);
        unsigned int numVert = mesh->vertices.size();

        std::cout << "# vertices in mesh: " << numVert << std::endl;
        outfileX.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );
        outfileY.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );
        outfileZ.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

        for (int count = 0; count < numVert; count++)
        {
            PointType pt;
            for (int vd = 0; vd < 3; vd++)
                pt[vd] = mesh->vertices[count][vd];

            VectorType v = gradientInterpolator->Evaluate(pt);
            VnlVectorType n = VnlVectorType(v.GetDataPointer());
//            std::cout << n.magnitude() << std::endl;
            n.normalize();

            float nVal = n[0];
            outfileX.write(reinterpret_cast<char *>(&nVal), sizeof(float) );
            nVal = n[1];
            outfileY.write(reinterpret_cast<char *>(&nVal), sizeof(float) );
            nVal = n[2];
            outfileZ.write(reinterpret_cast<char *>(&nVal), sizeof(float) );
        }
        outfileX.close();
        outfileY.close();
        outfileZ.close();
    }
    return 0;
}
