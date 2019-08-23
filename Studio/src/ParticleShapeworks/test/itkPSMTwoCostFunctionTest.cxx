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
#include "itkPSMTwoCostFunction.h"
#include "itkMacro.h"

namespace itk {

template <unsigned int VDimension>
class TestTwoCostFunction : public PSMCostFunction<VDimension>
{
public:
 /** Standard class typedefs. */
  typedef TestTwoCostFunction Self;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef PSMCostFunction<VDimension> Superclass;
  itkTypeMacro( TestTwoCostFunction, PSMCostFunction);

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
    maxtimestep = m_Value;
    energy = m_Value;
    VectorType a; 
    a.fill(m_Value);
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

  void SetValue(double v) {m_Value = v;}
  double GetValue() const {return m_Value;}

protected:
  TestTwoCostFunction() : m_Value(0.0f) {}
  virtual ~TestTwoCostFunction() {}
  void operator=(const TestTwoCostFunction &);
  TestTwoCostFunction(const TestTwoCostFunction &);

  double m_Value;
};

} //end namespace

/** This test exercises functionality of the base itkPSMParticleSystem class. */
int itkPSMTwoCostFunctionTest(int, char* [] )
{
  bool passed = true;
  std::string errstring = "";
  try
    {
      double aval = 1.0f;
      double bval = 2.0f;

      itk::TestTwoCostFunction<3>::VectorType res;
      itk::TestTwoCostFunction<3>::VectorType avec;
      itk::TestTwoCostFunction<3>::VectorType bvec;
      avec.fill(aval);
      bvec.fill(bval);

      itk::TestTwoCostFunction<3>::Pointer cfa = itk::TestTwoCostFunction<3>::New();
      cfa->SetValue(aval);
      itk::TestTwoCostFunction<3>::Pointer cfb = itk::TestTwoCostFunction<3>::New();
      cfb->SetValue(bval);
      itk::PSMParticleSystem<3>::Pointer ps = itk::PSMParticleSystem<3>::New();

      itk::PSMTwoCostFunction<3>::Pointer P = itk::PSMTwoCostFunction<3>::New();
      P->SetFunctionA(cfa);
      P->SetFunctionB(cfb);
      P->SetParticleSystem(ps);

      // Test the evaluate method
      double maxtimestep = aval; 
      double energy = aval;
      res =  P->Evaluate(0,0,ps,maxtimestep,energy);
      // std::cout << "res = " << res << std::endl;
      // std::cout << "maxtimestep = " << maxtimestep << std::endl;
      // std::cout << "energy = " << energy << std::endl;

      // Both A and B functions should be enabled by default.
      // Relative scaling should be 1.
      if (  res != avec + bvec )
	{
	  passed = false;
	  errstring = "Problem calling the Evaluate function";
	}

      // Turn B function off
      P->SetBOff();
      res =  P->Evaluate(0,0,ps,maxtimestep,energy);
      if (  res != avec )
	{
	  passed = false;
	  errstring = "Problem disabling B function";
	}

      // Turn B function on and A function off
      P->SetBOn();
      P->SetAOff();
      res =  P->Evaluate(0,0,ps,maxtimestep,energy);
      if (  res != bvec )
	{
	  passed = false;
	  errstring = "Problem re-enabling B function or disabling A function";
	}

      // Turn both functions on and scale the B part
      P->SetBOn();
      P->SetAOn();
      P->SetRelativeGradientScaling(2.0);
      res =  P->Evaluate(0,0,ps,maxtimestep,energy);
      if (  res != avec + (bvec * 2.0) )
	{
	  passed = false;
	  errstring = "Problem calling the Evaluate function";
	}
      
      // TEST ENERGY METHODS

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
