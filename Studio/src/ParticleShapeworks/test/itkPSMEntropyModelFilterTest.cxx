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

class MyIterationCommand : public itk::Command
{
public:
  /** Standard class typedefs. */
  typedef MyIterationCommand         Self;
  typedef Command                    Superclass;
  typedef SmartPointer< Self >       Pointer;
  typedef SmartPointer< const Self > ConstPointer;
  
  typedef Image<float, 3> ImageType;
  
  /** Run-time type information (and related methods). */
  itkTypeMacro(MyIterationCommand, Command);
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** This method will be passed a PSMGradientDescentOptimizer */
  virtual void Execute(Object *caller, const EventObject &)
  {
    PSMEntropyModelFilter<ImageType> *o
            = static_cast<PSMEntropyModelFilter<ImageType> *>(caller);
    
    // Print every 10 iterations
    if (o->GetNumberOfElapsedIterations() % 10 != 0) { return; }
    
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
  MyIterationCommand() {}
  ~MyIterationCommand() {}
private:
  MyIterationCommand(const Self &);        //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};
} // end namespace itk

/** This test exercises functionality of the base itkPSMEntropyModelFilter class */
int itkPSMEntropyModelFilterTest(int argc, char* argv[] )
{
  bool passed = true;
  std::string errstring = "";
  std::string output_path = "";
  std::string input_path_prefix = "";
  
  // Check for proper arguments
  if (argc < 2)
    {
        std::cout << "Wrong number of arguments. \nUse: "
        << "itkPSMEntropyModelFilterTest parameter_file [output_path] [input_path]\n"
        << "See itk::PSMParameterFileReader for documentation on the parameter file format.\n"
        <<" Note that input_path will be prefixed to any file names and paths in the xml parameter file.\n"
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
    
  typedef itk::Image<float, 3> ImageType;
    
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
      itk::MyIterationCommand::Pointer mycommand = itk::MyIterationCommand::New();
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
          
          std::cout << "  " << dt_files[i] << std::endl;
          
          P->SetInput(i,reader->GetOutput());
        }
        std::cout << "Done!" << std::endl;
        
        std::cout << "Number of inputs = " << P->GetNumberOfInputs() << std::endl;
        std::cout << "Correct number of inputs = " << dt_files.size() << std::endl;
        
        // Load the model initialization.  It should be specified as a model with a name.
        const std::vector<std::string> &pt_files = project->GetModel(std::string("initialization"));
        std::cout << "Reading the initial model correspondences ..." << std::endl;
        for (unsigned int i = 0; i < pt_files.size(); i++)
          {
            // Read the points for this file and add as a list
            std::vector<itk::PSMEntropyModelFilter<ImageType>::PointType> c;
            
            int counter = 0;
            // Open the ascii file.
            std::ifstream in( (input_path_prefix + pt_files[i]).c_str() );
            if ( !in )
            {
                errstring += "Could not open point file for input.";
                passed = false;
                break;
            }
            
            // Read all of the points, one point per line.
            while (in)
            {
              itk::PSMEntropyModelFilter<ImageType>::PointType pt;
              
              for (unsigned int d = 0; d < 3; d++)
                {
                  in >> pt[d];
                }
              c.push_back(pt);
              counter++;
            }
            // this algorithm pushes the last point twice
            c.pop_back();
            //  std::cout << "Read " << counter-1 << " points. " << std::endl;
            in.close();
            
            P->SetInputCorrespondencePoints(i,c);
            
            std::cout << "  " << pt_files[i] << std::endl;
          }
        std::cout << "Done!" << std::endl;
        
        //  Read some parameters from the file or provide defaults
        double regularization_initial   = 100.0f;
        double regularization_final     = 5.0f;
        double regularization_decayspan = 2000.0f;
        double tolerance                = 1.0e-8;
        unsigned int maximum_iterations = 200000;
        if ( project->HasOptimizationAttribute("regularization_initial") )
          regularization_initial = project->GetOptimizationAttribute("regularization_initial");
        if ( project->HasOptimizationAttribute("regularization_final") )
          regularization_final = project->GetOptimizationAttribute("regularization_final");
        if ( project->HasOptimizationAttribute("regularization_decayspan") )
          regularization_decayspan = project->GetOptimizationAttribute("regularization_decayspan");
        if ( project->HasOptimizationAttribute("tolerance") )
          tolerance = project->GetOptimizationAttribute("tolerance");
        if ( project->HasOptimizationAttribute("maximum_iterations") )
          maximum_iterations = static_cast<unsigned int>(project->GetOptimizationAttribute("maximum_iterations"));
        
        std::cout << "Optimization parameters: " << std::endl;
        std::cout << "    regularization_initial = " << regularization_initial << std::endl;
        std::cout << "      regularization_final = " << regularization_final << std::endl;
        std::cout << "  regularization_decayspan = " << regularization_decayspan << std::endl;
        std::cout << "                 tolerance = " << tolerance << std::endl;
        std::cout << "        maximum_iterations = " << maximum_iterations << std::endl;
        
        // Set the parameters and Run the optimization.
        P->SetMaximumNumberOfIterations(maximum_iterations);
        P->SetRegularizationInitial(regularization_initial);
        P->SetRegularizationFinal(regularization_final);
        P->SetRegularizationDecaySpan(regularization_decayspan);
        P->SetTolerance(tolerance);
        P->Update();
        
        if (P->GetNumberOfElapsedIterations() >= maximum_iterations)
        {
            errstring += "Optimization did not converge based on tolerance criteria.\n";
            passed = false;
        }
        
        // Print out points for domain d
        // Load the model initialization.  It should be specified as a model with a name.
        const std::vector<std::string> &out_files = project->GetModel(std::string("optimized"));
        if (out_files.size() != P->GetParticleSystem()->GetNumberOfDomains())
        {
            errstring += "Number of output files does not match the number of particle domains (inputs).";
            passed = false;
        }
        
        if (passed == true)
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
                      for (unsigned int i = 0; i < 3; i++)
                        {
                          out <<  P->GetParticleSystem()->GetPosition(j,d)[i]  << " ";
                        }
                        out << std::endl;
                    }
                }
              }
          }
        
        // Now run for a specific number of iterations.  Also tests
        // restart of the filter.
        P->SetMaximumNumberOfIterations(3);
        P->SetTolerance(0.0f); // impossible convergence criterium
        P->Update();
        if (P->GetNumberOfElapsedIterations() != 3)
          {
            errstring += "Optimization did not iterate the specified number of fixed iterations.\n";
            passed = false;
        }
    }
  catch(itk::ExceptionObject &e)
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
