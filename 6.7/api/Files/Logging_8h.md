---
title: Libs/Common/Logging.h

---

# Libs/Common/Logging.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| struct | **[fmt::formatter< QString >](../Classes/structfmt_1_1formatter_3_01QString_01_4.md)**  |
| class | **[shapeworks::Logging](../Classes/classshapeworks_1_1Logging.md)** <br>ShapeWorks [Logging]() Library.  |

## Functions

|                | Name           |
| -------------- | -------------- |
| template <typename... Args\> <br>std::string | **[safe_format](../Files/Logging_8h.md#function-safe-format)**(const std::string & fmt_str, const Args &... args) |

## Defines

|                | Name           |
| -------------- | -------------- |
|  | **[SW_LOG_STACK](../Files/Logging_8h.md#define-sw-log-stack)**(message) <br>Log stack macro.  |
|  | **[SW_LOG](../Files/Logging_8h.md#define-sw-log)**(message, ...) <br>Log message macro.  |
|  | **[SW_LOG_ONLY](../Files/Logging_8h.md#define-sw-log-only)**(message, ...) <br>Log only macro.  |
|  | **[SW_WARN](../Files/Logging_8h.md#define-sw-warn)**(message, ...) <br>Log warning macro.  |
|  | **[SW_ERROR](../Files/Logging_8h.md#define-sw-error)**(message, ...) <br>Log error macro.  |
|  | **[SW_DEBUG](../Files/Logging_8h.md#define-sw-debug)**(message, ...) <br>Log debug macro.  |
|  | **[SW_TRACE](../Files/Logging_8h.md#define-sw-trace)**(x) <br>Variable trace macro (e.g. output variable name = <variable value>)  |
|  | **[SW_MESSAGE](../Files/Logging_8h.md#define-sw-message)**(message, ...) <br>Log show message macro.  |
|  | **[SW_STATUS](../Files/Logging_8h.md#define-sw-status)**(message, ...) <br>Don't write to log, but set status (e.g. in the Studio statusbar)  |
|  | **[SW_PROGRESS](../Files/Logging_8h.md#define-sw-progress)**(value, message, ...)  |
|  | **[SW_CLOSE_LOG](../Files/Logging_8h.md#define-sw-close-log)**() <br>Close session macro.  |
|  | **[SW_LOG_ONCE](../Files/Logging_8h.md#define-sw-log-once)**(message, ...) <br>Log once macro, will only log the message once.  |


## Functions Documentation

### function safe_format

```cpp
template <typename... Args>
std::string safe_format(
    const std::string & fmt_str,
    const Args &... args
)
```




## Macros Documentation

### define SW_LOG_STACK

```cpp
#define SW_LOG_STACK(
    message
)
shapeworks::Logging::Instance().log_stack(message)
```

Log stack macro. 

### define SW_LOG

```cpp
#define SW_LOG(
    message,
    ...
)
shapeworks::Logging::Instance().log_message(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__);
```

Log message macro. 

### define SW_LOG_ONLY

```cpp
#define SW_LOG_ONLY(
    message,
    ...
)
shapeworks::Logging::Instance().log_only(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__);
```

Log only macro. 

### define SW_WARN

```cpp
#define SW_WARN(
    message,
    ...
)
shapeworks::Logging::Instance().log_warning(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__)
```

Log warning macro. 

### define SW_ERROR

```cpp
#define SW_ERROR(
    message,
    ...
)
shapeworks::Logging::Instance().log_error(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__)
```

Log error macro. 

### define SW_DEBUG

```cpp
#define SW_DEBUG(
    message,
    ...
)
shapeworks::Logging::Instance().log_debug(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__, __FUNCTION__)
```

Log debug macro. 

### define SW_TRACE

```cpp
#define SW_TRACE(
    x
)
SW_DEBUG(#x " = {}", x);
```

Variable trace macro (e.g. output variable name = <variable value>) 

### define SW_MESSAGE

```cpp
#define SW_MESSAGE(
    message,
    ...
)
shapeworks::Logging::Instance().show_message(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__)
```

Log show message macro. 

### define SW_STATUS

```cpp
#define SW_STATUS(
    message,
    ...
)
shapeworks::Logging::Instance().show_status(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__)
```

Don't write to log, but set status (e.g. in the Studio statusbar) 

### define SW_PROGRESS

```cpp
#define SW_PROGRESS(
    value,
    message,
    ...
)
shapeworks::Logging::Instance().show_progress(value, safe_format(message, ##__VA_ARGS__));
```


### define SW_CLOSE_LOG

```cpp
#define SW_CLOSE_LOG(
    
)
shapeworks::Logging::Instance().close_log();
```

Close session macro. 

### define SW_LOG_ONCE

```cpp
#define SW_LOG_ONCE(
    message,
    ...
)
  {                                   \
    static bool logged = false;       \
    if (!logged) {                    \
      SW_LOG(message, ##__VA_ARGS__); \
      logged = true;                  \
    }                                 \
  }
```

Log once macro, will only log the message once. 

## Source code

```cpp
#pragma once

#include <spdlog/fmt/fmt.h>

#include <QString>
#include <functional>
#include <iostream>

template <>
struct fmt::formatter<QString> {
  constexpr auto parse(format_parse_context& ctx) -> decltype(ctx.begin()) { return ctx.end(); }

  template <typename FormatContext>
  auto format(const QString& str, FormatContext& ctx) const -> decltype(ctx.out()) {
    return fmt::format_to(ctx.out(), "{}", qUtf8Printable(str));
  }
};

template <typename... Args>
std::string safe_format(const std::string& fmt_str, const Args&... args) {
  std::string result;
  try {
    result = fmt::format(fmt_str, args...);
  } catch (const std::exception& e) {
    // Handle formatting errors here, example:
    std::cerr << "Error formatting string: " << fmt_str << " : " << e.what() << std::endl;
  }
  return result;
}

namespace shapeworks {

class Logging {
 public:
  static Logging& Instance();

  void open_file_log(const std::string& filename);

  bool check_log_open() const;

  std::string get_log_filename() const;

  void log_message(const std::string& message, const int line, const char* file) const;

  void log_only(const std::string& message, const int line, const char* file) const;

  void log_stack(const std::string& message) const;

  void log_error(const std::string& message, const int line, const char* file) const;

  void show_message(const std::string& message, const int line, const char* file) const;

  void show_status(const std::string& message, const int line, const char* file) const;

  void show_progress(double value, const std::string& message);

  void log_debug(const std::string& message, const int line, const char* file, const char* function) const;

  void log_warning(const std::string& message, const int line, const char* file) const;

  void close_log();

  void set_error_callback(const std::function<void(std::string)>& callback);

  void set_message_callback(const std::function<void(std::string)>& callback);

  void set_warning_callback(const std::function<void(std::string)>& callback);

  void set_debug_callback(const std::function<void(std::string)>& callback);

  void set_status_callback(const std::function<void(std::string)>& callback);

  void set_progress_callback(const std::function<void(double, std::string)>& callback);

 private:
  Logging();

  std::string log_filename_;
  bool log_open_ = false;

  std::function<void(std::string)> error_callback_;

  std::function<void(std::string)> message_callback_;

  std::function<void(std::string)> warning_callback_;

  std::function<void(std::string)> debug_callback_;

  std::function<void(std::string)> status_callback_;

  std::function<void(double, std::string)> progress_callback_;
};

#define SW_LOG_STACK(message) shapeworks::Logging::Instance().log_stack(message)

#define SW_LOG(message, ...) \
  shapeworks::Logging::Instance().log_message(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__);

#define SW_LOG_ONLY(message, ...) \
  shapeworks::Logging::Instance().log_only(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__);

#define SW_WARN(message, ...) \
  shapeworks::Logging::Instance().log_warning(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__)

#define SW_ERROR(message, ...) \
  shapeworks::Logging::Instance().log_error(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__)

#define SW_DEBUG(message, ...) \
  shapeworks::Logging::Instance().log_debug(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__, __FUNCTION__)

#define SW_TRACE(x) SW_DEBUG(#x " = {}", x);

#define SW_MESSAGE(message, ...) \
  shapeworks::Logging::Instance().show_message(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__)

#define SW_STATUS(message, ...) \
  shapeworks::Logging::Instance().show_status(safe_format(message, ##__VA_ARGS__), __LINE__, __FILE__)

#define SW_PROGRESS(value, message, ...) \
  shapeworks::Logging::Instance().show_progress(value, safe_format(message, ##__VA_ARGS__));

#define SW_CLOSE_LOG() shapeworks::Logging::Instance().close_log();

#define SW_LOG_ONCE(message, ...)     \
  {                                   \
    static bool logged = false;       \
    if (!logged) {                    \
      SW_LOG(message, ##__VA_ARGS__); \
      logged = true;                  \
    }                                 \
  }
}  // namespace shapeworks
```


-------------------------------

Updated on 2025-06-02 at 20:04:22 +0000
