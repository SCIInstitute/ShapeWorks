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
#include "itkPSMContainerArrayAttribute.h"
#include "itkPSMParticleSystem.h"
#include "itkPSMRegionNeighborhood.h"
#include "itkPSMRegionDomain.h"
#include "itkMacro.h"

namespace itk {

template<class T, unsigned int VDimension>
class TestContainerArrayAttribute : public PSMContainerArrayAttribute<T, VDimension>
{
public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef TestContainerArrayAttribute Self;
  typedef PSMContainerArrayAttribute<T, VDimension> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(TestContainerArrayAttribute, PSMContainerArrayAttribute);

  /** Callbacks that may be defined by a subclass.  If a subclass defines one
      of these callback methods, the corresponding flag in m_DefinedCallbacks
      should be set to true so that the ParticleSystem will know to register
      the appropriate event with this method. */
  virtual void DomainAddEventCallback(Object *, const EventObject &)
  {
    this->resize( this->size() +1);
    this->operator[](this->size() -1) = PSMContainer<T>::New();
    m_AddDomainCount++;
  }

  virtual void PositionAddEventCallback(Object *, const EventObject &e) 
  {
    const itk::ParticlePositionAddEvent &event
      = dynamic_cast<const itk::ParticlePositionAddEvent &>(e);
    this->operator[](event.GetDomainIndex())->operator[](event.GetPositionIndex()) = 0.0;    
    m_AddPositionCount++;
  }

  unsigned int GetNumberOfPositions() { return m_AddPositionCount; }
  unsigned int GetNumberOfDomains() { return m_AddDomainCount; }

protected:
  TestContainerArrayAttribute() : m_AddPositionCount(0), m_AddDomainCount(0) {}
  virtual ~TestContainerArrayAttribute() {}
  void operator=(const TestContainerArrayAttribute &);
  TestContainerArrayAttribute(const TestContainerArrayAttribute &);

private:
  unsigned int m_AddPositionCount;
  unsigned int m_AddDomainCount;
};

} // end namespace

/** This test exercises functionality of the base itkPSMContainerArrayAttribute class */
int itkPSMContainerArrayAttributeTest(int, char* [] )
{
  bool passed = true;
  std::string errstring = "";

  try
    {
      typedef itk::Point<double, 3> PointType;

      itk::TestContainerArrayAttribute<double,3>::Pointer P = itk::TestContainerArrayAttribute<double,3>::New();

      itk::PSMParticleSystem<3>::Pointer ps = itk::PSMParticleSystem<3>::New();
      itk::PSMRegionNeighborhood<3>::Pointer nb1 = itk::PSMRegionNeighborhood<3>::New();
      itk::PSMRegionNeighborhood<3>::Pointer nb2 = itk::PSMRegionNeighborhood<3>::New();
      itk::PSMRegionDomain<3>::Pointer d1 = itk::PSMRegionDomain<3>::New();
      itk::PSMRegionDomain<3>::Pointer d2 = itk::PSMRegionDomain<3>::New();
      
      ps->RegisterAttribute(P);

      // Define bounding box
      const unsigned int SZ = 100;
      PointType ptl, ptu;
      ptl[0] = 0.0f; ptl[1] = 0.0f; ptl[2] = 0.0f;
      ptu[0] = static_cast<double>(SZ); ptu[1] = static_cast<double>(SZ); ptu[2] = static_cast<double>(SZ);
      
      // Add domains and neighborhoods
      d1->SetRegion(ptl, ptu);
      d2->SetRegion(ptl, ptu);
      ps->AddDomain(d1);
      ps->AddDomain(d2);
      ps->SetNeighborhood(0, nb1);
      ps->SetNeighborhood(1, nb2);

      // Add position
      PointType pt;
      for (unsigned int i = 0; i < SZ; i++)
      {
        pt[0] = static_cast<double>(i) + 0.1f;
        pt[1] = static_cast<double>(i) + 0.2f;
        pt[2] = static_cast<double>(i) + 0.3f;
        ps->AddPosition(pt, 0);
        ps->AddPosition(pt, 1);
      }

      P->ZeroAllValues();
      if (P->GetNumberOfPositions() != 2*SZ)
      {
        passed = false;
        errstring += std::string("PositionAddEventCallback method error. ");
      }
      if (P->GetNumberOfDomains() != 2)
      {
        passed = false;
        errstring += std::string("DomainAddEventCallback method error. ");
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
