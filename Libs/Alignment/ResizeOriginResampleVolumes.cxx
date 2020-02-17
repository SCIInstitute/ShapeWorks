#include "itkImage.h"

#include "itkImageFileReader.h"

#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"

#include "itkNearestNeighborInterpolateImageFunction.h"

#include "itkLinearInterpolateImageFunction.h"

#include "itkIdentityTransform.h"

#include "tinyxml.h"

int main( int argc, char * argv[] )
{
    // check input
    if( argc < 2 )
    {
        std::cerr << "-------------------------------" << std::endl;
        std::cerr << "ResizeOriginResampleVolumes " << std::endl;
        std::cerr << "-------------------------------" << std::endl;

        std::cerr << "Resize the given volumes, move origin to [0 0 0] and resample using common spacing. It uses a parameter file (xml) with the following tags:" << std::endl;
        std::cerr << "\t - inputs: a list of input image filenames" << std::endl;
        std::cerr << "\t - output: a list of output image filenames" << std::endl;
        std::cerr << "\t - spacing: output isotropic spacing (common to all files)" << std::endl;
        std::cerr << "\t - SizeX: size in x-dimension" << std::endl;
        std::cerr << "\t - SizeY: size in y-dimension" << std::endl;
        std::cerr << "\t - SizeZ: size in z-dimension" << std::endl;

        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;

        return EXIT_FAILURE;
    }

    // variables
    std::vector< std::string > inFilenames; inFilenames.clear();
    std::vector< std::string > outFilenames; outFilenames.clear();

    unsigned int sizeX=100, sizeY=100, sizeZ=100;
    float outSpacing=1.0;
    bool  isBinary = true;

    // read parameters
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string filename("/dev/null\0");

        elem = docHandle.FirstChild( "inputs" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                inFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        elem = docHandle.FirstChild( "outputs" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                outFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }

        elem = docHandle.FirstChild( "spacing" ).Element();
        if(elem)
        {
            outSpacing = atoi(elem->GetText());
            std::cout<<"Spacing = "<<outSpacing<<std::endl;

        }
        else
        {
            std::cout<<"Setting default spacing... 1.0mm"<<std::endl;
        }

        elem = docHandle.FirstChild( "SizeX" ).Element();
        if(elem)
        {
            sizeX = atoi(elem->GetText());
            std::cout<<"SizeX = "<<sizeX<<std::endl;
        }
        else
        {
            std::cout<<"Setting default sizeX... 100"<<std::endl;
        }

        elem = docHandle.FirstChild( "SizeY" ).Element();
        if(elem)
        {
            sizeY = atoi(elem->GetText());
            std::cout<<"SizeY = "<<sizeY<<std::endl;
        }
        else
        {
            std::cout<<"Setting default sizeY... 100"<<std::endl;
        }

        elem = docHandle.FirstChild( "SizeZ" ).Element();
        if(elem)
        {
            sizeZ = atoi(elem->GetText());
            std::cout<<"SizeZ = "<<sizeZ<<std::endl;
        }
        else
        {
            std::cout<<"Setting default sizeZ... 100"<<std::endl;
        }

        elem = docHandle.FirstChild( "isBinaryInput" ).Element();
        if(elem)
        {
            isBinary = atoi(elem->GetText()) > 0 ? true : false;
        }

    }

    double outOrigin[3];
    outOrigin[0] = 0.0;
    outOrigin[1] = 0.0;
    outOrigin[2] = 0.0;

    const     unsigned int    Dimension = 3;
    typedef   unsigned short  InputPixelType;
    typedef   unsigned short  InternalPixelType;
    typedef itk::Image< InputPixelType,    Dimension >   InputImageType;
    typedef itk::Image< InternalPixelType, Dimension >   InternalImageType;

    typedef itk::ImageFileReader< InputImageType  >  ReaderType;
    ReaderType::Pointer reader = ReaderType::New();

    typedef   unsigned short   OutputPixelType;
    typedef itk::Image< OutputPixelType,   Dimension >   OutputImageType;
    typedef itk::ResampleImageFilter<
            InternalImageType, OutputImageType >  ResampleFilterType;
    ResampleFilterType::Pointer resampler = ResampleFilterType::New();

    typedef itk::IdentityTransform< double, Dimension >  TransformType;
    TransformType::Pointer transform = TransformType::New();
    transform->SetIdentity();

    typedef itk:: NearestNeighborInterpolateImageFunction<InternalImageType, double> InterpolatorType;

    if(!isBinary)
    {
        typedef itk::LinearInterpolateImageFunction<InternalImageType, double >  InterpolatorType;
    }

    InterpolatorType::Pointer interpolator = InterpolatorType::New();

    OutputImageType::SpacingType spacing;
    spacing[0] = outSpacing;
    spacing[1] = outSpacing;
    spacing[2] = outSpacing;

    typedef InputImageType::SizeType::SizeValueType SizeValueType;

    InputImageType::SizeType   size;
    size[0] = static_cast<SizeValueType>( sizeX );
    size[1] = static_cast<SizeValueType>( sizeY );
    size[2] = static_cast<SizeValueType>( sizeZ );


    typedef itk::ImageFileWriter< OutputImageType >  WriterType;
    WriterType::Pointer writer = WriterType::New();

    for(unsigned int sh = 0; sh < inFilenames.size(); sh++)
    {
        std::cout << "processing " << inFilenames[sh] << std::endl;
        reader->SetFileName( inFilenames[sh].c_str() );
        try
        {
            reader->Update();
        }
        catch( itk::ExceptionObject & excep )
        {
            std::cerr << "Exception caught!" << std::endl;
            std::cerr << excep << std::endl;
        }

        resampler->SetInput( reader->GetOutput() );
        resampler->SetInterpolator( interpolator );
        resampler->SetTransform( transform );
        resampler->SetDefaultPixelValue( 255 ); // highlight regions without source
        resampler->SetOutputSpacing( spacing );
        resampler->SetOutputOrigin( outOrigin );
        resampler->SetSize( size );


        resampler->Update();


        writer->SetFileName( outFilenames[sh].c_str() );
        writer->SetInput( resampler->GetOutput() );
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
    }
    return EXIT_SUCCESS;
}
