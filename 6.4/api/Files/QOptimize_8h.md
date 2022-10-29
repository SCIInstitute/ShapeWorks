---
title: Studio/src/Optimize/QOptimize.h

---

# Studio/src/Optimize/QOptimize.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::QOptimize](../Classes/classshapeworks_1_1QOptimize.md)** <br>Wraps [Optimize](../Classes/classshapeworks_1_1Optimize.md) as a QObject.  |




## Source code

```cpp
#pragma once

#ifndef Q_MOC_RUN
#include <Analyze/Particles.h>
#include <Optimize/Optimize.h>
#endif
#include <QObject>
#include <QMutex>
#include <QElapsedTimer>

namespace shapeworks {

class QOptimize : public QObject, public Optimize {
Q_OBJECT;

public:
  QOptimize(QObject* parent = nullptr);
  virtual ~QOptimize();

  std::vector<std::vector<itk::Point<double>>> GetLocalPoints() override;
  std::vector<std::vector<itk::Point<double>>> GetGlobalPoints() override;

  std::vector<Particles> GetParticles();

  std::vector<std::vector<std::vector<double>>> GetProcrustesTransforms() override;

  void UpdateExportablePoints() override;

protected:
  virtual void SetIterationCallback() override;
  virtual void IterateCallback(itk::Object* caller, const itk::EventObject&) override;

Q_SIGNALS:
  void progress(int, QString);

private:

  itk::MemberCommand<QOptimize>::Pointer iterate_command_;

  // for concurrent access
  QMutex qmutex_;

  QElapsedTimer time_since_last_update_;

};

}
```


-------------------------------

Updated on 2022-10-29 at 22:38:49 +0000
