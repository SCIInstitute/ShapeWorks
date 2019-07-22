/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/

#include <iostream>
#include <vector>
#include <math.h>
#include <numeric>
#include "itkPSMCommandLineClass.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
//#include "itkPSMCommandLineClass.cxx"
#include "itkPSMImplicitSurfaceImageFilter.h"
#include "itkExceptionObject.h"
#include "itkDOMNode.h"
#include "itkDOMNodeXMLWriter.h"
#include "itkTranslationTransform.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkResampleImageFilter.h"
#include "itkChangeInformationImageFilter.h"
#include "itkNumericTraits.h"
#include "itkArray.h"

namespace itk{
// Function for generating random Gaussians using polar Box-Muller method
static void RandomGaussian(double n, double mean, double sigma, std::vector<double> & r)
{
  double r2, x = 0, y = 0;
  for(int i = 0; i < n; i++)
  {
    r2 = 0;
    // Generate random Gaussians 
    while(r2 >= 1.0 || r2 == 0)
    {      
      x = 2.0 * static_cast<double>(rand()) /
           static_cast<double>(RAND_MAX) - 1.0;
      y = 2.0 * static_cast<double>(rand()) /
           static_cast<double>(RAND_MAX) - 1.0;
      r2 = x * x + y * y;
    }
    r[i] = mean + sigma * y * sqrt(-2.0 * log(r2) / r2);
    // Check that random radius value does not exceed half the set image size.
    // This avoids an 'out of bounds' error during optimization.
    if (r[i] > 100.0)
      r[i] = 99.0;
  }
}

// Function for calculating slope of regression line
static double Slope(const std::vector<double>& x, const std::vector<double>& y)
{
  if(x.size() != y.size())
  {
    std::cerr << "Error! Vectors are not the same length." << std::endl;
    return EXIT_FAILURE;
  }
  double n = x.size();
  
  double avgX = std::accumulate(x.begin(), x.end(), 0.0) / n;
  double avgY = std::accumulate(y.begin(), y.end(), 0.0) / n;
  
  double numerator = 0.0;
  double denominator = 0.0;
  
  for(int i=0; i<n; ++i)
  {
    numerator += (x[i] - avgX) * (y[i] - avgY);
    denominator += (x[i] - avgX) * (x[i] - avgX);
  }
  
  if(denominator == 0)
  {
    std::cerr << "Error! Divide by zero." << std::endl;
    return EXIT_FAILURE;
  }
  // Calculate slope
  double b1 = numerator / denominator;
  // Calculate Y intercept
  //double b0 = avgY - (b1*avgX);
  
  double sumXY = 0.0, sum_squareX = 0.0, sum_squareY = 0.0;
  for(int i=0; i<n; i++)
  {
    sumXY += x[i] * y[i];
    sum_squareX += x[i] * x[i];
    sum_squareY += y[i] * y[i];
  }
  // Calculate correlation coeff and coeff of determination
  double r_xy = sumXY / sqrt(sum_squareX * sum_squareY);
  std::cout << "\nCorrelation coefficient          = " << r_xy << std::endl;
  std::cout << "Coefficient of determination     = " << (r_xy * r_xy) << std::endl;
  
  return b1;
}

// Function for centering the image in index space
static void Center(itk::Image<float, 2> *input)
{
  typedef itk::Image<float, 2> ImageType;
  ImageType::PixelType m_BackgroundValue = itk::NumericTraits< ImageType::PixelType >::Zero;
  // Reset the physical coordinates at the center of the image to (0,0,0)
  ImageType::PointType lower;
  ImageType::PointType upper;
  ImageType::PointType ul_physical_coords;
  input->TransformIndexToPhysicalPoint(input->GetBufferedRegion().GetIndex(), lower);
  input->TransformIndexToPhysicalPoint(input->GetBufferedRegion().GetIndex()
                                      + input->GetBufferedRegion().GetSize(), upper);
  for (unsigned int i = 0; i < 2; i++)
  {
    ul_physical_coords[i] = -(lower[i] + upper[i]) / 2.0;
  }

  // The following command sets the physical coordinates
  // of pixel number 0,0,0... (i.e. the upper-left corner of the volume).
  input->SetOrigin(ul_physical_coords);
  
  // Find the center of mass.
  itk::ImageRegionIteratorWithIndex<ImageType> oit(input, input->GetBufferedRegion());
  oit.GoToBegin();
  
  itk::Array<double> params(2);
  params.Fill(0.0);
  double count = 0.0;
  itk::Point<double, 2> point;
  for (; ! oit.IsAtEnd(); ++oit)
  {
    if (oit.Get() != m_BackgroundValue)
    {
      // Get the physical index from the image index.
      input->TransformIndexToPhysicalPoint(oit.GetIndex(), point);
      for (unsigned int i = 0; i < 2; i++) { params[i] += point[i]; }
      count += 1.0;
    }
  }
  
  // Compute center of mass.
  for (unsigned int i = 0; i < 2; i++)
  {
    params[i] = params[i] / count;
  }

  // Translate the segmentation into the output
  itk::TranslationTransform<double,2>::Pointer trans
  = itk::TranslationTransform<double,2>::New();
  trans->SetParameters(params);
  
  itk::NearestNeighborInterpolateImageFunction<ImageType,double>::Pointer
  interp = itk::NearestNeighborInterpolateImageFunction<ImageType,double>::New();
  
  itk::ResampleImageFilter<ImageType, ImageType>::Pointer resampler
  = itk::ResampleImageFilter<ImageType, ImageType>::New();
  resampler->SetOutputParametersFromImage(input);
  resampler->SetTransform(trans);
  resampler->SetInterpolator(interp);
  resampler->SetInput(input);
  resampler->Update();
}
} // end namespace itk

