
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkImageRegionIterator.h"
#include "itkChangeInformationImageFilter.h"
#include "string.h"

#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that extracts/isolates a specific voxel label from a given multi-label volume and outputs the corresponding binary image...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("The filename of the input image from which label has to be extracted.");
    parser.add_option("--labelVal").action("store").type("int").set_default(1).help("The label value which has to be extracted.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output image.");
    
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

    if(argc < 3)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }
    
    std::string inFilename    = (std::string) options.get("inFilename");
    std::string outFilename   = (std::string) options.get("outFilename");
    int         label         = (int) options.get("labelVal");
    
    typedef   float InputPixelType;
    typedef   float InternalPixelType;
    typedef   float OutputPixelType;

    const     unsigned int    Dimension = 3;
    typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
    typedef itk::Image< InternalPixelType, Dimension >   InternalImageType;
    typedef itk::ImageFileReader< InputImageType  >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();
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

	InputImageType::ConstPointer inputImage = reader->GetOutput();
	typedef itk::Image< OutputPixelType,   Dimension >   OutputImageType;

        OutputImageType::Pointer inImage = reader->GetOutput();
        OutputImageType::Pointer outputImage = OutputImageType::New();
        DeepCopy<OutputImageType>(inImage, outputImage);
	
	outputImage->SetSpacing(  inputImage->GetSpacing() );
	outputImage->SetOrigin(   inputImage->GetOrigin()  );
        itk::ImageRegionIterator<OutputImageType> imageIterator(inImage, inImage->GetLargestPossibleRegion());
        itk::ImageRegionIterator<OutputImageType> outIterator(outputImage, outputImage->GetLargestPossibleRegion());

        while(!imageIterator.IsAtEnd())
        {
            OutputPixelType val = imageIterator.Get();

            if(val==label)
                outIterator.Set((OutputPixelType)1);
            else
                outIterator.Set((OutputPixelType)0);

            ++imageIterator;
            ++outIterator;
        }

          
    typedef itk::ImageFileWriter< OutputImageType >  WriterType;
    WriterType::Pointer writer = WriterType::New();
    writer->SetFileName( outFilename );
    writer->SetInput(outputImage);
    writer->SetUseCompression(true);

    try
    {
        writer->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }
    return EXIT_SUCCESS;
  
}
