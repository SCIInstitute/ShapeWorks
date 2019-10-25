#include "TriMesh.h"
#include "TriMesh_algo.h"
#include "tinyxml.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkGradientImageFilter.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkPoint.h"

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "GenerateFeatureGradientFiles " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "Probe feature gradients at vertices and save as fea files " << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - mesh:      a list of ply file names for meshes to be processed" << std::endl;
        std::cout << "\t - fea_per_mesh:      number of fea files to be processed per mesh" << std::endl;
        std::cout << "\t - fea_vol:      a list of nrrd files to be processed such that the first fea_per_mesh nrrd files correspond to the first mesh and so on." << std::endl;

        std::cout << "Usage: " << argv[0] << "  parameter file" << std::endl;
        return EXIT_FAILURE;
    }

    typedef float  PixelType;
    typedef double DataType;
    typedef itk::Image<PixelType, 3> ImageType;
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typedef itk::ImageFileWriter< ImageType >  WriterType;
    typedef itk::Point<DataType, ImageType::ImageDimension> PointType;
    typedef itk::GradientImageFilter<ImageType> GradientImageFilterType;
    typedef typename GradientImageFilterType::OutputImageType GradientImageType;
    typedef itk::VectorLinearInterpolateImageFunction<GradientImageType, typename PointType::CoordRepType>
            GradientInterpolatorType;
    typedef itk::FixedArray<DataType, 3> VectorType;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;

        std::vector< std::string > meshFilename;
        std::vector< std::string > feaFilename;
        int fea_per_mesh;
        std::string tmpString;

        elem = docHandle.FirstChild( "mesh" ).Element();
        if (!elem)
        {
            std::cerr << "No mesh files have been specified" << std::endl;
            throw 1;
        }
        else
        {
            meshFilename.clear();
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                meshFilename.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        elem = docHandle.FirstChild( "fea_per_mesh" ).Element();
        if (elem)
        {
            fea_per_mesh = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "No fea_per_mesh specified!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "fea_vol" ).Element();
        if (!elem)
        {
            std::cerr << "No fea files have been specified" << std::endl;
            throw 1;
        }
        else
        {
            feaFilename.clear();
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                feaFilename.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        if (feaFilename.size() != (fea_per_mesh * meshFilename.size()))
        {
            std::cerr << "Number of fea files doesn't match number of given meshes x fea_per_mesh" << std::endl;
            throw 1;
        }

        int numShapes = meshFilename.size();

        for (unsigned int shCount = 0; shCount < numShapes; shCount++)
        {
            std::cout << "Reading mesh file: " << meshFilename[shCount] << std::endl;
            TriMesh *mesh = TriMesh::read( meshFilename[shCount].c_str() );
            orient(mesh);
            unsigned int numVert = mesh->vertices.size();

            for(unsigned int fId = 0 ; fId < fea_per_mesh; fId++)
            {
                std::string curfeaFilename = feaFilename[shCount*fea_per_mesh + fId];
                std::cout << "Reading fea volume File: " << curfeaFilename << std::endl;

                int index = curfeaFilename.rfind("/");
                std::string path  = curfeaFilename.substr(0, index+1);
                std::string file  = curfeaFilename.substr(index+1, curfeaFilename.length()-1);
                std::string gradientFileName;
                index = file.rfind(".");
                if (index == std::string::npos)
                {
                    gradientFileName = path + file + "_Gradient.fea";
                }
                else
                {
                    gradientFileName = path + file.substr(0, index) + "_Gradient.fea";
                }

                std::cout << "Feature gradient file name: " << gradientFileName << std::endl;

                std::ofstream outfile(gradientFileName.c_str(), std::ios::binary);
                std::cout << "# vertices in mesh: " << numVert << std::endl;
                outfile.write( reinterpret_cast<char *>(&numVert), sizeof(unsigned int) );

                ReaderType::Pointer reader = ReaderType::New();
                reader->SetFileName( curfeaFilename.c_str() );
                reader->Update();
                reader->UpdateLargestPossibleRegion();

                typename GradientImageFilterType::Pointer filter = GradientImageFilterType::New();
                filter->SetInput(reader->GetOutput());
                filter->SetUseImageSpacingOn();
                filter->Update();

                typename GradientInterpolatorType::Pointer gi = GradientInterpolatorType::New();
                gi->SetInputImage(filter->GetOutput());

                for (int i = 0; i < numVert; i++)
                {
                    PointType pt;
                    for (int vd = 0; vd < 3; vd++)
                        pt[vd] = mesh->vertices[i][vd];
                    VectorType gd = gi->Evaluate(pt);
                    for (int vd = 0; vd < 3; vd++)
                    {
                        float gdVal = gd[vd];
                        outfile.write(reinterpret_cast<char *>(&gdVal), sizeof(float) );
                    }
                }
                outfile.close();
            }
        }
    }
    else
    {
        std::cerr << "Failed at loading parameter file: " << argv[1] <<std::endl;
        throw 1;
    }
    return 1;
}
