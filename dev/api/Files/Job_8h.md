---
title: Libs/Application/Job/Job.h

---

# Libs/Application/Job/Job.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Job](../Classes/classshapeworks_1_1Job.md)**  |

## Functions

|                | Name           |
| -------------- | -------------- |
| | **[Q_DECLARE_METATYPE](../Files/Job_8h.md#function-q-declare-metatype)**(QSharedPointer< [shapeworks::Job](../Classes/classshapeworks_1_1Job.md) > ) |


## Functions Documentation

### function Q_DECLARE_METATYPE

```cpp
Q_DECLARE_METATYPE(
    QSharedPointer< shapeworks::Job > 
)
```




## Source code

```cpp
#pragma once
#include <QElapsedTimer>
#include <QObject>
#include <QSharedPointer>

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

  void set_quiet_mode(bool quiet) { quiet_mode_ = quiet; }

  bool get_quiet_mode() { return quiet_mode_; }

 public Q_SLOTS:

 Q_SIGNALS:

  void progress(double, QString = "");
  void finished();

 private:
  std::atomic<bool> complete_ = false;
  std::atomic<bool> abort_ = false;
  std::atomic<bool> quiet_mode_ = false;

  QElapsedTimer timer_;
};
}  // namespace shapeworks


Q_DECLARE_METATYPE(QSharedPointer<shapeworks::Job>);
```


-------------------------------

Updated on 2025-09-25 at 21:39:36 +0000
