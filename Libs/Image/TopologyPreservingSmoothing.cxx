#include "itkThresholdImageFilter.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkTPGACLevelSetImageFilter.h"
#include "tinyxml.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkGradientMagnitudeImageFilter.h"
#include "itkSigmoidImageFilter.h"
//#include "itkBinaryThresholdImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include <vector>
#include <iostream>
#include <string>

int main( int argc, char *argv[])
{
    if( argc < 2 )
    {
        std::cerr << "Usage: " << std::endl;
        std::cerr << argv[0] << " paramfile " << std::endl;
        return EXIT_FAILURE;
    }

    const unsigned int Dimension = 3;
    typedef float PixelType;

    typedef itk::Image<PixelType, Dimension> ImageType;
    typedef itk::Image<float, Dimension> OutputImageType;

    typedef itk::ImageFileReader< ImageType > ImageReaderType;
    typedef itk::ImageFileWriter< OutputImageType > ImageWriterType;

    typedef itk::CurvatureFlowImageFilter<ImageType, ImageType> SmoothingFilterType;
    //typedef itk::CurvatureAnisotropicDiffusionImageFilter<ImageType, ImageType> SmoothingFilterType;
    //typedef itk::GradientAnisotropicDiffusionImageFilter<ImageType, ImageType> SmoothingFilterType;
    //typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<ImageType, ImageType> GradientFilterType;
    typedef itk::GradientMagnitudeImageFilter<ImageType, ImageType> GradientFilterType;
    typedef itk::SigmoidImageFilter<ImageType, ImageType> SigmoidFilterType;
    typedef itk::TPGACLevelSetImageFilter<ImageType, ImageType> TPLevelSetImageFilterType;
    //typedef itk::BinaryThresholdImageFilter<OutputImageType, OutputImageType> ThresholdFilterType;

    // variables
    std::vector< std::string > inFilenames; inFilenames.clear();
    std::vector< std::string > outFilenames; outFilenames.clear();
    std::vector< std::string > distFilenames; distFilenames.clear();

    // parameters
    double propagationScaling = 0.0;
    double alpha = 10.0;
    double beta  = 10.0;
    double sigma = 2.0;
    double isoValue = 255.0;
    unsigned int smoothingIterations = 10;
    int verbose = 0;

    // read parameters
    TiXmlDocument doc(argv[1]);
    bool loadOkay = doc.LoadFile();

    if (loadOkay)
    {
        TiXmlHandle docHandle( &doc );
        TiXmlElement *elem;
        std::istringstream inputsBuffer;
        std::string filename("/dev/null\0");

        // Compile the list of input files.
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
        else
        {
            std::cerr << "No inputs to process!" << std::endl;
            return EXIT_FAILURE;
        }

        // Compile the list of input files.
        elem = docHandle.FirstChild( "dtFiles" ).Element();
        if (elem)
        {
            inputsBuffer.str(elem->GetText());
            while (inputsBuffer >> filename)
            {
                distFilenames.push_back(filename);
            }
            inputsBuffer.clear();
            inputsBuffer.str("");
        }
        else
        {
            std::cerr << "No inputs to process!" << std::endl;
            return EXIT_FAILURE;
        }

        // Compile the list of output files.
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

        // Make sure lists are the same size.
        if (inFilenames.size() > outFilenames.size())
        {
            std::cerr << "Input list size does not match output list size!" << std::endl;
            return EXIT_FAILURE;
        }

        elem = docHandle.FirstChild( "propagationScaling" ).Element();
        if(elem) propagationScaling = atof(elem->GetText());

        elem = docHandle.FirstChild( "alpha" ).Element();
        if(elem) alpha = atof(elem->GetText() );

        elem = docHandle.FirstChild( "beta" ).Element();
        if(elem) beta = atof( elem->GetText() );

        //      elem = docHandle.FirstChild( "sigma" ).Element();
        //      if(elem) sigma = atof( elem->GetText() );

        elem = docHandle.FirstChild( "isoValue" ).Element();
        if(elem) isoValue = atof( elem->GetText() );

        elem = docHandle.FirstChild( "smoothing_iterations" ).Element();
        if(elem) smoothingIterations = atoi( elem->GetText() );

        elem = docHandle.FirstChild( "verbose" ).Element();
        if(elem) verbose = atoi(elem->GetText());
    }

    for(unsigned int dtNo = 0; dtNo < inFilenames.size(); dtNo++)
    {
        ImageReaderType::Pointer reader = ImageReaderType::New();
        reader->SetFileName( inFilenames[dtNo].c_str() );
        try {
            reader->Update();
        } catch( itk::ExceptionObject &ex) {
            std::cerr << "Exception caudht!\n";
            std::cerr << ex << std::endl;
        }

        std::cout << "processing: " << inFilenames[dtNo] << "..";

        SmoothingFilterType::Pointer smoothing   = SmoothingFilterType::New();
        GradientFilterType::Pointer gradientMag  = GradientFilterType::New();
        SigmoidFilterType::Pointer sigmoid       = SigmoidFilterType::New();
        //ThresholdFilterType::Pointer thresholder = ThresholdFilterType::New();
        ImageWriterType::Pointer writer          = ImageWriterType::New();

        if(verbose) {
            std::cout << "\nSmoothing...";
        }
//        smoothing->SetTimeStep( 0.0005125 ); // for data : /usr/sci/projects/FAI/DATA/cortical_thickness
        smoothing->SetTimeStep( 0.0625 );
        smoothing->SetNumberOfIterations( smoothingIterations );
        //smoothing->SetConductanceParameter( 9.0 );
        smoothing->SetInput( reader->GetOutput() );
        smoothing->Update();
        ImageWriterType::Pointer w = ImageWriterType::New();
        w->SetInput( smoothing->GetOutput() );
        w->SetFileName(distFilenames[dtNo].c_str());
        w->SetUseCompression(true);
        w->Update();
        if(verbose) {
            std::cout << "Done\n";
        }

        if(verbose)
        {
            std::cout << "Gradient Magnitude...";
        }
        gradientMag->SetInput( smoothing->GetOutput() );
        //gradientMag->SetSigma( sigma );
        gradientMag->Update();
        if(verbose)
        {
            std::cout << "Done\n";
        }

        if(verbose)
        {
            std::cout << "Sigmoid filtering...";
        }
        sigmoid->SetAlpha( alpha );
        sigmoid->SetBeta( beta );
        sigmoid->SetOutputMinimum( 0.0 );
        sigmoid->SetOutputMaximum( 1.0 );
        sigmoid->SetInput( gradientMag->GetOutput() );
        sigmoid->Update();

        if(verbose)
        {
            std::cout << "Done\n";
        }

        if(verbose)
        {
            std::cout << "TPLevelSet filtering...";
        }
        TPLevelSetImageFilterType::Pointer levelSetFilter = TPLevelSetImageFilterType::New();
        const double propScale = propagationScaling;
        levelSetFilter->SetPropagationScaling( propScale );
        levelSetFilter->SetCurvatureScaling( 1.0 );
        levelSetFilter->SetAdvectionScaling( 1.0 );

        levelSetFilter->SetMaximumRMSError( 0.0 );
        levelSetFilter->SetNumberOfIterations( 20 );

        levelSetFilter->SetInput( smoothing->GetOutput() );
        levelSetFilter->SetFeatureImage( sigmoid->GetOutput() );
        levelSetFilter->Update();
        if(verbose)
        {
            std::cout << "Done\n";
        }

        /*std::cout << "Binary Threshold...";
        thresholder->SetLowerThreshold(-1000.0 );
        thresholder->SetUpperThreshold( 0.0 );
        thresholder->SetInsideValue( 0 );
        thresholder->SetOutsideValue( isoValue );
        thresholder->SetInput(levelSetFilter->GetOutput() );
        thresholder->Update();
        std::cout << "Done\n"; */

        writer->SetInput( levelSetFilter->GetOutput() );
        size_t ind = outFilenames[dtNo].find('.');
        std::string filename = outFilenames[dtNo];
        writer->SetFileName( filename.c_str() );
        writer->SetUseCompression(true);

        try {
            writer->Update();
        } catch (itk::ExceptionObject &ex) {
            std::cerr << "Exception caught!\n";
            std::cerr << ex << std::endl;
        }

        std::cout << " Done\n";

    }

    return EXIT_SUCCESS;
}
