#pragma once

#include <QObject>
#include <vtkLookupTable.h>

namespace shapeworks {

class ParticleColors : public QObject {
  Q_OBJECT;

 public:
  enum ParticleColorsType { Distinct = 0, Original = 1 };
  Q_ENUM(ParticleColorsType);

  static vtkSmartPointer<vtkLookupTable> construct(ParticleColorsType type, int num);
};

}  // namespace shapeworks