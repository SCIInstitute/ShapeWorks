#include "math.h"
#include "tinyxml.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <climits>

// ITK Dependencies
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImage.h"
#include "itkImageRegionIteratorWithIndex.h"

const unsigned int dimensions = 3;

int dataslice(std::vector< std::string > inputpaths, std::vector< std::string > outputpaths, 
    std::vector<double> cpVals){
    
    std::string inputname;
    std::string outputname;
    double o[] = {0.0, 0.0, 0.0};
    double p1[] = {0.0, 0.0, 0.0};
    double p2[] = {0.0, 0.0, 0.0};
    double res[] = {0.0, 0.0, 0.0};
    
    typedef double PixelType;

    typedef itk::Image< PixelType, dimensions > InputImageType;
    typedef itk::Image< PixelType, dimensions > OutputImageType;
    double temp = 0.0;

    for(int i = 0; i < inputpaths.size(); i++){
        std::cout << "Data Number: " << i << std::endl;

        inputname = inputpaths[i];
        std::cout << inputname << std::endl;
        outputname = outputpaths[i];

        // read the input image
        typedef itk::ImageFileReader< InputImageType > ReaderType;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName(inputname.c_str());
        reader->Update();
        const InputImageType::SpacingType&
        spacing = reader->GetOutput()->GetSpacing();
        const InputImageType::PointType&
        origin  = reader->GetOutput()->GetOrigin();

        o[0] = (cpVals[i*9 + 0] - origin[0]) / spacing[0];
        o[1] = (cpVals[i*9 + 1] - origin[1]) / spacing[1];
        o[2] = (cpVals[i*9 + 2] - origin[2]) / spacing[2];
        p1[0] = (cpVals[i*9 + 3] - origin[0]) / spacing[0];
        p1[1] = (cpVals[i*9 + 4] - origin[1]) / spacing[1];
        p1[2] = (cpVals[i*9 + 5] - origin[2]) / spacing[2];
        p2[0] = (cpVals[i*9 + 6] - origin[0]) / spacing[0];
        p2[1] = (cpVals[i*9 + 7] - origin[1]) / spacing[1];
        p2[2] = (cpVals[i*9 + 8] - origin[2]) / spacing[2];

        InputImageType::Pointer image = reader->GetOutput();
        InputImageType::RegionType region = image->GetLargestPossibleRegion();
        // find the image dimensions
        InputImageType::SizeType size = region.GetSize();
        
        // find the cross product vector
        res[0] = (p1[1] - o[1])*(p2[2] - o[2]) - (p1[2] - o[2])*(p2[1] - o[1]);
        res[1] = (p1[2] - o[2])*(p2[0] - o[0]) - (p1[0] - o[0])*(p2[2] - o[2]);
        res[2] = (p1[0] - o[0])*(p2[1] - o[1]) - (p1[1] - o[1])*(p2[0] - o[0]);
        // normalize
        res[0] = res[0] / (sqrt(res[0]*res[0] + res[1]*res[1] + res[2]*res[2]));
        res[1] = res[1] / (sqrt(res[0]*res[0] + res[1]*res[1] + res[2]*res[2]));
        res[2] = res[2] / (sqrt(res[0]*res[0] + res[1]*res[1] + res[2]*res[2]));

        // set up iterator over the entire image
        itk::ImageRegionIteratorWithIndex<InputImageType> imageIterator(image,region);
        while(!imageIterator.IsAtEnd())
        {
            temp = (double(imageIterator.GetIndex()[0]) - o[0])*res[0] + 
                   (double(imageIterator.GetIndex()[1]) - o[1])*res[1] + 
                   (double(imageIterator.GetIndex()[2]) - o[2])*res[2];
            
            if(temp < 0.0 ){
                imageIterator.Set(0);
            }
            ++imageIterator;
        }

        // define the output
        typedef itk::ImageFileWriter< OutputImageType > WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetInput( image );
        writer->SetFileName(outputname.c_str());
        writer->Update();
    }
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "Slice 3D Volume Using Cutting Planes" << std::endl;
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "Takes set of .nrrd volumes and corresponding cutting planes, and chops the volume accordingly" << std::endl;
        std::cerr << "It uses a parameter file with the following tags" << std::endl;
        std::cerr << "- num_shapes: number of volumes to be processed (put this correctly)" << std::endl;
        std::cerr << "- inputs: paths to input files" << std::endl;
        std::cerr << "- outputs: paths to output files" << std::endl;
        std::cerr << "- cutting_planes : Set of cutting planes, expressed as 3 3D points separated by spaces" << std::endl;
        std::cerr << "example : 1 2 -3 0 34 102 -23.4 2 0.44" << std::endl;
        return EXIT_FAILURE;
    }

    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;
    std::istringstream inputsBuffer;
    std::string tmpString;
    std::vector< std::string > inputFilePaths;
    std::vector< std::string > outputFilePaths;
    std::vector<double> cpVals;

    int numShapes = 0;

    if(loadOkay)
    {	

        elem = docHandle.FirstChild( "num_shapes" ).Element();
        if (elem)
        {
            numShapes = atoi( elem->GetText() );
        }
        else
        {
            std::cerr << "ERROR: Specify number of shapes" << std::endl;
            throw  1;
        }


    	// otherwise read separate cutting plane for every shape
            elem = docHandle.FirstChild( "cutting_planes" ).Element();
            if (elem)
            {
                inputsBuffer.str(elem->GetText());

                double pt;

                while (inputsBuffer >> pt)
                {
                    cpVals.push_back(pt);
                }
                inputsBuffer.clear();
                inputsBuffer.str("");

                if (cpVals.size() < 9*numShapes)
                {
                    std::cerr << "ERROR: Incomplete cutting plane data! Terminating ..." << std::endl;
                    throw 1;
                }
                
            }

        // read input mesh paths
        elem = docHandle.FirstChild( "inputs" ).Element();
        if (!elem)
        {
            std::cerr << "ERROR: No input files have been specified" << std::endl;
            throw 1;
        }
        else
        {	
        	
            // mesh input path read
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                inputFilePaths.push_back(tmpString);
                // std::cout << tmpString <<std::endl;
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        // read input mesh paths
        elem = docHandle.FirstChild( "outputs" ).Element();
        if (!elem)
        {
            std::cerr << "ERROR: No input files have been specified" << std::endl;
            throw 1;
        }
        else
        {	
        	
            // mesh input path read
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> tmpString)
            {
                outputFilePaths.push_back(tmpString);
                // std::cout << tmpString <<std::endl;
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
    }
    else
    {
        std::cerr << "ERROR: param file can not be opened ..." << std::endl;
        throw 1;
    }

    // perform the data chopping
    dataslice(inputFilePaths, outputFilePaths, cpVals);

    return EXIT_SUCCESS;
}
