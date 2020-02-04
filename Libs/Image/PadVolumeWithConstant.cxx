#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkImageRegionIterator.h"
#include "string.h"
#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>

#include "itkConstantPadImageFilter.h"

#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that pad a contant value in the x-, y-, and z-directions of a given volume....";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("Image file to be padded.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output padded image.");
    parser.add_option("--paddingSize").action("store").type("int").set_default(0).help("Number of voxels to be padded in each direction.");
    parser.add_option("--paddingValue").action("store").type("float").set_default(0).help("Value to be used to fill padded voxels.");

    return parser;
}


//this has been added for image region iteration as the image was not updated even after set
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

template<typename InImageType, typename OutImageType>
void DeepCopy2(typename InImageType::Pointer input, typename OutImageType::Pointer output)
{
    output->SetRegions(input->GetLargestPossibleRegion());
    output->Allocate();

    itk::ImageRegionConstIterator<InImageType> inputIterator(input, input->GetLargestPossibleRegion());
    itk::ImageRegionIterator<OutImageType> outputIterator(output, output->GetLargestPossibleRegion());

    while(!inputIterator.IsAtEnd())
    {
        outputIterator.Set(inputIterator.Get());
        ++inputIterator;
        ++outputIterator;
    }
}


int main( int argc, char * argv[] )
{
    optparse::OptionParser parser = buildParser();
    optparse::Values & options = parser.parse_args(argc,argv);
    std::vector<std::string> args = parser.args();

    if(argc < 3)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }

    std::string inFilename    = (std::string) options.get("inFilename");
    std::string outFilename   = (std::string) options.get("outFilename");
    int         paddingSize   = (int) options.get("paddingSize");
    float       paddingValue  = (float) options.get("paddingValue");


    //typedef   unsigned char InputPixelType;
    //typedef   unsigned char OutputPixelType;

    // ANTs output float-type images even with nearest neighbor resampling
    typedef   float InputPixelType;
    typedef   float OutputPixelType;

    const     unsigned int    Dimension = 3;
    typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
    typedef itk::Image< OutputPixelType,   Dimension >   OutputImageType;

    typedef itk::ImageFileReader< InputImageType  >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    std::cout<<"Padding: " << inFilename <<std::endl;
    reader->SetFileName( inFilename );

    try
    {
        reader->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught!" << std::endl;
        std::cerr << excep << std::endl;
    }

    typedef itk::ConstantPadImageFilter <InputImageType, OutputImageType> ConstantPadImageFilterType;

    OutputImageType::SizeType lowerExtendRegion;
    lowerExtendRegion[0] = paddingSize;
    lowerExtendRegion[1] = paddingSize;
    lowerExtendRegion[2] = paddingSize;

    OutputImageType::SizeType upperExtendRegion;
    upperExtendRegion[0] = paddingSize;
    upperExtendRegion[1] = paddingSize;
    upperExtendRegion[2] = paddingSize;

    OutputImageType::PixelType constantPixel = paddingValue;

    ConstantPadImageFilterType::Pointer padFilter
            = ConstantPadImageFilterType::New();
    padFilter->SetInput(reader->GetOutput());
    //padFilter->SetPadBound(outputRegion); // Calls SetPadLowerBound(region) and SetPadUpperBound(region)
    padFilter->SetPadLowerBound(lowerExtendRegion);
    padFilter->SetPadUpperBound(upperExtendRegion);
    padFilter->SetConstant(constantPixel);
    padFilter->Update();

    typedef itk::ImageFileWriter< OutputImageType  >  WriterType;
    WriterType::Pointer writer = WriterType::New();
    std::cout<<"Writing: " << outFilename <<std::endl;
    writer->SetFileName( outFilename );
    writer->SetInput(padFilter->GetOutput());
    writer->SetUseCompression(true);
    
    try
    {
        writer->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught!" << std::endl;
        std::cerr << excep << std::endl;
    }

    return EXIT_SUCCESS;

}
