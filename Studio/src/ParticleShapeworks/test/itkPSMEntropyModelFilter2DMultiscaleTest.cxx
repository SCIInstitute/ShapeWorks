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
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkPSMEntropyModelFilter.h"
#include "itkPSMProjectReader.h"
#include "itkCommand.h"

namespace itk{

class MyMultiscaleIterationCommand : public itk::Command
{
public:
 /** Standard class typedefs. */
  typedef MyMultiscaleIterationCommand         Self;
  typedef Command                    Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;

  typedef Image<float, 2> ImageType;

  /** Run-time type information (and related methods). */
  itkTypeMacro(MyMultiscaleIterationCommand, Command);

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** This method will be passed a PSMGradientDescentOptimizer */
  virtual void Execute(Object *caller, const EventObject &)
  {    
    PSMEntropyModelFilter<ImageType> *o 
      = static_cast<PSMEntropyModelFilter<ImageType> *>(caller);

    // Print every 10 iterations
    if (o->GetNumberOfElapsedIterations() % 10 != 0) return;
    
    std::cout << "Scale = " << o->GetCurrentScale() << std::endl;
    std::cout << "Iteration # " << o->GetNumberOfElapsedIterations() << std::endl;
    std::cout << " Eigenmode variances: ";
    for (unsigned int i = 0; i < o->GetShapePCAVariances().size(); i++)
      {
        std::cout << o->GetShapePCAVariances()[i] << " ";
      }
    std::cout << std::endl;
    std::cout << " Regularization = " << o->GetRegularizationConstant() << std::endl;
  }
  virtual void Execute(const Object *, const EventObject &)
  {
    std::cout << "SHOULDN'T BE HERE" << std::endl;
  }

protected:
  MyMultiscaleIterationCommand() {}
  ~MyMultiscaleIterationCommand() {}
private:
  MyMultiscaleIterationCommand(const Self &);        //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};

} // end namespace itk

/** This test exercises functionality of the base itkPSMEntropyModelFilter class */
int itkPSMEntropyModelFilter2DMultiscaleTest(int argc, char* argv[] )
{
  bool passed = true;
  std::string errstring = "";
  std::string output_path = "";
  std::string input_path_prefix = "";

  // Check for proper arguments
  if (argc < 2)
    {
      std::cout << "Wrong number of arguments. \nUse: " 
	<< "itkPSMEntropyModelFilterTest parameter_file [output_path]\n"
        << "See itk::PSMParameterFileReader for documentation on the parameter file format."
	<< std::endl;
      return EXIT_FAILURE;
    }
  
  if (argc >2)
    {
      output_path = std::string(argv[2]);
    }

  if (argc >3)
    {
      input_path_prefix = std::string(argv[3]);
    }
  
  typedef itk::Image<float, 2> ImageType;

  try
    {
     // Read the project parameters
     itk::PSMProjectReader::Pointer xmlreader =
       itk::PSMProjectReader::New();
     xmlreader->SetFileName(argv[1]);
     xmlreader->Update();

     itk::PSMProject::Pointer project = xmlreader->GetOutput();

     // Create the modeling filter and set up the optimization.
     itk::PSMEntropyModelFilter<ImageType>::Pointer P 
       = itk::PSMEntropyModelFilter<ImageType>::New();
 
     // Setup the Callback function that is executed after each
     // iteration of the solver.
     itk::MyMultiscaleIterationCommand::Pointer mycommand = itk::MyMultiscaleIterationCommand::New();
     P->AddObserver(itk::IterationEvent(), mycommand);

     // Load the distance transforms
     const std::vector<std::string> &dt_files = project->GetDistanceTransforms();
     std::cout << "Reading distance transforms ..." << std::endl;
     for (unsigned int i = 0; i < dt_files.size(); i++)
       {
         itk::ImageFileReader<ImageType>::Pointer reader = 
           itk::ImageFileReader<ImageType>::New();
         reader->SetFileName(input_path_prefix + dt_files[i]);
         reader->Update();
         
         std::cout << "  " << (input_path_prefix + dt_files[i]) << std::endl;

         P->SetInput(i,reader->GetOutput());
       }
     std::cout << "Done!" << std::endl;

     // Exception on failure
     unsigned int number_of_scales = project->GetNumberOfOptimizationScales();     
     std::cout << "Found " << number_of_scales << " number of scales. " << std::endl;

     // We need vectors of parameters, one for each scale.
     std::vector<double> regularization_initial(number_of_scales);
     std::vector<double> regularization_final(number_of_scales);
     std::vector<double> regularization_decayspan(number_of_scales);
     std::vector<double> tolerance(number_of_scales);
     std::vector<unsigned int> maximum_iterations(number_of_scales);

     // Read parameters for each scale
     for (unsigned int i = 0; i < number_of_scales; i++)
       {
         std::cout << "Optimization parameters for scale " << i << ": " << std::endl;
         regularization_initial[i] = project->GetOptimizationAttribute("regularization_initial",i);
         std::cout << "    regularization_initial = " << regularization_initial[i] << std::endl;
         regularization_final[i] = project->GetOptimizationAttribute("regularization_final",i);
         std::cout << "      regularization_final = " << regularization_final[i] << std::endl;
         regularization_decayspan[i] = project->GetOptimizationAttribute("regularization_decayspan",i);
         std::cout << "  regularization_decayspan = " << regularization_decayspan[i] << std::endl;
         tolerance[i] = project->GetOptimizationAttribute("tolerance",i);
         std::cout << "                 tolerance = " << tolerance[i] << std::endl;
         maximum_iterations[i] = static_cast<unsigned int>(project->GetOptimizationAttribute("maximum_iterations",i));
         std::cout << "        maximum_iterations = " << maximum_iterations[i] << std::endl;         
       }

     P->SetNumberOfScales(number_of_scales);
     P->SetRegularizationDecaySpan(regularization_decayspan);
     P->SetRegularizationInitial(regularization_initial);
     P->SetRegularizationFinal(regularization_final);
     P->SetTolerance(tolerance);
     P->SetMaximumNumberOfIterations(maximum_iterations);
     P->Update();
     
     // Print out points for domain d
     // Load the model initialization.  It should be specified as a model with a name. 
     const std::vector<std::string> &out_files = project->GetModel(std::string("optimized"));
     if (out_files.size() != P->GetParticleSystem()->GetNumberOfDomains())
       {
         errstring += "Number of output files does not match the number of particle domains (inputs).";
         passed = false;
       }

     if (passed = true)
       {
         for (unsigned int d = 0; d < P->GetParticleSystem()->GetNumberOfDomains(); d++)
           {
             // Open the output file.
             std::string fname = output_path + out_files[d];
             std::ofstream out( fname.c_str() );
             if ( !out )
               {
                 errstring += "Could not open point file for output: ";
               }
             else 
               {                 
                 for (unsigned int j = 0; j < P->GetParticleSystem()->GetNumberOfParticles(d); j++)
                   {
                     for (unsigned int i = 0; i < 2; i++)
                       {
                         out <<  P->GetParticleSystem()->GetPosition(j,d)[i]  << " ";
                       }
                     out << "0.0";
                     out << std::endl;
                   }         
               }
           }
       }

     
    }
  catch(itk::ExceptionObject &e)
    {
      errstring = "ITK exception with description: " + std::string(e.GetDescription())
        + std::string("\n at location:") + std::string(e.GetLocation())
        + std::string("\n in file:") + std::string(e.GetFile());
      passed = false;
    }
  catch(...)
    {
      errstring = "Unknown exception thrown";
      passed = false;
    }

  if (passed)
    {
    std::cout << "All tests passed" << std::endl;
    return EXIT_SUCCESS;
    }
  else
    {
    std::cout << "Test failed with the following error:" << std::endl;
    std::cout << errstring << std::endl;
    return EXIT_FAILURE;
    }
}

