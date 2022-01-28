---
title: Studio/src/Data/Worker.h

---

# Studio/src/Data/Worker.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Worker](../Classes/classshapeworks_1_1Worker.md)**  |




## Source code

```cpp
#pragma once

#include <QObject>
#include <QSharedPointer>
#include <Job/Job.h>

namespace shapeworks {

class Worker : public QObject {
  Q_OBJECT

public:
  Worker();
  ~Worker();

  static Worker *create_worker();

  void run_job(QSharedPointer<Job> job);

  void abort_job();

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
}
```


-------------------------------

Updated on 2022-01-28 at 07:11:45 +0000
