#include "TriMesh.h"
#include "meshFIM.h"
#include "math.h"
#include "tinyxml.h"
#include <iostream>
#include <sstream>
#include <string>
#include "vnl/vnl_vector.h"

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"


#define _USE_MATH_DEFINES

std::string num2str(float num)
{
    std::string Result;          // string which will contain the result
    std::ostringstream convert;   // stream used for the conversion
    convert << num;      // insert the textual representation of 'Number' in the characters in the stream
    Result = convert.str(); // set 'Result' to the contents of the stream
    return Result;
}

std::string GetFileExtension(const std::string& FileName)
{
    if(FileName.find_last_of(".") != std::string::npos)
        return FileName.substr(FileName.find_last_of(".")+1);
    return "";
}


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "GetFeatureVolume " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "Compute a volumetric representation of fea files by propagating feature values (from fea files) from mesh surface to a narrowband surrounding the mesh " << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - with_fids: a flag to indicate whether to use fids to generate the geodesics, if 0, kdtree will be used instead " << std::endl;
        std::cout << "\t - mesh:      a list of ply file names for meshes to be processed" << std::endl;
        std::cout << "\t - fids:      a list of corresponding fids files (in case with_fids = 1)" << std::endl;
        std::cout << "\t - dist:       a list of corresponding distance transforms to indicate where to fill the feature volume, i.e. the narrowband" << std::endl;
        std::cout << "\t - fea_per_mesh:      number of fea files to be processed per mesh" << std::endl;
        std::cout << "\t - fea:      a list of fea files to be processed such that the first fea_per_mesh fea files correspond to the first mesh and so on." << std::endl;
        std::cout << "\t - narrow_band:     distance in physical coordinates from the mesh its inside and outside where we propagate feature values." << std::endl;


        std::cout << "Usage: " << argv[0] << "  parameter file" << std::endl;
        return EXIT_FAILURE;
    }

    typedef  float  PixelType;
    typedef itk::Image< PixelType,  3 >   ImageType;
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typedef itk::BinaryThresholdImageFilter< ImageType, ImageType >  FilterType;
    typedef itk::ImageFileWriter< ImageType >  WriterType;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;

    bool withFids = false; // if false use kdtree instead,
    std::vector< std::string > meshFilename;
    std::vector< std::string > fidsFilename;
    std::vector< std::string > distFilename;
    std::vector< std::string > feaFilename;
    int fea_per_mesh;
    float narrow_band = 3.0;
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


    elem = docHandle.FirstChild( "with_fids" ).Element();
    if (elem)
    {
        atoi(elem->GetText()) > 0 ? withFids = true : withFids = false;
    }

    if(withFids)
    {
        elem = docHandle.FirstChild( "fids" ).Element();
        if (!elem)
        {
            std::cerr << "No fids files have been specified and with_fids is triggered" << std::endl;
            throw 1;
        }
        else
        {
            fidsFilename.clear();
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                fidsFilename.push_back(tmpString);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        if (meshFilename.size() != fidsFilename.size())
        {
            std::cerr << "Number of fids files doesn't match number of given meshes" << std::endl;
            throw 1;
        }
    }


    elem = docHandle.FirstChild( "dist" ).Element();
    if (!elem)
    {
        std::cerr << "No dist files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        distFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            distFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    if (meshFilename.size() != distFilename.size())
    {
        std::cerr << "Number of dist files doesn't match number of given meshes" << std::endl;
        throw 1;
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

    elem = docHandle.FirstChild( "fea" ).Element();
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

    elem = docHandle.FirstChild( "narrow_band" ).Element();
    if (elem)
    {
        narrow_band = atof( elem->GetText() );
    }

    int numShapes = meshFilename.size();
    for (unsigned int shCount = 0; shCount < numShapes; shCount++)
    {
        // read mesh
        std::cout << "Reading mesh file: " << meshFilename[shCount] << std::endl;
        TriMesh *mesh = TriMesh::read( meshFilename[shCount].c_str() );
        orient(mesh);

        if(withFids)
        {
            std::cout << "Reading fids File: " << fidsFilename[shCount] << std::endl;
            mesh->ReadFaceIndexMap(fidsFilename[shCount].c_str());
        }

        std::cout << "Reading DT File: " << distFilename[shCount] << std::endl;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName( distFilename[shCount].c_str() );
        reader->Update();
        reader->UpdateLargestPossibleRegion();

        ImageType::PointType origin    = reader->GetOutput()->GetOrigin();
        ImageType::SpacingType spacing = reader->GetOutput()->GetSpacing();
        ImageType::SizeType size       = reader->GetOutput()->GetLargestPossibleRegion().GetSize();

        if(withFids)
        {
            mesh->imageOrigin[0] = origin[0];
            mesh->imageOrigin[1] = origin[1];
            mesh->imageOrigin[2] = origin[2];

            // Store spacing of image domain
            mesh->imageSpacing[0] = spacing[0];
            mesh->imageSpacing[1] = spacing[1];
            mesh->imageSpacing[2] = spacing[2];

            // Store size of image domain
            mesh->imageSize[0] = size[0] ;
            mesh->imageSize[1] = size[1] ;
            mesh->imageSize[2] = size[2] ;
        }

        std::cout << "origin:" << origin << std::endl;
        std::cout << "spacing:" << spacing << std::endl;
        std::cout << "size:" << size << std::endl;


        for(unsigned int fId = 0 ; fId < fea_per_mesh; fId++)
        {
            std::cout << "filter narrow band... ";
            FilterType::Pointer filter = FilterType::New();
            filter->SetInput( reader->GetOutput() );
            filter->SetOutsideValue( 0.0f );
            filter->SetInsideValue(1.0f);
            filter->SetLowerThreshold( -1.0f * narrow_band );
            filter->SetUpperThreshold(1.0f * narrow_band);
            filter->Update();
            std::cout << "done." << std::endl;

            std::string curfeaFilename = feaFilename[shCount*fea_per_mesh + fId];

            std::cout << "Reading fea File: " << curfeaFilename << std::endl;
            mesh->ReadFeatureFromFile( curfeaFilename.c_str() );

            ImageType::Pointer narrowbandVolume = filter->GetOutput();
            itk::ImageRegionIteratorWithIndex<ImageType> it(narrowbandVolume,narrowbandVolume->GetLargestPossibleRegion());
            for (it.GoToBegin(); !it.IsAtEnd(); ++it)
            {
                if ( it.Get() > 0 )
                {
                    //                    ImageType::PointType dtPoint;
                    //                    narrowbandVolume->TransformIndexToPhysicalPoint(it.GetIndex(),dtPoint);
                    //                    point mPoint;
                    //                    mPoint[0] = dtPoint[0];
                    //                    mPoint[1] = dtPoint[1];
                    //                    mPoint[2] = dtPoint[2];

                    ImageType::IndexType idx = it.GetIndex();
                    int idx_[3];
                    idx_[0] = idx[0]; idx_[1] = idx[1]; idx_[2] = idx[2];
                    point mPoint = mesh->indexToPhysicalPoint(idx_);

                    float feaValue = mesh->GetFeatureValue(mPoint,fId) + 0.0001f;
                    // SHIREEN - allow negative features, not all features are distances
                    //                    if (feaValue < 0.0f) {
                    //                        std::cout << feaValue << std::endl;
                    //                        std::cout << "bug\n";
                    //                    }
                    //it.Set( feaValue < 0.0 ? -1.0*feaValue : feaValue );
                    it.Set( feaValue );
                }
                else
                {
                    it.Set( 0.000001f );
                }
            }

            // get name of output file
            int index = curfeaFilename.rfind("/");
            std::string path  = curfeaFilename.substr(0, index+1);
            std::string file  = curfeaFilename.substr(index+1, curfeaFilename.length()-1);
            std::string featureFileName;

            int index_dt        = distFilename[shCount].rfind("/");
            std::string file_dt = distFilename[shCount].substr(index_dt+1, distFilename[shCount].length()-1);
            std::string ext     = GetFileExtension(file_dt);

            index = file.rfind(".");
            if (index == std::string::npos)
            {
                featureFileName = path + file + "_nb" + num2str(narrow_band) + "." + ext;
            }
            else
            {
                featureFileName = path + file.substr(0, index) + "_nb" + num2str(narrow_band) + "." + ext;
            }

            std::cout << "feature volume filename: " << featureFileName << std::endl;

            WriterType::Pointer writer = WriterType::New();
            writer->SetInput( narrowbandVolume );
            writer->SetFileName( featureFileName );
            writer->Update();

        }

    }

    return EXIT_SUCCESS;
}
