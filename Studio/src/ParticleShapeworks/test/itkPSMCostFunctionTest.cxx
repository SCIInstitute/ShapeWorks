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
#include "itkPSMCostFunction.h"
#include "itkMacro.h"

namespace itk {

template <unsigned int VDimension>
class TestCostFunction : public PSMCostFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef TestCostFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef PSMCostFunction<VDimension> Superclass;
  itkTypeMacro( TestCostFunction, PSMCostFunction);

  /** Type of particle system. */
  typedef typename Superclass::ParticleSystemType ParticleSystemType;

  /** Vector & Point types. */
  typedef typename Superclass::VectorType VectorType;
  typedef typename ParticleSystemType::PointType PointType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, VDimension);

  /** The first argument is a pointer to the particle system.  The second
      argument is the index of the domain within that particle system.  The
      third argument is the index of the particle location within the given
      domain. */  
  virtual VectorType Evaluate(unsigned int, unsigned int,
                              const ParticleSystemType *, double &maxtimestep, 
			      double &energy) const
  { 
    maxtimestep = 1.0f;
    energy = 1.0f;
    VectorType a; 
    a.fill(1.0f);
    return a;
  }
  virtual VectorType Evaluate(unsigned int a,  unsigned int b, 
                              const ParticleSystemType *c, double &d) const
  {
    double e;
    return this->Evaluate(a,b,c,d,e);
  }  
  virtual double Energy(unsigned int a, unsigned int b,
                        const ParticleSystemType *c) const
  {
    double e, d;
    this->Evaluate(a,b,c,d,e);
    return e;
  }
  
  /** Called before each iteration of a solver. */
  virtual void BeforeIteration() {}

  /** Called after each iteration of the solver. */
  virtual void AfterIteration() {}

protected:
  TestCostFunction()
  {     }
  virtual ~TestCostFunction() {}
  void operator=(const TestCostFunction &);
  TestCostFunction(const TestCostFunction &);
};


} //end namespace

/** This test exercises functionality of the base itkPSMParticleSystem class. */
int itkPSMCostFunctionTest(int, char* [] )
{
  bool passed = true;
  std::string errstring = "";

  try
    {
      itk::TestCostFunction<3>::Pointer p = itk::TestCostFunction<3>::New();
      itk::TestCostFunction<3>::VectorType a;
      double maxtimestep = 0.0f;
      double energy = 0.0f;
      a.fill(1.0f);
     
      itk::PSMParticleSystem<3>::Pointer ps = itk::PSMParticleSystem<3>::New();

      if (a != p->Evaluate(0,0,ps,maxtimestep,energy) || maxtimestep != 1.0f || energy != 1.0f) 
	{
	  passed = false;
	  errstring = "Problem calling the Evaluate function";
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
