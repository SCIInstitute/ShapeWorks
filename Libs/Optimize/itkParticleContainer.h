#pragma once

#include <map>

#include "itkCommand.h"
#include "itkDataObject.h"
#include "itkWeakPointer.h"

namespace itk {
/** \class ParticleContainer
 *  \brief
 */
template <class T>
class ParticleContainer : public DataObject {
 public:
  /** Standard class typedefs */
  typedef T DataType;
  typedef ParticleContainer Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self> ConstWeakPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticleContainer, DataObject);

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
  ParticleContainer() {}
  void PrintSelf(std::ostream& os, Indent indent) const {
    Superclass::PrintSelf(os, indent);

    os << indent << "ParticleContainer: " << std::endl;
  }
  virtual ~ParticleContainer(){};

 private:
  ParticleContainer(const Self&);  // purposely not implemented
  void operator=(const Self&);     // purposely not implemented

  std::vector<T> data;
};

}  // end namespace itk
