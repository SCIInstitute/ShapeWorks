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
#include <QElapsedTimer>
#include <QObject>

namespace shapeworks {

class Job : public QObject {
  Q_OBJECT

 public:
  Job();
  virtual ~Job();

  virtual void run() = 0;

  virtual QString name() = 0;

  virtual QString get_completion_message();

  virtual QString get_abort_message();

  void start_timer();

  qint64 timer_elapsed();

  void set_complete(bool complete) { complete_ = complete; }

  bool is_complete() const { return complete_; }

  void abort() { abort_ = true; }

  bool is_aborted() const { return abort_; }

 public Q_SLOTS:

 Q_SIGNALS:

  void progress(double);
  void finished();

 private:
  std::atomic<bool> complete_ = false;
  std::atomic<bool> abort_ = false;

  QElapsedTimer timer_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2023-10-10 at 02:21:13 +0000
