---
title: Studio/Utils/AnalysisUtils.h

---

# Studio/Utils/AnalysisUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::AnalysisUtils](../Classes/classshapeworks_1_1AnalysisUtils.md)**  |




## Source code

```cpp
#pragma once

#include <Particles/ParticleSystemEvaluation.h>

#include <QColor>
#include <QString>

class JKQTPlotter;

namespace shapeworks {

class Session;

class AnalysisUtils {
 public:
  static ParticleSystemEvaluation get_local_particle_system(Session* session, int domain);

  static void create_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title, QString x_label, QString y_label);

  static void create_box_plot(JKQTPlotter* plot, Eigen::VectorXd data, QString title, QString x_label,
                              QColor color = Qt::blue);
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-04-10 at 21:32:43 +0000
