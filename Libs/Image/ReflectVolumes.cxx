
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkAffineTransform.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkChangeInformationImageFilter.h"
#include "string.h"
#include "itkImageMomentsCalculator.h"
#include "itkIndex.h"
#include <fstream>

#include "itkChangeInformationImageFilter.h"

#include "OptionParser.h"

optparse::OptionParser buildParser()
{
    const std::string usage = "%prog [OPTION]";
    const std::string version = "%prog 0.1";
    const std::string desc = "A command line tool that reflect images with respect to image center and specific axis.....";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("Image file to be reflected.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output reflection image.");
    parser.add_option("--centerFilename").action("store").type("string").set_default("").help("The filename where the image center information will be stored.");
    parser.add_option("--inputDirection").action("store").type("int").set_default(0).help("Axis along which it needs to be reflected");

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

int main( int argc, char * argv[] )
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
    std::string center_filename   = (std::string) options.get("centerFilename");
    int        inputDirection   = (int) options.get("inputDirection");

    const unsigned int  Dimension = 3;
    typedef   float InPixelType;
    typedef   float OutPixelType;
    typedef itk::Image< InPixelType,    Dimension >   InImageType;
    typedef itk::Image< OutPixelType,    Dimension >   OutImageType;
    typedef itk::ImageFileReader< InImageType  >  RType;
    RType::Pointer reader = RType::New();
    reader->SetFileName( inFilename );
    reader->Update();

    InImageType::PointType inOrigin_    = reader->GetOutput()->GetOrigin();
    InImageType::SpacingType inSpacing_ = reader->GetOutput()->GetSpacing();
    InImageType::DirectionType inDirection_ = reader->GetOutput()->GetDirection();

    std::cout << "inOrigin before center on = " << inOrigin_ << std::endl;
    std::cout << "inSpacing before center on = " << inSpacing_ << std::endl;
    std::cout << "inDirection before center on = " << inDirection_ << std::endl;
    // std::cout << "in Orientation before center on = " << reader->GetOutput()->GetOrientation();

    typedef itk::ChangeInformationImageFilter< InImageType > ImageInfoFilterType;
    ImageInfoFilterType::Pointer infoFilter = ImageInfoFilterType::New();
    infoFilter->SetInput(reader->GetOutput());
    infoFilter->CenterImageOn();
    infoFilter->Update();
    InImageType::ConstPointer inImage = infoFilter->GetOutput();

    // InImageType::PointType inCenter;  inCenter[0] = 0; inCenter[1] = 0; inCenter[2] = 0;
    InImageType::PointType inOrigin    = inImage->GetOrigin();
    InImageType::SpacingType inSpacing = inImage->GetSpacing();
    InImageType::DirectionType inDirection = inImage->GetDirection();

    std::cout << "inOrigin = " << inOrigin << std::endl;
    std::cout << "inSpacing = " << inSpacing << std::endl;
    std::cout << "inDirection = " << inDirection << std::endl;

    typedef itk::ResampleImageFilter<InImageType, OutImageType >  RFilterType;
    RFilterType::Pointer resampler = RFilterType::New();

    typedef itk::LinearInterpolateImageFunction<InImageType, double >  IntType;
    IntType::Pointer interpolator = IntType::New();

    typedef itk::AffineTransform< double, Dimension > TransformType;
    TransformType::Pointer transform = TransformType::New();
    TransformType::MatrixType reflection;

    double x=1,y=1,z=1;
    if(inputDirection == 0)
        x = -1;
    if(inputDirection == 1)
        y = -1;
    if(inputDirection == 2)
        z = -1;

    reflection(0,0) = x; reflection(0,1) = 0; reflection(0,2) = 0; //reflection(0,3) = 0;
    reflection(1,0) = 0; reflection(1,1) = y; reflection(1,2) = 0; //reflection(1,3) = 0;
    reflection(2,0) = 0; reflection(2,1) = 0; reflection(2,2) = z; //reflection(2,3) = 0;

    std::cout << "reflection: " << reflection << std::endl;
    transform->SetMatrix(reflection);

    resampler->SetInterpolator( interpolator );
    resampler->SetDefaultPixelValue(0);
    resampler->SetTransform(transform.GetPointer());
    resampler->SetInput(inImage);
    resampler->SetSize(inImage->GetLargestPossibleRegion().GetSize());
    resampler->SetOutputOrigin( inOrigin );
    resampler->SetOutputDirection( inDirection );
    resampler->SetOutputSpacing(inSpacing);
    resampler->Update();

    InImageType::PointType outOrigin;
    outOrigin[0] = inOrigin_[0];
    outOrigin[1] = inOrigin_[1];
    outOrigin[2] = inOrigin_[2];

    std::cout << "outOrigin = " << outOrigin << std::endl;
    ImageInfoFilterType::Pointer infoFilter_ = ImageInfoFilterType::New();
    infoFilter_->SetInput(resampler->GetOutput());
    infoFilter_->SetOutputOrigin( outOrigin );
    infoFilter_->ChangeOriginOn();
    infoFilter_->Update();

    typedef itk::ImageFileWriter< OutImageType >  WType;
    WType::Pointer writer = WType::New();
    writer->SetFileName( outFilename);
    writer->SetInput(infoFilter_->GetOutput());
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

    // save the center of the image information
    std::ofstream ofs_center(center_filename.c_str());
    ofs_center << inOrigin[0] - inOrigin_[0] << " " << inOrigin[1] - inOrigin_[1] << " " << inOrigin[2] - inOrigin_[2] << std::endl;
    ofs_center.close();

    return EXIT_SUCCESS;
}
