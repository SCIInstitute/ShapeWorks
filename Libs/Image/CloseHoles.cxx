#include "itkImage.h"
#include <itkImageFileReader.h>
#include <itkExtractImageFilter.h>
#include "itkImageFileWriter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkIndex.h"
#include "itkBinaryFillholeImageFilter.h"
#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that closes holes in a given binary volume ...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("The filename of the input shape to be processed.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output shape with holes closed.");

    return parser;
}



int main(int argc, char * argv[] )
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
  std::string outFilename        = (std::string) options.get("outFilename");
  const unsigned int Dimension = 3;
  typedef float PixelType;
  typedef itk::Image< PixelType, Dimension > ImageType;
  typedef itk::ImageFileReader< ImageType > ReaderType;
  ReaderType::Pointer reader = ReaderType::New();
  reader->SetFileName( inFilename );
  typedef itk::BinaryFillholeImageFilter< ImageType > FilterType;
  FilterType::Pointer filter = FilterType::New();
  filter->SetInput( reader->GetOutput() );
  filter->SetForegroundValue( itk::NumericTraits< PixelType >::min() );
  typedef itk::ImageFileWriter< ImageType > WriterType;
  WriterType::Pointer writer = WriterType::New();
  writer->SetFileName( outFilename );
  writer->SetInput( filter->GetOutput() );
  writer->SetUseCompression(true);
  try
    {
    writer->Update();
    }
  catch( itk::ExceptionObject & error )
    {
    std::cerr << "Error: " << error << std::endl;
    return EXIT_FAILURE;
    }
  return EXIT_SUCCESS;
}
