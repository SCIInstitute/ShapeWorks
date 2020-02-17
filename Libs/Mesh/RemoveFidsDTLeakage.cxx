#include "tinyxml.h"
#include <iostream>
#include <sstream>
#include <string>

#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkImageDuplicator.h"
#include "itkImageRegionIterator.h"
#define _USE_MATH_DEFINES


int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        std::cout << "-------------------------------" << std::endl;
        std::cout << "RemoveFidsDTLeakage " << std::endl;
        std::cout << "-------------------------------" << std::endl;

        std::cout << "This tool can be used a postprocessing for fids distance trasnform to fix voxels that are mis-signed as in or out of the isosurface in fids computation (mainly due to irregular triangulation " << std::endl;

        std::cout << "It uses a parameter file with the following tags" << std::endl;
        std::cout << "\t - fids_dist:  a list of distance transforms computed via fids" << std::endl;
        std::cout << "\t - approx_dist:       the corresponding approximate distances (from rasterization then dt computation)" << std::endl;
        std::cout << "\t - out_dist:       output distance transform filenames" << std::endl;

        std::cout << "Usage: " << argv[0] << "  parameter file" << std::endl;
        return EXIT_FAILURE;
    }

    typedef  float  PixelType;
    typedef itk::Image< PixelType,  3 >   ImageType;
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typedef itk::ImageFileWriter< ImageType >  WriterType;

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;

    bool withFids = false; // if false use kdtree instead,
    std::vector< std::string > fidsDistFilename;
    std::vector< std::string > approxDistFilename;
    std::vector< std::string > outDistFilename;

    std::string tmpString;

    elem = docHandle.FirstChild( "fids_dist" ).Element();
    if (!elem)
    {
        std::cerr << "No fids_dist files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        fidsDistFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            fidsDistFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "approx_dist" ).Element();
    if (!elem)
    {
        std::cerr << "No approx_dist files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        approxDistFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            approxDistFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "out_dist" ).Element();
    if (!elem)
    {
        std::cerr << "No out_dist files have been specified" << std::endl;
        throw 1;
    }
    else
    {
        outDistFilename.clear();
        inputsBuffer.str(elem->GetText());
        while (inputsBuffer >> tmpString)
        {
            outDistFilename.push_back(tmpString);
        }
        inputsBuffer.clear();
        inputsBuffer.str("");
    }


    int numShapes = fidsDistFilename.size();
    for (unsigned int shCount = 0; shCount < numShapes; shCount++)
    {
        std::cout << "Reading fids DT File: " << fidsDistFilename[shCount] << std::endl;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName( fidsDistFilename[shCount].c_str() );
        reader->Update();
        reader->UpdateLargestPossibleRegion();
        ImageType::Pointer fidsDist = reader->GetOutput();

        std::cout << "Reading approximate DT File: " << approxDistFilename[shCount] << std::endl;
        ReaderType::Pointer reader2 = ReaderType::New();
        reader2->SetFileName( approxDistFilename[shCount].c_str() );
        reader2->Update();
        reader2->UpdateLargestPossibleRegion();
        ImageType::Pointer approxDist = reader2->GetOutput();

        // copying fids distance to output distance
        typedef itk::ImageDuplicator< ImageType > DuplicatorType;
        DuplicatorType::Pointer duplicator = DuplicatorType::New();
        duplicator->SetInputImage(fidsDist);
        duplicator->Update();
        ImageType::Pointer outDist = duplicator->GetOutput();

        //ImageType::Pointer outDist  = ImageType::New();
        //outDist->SetRegions(fidsDist->GetLargestPossibleRegion());
        //outDist->Allocate();

        itk::ImageRegionConstIterator<ImageType> fidsIterator(fidsDist, fidsDist->GetLargestPossibleRegion());
        itk::ImageRegionConstIterator<ImageType> approxIterator(approxDist, approxDist->GetLargestPossibleRegion());
        itk::ImageRegionIterator<ImageType>      outIterator(outDist, outDist->GetLargestPossibleRegion());

        std::cout << "Fixing fids distance transform ...." << std::endl;
        while(!fidsIterator.IsAtEnd())
        {
            float fidsVal   = fidsIterator.Get();
            float approxVal = approxIterator.Get();

            if (fabs(fidsVal) > 0.1) // fudge region around the isosurface
            {
                if (fidsVal * approxVal < 0) // opposite sign
                {
                    std::cout << "fixing fids sign ..." << std::endl << std::flush;
                    //outIterator.Set(approxVal);
                    outIterator.Set(-fidsVal);
                }
                //else
                //    outIterator.Set(fidsVal);
            }

            ++fidsIterator;
            ++approxIterator;
            ++outIterator;
        }

        std::cout << "Writing: " << outDistFilename[shCount] << std::endl;
        WriterType::Pointer writer = WriterType::New();
        writer->SetInput( outDist );
        writer->SetFileName( outDistFilename[shCount].c_str() );
        writer->SetUseCompression(true);
        writer->Update();
    }

    return EXIT_SUCCESS;
}
