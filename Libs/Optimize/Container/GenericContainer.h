#pragma once

#include <map>

#include "itkCommand.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"

namespace shapeworks {
/*!
 * @class GenericContainer
 * @brief This class seems to be only a wrapper around std::vector implementing ITK DataObject (for smart pointer?)
 *
 * One difference being that the set operator will expand the vector if the index is out of bounds.
 */
template <class T>
class GenericContainer : public itk::DataObject {
 public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef GenericContainer Self;
  typedef DataObject Superclass;
  typedef itk::SmartPointer<Self> Pointer;
  typedef itk::SmartPointer<const Self> ConstPointer;
  typedef itk::WeakPointer<const Self> ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleContainer, itk::DataObject);

  /** Returns a reference to the object associated with index k.  If the index
      k does not already exist, this method inserts a new entry for k. */
  inline T& operator[](size_t k) {
    if (k >= data.size()) {
      data.resize(k + 1);
    }
    return data[k];
  }

  /** Convenience method since all usage of this function is via a pointer, leading
      ugly syntax like ->operator[](k) */
  inline T& Get(size_t k) { return (*this)[k]; }

  /** Number of objects in the container. */
  unsigned long int GetSize() const { return data.size(); }

 protected:
  GenericContainer() {}
  void PrintSelf(std::ostream& os, itk::Indent indent) const {
    Superclass::PrintSelf(os, indent);

    os << indent << "ParticleContainer: " << std::endl;
  }
  virtual ~GenericContainer(){};

 private:
  GenericContainer(const Self&);  // purposely not implemented
  void operator=(const Self&);     // purposely not implemented

  std::vector<T> data;
};

}  // end namespace shapeworks
