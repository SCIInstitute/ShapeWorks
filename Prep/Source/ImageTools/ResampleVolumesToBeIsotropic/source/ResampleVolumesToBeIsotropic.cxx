
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
    const std::string desc = "A command line tool that resamples given mri/binary volumes to have isotropic voxel spacing....";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);

    parser.add_option("--inFilename").action("store").type("string").set_default("").help("The filename of the input image to be resampled.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output resampled image.");
    parser.add_option("--isBinaryImage").action("store").type("bool").set_default(false).help("A flag to treat the input image as a binary image (specialized resampling pipeline) [default disabled].");
    parser.add_option("--isoSpacing").action("store").type("float").set_default("").help("The isotropic spacing in all dimensions.");
    parser.add_option("--sizeX").action("store").type("int").set_default(0).help("Image size in x-direction (optional, if set to 0, the size is autmatically estimated from the input image).");
    parser.add_option("--sizeY").action("store").type("int").set_default(0).help("Image size in y-direction (optional, if set to 0, the size is autmatically estimated from the input image).");
    parser.add_option("--sizeZ").action("store").type("int").set_default(0).help("Image size in z-direction (optional, if set to 0, the size is autmatically estimated from the input image).");
    parser.add_option("--isCenterImageOn").action("store").type("bool").set_default(false).help("A flag to center the image, i.e. change the origin in the image header to the physcial coordinates of the first voxel (lower left corner) [default disabled].");

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
    int         sizeX         = (int) options.get("sizeX");
    int         sizeY         = (int) options.get("sizeY");
    int         sizeZ         = (int) options.get("sizeZ");
    float isoSpacing          = (float) options.get("isoSpacing");
    bool isBinaryImage        = (bool) options.get("isBinaryImage");
    bool isCenterImageOn        = (bool) options.get("isCenterImageOn");

    if(isBinaryImage==true)
    {
        typedef   float InputPixelType;
        typedef   float InternalPixelType;
        typedef   float OutputPixelType;

        const     unsigned int    Dimension = 3;
        typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
        typedef itk::Image< InternalPixelType, Dimension >   InternalImageType;
        typedef itk::ImageFileReader< InputImageType  >  ReaderType;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName( inFilename );
        InputImageType::ConstPointer inputImage = reader->GetOutput();

        try
        {
            reader->Update();
        }
        catch( itk::ExceptionObject & excep )
        {
            std::cerr << "Exception caught!" << std::endl;
            std::cerr << excep << std::endl;
        }


        typedef itk::Image< OutputPixelType,   Dimension >   OutputImageType;

        typedef itk::ResampleImageFilter<InternalImageType, OutputImageType >  ResampleFilterType;
        ResampleFilterType::Pointer resampler = ResampleFilterType::New();

        typedef itk::IdentityTransform< double, Dimension >  TransformType;
        TransformType::Pointer transform = TransformType::New();
        transform->SetIdentity();
        resampler->SetTransform( transform );

        //typedef itk::LinearInterpolateImageFunction<InternalImageType, double >  InterpolatorType;
        typedef itk::BSplineInterpolateImageFunction<InternalImageType, double, double> InterpolatorType;
        InterpolatorType::Pointer interpolator = InterpolatorType::New();

        interpolator->SetSplineOrder(3);
        resampler->SetInterpolator( interpolator );

        //this is to overcome padding that was occuring after resampling step due to enlargement of size as per input

        resampler->SetDefaultPixelValue( -1);

        OutputImageType::SpacingType spacing;
        spacing[0] = isoSpacing;
        spacing[1] = isoSpacing;
        spacing[2] = isoSpacing;

        resampler->SetOutputSpacing( spacing );
        resampler->SetOutputOrigin( inputImage->GetOrigin() );
        resampler->SetOutputDirection( inputImage->GetDirection() );

        InputImageType::SizeType   outputSize;
        InputImageType::SizeType   inputSize =inputImage->GetLargestPossibleRegion().GetSize();
        typedef InputImageType::SizeType::SizeValueType SizeValueType;
        const InputImageType::SpacingType& inputSpacing = inputImage->GetSpacing();

        if(sizeX == 0 || sizeY == 0 || sizeZ == 0){

            sizeX = std::ceil(inputSize[0] * inputSpacing[0] / isoSpacing);
            sizeY = std::ceil(inputSize[1] * inputSpacing[1] / isoSpacing);
            sizeZ = std::ceil((inputSize[2] - 1 ) * inputSpacing[2] / isoSpacing);
        }

        outputSize[0] = static_cast<SizeValueType>( sizeX );
        outputSize[1] = static_cast<SizeValueType>( sizeY );
        outputSize[2] = static_cast<SizeValueType>( sizeZ );

        std::cout<<"InputFileName:"<<inFilename<<std::endl;
        std::cout<<"InputSize:"<<inputSize[0]<<" "<<inputSize[1]<<" "<<inputSize[2]<<std::endl;
        std::cout<<"OutputSize:"<<outputSize[0]<<" "<<outputSize[1]<<" "<<outputSize[2]<<std::endl;
        std::cout<<"InputSpacing:"<<inputSpacing[0]<<" "<<inputSpacing[1]<<" "<<inputSpacing[2]<<std::endl;
        std::cout<<"OutputSpacing:"<<isoSpacing<<std::endl;

        resampler->SetSize( outputSize);

        /*Anti aliasing for binary images*/
        typedef itk::AntiAliasBinaryImageFilter< InternalImageType, InternalImageType > FilterType;
        FilterType::Pointer antialiasFilter = FilterType::New();
        antialiasFilter->SetInput( reader->GetOutput() );
        antialiasFilter->SetMaximumRMSError( 0.01);
        antialiasFilter->SetNumberOfIterations( 50 );
        antialiasFilter->Update();

        /*resampling the binary image*/
        resampler->SetInput( antialiasFilter->GetOutput() );
        resampler->Update();

        OutputImageType::Pointer resampledImage = resampler->GetOutput();
        OutputImageType::Pointer outputImage = OutputImageType::New();
        DeepCopy<OutputImageType>(resampledImage, outputImage);

        outputImage->SetSpacing( spacing );
        outputImage->SetOrigin(   inputImage->GetOrigin()  );
        itk::ImageRegionIterator<OutputImageType> imageIterator(resampledImage, resampledImage->GetLargestPossibleRegion());
        itk::ImageRegionIterator<OutputImageType> outIterator(outputImage, outputImage->GetLargestPossibleRegion());

        /*thresholding for binary images*/
        while(!imageIterator.IsAtEnd())
        {
            OutputPixelType val = imageIterator.Get();

            if(val>=0)
                outIterator.Set((OutputPixelType)1);
            else
                outIterator.Set((OutputPixelType)0);

            ++imageIterator;
            ++outIterator;
        }


        typedef itk::ImageFileWriter< OutputImageType >  WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outFilename );


        // center volume if indicated (i.e. move (0,0,0) to the center of the bounding box)
        typedef itk::ChangeInformationImageFilter< OutputImageType > ImageInfoFilterType;
        ImageInfoFilterType::Pointer infoFilter = ImageInfoFilterType::New();

        if(isCenterImageOn == true)
        {
            std::cout << "CENTER IS ON ... " << std::endl;

            // itk assumes center to be at pixel(0,0,0) - lower left corner so if we explicitly modify
            // the origin to the region center, any subsequent processing will force the origin to be at the corner and moves the image region accordingly

            infoFilter->SetInput(outputImage);
            //infoFilter->SetOutputOrigin( origin );
            //infoFilter->ChangeOriginOn();
            infoFilter->CenterImageOn();
            infoFilter->Update();
            writer->SetInput( infoFilter->GetOutput() );
        }
        else
            writer->SetInput(outputImage);

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

    //for MRI images
    else
    {
        //        typedef   unsigned short InputPixelType;
        //        typedef   unsigned short InternalPixelType;
        //        typedef   unsigned short OutputPixelType;

        typedef   float InputPixelType;
        typedef   float InternalPixelType;
        typedef   float OutputPixelType;

        const     unsigned int    Dimension = 3;
        typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
        typedef itk::Image< InternalPixelType, Dimension >   InternalImageType;
        typedef itk::ImageFileReader< InputImageType  >  ReaderType;
        ReaderType::Pointer reader = ReaderType::New();
        reader->SetFileName( inFilename );
        InputImageType::ConstPointer inputImage = reader->GetOutput();

        try
        {
            reader->Update();
        }
        catch( itk::ExceptionObject & excep )
        {
            std::cerr << "Exception caught!" << std::endl;
            std::cerr << excep << std::endl;
        }


        typedef itk::Image< OutputPixelType,   Dimension >   OutputImageType;

        typedef itk::ResampleImageFilter<InternalImageType, OutputImageType >  ResampleFilterType;
        ResampleFilterType::Pointer resampler = ResampleFilterType::New();

        typedef itk::IdentityTransform< double, Dimension >  TransformType;
        TransformType::Pointer transform = TransformType::New();
        transform->SetIdentity();
        resampler->SetTransform( transform );

        typedef itk::LinearInterpolateImageFunction<InternalImageType, double >  InterpolatorType;
        InterpolatorType::Pointer interpolator = InterpolatorType::New();

        resampler->SetInterpolator( interpolator );

        //this is to overcome padding that was occuring after resampling step due to enlargement of size as per input

        resampler->SetDefaultPixelValue( 0 );

        OutputImageType::SpacingType spacing;
        spacing[0] = isoSpacing;
        spacing[1] = isoSpacing;
        spacing[2] = isoSpacing;

        resampler->SetOutputSpacing( spacing );
        resampler->SetOutputOrigin( inputImage->GetOrigin() );
        resampler->SetOutputDirection( inputImage->GetDirection() );

        InputImageType::SizeType   outputSize;
        InputImageType::SizeType   inputSize =inputImage->GetLargestPossibleRegion().GetSize();
        typedef InputImageType::SizeType::SizeValueType SizeValueType;
        const InputImageType::SpacingType& inputSpacing = inputImage->GetSpacing();

        if(sizeX == 0 || sizeY == 0 || sizeZ == 0){

            sizeX = std::ceil(inputSize[0] * inputSpacing[0] / isoSpacing);
            sizeY = std::ceil(inputSize[1] * inputSpacing[1] / isoSpacing);
            sizeZ = std::ceil((inputSize[2] - 1 ) * inputSpacing[2] / isoSpacing);
        }

        outputSize[0] = static_cast<SizeValueType>( sizeX );
        outputSize[1] = static_cast<SizeValueType>( sizeY );
        outputSize[2] = static_cast<SizeValueType>( sizeZ );

        std::cout<<"InputFileName:"<<inFilename<<std::endl;
        std::cout<<"InputSize:"<<inputSize[0]<<" "<<inputSize[1]<<" "<<inputSize[2]<<std::endl;
        std::cout<<"OutputSize:"<<outputSize[0]<<" "<<outputSize[1]<<" "<<outputSize[2]<<std::endl;
        std::cout<<"InputSpacing:"<<inputSpacing[0]<<" "<<inputSpacing[1]<<" "<<inputSpacing[2]<<std::endl;
        std::cout<<"OutputSpacing:"<<isoSpacing<<std::endl;

        resampler->SetSize( outputSize);

        /*resampling the non binary images*/
        resampler->SetInput(reader->GetOutput() );
        resampler->Update();

        OutputImageType::Pointer resampledImage = resampler->GetOutput();
        OutputImageType::Pointer outputImage = resampler->GetOutput();

        outputImage->SetSpacing( spacing );
        outputImage->SetOrigin(   inputImage->GetOrigin()  );

        typedef itk::ImageFileWriter< OutputImageType >  WriterType;
        WriterType::Pointer writer = WriterType::New();
        writer->SetFileName( outFilename );

        // center volume if indicated (i.e. move (0,0,0) to the center of the bounding box)
        typedef itk::ChangeInformationImageFilter< OutputImageType > ImageInfoFilterType;
        ImageInfoFilterType::Pointer infoFilter = ImageInfoFilterType::New();

        if(isCenterImageOn)
        {
            std::cout << "CENTER IS ON ... " << std::endl;

            // itk assumes center to be at pixel(0,0,0) - lower left corner so if we explicitly modify
            // the origin to the region center, any subsequent processing will force the origin to be at the corner and moves the image region accordingly

            infoFilter->SetInput(outputImage);
            //infoFilter->SetOutputOrigin( origin );
            //infoFilter->ChangeOriginOn();
            infoFilter->CenterImageOn();
            infoFilter->Update();
            writer->SetInput( infoFilter->GetOutput() );
        }

        else
            writer->SetInput(resampler->GetOutput());
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


}
