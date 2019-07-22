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

#ifndef ____itkPSMCommandLineClass__hxx
#define ____itkPSMCommandLineClass__hxx

#include "itkPSMCommandLineClass.h"
#include <fstream>
#include <vector>

namespace itk
{
  //template class PSMCommandLineClass<2>;
  //template class PSMCommandLineClass<3>;

  template <unsigned int VDimension>
  PSMCommandLineClass<VDimension>
    ::PSMCommandLineClass()
  {
    m_ProcrustesCounter = 0;
    m_ReportInterval = 10;
    m_ProjectReader = ProjectReaderType::New();
    m_Project = ProjectType::New();
    m_ProcrustesRegistration = ProcrustesRegistrationType::New();
    m_PSMFilter = EntropyModelFilterType::New();


  }

  template <unsigned int VDimension>
  void PSMCommandLineClass<VDimension>
    ::IterateCallback(itk::Object *caller, const itk::EventObject &)
  {
    EntropyModelFilterType *o =
      static_cast<EntropyModelFilterType *>(caller);

    // Print every 10 iterations
    if (o->GetNumberOfElapsedIterations() % m_ReportInterval != 0) { return; }

    std::cout << "Iteration # " << o->GetNumberOfElapsedIterations() << std::endl;
    std::cout << " Eigenmode variances: ";
    for (unsigned int i = 0; i < o->GetShapePCAVariances().size(); i++)
    {
      std::cout << o->GetShapePCAVariances()[i] << " ";
    }
    std::cout << std::endl;
    std::cout << " Regularization = " << o->GetRegularizationConstant() << std::endl;

    // Check if optimization is run using scales. Get Procrustes
    // interval for the current scale.
    int interval = 0;
    std::cout << "Optimization Scale " << (o->GetCurrentScale() + 1) << "/"
      << o->GetNumberOfScales() << std::endl;

    if (o->GetNumberOfScales() > 1)
    {
      interval = m_ProcrustesInterval[o->GetCurrentScale()];
      std::cout << "Interval = " << interval << std::endl;
    } else // Use the default interval
    {
      interval = m_ProcrustesRegistration->GetProcrustesInterval();
    }

    // Check if the Procrustes interval is set to a value other than 0
    if (interval > 0)
    {
      this->m_ProcrustesCounter++;

      // If the counter is greater than the interval value, run
      // Procrustes registration
      if (this->m_ProcrustesCounter >= interval)
      {
        // Reset the counter
        this->m_ProcrustesCounter = 0;
        this->m_ProcrustesRegistration->RunRegistration();
        std::cout << "Ran Procrustes Registration" << std::endl;
      }
    }
  }

  template <unsigned int VDimension>
  void PSMCommandLineClass<VDimension>
    ::ReadDistanceTransforms(std::string input_path_prefix)
  {
    if (!m_Project->HasDomains())
    {
      throw itk::ExceptionObject("Domains must be defined in the project file.");
    }

    // Compile the list of distance transforms
    std::vector<std::string> domain_names = m_Project->GetDomainNames();
    std::vector<std::string> dt_files;
    for (unsigned int i = 0; i < domain_names.size(); i++)
    {
      std::string s = (m_Project->GetDomainDistanceTransform(domain_names[i]))[0];
      dt_files.push_back(s);


      this->m_PSMFilter->SetDomainName(domain_names[i], i);
    }

    // Read in the distance transforms
    //  const std::vector<std::string> &dt_files = this->m_Project->GetDistanceTransforms();
    std::cout << "Reading distance transforms ..." << std::endl;
    for (unsigned int i = 0; i < dt_files.size(); i++)
    {
      typename itk::ImageFileReader<PSMCommandLineClass::ImageType>::Pointer reader =
        itk::ImageFileReader<PSMCommandLineClass::ImageType>::New();
      reader->SetFileName(input_path_prefix + dt_files[i]);
      //reader->SetFileName(dt_files[i]);
      reader->Update();

      std::cout << domain_names[i] << ": " << dt_files[i] << std::endl;

      this->m_PSMFilter->SetInput(i, reader->GetOutput());

    }
    std::cout << "Done!" << std::endl;
  }

