#include "math.h"
#include <iostream>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>
#include <climits>

// ITK Dependencies
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkImage.h"

const unsigned int dimensions = 3;

int binarythreshold(std::string inputpath, std::string outputpath, 
    float loT, float upT, float outVal, float inVal){
	
	typedef double PixelType;

    typedef itk::Image< PixelType, dimensions > InputImageType;
    typedef itk::Image< PixelType, dimensions > OutputImageType;

    // read the input image
    typedef itk::ImageFileReader< InputImageType > ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inputpath.c_str());

    // read the parameters 
    PixelType lowerThreshold = static_cast< PixelType >( loT );
    PixelType upperThreshold = static_cast< PixelType >( upT );
    typedef itk::BinaryThresholdImageFilter< InputImageType, InputImageType > BinaryThresholdImageType;
    BinaryThresholdImageType::Pointer threshold = BinaryThresholdImageType::New();
    threshold->SetInput( reader->GetOutput() );
    threshold->SetLowerThreshold( lowerThreshold );
    threshold->SetUpperThreshold( upperThreshold );
    threshold->SetOutsideValue( outVal );
    threshold->SetInsideValue( inVal );

    // define the output
    typedef itk::ImageFileWriter< OutputImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( threshold->GetOutput() );
    writer->SetFileName(outputpath.c_str());
    writer->Update();
    
	return EXIT_SUCCESS;
}

#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that threholds a given image into a binary label based on upper and lower intensity bounds given by the user.....";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("Input image file path .");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output threshold image.");
    parser.add_option("--lowerThresholdLevel").action("store").type("float").set_default(-1 * std::numeric_limits<float>::max()).help("The lower threshold level (optional, default = FLT_MIN)");
    parser.add_option("--upperThresholdLevel").action("store").type("float").set_default(std::numeric_limits<float>::max()).help("The upper threshold level (optional, default = FLT_MAX)");
    parser.add_option("--insideValue").action("store").type("float").set_default(1).help("The inside pixel value after threshold");
    parser.add_option("--outsideValue").action("store").type("float").set_default(0).help("The outside pixel value after threshold");

    return parser;
}

template<typename TImage>
void DeepCopy(typename TImage::Pointer input, typename TImage::Pointer output)
{
    output->SetRegions(input->GetLargestPossibleRegion());
    output->Allocate();

    itk::ImageRegionConstIterator<TImage> inputIterator(input, input->GetLargestPossibleRegion());
    itk::ImageRegionIterator<TImage> outputIterator(output, output->GetLargestPossibleRegion());

    while(!inputIterator.IsAtEnd())
    {
        outputIterator.Set(inputIterator.Get());
        ++inputIterator;
        ++outputIterator;
    }
}

int main(int argc, char *argv[])
{
    // read the command line parameters using the option parser
    optparse::OptionParser parser = buildParser();
    optparse::Values & options = parser.parse_args(argc,argv);
    std::vector<std::string> args = parser.args();

    if(argc < 2)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }

    std::string inFilename    = (std::string) options.get("inFilename");
    std::string outFilename   = (std::string) options.get("outFilename");
    float loT   = (float) options.get("lowerThresholdLevel");
    float upT   = (float) options.get("upperThresholdLevel");
    float outVal = (float) options.get("outsideValue");
    float inVal = (float) options.get("insideValue");
    // perform the segmentation
    binarythreshold(inFilename, outFilename, loT, upT, outVal, inVal);

    return EXIT_SUCCESS;
}
