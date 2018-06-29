#include "itkImage.h"
#include <itkImageFileReader.h>
#include <itkExtractImageFilter.h>
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkIndex.h"


#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that performs translational alignment of a given shape image based on either its center of mass or a given 3d point...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("The filename of the input shape to be cropped.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output cropped shape.");
    parser.add_option("--MRIinFilename").action("store").type("string").set_default("").help("The assoicated image filename to be cropped.");
    parser.add_option("--MRIoutFilename").action("store").type("string").set_default("").help("The filename of the output cropped image.");

    parser.add_option("--bbX").action("store").type("int").set_default(0.0).help("bounding box value in X direction.");
    parser.add_option("--bbY").action("store").type("float").set_default(0.0).help("bounding box value in Y direction.");
    parser.add_option("--bbZ").action("store").type("float").set_default(0.0).help("bounding box value in Z direction.");

    parser.add_option("--startingIndexX").action("store").type("float").set_default(0.0).help("starting index in X direction.");
    parser.add_option("--startingIndexY").action("store").type("float").set_default(0.0).help("starting index in Y direction.");
    parser.add_option("--startingIndexZ").action("store").type("float").set_default(0.0).help("starting index in Z direction.");

    return parser;
}



int main(int argc, char * argv[] )
{
    optparse::OptionParser parser = buildParser();
    optparse::Values & options = parser.parse_args(argc,argv);
    std::vector<std::string> args = parser.args();

    if(argc < 9)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }

    std::string inFilename         = (std::string) options.get("inFilename");
    std::string outFilename        = (std::string) options.get("outFilename");
    std::string MRIinFilename      = (std::string) options.get("MRIinFilename");
    std::string MRIoutFilename     = (std::string) options.get("MRIoutFilename");

    float bbX    = (float) options.get("bbX");
    float bbY    = (float) options.get("bbY");
    float bbZ    = (float) options.get("bbZ");

    float startingIndexX    = (float) options.get("startingIndexX");
    float startingIndexY    = (float) options.get("startingIndexY");
    float startingIndexZ    = (float) options.get("startingIndexZ");

    typedef   float InputPixelType;
    typedef   float InternalPixelType;
    typedef   float OutputPixelType;

    const     unsigned int    Dimension = 3;
    typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
    typedef itk::Image< OutputPixelType,   Dimension >   OutputImageType;
    typedef itk::ImageFileReader< InputImageType  >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName(inFilename);
    try
    {
        reader->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught!" << std::endl;
        std::cerr << excep << std::endl;
    }

    InputImageType::IndexType index;
    index[0]=startingIndexX;
    index[1]=startingIndexY;
    index[2]=startingIndexZ;
    InputImageType::SizeType desiredSize;
    desiredSize[0]=bbX;
    desiredSize[1]=bbY;
    desiredSize[2]=bbZ;

    InputImageType::RegionType desiredRegion(index, desiredSize);

    typedef itk::ExtractImageFilter< InputImageType, OutputImageType > FilterType;
    FilterType::Pointer filter = FilterType::New();
    filter->SetExtractionRegion(desiredRegion);
    filter->SetInput(reader->GetOutput());

#if ITK_VERSION_MAJOR >= 4
    filter->SetDirectionCollapseToIdentity(); // This is required.
#endif
    try
    {
        filter->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

    InputImageType::Pointer output = filter->GetOutput();
    output->DisconnectPipeline();

    typedef itk::ImageFileWriter< OutputImageType >  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outFilename);
    writer->SetInput(output);

    try
    {
        writer->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

    if(MRIinFilename!="")
    {
        typedef   float  MRIInputPixelType;
        typedef   float  MRIInternalPixelType;
        typedef   float  MRIOutputPixelType;

        const     unsigned int    Dimension = 3;
        typedef itk::Image< MRIInputPixelType,    Dimension >   MRIInputImageType;
        typedef itk::Image< MRIOutputPixelType,   Dimension >   MRIOutputImageType;
        typedef itk::ImageFileReader< MRIInputImageType  >  MRIReaderType;
        MRIReaderType::Pointer MRIreader = MRIReaderType::New();
        MRIreader->SetFileName(MRIinFilename);
        try
        {
            MRIreader->Update();
        }
        catch( itk::ExceptionObject & excep )
        {
            std::cerr << "Exception caught!" << std::endl;
            std::cerr << excep << std::endl;
        }

        MRIInputImageType::IndexType MRIindex;
        MRIindex[0]=startingIndexX;
        MRIindex[1]=startingIndexY;
        MRIindex[2]=startingIndexZ;
        MRIInputImageType::SizeType MRIdesiredSize;
        MRIdesiredSize[0]=bbX;
        MRIdesiredSize[1]=bbY;
        MRIdesiredSize[2]=bbZ;

        MRIInputImageType::RegionType MRIdesiredRegion(MRIindex, MRIdesiredSize);

        typedef itk::ExtractImageFilter< MRIInputImageType, MRIOutputImageType > MRIFilterType;
        MRIFilterType::Pointer MRIfilter = MRIFilterType::New();
        MRIfilter->SetExtractionRegion(MRIdesiredRegion);
        MRIfilter->SetInput(MRIreader->GetOutput());

#if ITK_VERSION_MAJOR >= 4
        MRIfilter->SetDirectionCollapseToIdentity(); // This is required.
#endif
        try
        {
            MRIfilter->Update();
        }
        catch( itk::ExceptionObject & excep )
        {
            std::cerr << "Exception caught !" << std::endl;
            std::cerr << excep << std::endl;
        }

        MRIInputImageType::Pointer MRIoutput = MRIfilter->GetOutput();
        MRIoutput->DisconnectPipeline();

        typedef itk::ImageFileWriter< MRIOutputImageType >  MRIWriterType;
        MRIWriterType::Pointer MRIwriter = MRIWriterType::New();
        MRIwriter->SetFileName( MRIoutFilename);
        MRIwriter->SetInput(MRIoutput);

        try
        {
            MRIwriter->Update();
        }
        catch( itk::ExceptionObject & excep )
        {
            std::cerr << "Exception caught !" << std::endl;
            std::cerr << excep << std::endl;
        }
    }
    return EXIT_SUCCESS;
}