  template <unsigned int VDimension>
  void PSMCommandLineClass<VDimension>
    ::ReadCuttingPlanes()
  {
    if (!m_Project->HasDomains())
    {
      itkExceptionMacro("Domains must be defined in the project file.");
    }

    // Compile the list of domains
    std::vector<std::string> domain_names = m_Project->GetDomainNames();

    for (unsigned int i = 0; i < domain_names.size(); i++)
    {
      if (m_Project->HasDomainCuttingPlanes(domain_names[i]))
      {
        // Cutting planes are returned as a list of vnl_vectors.
        std::vector<vnl_vector_fixed<double, 3> > planes
          = m_Project->GetDomainCuttingPlanes(domain_names[i]);

        // The list should have size that is multiple of 3, since
        // every plane is defined by exactly 3 points.
        if (planes.size() % 3 != 0)
        {
          itkExceptionMacro("Something is wrong with the cutting plane data for domain " + domain_names[i] + ".  Correct number of points was not found.");
        }

        // Traverse the list and pull out sets of 3 points.  Each
        // set of 3 defines a cutting plane.
        vnl_vector_fixed<double, 3> x;
        vnl_vector_fixed<double, 3> y;
        vnl_vector_fixed<double, 3> z;
        std::vector<vnl_vector_fixed<double, 3> >::iterator it =
          planes.begin();

        while (it != planes.end())
        {
          x = *it; it++;
          y = *it; it++;
          z = *it; it++;

          std::cout << "Found cutting plane for domain " << domain_names[i] <<
            ": " << std::endl;
          std::cout << x[0] << " " << x[1] << " " << x[2] << std::endl;
          std::cout << y[0] << " " << y[1] << " " << y[2] << std::endl;
          std::cout << z[0] << " " << z[1] << " " << z[2] << std::endl;

          // Add cutting plane to the PSM filter by domain name

          m_PSMFilter->AddCuttingPlane(x, y, z, domain_names[i]);

          //	      int d = m_PSMFilter->GetDomainIndexByName(domain_names[i]);
          //	      m_PSMFilter->GetDomain(d)->SetCuttingPlane(x,y,z);

        }


      } // end has cutting planes
    } // end traverse domains

  }

  template<unsigned int VDimension>
  void PSMCommandLineClass<VDimension>
    ::ReadModelPointFiles()
  {
    // double value1, value2, value3;
    // unsigned int numOfPoints;
    // typename PSMCommandLineClass::EntropyModelFilterType::PointType pt;

    // // Check if model initialization is supplied. If not, then optimization scales will need to be
    // // used later.
    // if(this->m_Project->HasModel(std::string("initialization")))
    // {
    //   const std::vector<std::string> &pt_files = this->m_Project->GetModel(std::string("initialization"));
    //   std::cout << "Reading the initial model correspondences ..." << std::endl;
    //   for (unsigned int i = 0; i < pt_files.size(); i++)
    //   {
    //     // Read the points for this file and add as a list
    //     typename std::vector<typename PSMCommandLineClass::EntropyModelFilterType::PointType> init_points;
    //     numOfPoints = 0;
    //     // Open the ascii file.
    //     std::ifstream in( (input_path_prefix + pt_files[i]).c_str() );
    //     if ( !in )
    //     {
    //       std::cerr << "Could not open point file for input." << std::endl;
    //       break;
    //     }

    //     // Read all of the points, one point per line.
    //     while (in)
    //     {
    //       in>>value1>>value2>>value3;
    //       pt[0] = value1;
    //       pt[1] = value2;
    //       if(VDimension == 3)
    //       {
    //         pt[2] = value3;
    //       }
    //       init_points.push_back(pt);
    //       numOfPoints++;
    //     }
    //     // this algorithm pushes the last point twice
    //     init_points.pop_back();
    //     std::cout << "Read " << numOfPoints-1 << " points. " << std::endl;
    //     in.close();
    //     this->m_PSMFilter->SetInputCorrespondencePoints(i,init_points);
    //     std::cout << "  " << pt_files[i] << std::endl;
    //   }
    //   std::cout << "Done!" << std::endl;


    // }
    // else
    // {
    //   // Check if scales have been supplied
    //   if(this->m_Project->HasOptimizationAttribute("number_of_scales")
    //      &&
    //      (this->m_Project->GetNumberOfOptimizationScales()) > 0)
    //   {
    //     // Read the input optimization scales from the project file.
    //     this->ReadOptimizationScales();
    //   }
    //   else
    //   {
    //     // REQUIRE THAT WE HAVE SCALES DEFINED.  If there is only a single
    //     // optimization scale, it should be defined as scale 0, but for now
    //     // we are enforcing that scales be defined in the parameter file.

    //     throw itk::ExceptionObject("The number of optimization scales and their parameters were not defined in the parameter file.");

    //     //this->SetDefaultScales();
    //   }
    // }
  }

  template <unsigned int VDimension>
  void PSMCommandLineClass<VDimension>
    ::ReadProjectFile(const char *fn)
  {
    // Read the project parameter file
    m_ProjectFileName = fn;
    this->m_ProjectReader->SetFileName(fn);
    this->m_ProjectReader->Update();

    // Store the project parameters  
    this->m_Project = m_ProjectReader->GetOutput();
  }