// Function to return the greater absolute of two values with different signs
static bool abs_compare(double a, double b)
{
  return (std::abs(a) < std::abs(b));
}

/** This test exercises functionality of the base itkPSMCommandLineClass class */
int itkPSMCommandLineClass2DTest( int argc, char* argv[] )
{
  std::string output_path = "";
  std::string input_path_prefix = "";
  std::string errstring = "";
  // Check for proper arguments
  if (argc < 1)
  {
    std::cout << "Wrong number of arguments. \nUse: "
    << "itkPSMCommandLineClass2DTest [output_path] [input_path]\n"
    << "Note that input_path will be prefixed to any file names and paths in the xml parameter file.\n"
    << std::endl;
    return EXIT_FAILURE;
  }
  
  if (argc > 1)
  {
    output_path = std::string(argv[1]);
  }
  
  if (argc > 2)
  {
    input_path_prefix = std::string(argv[2]);
  }
  
  try
  {
    // Standard typedefs
    typedef itk::Image< unsigned char, 2 > ImageType;
    typedef itk::Image<float, 2> DTImageType;
    typedef itk::PSMImplicitSurfaceImageFilter<ImageType, DTImageType> ImplicitSurfaceImageFilterType;
    typedef itk::PSMCommandLineClass<2> CommandLineClassType;
    ImplicitSurfaceImageFilterType::Pointer implicitSurfaceFilter = ImplicitSurfaceImageFilterType::New();
  
    // Generate 10 ellipses that will be used as inputs to the optimization.
    // Variables for generating random radius values.
    int number_of_ellipses = 10;
    int r1 = 30;
    int r2 = 80;
    
    srand(time(NULL));
    // Randomly generate a value for sigma greater than 1
    double sigma_intercept = ((double)rand()) / ((double)RAND_MAX) * 9.9 + 2.0;
    std::cout << "sigma_intercept: " << sigma_intercept << std::endl;
    
    std::vector<double> radius_minor, radius_major;
    radius_minor.resize(number_of_ellipses);
    radius_major.resize(number_of_ellipses);
    
    // Generate the radius values
    itk::RandomGaussian(number_of_ellipses, r1, sigma_intercept, radius_minor);
    itk::RandomGaussian(number_of_ellipses, r2, sigma_intercept, radius_major);
   
    for(int i=0; i<radius_major.size(); i++)
    {
      std::cout << "Input_R: " << radius_major[i] << " Input_r: " << radius_minor[i] << std::endl;
    }

    // Create DOMNode elements for output project file
    itk::DOMNode::Pointer inputDOMObject = itk::DOMNode::New();
    inputDOMObject->SetName( "psm_project" ); 
    
    // Create nodes and add them to the DOM object
    itk::DOMNode::Pointer data_node = itk::DOMNode::New();
    data_node->SetName( "data" );
    inputDOMObject->AddChildAtEnd( data_node );
    
    // Create the distance transforms node
    itk::DOMNode::Pointer distance_trans_node = itk::DOMNode::New();
    distance_trans_node->SetName( "distance_transforms" );
    data_node->AddChildAtEnd( distance_trans_node );
    
    // Create the model node
    itk::DOMNode::Pointer model_node = itk::DOMNode::New();
    model_node->SetName( "model" );
    data_node->AddChildAtEnd( model_node );
    
    // Add an attribute to the model node
    std::string att = "optimized";
    itk::FancyString fs;
    fs << att;
    model_node->SetAttribute( "name", att );
    
    // Create the optimization node
    itk::DOMNode::Pointer opt_node = itk::DOMNode::New();
    opt_node->SetName( "optimization" );
    inputDOMObject->AddChildAtEnd( opt_node );
    
    // Default optimization parameters
    int num_of_scales = 8, reg_ds = 5000, max_iter = 1000;
    double reg_init = 10.0, reg_final = 2.0, tol = 0.01;
    fs << itk::ClearContent << num_of_scales;
    opt_node->SetAttribute( "number_of_scales", fs );
    
    for(int i=0; i<num_of_scales; i++)
    {
      // Create the scale node with default paramter values
      itk::DOMNode::Pointer scale_node = itk::DOMNode::New();
      scale_node->SetName( "scale" );
      fs << itk::ClearContent << i;
      scale_node->SetAttribute( "number", fs );
      fs << itk::ClearContent << reg_init;
      scale_node->SetAttribute( "regularization_initial", fs );
      fs << itk::ClearContent << reg_final;
      scale_node->SetAttribute( "regularization_final", fs );
      fs << itk::ClearContent << reg_ds;
      scale_node->SetAttribute( "regularization_decayspan", fs );
      fs << itk::ClearContent << tol;
      scale_node->SetAttribute( "tolerance", fs );
      fs << itk::ClearContent << max_iter;
      scale_node->SetAttribute( "maximum_iterations", fs );
      // Add the scale nodes to the optimization node
      opt_node->AddChildAtEnd( scale_node );
    }
    // Distance transform parameters
    double sigma = 0.5;
    double isosurface_value = 0.5;
    
    // Iterator for ellipse image
    typedef itk::ImageRegionIteratorWithIndex<ImageType>  OutputIteratorType;
    
    std::cout << "Writing ellipse distance transforms..." << std::endl;
    // Now generate the ellipses and their distance transforms
    for(int i=0; i<number_of_ellipses; i++)
    {
      // Create an ellipse image with specified size and spacing.
      ImageType::Pointer ellipseImage = ImageType::New();
      ImageType::SizeType regionSize;
      regionSize[0] = 200;
      regionSize[1] = 200;
      
      ImageType::IndexType regionIndex;
      regionIndex[0] = 0;
      regionIndex[1] = 0;
      
      ImageType::RegionType region;
      region.SetSize(regionSize);
      region.SetIndex(regionIndex);
      
      ImageType::SpacingType spacing;
      spacing[0] =  1;
      spacing[1] =  1;
      ellipseImage->SetSpacing( spacing );
      ellipseImage->SetRegions(region);
      ellipseImage->Allocate();

      ellipseImage->FillBuffer(0);
      
      double r = radius_minor[i];
      double R = radius_major[i];      
           
      OutputIteratorType it( ellipseImage, ellipseImage->GetRequestedRegion() );
      it.GoToBegin();
      
      ImageType::IndexType index;
      // Set pixels which are within ellipse area to white. Center of ellipse is (100,100)
      for(int y=-r; y<=r; y++)
      {
        for(int x=-R; x<=R; x++)
        {
          if(x*x*r*r + y*y*R*R <= r*r*R*R)
          {
            index[0] = 100+x;
            index[1] = 100+y;
            it.SetIndex( index );
            it.Set( itk::NumericTraits<ImageType::PixelType>::max() );
          }
        }
      }
      
      // Generate the distance transforms
      implicitSurfaceFilter->SetSmoothingSigma(sigma);
      implicitSurfaceFilter->SetIsosurfaceValue(isosurface_value);
      implicitSurfaceFilter->SetInput(ellipseImage);
      implicitSurfaceFilter->Update();
      
      // Center the image in index space
      itk::Center(implicitSurfaceFilter->GetOutput());
      
      std::string str = "ellipse";
      typedef itk::ImageFileWriter< DTImageType > WriterType;
      WriterType::Pointer writer = WriterType::New();
      std::ostringstream ss;
      ss << i;
      std::string iname = output_path + str + "_DT_Test" + ss.str() + ".nrrd";
      writer->SetFileName( iname.c_str() );
      writer->SetInput( implicitSurfaceFilter->GetOutput() );
      std::cout << "....";
      writer->Update();
      
      // Add the image file names to the node
      distance_trans_node->AddTextChildAtEnd( iname );
      
      // Add the output model names to the node
      std::string opt_name = str + "_" + ss.str() + "_optimized.lpts";
      model_node->AddTextChildAtEnd( opt_name );
    }

    // Write the DOM object to an XML file
    //const char*
    std::string outputXMLFileName = "PSMCommandLineClass2DTest.xml";
    std::string projectFileName = output_path + outputXMLFileName;
    itk::DOMNodeXMLWriter::Pointer writer_xml = itk::DOMNodeXMLWriter::New();
    writer_xml->SetInput( inputDOMObject );
    writer_xml->SetFileName( projectFileName.c_str() );
    writer_xml->Update();
    
    // Run the command line tool with the newly generated XML project
    // file as input. 
    itk::PSMCommandLineClass<2>::Pointer psmClass = itk::PSMCommandLineClass<2>::New();
    psmClass->Run( "PSMCommandLineClass2DTest.xml", output_path, output_path );
    
    CommandLineClassType::EntropyModelFilterType::Pointer filter = psmClass->GetPSMFilter();
    
    std::vector<double> output_x_coord;
    std::vector<double> output_y_coord;
    std::vector<double> output_radius_minor;
    std::vector<double> output_radius_major;
    // Find the radii of the output ellipse shapes
    for (unsigned int d=0; d<filter->GetParticleSystem()->GetNumberOfDomains(); d++)
    {
      for (unsigned int ind=0; ind<filter->GetParticleSystem()->GetNumberOfParticles(d); ind++)
      {
        output_x_coord.push_back( filter->GetParticleSystem()->GetPosition(ind,d)[0] ); // Get x co-ord
        output_y_coord.push_back( filter->GetParticleSystem()->GetPosition(ind,d)[1] ); // Get y co-ord
      }
      // Find the maxiumum absolute x and y co-ord values for each output ellipse
      output_radius_minor.push_back( std::abs(*std::max_element(output_y_coord.begin(), output_y_coord.end(), abs_compare)) );
      output_radius_major.push_back( std::abs(*std::max_element(output_x_coord.begin(), output_x_coord.end(), abs_compare)) );
      output_x_coord.clear();
      output_y_coord.clear();
    }
    // Print out the input radii along with mean and standard deviation
    std::cout << "\nR: major ellipse radius   r: minor ellipse radius" << std::endl;
    for(int i=0; i<radius_major.size(); i++)
    {
      std::cout << "Input_R: " << radius_major[i] << " Input_r: " << radius_minor[i] << std::endl;
    }

    std::cout << "\nInput R_mean    = " << r2 << std::endl;
    std::cout << "Input R_std_dev = " << sigma_intercept << std::endl;
    std::cout << "Input r_mean    = " << r1 << std::endl;
    std::cout << "Input r_std_dev = " << sigma_intercept << std::endl;
    
    // Linear regression slope calculations
    double sl = itk::Slope(radius_major, radius_minor);
    std::cout << "Regression line slope at input = " << sl << "\n" << std::endl;
    
    for(int i=0; i<output_radius_minor.size(); i++)
    {
      std::cout << "Output_R: " << output_radius_major[i] << " Output_r: " << output_radius_minor[i] << std::endl;
    }
    
    // Calculate the mean and standard deviation of the output radii
    double mean_R_output = std::accumulate(output_radius_major.begin(), output_radius_major.end(), 0.0) / number_of_ellipses;
    double mean_r_output = std::accumulate(output_radius_minor.begin(), output_radius_minor.end(), 0.0) / number_of_ellipses;
    
    double sq_sum_R = std::inner_product(output_radius_major.begin(), output_radius_major.end(), output_radius_major.begin(), 0.0);
    double stdev_R = std::sqrt((sq_sum_R / output_radius_major.size()) - (mean_R_output * mean_R_output) );
    
    std::cout << "\nOutput R_mean    = " << mean_R_output << std::endl;
    std::cout << "Output R_std_dev = " << stdev_R << std::endl;
    
    double sq_sum_r = std::inner_product(output_radius_minor.begin(), output_radius_minor.end(), output_radius_minor.begin(), 0.0);
    double stdev_r = std::sqrt((sq_sum_r / output_radius_minor.size()) - (mean_r_output * mean_r_output) );
    
    std::cout << "Output r_mean    = " << mean_r_output << std::endl;
    std::cout << "Output r_std_dev = " << stdev_r << std::endl;
    
    sl = itk::Slope(output_radius_major, output_radius_minor);
    std::cout << "Regression line slope for output = " << sl << "\n" << std::endl;
  }
  
  catch(itk::ExceptionObject &e)
  {
    std::cerr << "ITK exception with description: " << e.GetDescription()
    << "\n at location:" << e.GetLocation()
    << "\n in file:" << e.GetFile() << std::endl;
    return EXIT_FAILURE;
  }
  
  catch(...)
  {
    errstring = "Unknown exception thrown";
    return EXIT_FAILURE;
  }
  
  return EXIT_SUCCESS;
}
