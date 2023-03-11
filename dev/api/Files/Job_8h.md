---
title: Studio/Job/Job.h

---

# Studio/Job/Job.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**  |




## Source code

```cpp
#pragma once
#include <QObject>
#include <QElapsedTimer>

namespace shapeworks {

class Job : public QObject {
  Q_OBJECT

public:

  Job();
  virtual ~Job();

  virtual void run() = 0;

  virtual QString name() = 0;

  virtual QString get_completion_message();

  void start_timer();
  qint64 timer_elapsed();

public Q_SLOTS:

Q_SIGNALS:
  void progress(double);
  void finished();

private:
  QElapsedTimer timer_;
};
}
```


-------------------------------

Updated on 2023-03-11 at 06:26:44 +0000