  template <unsigned int VDimension>
  void PSMCommandLineClass<VDimension>
    ::SetPSMFilterParameters()
  {
    // Read the optimization parameters and set their corresponding
    // parameters in the PSM optimization filter.  All of the following
    // parameters are required to be present in the project file.
    std::cout << "Reading optimization parameters ... " << std::endl;
    unsigned int number_of_scales = this->m_Project->GetNumberOfOptimizationScales();
    std::cout << " Optimization scales = " << number_of_scales << std::endl;

    if (number_of_scales > 0) {
      // We need vectors of parameters, one for each scale.
      std::vector<double> regularization_initial(number_of_scales);
      std::vector<double> regularization_final(number_of_scales);
      std::vector<double> regularization_decayspan(number_of_scales);
      std::vector<double> tolerance(number_of_scales);
      std::vector<unsigned int> maximum_iterations(number_of_scales);
      m_ProcrustesInterval.resize(number_of_scales);

      // Read parameters for each scale
      for (unsigned int i = 0; i < number_of_scales; i++)  {
        std::cout << "Optimization parameters for scale " << i << ": " << std::endl;
        if (this->m_Project->HasOptimizationAttribute("regularization_initial", i))
        {
          regularization_initial[i] = this->m_Project->GetOptimizationAttribute("regularization_initial", i);
        } else { throw itk::ExceptionObject("Missing parameter: regularization_initial"); }
        std::cout << "    regularization_initial = " << regularization_initial[i] << std::endl;

        if (this->m_Project->HasOptimizationAttribute("regularization_final", i))
        {
          regularization_final[i] = this->m_Project->GetOptimizationAttribute("regularization_final", i);
        } else { throw itk::ExceptionObject("Missing parameter: regularization_final"); }
        std::cout << "      regularization_final = " << regularization_final[i] << std::endl;

        if (this->m_Project->HasOptimizationAttribute("regularization_decayspan", i))
        {
          regularization_decayspan[i]
            = this->m_Project->GetOptimizationAttribute("regularization_decayspan", i);
        } else { throw itk::ExceptionObject("Missing parameter: regularization_decayspan"); }
        std::cout << "  regularization_decayspan = " << regularization_decayspan[i] << std::endl;

        if (this->m_Project->HasOptimizationAttribute("tolerance", i))
        {
          tolerance[i] = this->m_Project->GetOptimizationAttribute("tolerance", i);
        } else { throw itk::ExceptionObject("Missing parameter: tolerance"); }
        std::cout << "                 tolerance = " << tolerance[i] << std::endl;

        if (this->m_Project->HasOptimizationAttribute("maximum_iterations", i))
        {
          maximum_iterations[i]
            = static_cast<unsigned int>(this->m_Project->GetOptimizationAttribute("maximum_iterations", i));
        } else { throw itk::ExceptionObject("Missing parameter: maximum_iterations"); }
        std::cout << "        maximum_iterations = " << maximum_iterations[i] << std::endl;

        //	  if(this->m_Project->HasProcrustes() == true)
        //   {
        if (this->m_Project->HasOptimizationAttribute("procrustes_interval", i))
        {
          m_ProcrustesInterval[i]
            = static_cast<unsigned int>(this->m_Project->GetOptimizationAttribute("procrustes_interval", i));
        } else { throw itk::ExceptionObject("Missing parameter: procrustes_interval"); }
        std::cout << "        procrustes_interval = " << m_ProcrustesInterval[i] << std::endl;
        //}	  
      }

      // Now set the parameters in the filter
      this->m_PSMFilter->SetNumberOfScales(number_of_scales);
      this->m_PSMFilter->SetRegularizationInitial(regularization_initial);
      this->m_PSMFilter->SetRegularizationFinal(regularization_final);
      this->m_PSMFilter->SetRegularizationDecaySpan(regularization_decayspan);
      this->m_PSMFilter->SetTolerance(tolerance);
      this->m_PSMFilter->SetMaximumNumberOfIterations(maximum_iterations);
    } else
    {
      throw itk::ExceptionObject("Project must define at least one optimization scale");
    }
  }

  template <unsigned int VDimension>
  void PSMCommandLineClass<VDimension>
    ::WriteOutputs(std::string output_path)
  {
    const std::vector<std::string> &out_files = this->m_Project->GetModel(std::string("optimized"));
    if (out_files.size() != this->m_PSMFilter->GetParticleSystem()->GetNumberOfDomains())
    {
      std::cerr << "Number of output files does not match the number of particle domains (inputs)." << std::endl;
    } else
    {
      for (unsigned int d = 0; d < this->m_PSMFilter->GetParticleSystem()->GetNumberOfDomains(); d++)
      {
        // Open the output file.
        std::string fname = output_path + out_files[d];
        std::ofstream out(fname.c_str());
        if (!out)
        {
          std::cerr << "Could not open point file for output: " << std::endl;
          break;
        } else
        {
          for (unsigned int j = 0; j < this->m_PSMFilter->GetParticleSystem()->GetNumberOfParticles(d); j++)
          {
            for (unsigned int i = 0; i < VDimension; i++)
            {
              out << this->m_PSMFilter->GetParticleSystem()->GetPosition(j, d)[i] << " ";
            }
            if (VDimension == 2)
            {
              out << "0.0"; // printed for display in ShapeWorksViewer2
            }
            out << std::endl;
          }
        }
      } // end for number of domains
    }
  }

