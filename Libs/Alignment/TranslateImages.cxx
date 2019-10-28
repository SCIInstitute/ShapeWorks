
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkTranslationTransform.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkChangeInformationImageFilter.h"
#include "itkLinearInterpolateImageFunction.h"
#include "string.h"
#include "itkImageMomentsCalculator.h"
#include "itkIndex.h"
#include <fstream>

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


#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that performs a given translational alignment to an image ...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--MRIinFilename").action("store").type("string").set_default("").help("The assoicated image filename to be transformed.");
    parser.add_option("--MRIoutFilename").action("store").type("string").set_default("").help("The filename of the output transformed image.");
    parser.add_option("--translationX").action("store").type("float").set_default(0.0).help("x-coordinate of a user-defined center point.");
    parser.add_option("--translationY").action("store").type("float").set_default(0.0).help("y-coordinate of a user-defined center point.");
    parser.add_option("--translationZ").action("store").type("float").set_default(0.0).help("z-coordinate of a user-defined center point.");

    return parser;
}


int main( int argc, char * argv[] )
{
    optparse::OptionParser parser = buildParser();
    optparse::Values & options = parser.parse_args(argc,argv);
    std::vector<std::string> args = parser.args();

    if(argc < 5)
    {
        parser.print_help();
        return EXIT_FAILURE;
    }


    std::string MRIinFilename      = (std::string) options.get("MRIinFilename");
    std::string MRIoutFilename     = (std::string) options.get("MRIoutFilename");



    float translationX    = (float) options.get("translationX");
    float translationY    = (float) options.get("translationY");
    float translationZ    = (float) options.get("translationZ");


    ////////////////////////////////////////////////////////

    //Transform MRI Image

    const     unsigned int    Dimension = 3;
    typedef   float InPixelType;
    typedef   float OutPixelType;

    typedef itk::Image< InPixelType,    Dimension >   InImageType;
    typedef itk::Image< OutPixelType,    Dimension >   OutImageType;
    typedef itk::ImageFileReader< InImageType  >  RType;

    RType::Pointer mrireader = RType::New();
    mrireader->SetFileName( MRIinFilename );
    mrireader->Update();
    InImageType::ConstPointer inImage = mrireader->GetOutput();

    typedef itk::ResampleImageFilter<InImageType, OutImageType >  RFilterType;
    RFilterType::Pointer mriresampler = RFilterType::New();

    typedef itk::TranslationTransform< double, Dimension > TransformType;
    TransformType::OutputVectorType translation;
    TransformType::Pointer transform = TransformType::New();

    translation[0] = translationX;
    translation[1] = translationY;
    translation[2] = translationZ;

    transform->Translate(translation);

    typedef itk::LinearInterpolateImageFunction<InImageType, double > InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    mriresampler->SetInterpolator( interpolator );

    //this is to overcome padding that was occuring after resampling step due to enlargement of size as per input

    mriresampler->SetDefaultPixelValue(0);

    mriresampler->SetTransform(transform.GetPointer());
    mriresampler->SetInput(inImage);
    mriresampler->SetSize(inImage->GetLargestPossibleRegion().GetSize());
    mriresampler->SetOutputOrigin( inImage->GetOrigin() );
    mriresampler->SetOutputDirection( inImage->GetDirection() );
    mriresampler->SetOutputSpacing(inImage->GetSpacing());
    mriresampler->Update();

    typedef itk::ImageFileWriter< OutImageType >  WType;
    WType::Pointer mriwriter = WType::New();
    mriwriter->SetFileName( MRIoutFilename);
    mriwriter->SetInput(mriresampler->GetOutput());
    try
    {
        mriwriter->Update();
    }

    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }



    return EXIT_SUCCESS;
}
