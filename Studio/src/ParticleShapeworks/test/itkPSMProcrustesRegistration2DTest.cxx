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
#include <string>
#include <sstream>
#include "itkPSMProcrustesRegistration.h"
#include "itkImage.h"
#include "itkImageFileReader.h"
#include "itkPSMEntropyModelFilter.h"
#include "itkPSMProjectReader.h"
#include "itkPSMParticleSystem.h"
#include "itkPSMRegionDomain.h"
#include "vnl/vnl_matrix_fixed.h"
#include "vnl/vnl_vector_fixed.h"
#include "itkCommand.h"

namespace itk{

class MyPSMProcrustesIterationCommand : public itk::Command
{
public:
  /** Standard class typedefs. */
  typedef MyPSMProcrustesIterationCommand         Self;
  typedef Command                                 Superclass;
  typedef SmartPointer< Self >                    Pointer;
  typedef SmartPointer< const Self >              ConstPointer;
  
  typedef Image<float, 2> ImageType;
  
  PSMProcrustesRegistration<2> *procrustesRegistration;
  /** Run-time type information (and related methods). */
  itkTypeMacro(MyPSMProcrustesIterationCommand, Command);
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);
  
  /** This method will be passed a PSMProcrustesRegistration */
  virtual void Execute(Object *caller, const EventObject &)
  {
    PSMEntropyModelFilter<ImageType> *o
      = static_cast<PSMEntropyModelFilter<ImageType> *>(caller);
    
    if (this->procrustesRegistration->GetProcrustesInterval() != 0)
      {
      this->m_ProcrustesCounter++;
      
      if (this->m_ProcrustesCounter >= (int)this->procrustesRegistration->GetProcrustesInterval())
        {
        // Reset the counter
        this->m_ProcrustesCounter = 0;
        this->procrustesRegistration->RunRegistration();
        std::cout << "Run Procrustes Registration" << std::endl;
        }
      }
    
    // Print every 10 iterations
    if (o->GetNumberOfElapsedIterations() % 10 != 0) return;
    
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
  void SetPSMProcrustesRegistration(PSMProcrustesRegistration<2> *p)
  { procrustesRegistration = p;  }
  
protected:
  MyPSMProcrustesIterationCommand()
  {
    m_ProcrustesCounter = 0;
  }
  ~MyPSMProcrustesIterationCommand() {}
private:
  int m_ProcrustesCounter;
  MyPSMProcrustesIterationCommand(const Self &);        //purposely not implemented
  void operator=(const Self &); //purposely not implemented
};

} // end namespace itk

/** \class object_reader
 * Reads a std::vector of c++ objects.  The first integer in the file is assumed to
 * represent the number of transforms in the file.  The size of each transform
 * is determined by the templating.
 */
template <class T>
class object_reader
{
public:
  /** Standard class typedefs */
  typedef object_reader Self;
  typedef T ObjectType;
  
  /** Get the output of the reader.  The output is a std::vector of TransformType. */
  const std::vector<ObjectType> &GetOutput() const
  {
    return m_Output;
  }
  std::vector<ObjectType> &GetOutput()
  {
    return m_Output;
  }
  
  void SetFileName(const char *fn)
  { m_FileName = fn; }
  void SetFileName(const std::string &fn)
  { m_FileName = fn; }
  const std::string& GetFileName() const
  { return m_FileName; }
  
  /** Read the file. */
  inline void Read()
  { this->Update(); }
  
  void Update()
  {
    // Open the input file.
    std::ifstream in( m_FileName.c_str(), std::ios::binary );
    
    if (!in)
      {
      std::cerr << "Could not open filename " << m_FileName << std::endl;
      throw 1;
      }
    // Read the number of transforms
    int N;
    in.read(reinterpret_cast<char *>(&N), sizeof(int));
    
    int sz = sizeof(ObjectType);
    // Read the transforms
    for (unsigned int i = 0; i < (unsigned int)N; i++)
      {
      ObjectType q; // maybe not the most efficient, but safe
      in.read(reinterpret_cast<char *>(&q), sz);
      m_Output.push_back(q);
      }
        in.close();
  }
  
