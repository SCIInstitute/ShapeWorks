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
#include <map>

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
  
  /** Returns a reference to the object associated with index k.  If the index
      k does not already exist, this method inserts a new entry for k. */
  inline T &operator[](size_t k)  {
    if(k >= data.size()) {
      data.resize(k + 1);
    }
    return data[k];
  }

  /** Convenience method since all usage of this function is via a pointer, leading
      ugly syntax like ->operator[](k) */
  inline T& Get(size_t k) {
    return (*this)[k];
  }

  /** Number of objects in the container. */
  unsigned long int GetSize() const  { return data.size(); }

protected:
  ParticleContainer() { }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  
    os << indent << "ParticleContainer: " << std::endl;
  }
  virtual ~ParticleContainer() {};

 private:
  ParticleContainer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<T> data;
  
};

} // end namespace itk


//#if ITK_TEMPLATE_EXPLICIT
//# include "Templates/itkParticleContainer+-.h"
//#endif

//#if ITK_TEMPLATE_TXX
//# include "itkParticleContainer.txx"
//#endif

#endif
