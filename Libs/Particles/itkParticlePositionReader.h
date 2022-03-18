#pragma once

#include <fstream>
#include <string>
#include <vector>

#include "itkDataObject.h"
#include "itkObjectFactory.h"
#include "itkPoint.h"
#include "itkWeakPointer.h"

namespace itk {
/** \class ParticlePositionReader
 *  This class reads a set of Points from disk and stores them in a vector.
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
class ITK_EXPORT ParticlePositionReader : public DataObject {
 public:
  /** Standard class typedefs */
  typedef ParticlePositionReader Self;
  typedef DataObject Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef WeakPointer<const Self> ConstWeakPointer;

  /** The point type that this class reads. */
  typedef Point<double, VDimension> PointType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ParticlePositionReader, DataObject);

  /** Get the output of the reader.  The output is a std::vector of PointType. */
  const std::vector<PointType>& GetOutput() const { return m_Output; }

  /** Set/Get the filename. */
  itkSetStringMacro(FileName);
  itkGetStringMacro(FileName);

  /** Read the file. */
  inline void Read() { this->Update(); }
  void Update() {
    int counter = 0;
    // Open the ascii file.
    std::ifstream in(m_FileName.c_str());
    if (!in) {
      itkExceptionMacro("Could not open point file for input: " << m_FileName.c_str());
    }

    //  in >> num_points;

    // Read all of the points, one point per line.
    while (in) {
      PointType pt;
      for (unsigned int d = 0; d < VDimension; d++) {
        in >> pt[d];
      }

      m_Output.push_back(pt);
      counter++;
    }
    // this algorithm pushes the last point twice
    m_Output.pop_back();
    //  std::cout << "Read " << counter-1 << " points. " << std::endl;
    in.close();
  };

 protected:
  ParticlePositionReader() {}
  void PrintSelf(std::ostream& os, Indent indent) const {
    Superclass::PrintSelf(os, indent);

    os << indent << "ParticlePositionReader: " << std::endl;
  }
  virtual ~ParticlePositionReader(){};

 private:
  ParticlePositionReader(const Self&);  // purposely not implemented
  void operator=(const Self&);          // purposely not implemented

  std::vector<PointType> m_Output;
  std::string m_FileName;
};

}  // end namespace itk
