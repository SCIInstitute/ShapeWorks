---
title: Studio/Python/PythonWorker.h

---

# Studio/Python/PythonWorker.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::PythonWorker](../Classes/classshapeworks_1_1PythonWorker.md)**  |




## Source code

```cpp
#pragma once

// qt
#include <QObject>
#include <QSharedPointer>

// vtk
#include <vtkSmartPointer.h>

// studio
#include <Job/Job.h>
#include <Visualization/StudioVtkOutputWindow.h>

namespace shapeworks {
class PythonLogger;

class PythonWorker : public QObject {
  Q_OBJECT

 public:
  constexpr static const char* python_api_version = "6.5";

  PythonWorker();
  ~PythonWorker();

  void set_vtk_output_window(vtkSmartPointer<StudioVtkOutputWindow> output_window);

  void run_job(QSharedPointer<Job> job);

  void incoming_python_message(std::string message_string);
  void incoming_python_progress(double value, std::string message);

  void end_python();

  void abort_job();

 public Q_SLOTS:

  bool init();

  void start_job(QSharedPointer<Job> job);

  void finalize_python();

 Q_SIGNALS:

  void result_ready();
  void finished();

 private:
  bool initialized_ = false;
  bool initialized_success_ = false;

  vtkSmartPointer<StudioVtkOutputWindow> studio_vtk_output_window_;

  QSharedPointer<PythonLogger> python_logger_;

  QSharedPointer<Job> current_job_;

  QThread* thread_;
};
}  // namespace shapeworks
```


-------------------------------

Updated on 2024-03-29 at 08:20:55 +0000
