
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkIdentityTransform.h"
#include "itkTranslationTransform.h"
#include "itkAntiAliasBinaryImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkChangeInformationImageFilter.h"
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
    const std::string desc = "A command line tool that performs translational alignment of a given shape image based on either its center of mass or a given 3d point...";
    //const std::string epilog = "note: --numthreads 0 means use system default (usually max number supported).\n";
    const std::string epilog = "";

    optparse::OptionParser parser = optparse::OptionParser()
            .usage(usage)
            .version(version)
            .description(desc)
            .epilog(epilog);
    /*
    parser.add_option("--inFilename").action("store").type("string").set_default("").help("The filename of the input shape to be transformed.");
    parser.add_option("--outFilename").action("store").type("string").set_default("").help("The filename of the output transformed shape.");
    parser.add_option("--MRIinFilename").action("store").type("string").set_default("").help("The assoicated image filename to be transformed.");
    parser.add_option("--MRIoutFilename").action("store").type("string").set_default("").help("The filename of the output transformed image.");
    parser.add_option("--useCenterOfMass").action("store").type("bool").set_default(true).help("A flag to transform shape using its center of mass.");
    parser.add_option("--centerX").action("store").type("float").set_default(0.0).help("x-coordinate of a user-defined center point.");
    parser.add_option("--centerY").action("store").type("float").set_default(0.0).help("y-coordinate of a user-defined center point.");
    parser.add_option("--centerZ").action("store").type("float").set_default(0.0).help("z-coordinate of a user-defined center point.");
    parser.add_option("--parameterFilename").action("store").type("string").set_default("").help("The filename to store the transform parameters");*/

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

   // std::string inFilename         = (std::string) options.get("inFilename");
   // std::string outFilename        = (std::string) options.get("outFilename");
    std::string MRIinFilename      = (std::string) options.get("MRIinFilename");
    std::string MRIoutFilename     = (std::string) options.get("MRIoutFilename");
   //std::string parameterFilename  = (std::string) options.get("parameterFilename");

    //bool useCenterOfMass    = (bool) options.get("useCenterOfMass");
    float translationX    = (float) options.get("translationX");
    float translationY    = (float) options.get("translationY");
    float translationZ    = (float) options.get("translationZ");

    /*
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


    //InputImageType::PointType origin ;

    //    //setting the origin to 0
    //    origin[0]=0;
    //    origin[1]=0;
    //    origin[2]=0;

    const InputImageType::SpacingType& inputSpacing = inputImage->GetSpacing();

    try
    {
        reader->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught!" << std::endl;
        std::cerr << excep << std::endl;
    }

    // the origin of the input image
    //origin = reader->GetOutput()->GetOrigin();

    typedef itk::Image< OutputPixelType,   Dimension >   OutputImageType;
    OutputImageType::Pointer inpImage = reader->GetOutput();
    OutputImageType::Pointer outImage = OutputImageType::New();

    /*Anti aliasing for binary images
    typedef itk::AntiAliasBinaryImageFilter< InternalImageType, InternalImageType > FilterType;
    FilterType::Pointer antialiasFilter = FilterType::New();
    antialiasFilter->SetInput( reader->GetOutput() );
    antialiasFilter->SetMaximumRMSError( 0.01);
    antialiasFilter->SetNumberOfIterations( 50 );
    antialiasFilter->Update();
    InputImageType::ConstPointer antialiasimage = antialiasFilter->GetOutput();

    typedef itk::ResampleImageFilter<InternalImageType, OutputImageType >  ResampleFilterType;
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    typedef itk::TranslationTransform< double, Dimension > TransformType;
    TransformType::OutputVectorType translation;
    TransformType::Pointer transform = TransformType::New();

    typedef itk::LinearInterpolateImageFunction<InternalImageType, double >  InterpolatorType;
    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    resampler->SetInterpolator( interpolator );

    //this is to overcome padding that was occuring after resampling step due to enlargement of size as per input

    resampler->SetDefaultPixelValue( -1);

    InputImageType::SizeType   outputSize;
    InputImageType::SizeType   inputSize =inputImage->GetLargestPossibleRegion().GetSize();
    typedef InputImageType::SizeType::SizeValueType SizeValueType;
/*
    double imageCenterX, imageCenterY, imageCenterZ;

    if(useCenterOfMass==true)
    {
        // getting the origin of the shape
        itk::ImageRegionIteratorWithIndex<OutputImageType> imageIt(inpImage, inpImage->GetLargestPossibleRegion());
        float numPixels=0.0, meanX=0.0, meanY=0.0, meanZ=0.0;
        while(!imageIt.IsAtEnd())
        {
            OutputPixelType val = imageIt.Get();
            InputImageType::IndexType index;
            InputImageType::PointType point;
            index=imageIt.GetIndex();

            if(val==1)
            {
                numPixels=numPixels+1;
                inputImage->TransformIndexToPhysicalPoint(index, point);
                meanX=meanX+point[0];
                meanY=meanY+point[1];
                meanZ=meanZ+point[2];
            }
            ++imageIt;

        }

        meanX=meanX/numPixels;
        meanY=meanY/numPixels;
        meanZ=meanZ/numPixels;

        imageCenterX =meanX;
        imageCenterY =meanY;
        imageCenterZ =meanZ;
    }

    else  /*using center of ostium coordinates
    {
        //        typedef itk::ImageFileReader< InputImageType  >  ReaderTypeOriginal;
        //        ReaderTypeOriginal::Pointer readeroriginal = ReaderTypeOriginal::New();
        //        readeroriginal->SetFileName( inParams.baseFilename );
        //        readeroriginal->Update();
        //        InputImageType::ConstPointer baseImage = readeroriginal->GetOutput();
        //        const InputImageType::PointType & originbase=baseImage->GetOrigin() ;
        //        const InputImageType::PointType & originupd=inputImage->GetOrigin() ;

        //        imageCenterX =inParams.centerX+(originupd[0]-originbase[0]);
        //        imageCenterY =inParams.centerY+(originupd[1]-originbase[1]);
        //        imageCenterZ =inParams.centerZ+(originupd[2]-originbase[2]);

        imageCenterX =centerX;
        imageCenterY =centerY;
        imageCenterZ =centerZ;
    }

    // the below was not testest for center of ostia
    // get the center of the image region (bounding box), translate the object's center to this center so that the shape will be at the center of the image
    InputImageType::PointType origin = inputImage->GetOrigin();
    InputImageType::SizeType   size  = inputImage->GetLargestPossibleRegion().GetSize();

    InputImageType::IndexType index;
    InputImageType::PointType point;
    InputImageType::PointType center;

    index[0] = 0; index[1] = 0; index[2] = 0;
    inputImage->TransformIndexToPhysicalPoint(index, point);
    center[0] = point[0]; center[1] = point[1]; center[2] = point[2];

    index[0] = 0; index[1] = 0; index[2] = size[2]-1;
    inputImage->TransformIndexToPhysicalPoint(index, point);
    center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

    index[0] = 0; index[1] = size[1]-1; index[2] = 0;
    inputImage->TransformIndexToPhysicalPoint(index, point);
    center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

    index[0] = 0; index[1] = size[1]-1; index[2] = size[2]-1;
    inputImage->TransformIndexToPhysicalPoint(index, point);
    center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

    index[0] = size[0]-1; index[1] = 0; index[2] = 0;
    inputImage->TransformIndexToPhysicalPoint(index, point);
    center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

    index[0] = size[0]-1; index[1] = 0; index[2] = size[2]-1;
    inputImage->TransformIndexToPhysicalPoint(index, point);
    center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

    index[0] = size[0]-1; index[1] = size[1]-1; index[2] = 0;
    inputImage->TransformIndexToPhysicalPoint(index, point);
    center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

    index[0] = size[0]-1; index[1] = size[1]-1; index[2] = size[2]-1;
    inputImage->TransformIndexToPhysicalPoint(index, point);
    center[0] += point[0]; center[1] += point[1]; center[2] += point[2];

    center[0] /= 8.0; center[1] /= 8.0; center[2] /= 8.0;

    // move object's origin to zero then move to the region center
    translation[0] = -1*(-imageCenterX + center[0]);
    translation[1] = -1*(-imageCenterY + center[1]);
    translation[2] = -1*(-imageCenterZ + center[2]);

    std::cout<<"Input file name:"<<inFilename<<std::endl;
    std::cout << "translation:"<< translation[0] << " " << translation[1] <<" "<< translation[2]<<"\n";
    std::cout <<"origin:"<<origin[0]<<" "<<origin[1]<<" "<<origin[2]<<"\n";
    std::cout << "object center:"<< imageCenterX << " " << imageCenterY <<" "<< imageCenterZ <<"\n";
    std::cout << "image center:"<< center[0] << " " << center[1] <<" "<< center[2] <<"\n";

    transform->Translate(translation);
    resampler->SetTransform( transform.GetPointer());

    /*resampling the binary image
    resampler->SetInput( antialiasFilter->GetOutput() );
    resampler->SetSize(inputImage->GetLargestPossibleRegion().GetSize());
    resampler->SetOutputOrigin( inputImage->GetOrigin() );
    resampler->SetOutputDirection( inputImage->GetDirection() );
    resampler->SetOutputSpacing(inputImage->GetSpacing());
    resampler->Update();

    OutputImageType::Pointer resampledImage = resampler->GetOutput();
    OutputImageType::Pointer outputImage = OutputImageType::New();
    DeepCopy<OutputImageType>(resampledImage, outputImage);

    outputImage->SetSpacing( inputSpacing );
    outputImage->SetOrigin(   inputImage->GetOrigin()  );
    itk::ImageRegionIteratorWithIndex<OutputImageType> imageIterator(resampledImage, resampledImage->GetLargestPossibleRegion());
    itk::ImageRegionIteratorWithIndex<OutputImageType> outIterator(outputImage, outputImage->GetLargestPossibleRegion());

    /*thresholding for binary images
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
    writer->SetFileName( outFilename);
    writer->SetInput(outputImage);

    try
    {
        writer->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }*/
    ////////////////////////////////////////////////////////

    //Transform MRI Image

    translation[0] = translationX;
    translation[1] = translationY;
    translation[2] = translationZ;


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

    transform->Translate(translation)

    typedef itk::LinearInterpolateImageFunction<InImageType, double >  IntType;
    IntType::Pointer inpolator = IntType::New();

    mriresampler->SetInterpolator( inpolator );

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
