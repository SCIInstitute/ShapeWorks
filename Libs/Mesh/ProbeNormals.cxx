#include <iostream>
#include <string>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkVectorLinearInterpolateImageFunction.h"
#include "itkGradientImageFilter.h"
#include "itkPoint.h"
#include <sstream>
#include <fstream>
#include "itkFixedArray.h"
#include "vnl/vnl_vector_fixed.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cout << "Usage: " << argv[0] << " DTfilename pointsFilename outFileName" << std::endl;
        return EXIT_FAILURE;
    }
    std::string imgFname = std::string(argv[1]);
    std::string ptsFname = std::string(argv[2]);
    std::string outFname = std::string(argv[3]);
    typedef  float  PixelType;
    typedef itk::Point<double, 3> PointType;
    typedef itk::FixedArray<PixelType, 3> VectorType;
    typedef vnl_vector_fixed<PixelType, 3> VnlVectorType;
    typedef itk::Image< PixelType,  3 >   ImageType;
    typedef itk::ImageFileReader< ImageType >  ReaderType;
    typedef itk::GradientImageFilter<ImageType> GradientImageFilterType;
    typedef typename GradientImageFilterType::OutputImageType GradientImageType;
    typedef itk::VectorLinearInterpolateImageFunction<GradientImageType, typename PointType::CoordRepType>
    GradientInterpolatorType;

    ReaderType::Pointer reader = ReaderType::New();
    reader->SetFileName( imgFname.c_str() );
    reader->Update();
    reader->UpdateLargestPossibleRegion();

    typename GradientImageFilterType::Pointer filter = GradientImageFilterType::New();
    filter->SetInput(reader->GetOutput());
    filter->SetUseImageSpacingOn();
    filter->Update();

    typename GradientInterpolatorType::Pointer gradientInterpolator = GradientInterpolatorType::New();
    gradientInterpolator->SetInputImage(filter->GetOutput());

    std::ifstream pointsFile(ptsFname.c_str());
    std::ofstream outfile(outFname.c_str());

    if (!pointsFile)
    {
        std::cout << "point file does not exist... " << ptsFname << std::endl;
        return EXIT_FAILURE;
    }
    int count = 0;
    while(pointsFile)
    {
        count++;
        PointType tmpPt;
        pointsFile >> tmpPt[0];
        if (!pointsFile)
        {
            count--;
            break;
        }
        pointsFile >> tmpPt[1];
        pointsFile >> tmpPt[2];
        std::cout << count << " " << std::endl;

        VectorType v = gradientInterpolator->Evaluate(tmpPt);
        VnlVectorType n = VnlVectorType(v.GetDataPointer());
        n.normalize();

        outfile << n[0] << " " << n[1] << " " << n[2] << std::endl;
    }
    pointsFile.close();
    outfile.close();
    std::cout << "Total number of points: " << count << std::endl;
    return 0;
}

