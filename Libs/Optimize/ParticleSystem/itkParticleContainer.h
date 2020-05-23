/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleContainer.h,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticleContainer_h
#define __itkParticleContainer_h

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkCommand.h"
#include "itkParticleAttribute.h"
#include <unordered_map>

namespace itk
{
/** \class ParticleContainer
 *  \brief 
 */
template <class T>
class ITK_EXPORT ParticleContainer : public DataObject
{
public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef ParticleContainer Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  //  itkTypeMacro(ParticleContainer, ParticleAttribute);
  itkTypeMacro(ParticleContainer, DataObject);
  
  /** Define a map container type.  A map is used because elements will be
      frequently deleted and added.  Map has the property that addition and
      deletion operations do not invalidate existing pointers. */
  struct ltcmp
  {
    inline bool operator()(unsigned int a, unsigned int b) const
    { return a < b; }
  };
  typedef std::vector<T> MapType;

  /** Returns a reference to the object associated with index k.  If the index
      k does not already exist, this method inserts a new entry for k. */
  inline T &operator[](const unsigned long int &k)  { return m_Map[k]; }
  inline const T&operator[](const unsigned long int &k) const { return m_Map[k]; }

  /** Number of objects in the container. */
  unsigned long int GetSize() const  { return m_Map.size(); }
  MapType m_Map;

protected:
  ParticleContainer() { m_Map = std::vector<T>(4096); m_Map.resize(4096); }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  
    os << indent << "ParticleContainer: " << std::endl;
    //    for (typename MapType::const_iterator it= this->GetMapBegin();
    //         it != this->GetMapEnd();  it++)
    //      {      os <<  it->first << " " << it->second << std::endl;      }
  }
  virtual ~ParticleContainer() {};

 private:
  ParticleContainer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented


};

} // end namespace itk


//#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleContainer+-.h"
//#endif

//#if ITK_TEMPLATE_TXX
//# include "itkParticleContainer.txx"
//#endif

#endif
