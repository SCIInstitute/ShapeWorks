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
#include <sstream>
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkPSMEntropyMixedEffectsModelFilter.h"
#include "itkPSMProjectReader.h"
#include "itkCommand.h"

namespace itk{
  
  class MyMixedEffectsIterationCommand : public itk::Command
  {
  public:
    /** Standard class typedefs. */
    typedef MyMixedEffectsIterationCommand         Self;
    typedef Command                    Superclass;
    typedef SmartPointer< Self >       Pointer;
    typedef SmartPointer< const Self > ConstPointer;
    
    typedef Image<float, 3> ImageType;
    
    /** Run-time type information (and related methods). */
    itkTypeMacro(MyMixedEffectsIterationCommand, Command);
    
    /** Method for creation through the object factory. */
    itkNewMacro(Self);
    
    /** This method will be passed a PSMGradientDescentOptimizer */
    virtual void Execute(Object *caller, const EventObject &)
    {
      PSMEntropyMixedEffectsModelFilter<ImageType> *o
      = static_cast<PSMEntropyMixedEffectsModelFilter<ImageType> *>(caller);
      
      // Print every 10 iterations
      //    if (o->GetNumberOfElapsedIterations() % 10 != 0) return;
      
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
    MyMixedEffectsIterationCommand() {}
    ~MyMixedEffectsIterationCommand() {}
  private:
    MyMixedEffectsIterationCommand(const Self &);        //purposely not implemented
    void operator=(const Self &); //purposely not implemented
  };
  
} // end namespace itk

/** This test exercises functionality of the base itkPSMEntropyMixedEffectsModelFilter class */
int itkPSMEntropyMixedEffectsModelFilterTest(int argc, char* argv[] )
{
  bool passed = true;
  std::string errstring = "";
  std::string output_path = "";
  std::string input_path_prefix = "";
  
  // Check for proper arguments
  if (argc < 2)
  {
    std::cout << "Wrong number of arguments. \nUse: "
	<< "itkPSMEntropyMixedEffectsModelFilterTest parameter_file [output_path] [input_path]\n"
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
    itk::PSMEntropyMixedEffectsModelFilter<ImageType>::Pointer P
    = itk::PSMEntropyMixedEffectsModelFilter<ImageType>::New();
    
    // Setup the Callback function that is executed after each
    // iteration of the solver.
    itk::MyMixedEffectsIterationCommand::Pointer mycommand = itk::MyMixedEffectsIterationCommand::New();
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
    
    // Load the model initialization.  It should be specified as a model with a name.
    /*const std::vector<std::string> &pt_files = project->GetModel(std::string("initialization"));
    std::cout << "Reading the initial model correspondences ..." << std::endl;
    for (unsigned int i = 0; i < pt_files.size(); i++)
    {
      // Read the points for this file and add as a list
      std::vector<itk::PSMEntropyMixedEffectsModelFilter<ImageType>::PointType> c;
      
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
        itk::PSMEntropyMixedEffectsModelFilter<ImageType>::PointType pt;
        
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
    std::cout << "Done!" << std::endl;*/
    
    // Read the explanatory variables (e.g. time)
    std::vector<double> expl;
    if (project->HasVariables("explanatory_variables"))
    {
      expl = project->GetVariables("explanatory_variables");
      
      if (expl.size() < dt_files.size())
      {
        errstring += "There are fewer explanatory variables than input files";
        passed = false;
      }
      else // set the explanatory variables
      {
        std::cout << "Setting variables" << std::endl;
        P->SetVariables(expl);
      }
    }
    else
    {
      errstring += "No explanatory variables were present for the regression.";
      passed = false;
    }
    
    //  Read some parameters from the file or provide defaults
    /*double regularization_initial   = 100.0f;
    double regularization_final     = 5.0f;
    double regularization_decayspan = 2000.0f;
    double tolerance                = 1.0e-8;
    unsigned int maximum_iterations = 20000;
    unsigned int num_individuals = 1;
    
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
    if ( project->HasOptimizationAttribute("num_individuals") )
      num_individuals = static_cast<unsigned int>(project->GetOptimizationAttribute("num_individuals"));
    
    std::cout << "Optimization parameters: " << std::endl;
    std::cout << "    regularization_initial = " << regularization_initial << std::endl;
    std::cout << "      regularization_final = " << regularization_final << std::endl;
    std::cout << "  regularization_decayspan = " << regularization_decayspan << std::endl;
    std::cout << "                 tolerance = " << tolerance << std::endl;
    std::cout << "        maximum_iterations = " << maximum_iterations << std::endl;*/
    
    // Setting scales to 9 produces 256 points at the output.
    unsigned int number_of_scales = 6;
    // Set default parameters for the optimization scales
    std::vector<double> regularization_initial(number_of_scales);
    std::vector<double> regularization_final(number_of_scales);
    std::vector<double> regularization_decayspan(number_of_scales);
    std::vector<double> tolerance(number_of_scales);
    std::vector<unsigned int> maximum_iterations(number_of_scales);
    unsigned int num_individuals = 4;
    
    std::cout << "Setting default optimization parameters for scales..." << std::endl;
    for (unsigned int i = 0; i < number_of_scales; i++)
    {
      std::cout << "Scale #" << i << " :" << std::endl;
      regularization_initial[i] = 10.0;
      std::cout << "    regularization_initial = " << regularization_initial[i] << std::endl;
      regularization_final[i] = 1.0;
      std::cout << "      regularization_final = " << regularization_final[i] << std::endl;
      regularization_decayspan[i] = 1000;
      std::cout << "  regularization_decayspan = " << regularization_decayspan[i] << std::endl;
      tolerance[i] = 0.1;
      std::cout << "                 tolerance = " << tolerance[i] << std::endl;
      maximum_iterations[i] = 20;
      std::cout << "        maximum_iterations = " << maximum_iterations[i] << std::endl;
      /*if(this->m_Project->HasProcrustes() == true)
      {
        // Set the Procrustes interval for each scale
        unsigned int procrustes_interval = 10;
        m_ProcrustesInterval[i] = procrustes_interval;
        std::cout << "        procrustes_interval = " << procrustes_interval << std::endl;
      }*/
    }  
    
    // Set the parameters and Run the optimization.
    P->SetNumberOfScales(number_of_scales);
    P->SetMaximumNumberOfIterations(maximum_iterations);
    P->SetRegularizationInitial(regularization_initial);
    P->SetRegularizationFinal(regularization_final);
    P->SetRegularizationDecaySpan(regularization_decayspan);
    P->SetTolerance(tolerance);
    P->SetNumIndividuals(num_individuals);
    
    std::cout << "        num_individuals = " << P->GetNumIndividuals() << std::endl;
    
    // Read the number of timepoints per individual
    if (project->HasVariables("timepts_per_individual"))
    {
      std::vector<double> tp = project->GetVariables("timepts_per_individual");
      vnl_vector<int> timepts; timepts.set_size(tp.size());
      for(int i = 0; i < timepts.size(); i++)
        timepts[i] = static_cast<int>(tp[i]);
      
      if (timepts.size() != num_individuals)
      {
        errstring += "Number of individuals and timepts associated dont match!";
        passed = false;
      }
      else // set the timepts variables
      {
        std::cout << "Setting variables" << std::endl;
        P->SetTimePointsPerIndividual(timepts);
      }
    }
    else
    {
      errstring += "No timepts variables were present.";
      passed = false;
    }
    
    // Update parameters
    P->Update();
    
    //   if (P->GetNumberOfElapsedIterations() >= maximum_iterations)
    //  {
    //    errstring += "Optimization did not converge based on tolerance criteria.\n";
    //    passed = false;
    //  }
    
    
    const std::vector<std::string> &out_files = project->GetModel(std::string("optimized"));
    if (out_files.size() != P->GetParticleSystem()->GetNumberOfDomains())
    {
      errstring += "Number of output files does not match the number of particle domains (inputs).";
      passed = false;
    }
    // Print out points for domain d
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
    
    std::vector<double>::iterator it_expl;
    vnl_vector<double> output_intercept_slope;
    vnl_vector<double>::iterator output_it;
    double min_expl = *std::min_element(expl.begin(), expl.end());
    double max_expl = *std::max_element(expl.begin(), expl.end());
    int step = 4;
    int file_count = 0;
    //for(it_expl = expl.begin(); it_expl != expl.end(); ++it_expl)
    for(int i = 0; i < step; i++)
    {
      double value = min_expl + i*((max_expl - min_expl)/(step - 1));
      output_intercept_slope = P->GetShapeMatrix()->ComputeMean(value);
      std::ostringstream ss;
      ss << file_count;
      std::string fname = output_path + "Intercept+TimexSlope" + "_" + ss.str() + ".lpts";
      std::ofstream out(fname.c_str());
      if ( !out )
      {
        errstring += "Could not open point file for output: ";
      }
      else
      {
        int counter = 0;
        for(output_it = output_intercept_slope.begin(); output_it != output_intercept_slope.end(); ++output_it)
        {
          out << *output_it << ' ';
          counter++;
          if(counter == 3)
          {
            out << std::endl;
            counter = 0;
          }
        }
      }
      file_count++;
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
