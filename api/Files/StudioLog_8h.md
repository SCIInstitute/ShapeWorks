---
title: Studio/src/Data/StudioLog.h

---

# Studio/src/Data/StudioLog.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StudioLog](../Classes/classshapeworks_1_1StudioLog.md)**  |

## Defines

|                | Name           |
| -------------- | -------------- |
|  | **[STUDIO_LOG_STACK](../Files/StudioLog_8h.md#define-studio-log-stack)**(message) <br>Log stack macro.  |
|  | **[STUDIO_LOG_MESSAGE](../Files/StudioLog_8h.md#define-studio-log-message)**(message) <br>Log message macro.  |
|  | **[STUDIO_LOG_ERROR](../Files/StudioLog_8h.md#define-studio-log-error)**(message) <br>Log error macro.  |
|  | **[STUDIO_SHOW_ERROR](../Files/StudioLog_8h.md#define-studio-show-error)**(message) <br>Log show error macro.  |
|  | **[STUDIO_CLOSE_LOG](../Files/StudioLog_8h.md#define-studio-close-log)**() <br>Close session macro.  |
|  | **[STUDIO_FLUSH_LOG](../Files/StudioLog_8h.md#define-studio-flush-log)**() <br>Close session macro.  |




## Macros Documentation

### define STUDIO_LOG_STACK

```cpp
#define STUDIO_LOG_STACK(
    message
)
shapeworks::StudioLog::Instance().log_stack(message)
```

Log stack macro. 

### define STUDIO_LOG_MESSAGE

```cpp
#define STUDIO_LOG_MESSAGE(
    message
)
shapeworks::StudioLog::Instance().log_message(message, __LINE__, __FILE__)
```

Log message macro. 

### define STUDIO_LOG_ERROR

```cpp
#define STUDIO_LOG_ERROR(
    message
)
shapeworks::StudioLog::Instance().log_error(message, __LINE__, __FILE__)
```

Log error macro. 

### define STUDIO_SHOW_ERROR

```cpp
#define STUDIO_SHOW_ERROR(
    message
)
shapeworks::StudioLog::Instance().show_error(message, __LINE__, __FILE__)
```

Log show error macro. 

### define STUDIO_CLOSE_LOG

```cpp
#define STUDIO_CLOSE_LOG(
    
)
shapeworks::StudioLog::Instance().close_log();
```

Close session macro. 

### define STUDIO_FLUSH_LOG

```cpp
#define STUDIO_FLUSH_LOG(
    
)
shapeworks::StudioLog::Instance().flush_log();
```

Close session macro. 

## Source code

```cpp
#pragma once

#include <QObject>
#include <QString>
#include <fstream>
#include <string>

namespace shapeworks {

class StudioLog : public QObject {
  Q_OBJECT;

 public:
  StudioLog();

  static StudioLog& Instance();

  void new_log();

  void log_message(QString message, const int line, const char* file);

  void log_stack(std::string message);

  void log_error(QString message, const int line, const char* file);

  void show_error(QString message, const int line, const char* file);

  void log_debug(QString message, const int line, const char* file);

  void close_log();

  void flush_log();

  bool check_log_open();

  QString get_log_filename();

 Q_SIGNALS:
  void error(QString message);

 private:
  QString create_header(const int line, const char* filename);

  QString get_current_datetime();

  std::ofstream log_;  

  QString log_filename_;

  static const QString datetime_format_;

  static const QString log_datetime_format_;
};

#define STUDIO_LOG_STACK(message) shapeworks::StudioLog::Instance().log_stack(message)

#define STUDIO_LOG_MESSAGE(message) shapeworks::StudioLog::Instance().log_message(message, __LINE__, __FILE__)

#define STUDIO_LOG_ERROR(message) shapeworks::StudioLog::Instance().log_error(message, __LINE__, __FILE__)

#define STUDIO_SHOW_ERROR(message) shapeworks::StudioLog::Instance().show_error(message, __LINE__, __FILE__)

#define STUDIO_CLOSE_LOG() shapeworks::StudioLog::Instance().close_log();

#define STUDIO_FLUSH_LOG() shapeworks::StudioLog::Instance().flush_log();
}  // namespace shapeworks
```


-------------------------------

Updated on 2022-03-05 at 23:20:35 +0000
