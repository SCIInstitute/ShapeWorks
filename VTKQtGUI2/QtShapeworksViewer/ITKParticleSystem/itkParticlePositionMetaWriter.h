/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticlePositionMetaWriter.h,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticlePositionMetaWriter_h
#define __itkParticlePositionMetaWriter_h

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include <vector>
#include <string>

namespace itk
{
/** \class ParticlePositionMetaWriter
 *  This class writes a set of Points to disk.  Its input is a std::vector of points.
 *  The file format is simple an ascii list of VDimension-tuples stored one per
 *  line (delimited by std::endl).  There is no header required for the file,
 *  but the file should not contain leading or trailing empty lines.
 *
 * In 3D, for example, a fragment of a points file looks like this:
 *
 * 1.0 2.0 5.4
 * 2.3 8.7 33.0
 * 4.0 8.21 4.44
 *
 * etc..
 */
template <unsigned int VDimension>
class ITK_EXPORT ParticlePositionMetaWriter : public DataObject
{
public:
  /** Standard class typedefs */
  typedef ParticlePositionMetaWriter Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** The point type that this class reads. */
  typedef Point<double, VDimension> PointType;
  
  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticlePositionMetaWriter, DataObject);

  /** Set the input vector.  The input is a reference to a std::vector of PointType. */
  void SetInput( const std::vector<PointType> &p)
  {
   m_Input = p;
  }

  /** Set/Get the filename. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  /** Write the file. */
  inline void Write()
  { this->Update(); }
  void Update();
  
protected:
  ParticlePositionMetaWriter()  { }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  
    os << indent << "ParticlePositionMetaWriter: " << std::endl;
  }
  virtual ~ParticlePositionMetaWriter() {};

 private:
  ParticlePositionMetaWriter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  std::vector<PointType> m_Input;
  std::string m_FileName;
};

} // end namespace itk


#if ITK_TEMPLATE_EXPLICIT
# include "Templates/itkParticlePositionMetaWriter+-.h"
#endif

#if ITK_TEMPLATE_TXX
# include "itkParticlePositionMetaWriter.txx"
#endif

#endif
