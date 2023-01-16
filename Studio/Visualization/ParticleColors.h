#pragma once

#include <vtkLookupTable.h>

#include <QObject>

namespace shapeworks {

//! Supplies particle colors
/*!
 * The ParticleColors supplies distinct colors for particles
 */
class ParticleColors : public QObject {
  Q_OBJECT;

 public:
  enum ParticleColorsType { Distinct = 0, Original = 1 };
  Q_ENUM(ParticleColorsType);

  static vtkSmartPointer<vtkLookupTable> construct_distinct();
};

}  // namespace shapeworks