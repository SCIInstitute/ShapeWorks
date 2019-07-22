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
#ifndef __itkPSMContainer_h
#define __itkPSMContainer_h

#include "itkDataObject.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"
#include "itkCommand.h"
#include "itkPSMAttribute.h"
#include <map>

namespace itk
{
/** \class PSMContainer
 *
 *  \brief A container class that holds particle position information
 *  for the PSMParticleSystem class.
 * 
 * This class is a wrapper around an stl map data structure that holds
 * particle position information (points).  It provides the necessary
 * iterators and less-than comparison methods.
 *
 *  \ingroup ParticleShapeModeling
 */
template <class T>
class ITK_EXPORT PSMContainer : public DataObject
{
public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef PSMContainer Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self>  Pointer;
  typedef SmartPointer<const Self>  ConstPointer;
  typedef WeakPointer<const Self>  ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMContainer, DataObject);
  
  /** Define a map container type.  A map is used because elements will be
      frequently deleted and added.  Map has the property that addition and
      deletion operations do not invalidate existing pointers. */
  struct ltcmp
  {
    inline bool operator()(unsigned int a, unsigned int b) const
    { return a < b; }
  };

  /** The underlying type of the container structure: an stl map*/
  typedef std::map<unsigned int, T, ltcmp> MapType;  
  
  /** Define a const iterator type for this container.  This class is just a wrapper
      around the stl::map iterator which provides a simpler API. */
  class ConstIterator : public MapType::const_iterator
  {
  public:
    typedef typename MapType::const_iterator Superclass;
    ConstIterator( const typename MapType::const_iterator &o) : Superclass(o) {}
    ConstIterator() : Superclass() {}
    
    inline const T &operator*() const
    { return Superclass::operator*().second;  } 
    inline unsigned long int GetIndex() const
    { return Superclass::operator*().first; }
  }; 

  /** Define an iterator type for this container.  This class is just a wrapper
      around the stl::map iterator which provides a simpler API. */
  class Iterator : public MapType::iterator
  {
  public:
    typedef typename MapType::iterator Superclass;
    Iterator( const typename MapType::iterator &o) : Superclass(o) {}
    Iterator() : Superclass() {}
    
    inline T &operator*() const
    { return Superclass::operator*().second;  }
    
    inline unsigned long int GetIndex() const
    { return Superclass::operator*().first; } 
  }; 
  
  /** Return iterators for container values. */
  inline ConstIterator GetBegin() const
  { return ConstIterator( m_Map.begin()); }
  inline ConstIterator GetEnd() const
  { return ConstIterator( m_Map.end()); }

  /** Return iterators for the underlying map container.  */
  inline typename MapType::const_iterator GetMapBegin() const
  { return m_Map.begin(); }
  inline typename MapType::const_iterator GetMapEnd() const
  { return m_Map.end(); }
  inline typename MapType::iterator GetMapBegin() 
  { return m_Map.begin(); }
  inline typename MapType::iterator GetMapEnd()
  { return m_Map.end(); }

  /** Returns a reference to the object associated with index k.  If the index
      k does not already exist, this method inserts a new entry for k. */
  inline T &operator[](const unsigned long int &k)  { return m_Map[k]; }
  inline const T&operator[](const unsigned long int &k) const { return m_Map[k]; }

  /** Returns true if index k is in the container and false otherwise. */
  bool HasIndex(unsigned long int k) const
  {
    if ( m_Map.find(k) != m_Map.end()) return true;
    else return false;
  }

  /** Number of objects in the container. */
  unsigned long int GetSize() const  { return m_Map.size(); }

  /**  Erase the element in the container with index k.  Return value is 1 on
       success. */
  typename MapType::size_type Erase( const unsigned int &k )
  { return m_Map.erase(k); }
  
protected:
  PSMContainer() { }
  void PrintSelf(std::ostream& os, Indent indent) const
  {
    Superclass::PrintSelf(os,indent);
  
    os << indent << "PSMContainer: " << std::endl;
    //    for (typename MapType::const_iterator it= this->GetMapBegin();
    //         it != this->GetMapEnd();  it++)
    //      {      os <<  it->first << " " << it->second << std::endl;      }
  }
  virtual ~PSMContainer() {};

 private:
  PSMContainer(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  /** The actual map data container. */
  MapType m_Map;
};

} // end namespace itk

#endif
