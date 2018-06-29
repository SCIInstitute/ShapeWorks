
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkChangeInformationImageFilter.h"
#include "string.h"
#include "itkBSplineInterpolateImageFunction.h"

#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that antialiases binary volumes ....";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("The filename of the input image to be resampled.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output resampled image.");
    parser.add_option("--maxRMSError").action("store").type("float").set_default(0.01).help("The maximum RMS error allowed.");
    parser.add_option("--numIterations").action("store").type("int").set_default(50).help("Number of iterations.");

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

int main( int argc, char * argv[] )
{
    optparse::OptionParser parser = buildParser();
    optparse::Values & options = parser.parse_args(argc,argv);
    std::vector<std::string> args = parser.args();

    if(argc < 4)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }

    std::string inFilename    = (std::string) options.get("inFilename");
    std::string outFilename   = (std::string) options.get("outFilename");
    float maxRMSErr = (float) options.get("maxRMSError");
    int numIter = (int) options.get("numIterations");

    typedef   float InputPixelType;
    typedef   float InternalPixelType;
    typedef   float OutputPixelType;

    const     unsigned int    Dimension = 3;
    typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
    typedef itk::Image< OutputPixelType,    Dimension >   OutputImageType;
    typedef itk::Image< InternalPixelType, Dimension >   InternalImageType;
    typedef itk::ImageFileReader< InputImageType  >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inFilename );
    InputImageType::ConstPointer inputImage = reader->GetOutput();

    typedef itk::AntiAliasBinaryImageFilter< InternalImageType, InternalImageType > FilterType;
    FilterType::Pointer antialiasFilter = FilterType::New();
    antialiasFilter->SetInput( reader->GetOutput() );
    antialiasFilter->SetMaximumRMSError(maxRMSErr);
    antialiasFilter->SetNumberOfIterations( numIter );
    antialiasFilter->Update();

    // Write the final output

    typedef itk::ImageFileWriter< OutputImageType > WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetInput( antialiasFilter->GetOutput() );
    writer->SetFileName(outFilename.c_str());
    writer->Update();
    
    return EXIT_SUCCESS;
}