  object_reader() { }
  virtual ~object_reader() {};
  
private:
  object_reader(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  std::vector<ObjectType> m_Output;
  std::string m_FileName;
};


/** This test exercises functionality of the base itkPSMProcrustesRegistration class */
int itkPSMProcrustesRegistration2DTest(int argc, char* argv[] )
{
  bool passed = true;
  std::string errstring = "";
  std::string output_path = "";
  std::string input_path_prefix = "";
  
  // Check for proper arguments
  if (argc < 3)
    {
    std::cout << "Wrong number of arguments. \nUse: "
              << "itkPSMProcrustesRegistration2DTest parameter_file transforms_file [output_path] [input_path]\n"
              << "See itk::PSMParameterFileReader for documentation on the parameter file format.\n"
              <<" Note that input_path will be prefixed to any file names and paths in the xml parameter file.\n"
              << std::endl;
    return EXIT_FAILURE;
    }
  
  if (argc >3)
    {
    output_path = std::string(argv[3]);
    }
  
  if (argc >4)
    {
    input_path_prefix = std::string(argv[4]);
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
    itk::MyPSMProcrustesIterationCommand::Pointer mycommand
      = itk::MyPSMProcrustesIterationCommand::New();
    P->AddObserver(itk::IterationEvent(), mycommand);
    
    // Create the ProcrustesRegistration pointer
    itk::PSMProcrustesRegistration<2>::Pointer procrustesRegistration
      = itk::PSMProcrustesRegistration<2>::New();
    
    mycommand->SetPSMProcrustesRegistration( procrustesRegistration );
    // Load the distance transforms
    const std::vector<std::string> &dt_files = project->GetDistanceTransforms();
    itk::ImageFileReader<ImageType>::Pointer reader =
      itk::ImageFileReader<ImageType>::New();
    
    std::cout << "Reading distance transforms ..." << std::endl;
    for (unsigned int i = 0; i < dt_files.size(); i++)
      {
      reader->SetFileName(input_path_prefix + dt_files[i]);
      reader->Update();
      
      std::cout << "  " << dt_files[i] << std::endl;
      }
    //TODO: Why does number of inputs need to be set to greater than 100?
    for(unsigned int i = 0; i < 103; i++)
      {
      P->SetInput(i,reader->GetOutput());
      }
    std::cout << "Done!" << std::endl;
    std::cout << "Number of inputs: " << P->GetNumberOfInputs() << std::endl;
    
    // Load the model initialization.  It should be specified as a model with a name.
    const std::vector<std::string> &pt_files = project->GetModel(std::string("initialization"));
    std::vector<itk::PSMEntropyModelFilter<ImageType>::PointType> c;
    std::cout << "Reading the initial model correspondences ..." << std::endl;
    unsigned int numOfPoints;
    for (unsigned int i = 0; i < pt_files.size(); i++)
    {
        // Read the points for this file and add as a list
        numOfPoints = 0;
        // Open the ascii file.
        std::ifstream in( (input_path_prefix + pt_files[0]).c_str() );
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
            
            for (unsigned int d = 0; d < 2; d++)
            {
                in >> pt[d];
            }
            c.push_back(pt);
            numOfPoints++;
        }
        // this algorithm pushes the last point twice
        c.pop_back();
        std::cout << "Read " << numOfPoints-1 << " points. " << std::endl;
        in.close();
    }
    
    for(unsigned int i = 0; i < 100; i++)
    {
        P->SetInputCorrespondencePoints(i,c);
    }
    
    std::cout << "Done!" << std::endl;
    
    // Read the input transforms
    object_reader< itk::PSMParticleSystem<2>::TransformType > transform_reader;
    transform_reader.SetFileName(argv[2]);
    transform_reader.Update();

    std::cout << "Reading transforms." << std::endl;
    // Read transforms and apply to the Particle System
    for (unsigned int i = 0; i < P->GetParticleSystem()->GetNumberOfDomains(); i++)
      {
      for(unsigned int j = 0; j < numOfPoints; j++)
        {
        itk::PSMEntropyModelFilter<ImageType>::PointType point, trPoint;
        itk::PSMParticleSystem<2>::TransformType transform;
        itk::PSMParticleSystem<2>::Pointer PS = P->GetParticleSystem();
        
        point[0] = PS->GetPosition(j,i)[0];
        point[1] = PS->GetPosition(j,i)[1];

        // Transform the points and set them in the Particle System
        trPoint = PS->TransformPoint( point, transform_reader.GetOutput()[i] );
        PS->SetPosition( trPoint, j, i);
        }
      }
        
    //  Read some parameters from the file or provide defaults
    double regularization_initial   = 100.0f;
    double regularization_final     = 5.0f;
    double regularization_decayspan = 2000.0f;
    double tolerance                = 1.0e-8;
    unsigned int maximum_iterations = 200000;
    unsigned int procrustes_interval = 1;
    if ( project->HasOptimizationAttribute("regularization_initial") )
    {
        regularization_initial = project->GetOptimizationAttribute("regularization_initial");
    }
    if ( project->HasOptimizationAttribute("regularization_final") )
    {
        regularization_final = project->GetOptimizationAttribute("regularization_final");
    }
    if ( project->HasOptimizationAttribute("regularization_decayspan") )
    {
        regularization_decayspan = project->GetOptimizationAttribute("regularization_decayspan");
    }
    if ( project->HasOptimizationAttribute("tolerance") )
    {
        tolerance = project->GetOptimizationAttribute("tolerance");
    }
    if ( project->HasOptimizationAttribute("maximum_iterations") )
    {
        maximum_iterations
        = static_cast<unsigned int>(project->GetOptimizationAttribute("maximum_iterations"));
    }
    if ( project->HasOptimizationAttribute("procrustes_interval") )
    {
        procrustes_interval
        = static_cast<unsigned int>(project->GetOptimizationAttribute("procrustes_interval"));
    }
        
    // Set variables for PSMProcrustesRegistration
    procrustesRegistration->SetProcrustesInterval(procrustes_interval);
    procrustesRegistration->SetPSMParticleSystem(P->GetParticleSystem());
    
    std::cout << "Optimization parameters: " << std::endl;
    std::cout << "    regularization_initial = " << regularization_initial << std::endl;
    std::cout << "      regularization_final = " << regularization_final << std::endl;
    std::cout << "  regularization_decayspan = " << regularization_decayspan << std::endl;
    std::cout << "                 tolerance = " << tolerance << std::endl;
    std::cout << "        maximum_iterations = " << maximum_iterations << std::endl;
    std::cout << "       procrustes_interval = " << procrustes_interval << std::endl;
    
    // Set the parameters and run the optimization.
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
    
    // Write out the transforms
    std::string output_transform_file = "output_transforms_PSMProcrustesRegistration2DTest.txt";
    std::string out_file = output_path + output_transform_file;
    std::ofstream out(out_file.c_str());
    for (unsigned int d = 0; d < P->GetParticleSystem()->GetNumberOfDomains(); d++)
    {
        if(!out)
        {
           errstring += "Could not open file for output: ";
        }
        else
        {
           out << P->GetParticleSystem()->GetTransform(d);
           out << std::endl;
        }
    }
        
    // Print out points for domain d
    // Load the model initialization.  It should be specified as a model with a name.
    const std::vector<std::string> &out_files = project->GetModel(std::string("optimized"));
    
    for (unsigned int d = 0; d < P->GetParticleSystem()->GetNumberOfDomains(); d++)
      {
      // Open the output file and append the number
      std::ostringstream ss;
      ss << d;
      std::string fname = output_path + out_files[0] + "_" + ss.str() + ".lpts";
      std::ofstream out( fname.c_str() );
      if ( !out )
        {
        errstring += "Could not open point file for output: ";
        }
      else
        {
        for (unsigned int j = 0; j < P->GetParticleSystem()->GetNumberOfParticles(d); j++)
          {
          //for (unsigned int i = 0; i < 2; i++)
          //  {
          // Print the last point as 0.0 so that SWViewer can read the point files
          out <<  P->GetParticleSystem()->GetPosition(j,d)[0]  << " " << P->GetParticleSystem()->GetPosition(j,d)[1] << " " << 0.0;
          //  }
          out << std::endl;
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
