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

#include <Data/StudioParticles.h>
#include <Libs/Optimize/Optimize.h>
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

  std::vector<StudioParticles> GetParticles();

  std::vector<std::vector<std::vector<double>>> GetProcrustesTransforms();

  void UpdateExportablePoints() override;

protected:
  virtual void SetIterationCallback() override;
  virtual void IterateCallback(itk::Object* caller, const itk::EventObject&) override;

signals:
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

Updated on 2021-11-18 at 01:30:46 +0000
