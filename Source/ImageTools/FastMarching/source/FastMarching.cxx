
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itksys/SystemTools.hxx"
#include "itkReinitializeLevelSetImageFilter.h" // for distance transform computation

#include "string.h"
#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that computes distance transform volume from a binary (antialiased) image ...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("The filename of the input image to be processed.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output distance transform image.");
    parser.add_option("--isoValue").action("store").type("float").set_default(0.0).help("The level set value that defines the interface between foreground and background.");
=
    return parser;
}



int main(int argc, char * argv[])
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
    float isoValue = (float) options.get("isoValue");

    const int VDimension = 3;

    // Setup types
    typedef float ScalarType;
    typedef itk::Image<ScalarType, VDimension> ScalarImageType;

    typedef itk::ImageFileReader< ScalarImageType >  ReaderType;
    typedef itk::ImageFileWriter< ScalarImageType >  WriterType;    
    typedef itk::ReinitializeLevelSetImageFilter< ScalarImageType > ReinitializeLevelSetImageFilterType;

    // Create and setup a reader
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( inFilename.c_str() );

    typename ReinitializeLevelSetImageFilterType::Pointer distanceMapImageFilter = ReinitializeLevelSetImageFilterType::New();
    distanceMapImageFilter->SetInput(reader->GetOutput());
    distanceMapImageFilter->NarrowBandingOff();
    distanceMapImageFilter->SetLevelSetValue(isoValue);
    distanceMapImageFilter->Update();

    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName(outFilename.c_str());
    writer->SetInput( distanceMapImageFilter->GetOutput() );
    writer->Update();
    
    return EXIT_SUCCESS;
}
