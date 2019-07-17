
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "string.h"
#include <fstream>

#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that extracts header information from a nrrd file and write it to a text file...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("The filename of the input image to extract its header information (origin, size, spacing).");
    parser.add_option("--outPrefix").action("store").type("string").set_default("").help("The output prefix to be used to save header info to <outPrefix>_<info>.txt where info is origin, size, and spacing.");

    return parser;
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

    std::string inFilename         = (std::string) options.get("inFilename");
    std::string outPrefix          = (std::string) options.get("outPrefix");

    const unsigned int  Dimension = 3;
    typedef   float InPixelType;
    typedef   float OutPixelType;
    typedef itk::Image< InPixelType,    Dimension >   InImageType;
    typedef itk::Image< OutPixelType,    Dimension >   OutImageType;
    typedef itk::ImageFileReader< InImageType  >  RType;
    RType::Pointer reader = RType::New();
    reader->SetFileName( inFilename );
    reader->Update();

    InImageType::ConstPointer inImage = reader->GetOutput();

    InImageType::PointType inOrigin    = inImage->GetOrigin();
    InImageType::SpacingType inSpacing = inImage->GetSpacing();
    InImageType::SizeType inSize = inImage->GetLargestPossibleRegion().GetSize();

    std::cout << "inOrigin = " << inOrigin << std::endl;
    std::cout << "inSpacing = " << inSpacing << std::endl;
    std::cout << "inSize = " << inSize << std::endl;

    std::ofstream outfile;

    outfile.open(std::string(outPrefix + "_origin.txt" ).c_str());
    outfile << inOrigin[0] << std::endl;
    outfile << inOrigin[1] << std::endl;
    outfile << inOrigin[2] << std::endl;
    outfile.close();

    outfile.open(std::string(outPrefix + "_spacing.txt" ).c_str());
    outfile << inSpacing[0] << std::endl;
    outfile << inSpacing[1] << std::endl;
    outfile << inSpacing[2] << std::endl;
    outfile.close();

    outfile.open(std::string(outPrefix + "_size.txt" ).c_str());
    outfile << inSize[0] << std::endl;
    outfile << inSize[1] << std::endl;
    outfile << inSize[2] << std::endl;
    outfile.close();

    return EXIT_SUCCESS;
}
