#pragma once

#include <string>
#include <vector>

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace itk {
/** \class ParticlePositionWriter
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
class ITK_EXPORT ParticlePositionWriter : public DataObject {
 public:
  /** Standard class typedefs */
  typedef ParticlePositionWriter Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self> ConstWeakPointer;

  /** The point type that this class reads. */
  typedef Point<double, VDimension> PointType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticlePositionWriter, DataObject);

  /** Set the input vector.  The input is a reference to a std::vector of PointType. */
  void SetInput(const std::vector<PointType>& p) { m_Input = p; }

  /** Set/Get the filename. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  /** Write the file. */
  inline void Write() { this->Update(); }
  void Update() {
    // Open the output file.
    std::ofstream out(m_FileName.c_str());

    if (!out) {
      itkExceptionMacro("Could not open point file for output: " << m_FileName.c_str());
    }

    //  out << num_points;

    // Write points.
    for (typename std::vector<PointType>::const_iterator it = m_Input.begin(); it != m_Input.end(); it++) {
      for (unsigned int i = 0; i < VDimension; i++) {
        out << (*it)[i] << " ";
      }
      out << std::endl;
    }

    out.close();
  };

 protected:
  ParticlePositionWriter() {}
  void PrintSelf(std::ostream& os, Indent indent) const {
    Superclass::PrintSelf(os, indent);

    os << indent << "ParticlePositionWriter: " << std::endl;
  }
  virtual ~ParticlePositionWriter(){};

 private:
  ParticlePositionWriter(const Self&);  // purposely not implemented
  void operator=(const Self&);          // purposely not implemented

  std::vector<PointType> m_Input;
  std::string m_FileName;
};

}  // end namespace itk
