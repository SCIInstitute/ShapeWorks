---
title: Studio/Data/Worker.h

---

# Studio/Data/Worker.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Worker](../Classes/classshapeworks_1_1Worker.md)**  |




## Source code

```cpp
#pragma once

#include <Job/Job.h>

#include <QObject>
#include <QSharedPointer>

namespace shapeworks {

class Worker : public QObject {
  Q_OBJECT

 public:
  Worker();
  ~Worker();

  static Worker *create_worker();

  void run_job(QSharedPointer<Job> job);

 public Q_SLOTS:
  void process();

 Q_SIGNALS:
  void error_message(QString);
  void warning_message(QString);
  void status_message(QString);
  void progress(int);
  void finished();

 private:
  QSharedPointer<Job> job_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-10-09 at 21:37:31 +0000