  template <unsigned int VDimension>
  void PSMCommandLineClass<VDimension>
    ::Run(const char *fname, std::string input_path_prefix,
    std::string output_path)
  {
    // Read the project parameter files.  Also sets the project
    // filename (m_ProjectFileName)
    this->ReadProjectFile(fname);

    // Set the optimization parameters.  This also checks to be sure
    // everything is there.  If not, this method throws an exception.
    this->SetPSMFilterParameters();

    // Read the distance transform files
    this->ReadDistanceTransforms(input_path_prefix);

    // Read any geometric contraints (cutting planes)
    this->ReadCuttingPlanes();

    // Create the callback function to run Procrustes and report
    // progress.
    m_IterateCmd = itk::MemberCommand<PSMCommandLineClass>::New();
    m_IterateCmd->SetCallbackFunction(this, &PSMCommandLineClass::IterateCallback);
    this->m_PSMFilter->AddObserver(itk::IterationEvent(), m_IterateCmd);

    // Set ParticleSystem for ProcrustesRegistration
    this->m_ProcrustesRegistration->SetPSMParticleSystem(m_PSMFilter->GetParticleSystem());

    // Run the optimization filter
    this->m_PSMFilter->Update();
    // TODO: Is this error message really required?
    if (this->m_PSMFilter->GetNumberOfElapsedIterations() >= this->m_PSMFilter->GetMaximumNumberOfIterations())
    {
      std::cerr << "Optimization did not converge based on tolerance criteria." << std::endl;
    }

    std::cout << "Optimization done " << std::endl;

    // Write out the optimized point sets
    this->WriteOutputs(output_path);
  }

  // template <unsigned int VDimension>
  // void PSMCommandLineClass<VDimension>
  // ::SetDefaultScales()
  // {
  //   // Setting scales to 10 produces 512 points at the output.
  //   unsigned int number_of_scales = 10;
  //   // Set default parameters for the optimization scales
  //   std::vector<double> regularization_initial(number_of_scales);
  //   std::vector<double> regularization_final(number_of_scales);
  //   std::vector<double> regularization_decayspan(number_of_scales);
  //   std::vector<double> tolerance(number_of_scales);
  //   std::vector<unsigned int> maximum_iterations(number_of_scales);

  //   std::cout << "Setting default optimization parameters for scales..." << std::endl;
  //   for (unsigned int i = 0; i < number_of_scales; i++)
  //   {
  //     std::cout << "Scale #" << i << " :" << std::endl;
  //     regularization_initial[i] = 10.0;
  //     std::cout << "    regularization_initial = " << regularization_initial[i] << std::endl;
  //     regularization_final[i] = 2.0;
  //     std::cout << "      regularization_final = " << regularization_final[i] << std::endl;
  //     regularization_decayspan[i] = 5000;
  //     std::cout << "  regularization_decayspan = " << regularization_decayspan[i] << std::endl;
  //     tolerance[i] = 0.01;
  //     std::cout << "                 tolerance = " << tolerance[i] << std::endl;
  //     maximum_iterations[i] = 1000;
  //     std::cout << "        maximum_iterations = " << maximum_iterations[i] << std::endl;
  //     //    if(this->m_Project->HasProcrustes() == true)
  //     //    {
  //       // Set the Procrustes interval for each scale
  //       unsigned int procrustes_interval = 0;
  //       m_ProcrustesInterval[i] = procrustes_interval;
  //       std::cout << "        procrustes_interval = " << procrustes_interval << std::endl;
  //       //    }
  //   }  

  //   // Set the parameters in the filter
  //   this->m_PSMFilter->SetNumberOfScales(number_of_scales);
  //   this->m_PSMFilter->SetRegularizationInitial(regularization_initial);
  //   this->m_PSMFilter->SetRegularizationFinal(regularization_final);
  //   this->m_PSMFilter->SetRegularizationDecaySpan(regularization_decayspan);
  //   this->m_PSMFilter->SetTolerance(tolerance);
  //   this->m_PSMFilter->SetMaximumNumberOfIterations(maximum_iterations);
  // }


} // end namespace itk

#endif
