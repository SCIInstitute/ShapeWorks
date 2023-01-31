---
title: shapeworks::Logging
summary: ShapeWorks Logging Library. 

---

# shapeworks::Logging

**Module:** **[Common Classes](../Modules/group__Group-Common.md)**



ShapeWorks [Logging]() Library.  [More...](#detailed-description)


`#include <Logging.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [Logging](../Classes/classshapeworks_1_1Logging.md) & | **[Instance](../Classes/classshapeworks_1_1Logging.md#function-instance)**()<br>Return the singleton instance.  |
| void | **[open_file_log](../Classes/classshapeworks_1_1Logging.md#function-open-file-log)**(const std::string & filename)<br>Create a file log.  |
| bool | **[check_log_open](../Classes/classshapeworks_1_1Logging.md#function-check-log-open)**() const<br>Return if the log is open.  |
| std::string | **[get_log_filename](../Classes/classshapeworks_1_1Logging.md#function-get-log-filename)**() const<br>Return the log filename.  |
| void | **[log_message](../Classes/classshapeworks_1_1Logging.md#function-log-message)**(const std::string & message, const int line, const char * file) const<br>Log a message, use SW_LOG macro.  |
| void | **[log_stack](../Classes/classshapeworks_1_1Logging.md#function-log-stack)**(const std::string & message) const<br>Log a stack trace message, use SW_LOG_STACK macro.  |
| void | **[log_error](../Classes/classshapeworks_1_1Logging.md#function-log-error)**(const std::string & message, const int line, const char * file) const<br>Log an error, use SW_ERROR macro.  |
| void | **[show_message](../Classes/classshapeworks_1_1Logging.md#function-show-message)**(const std::string & message, const int line, const char * file) const<br>Log a message, use SW_MESSAGE macro.  |
| void | **[show_status](../Classes/classshapeworks_1_1Logging.md#function-show-status)**(const std::string & message, const int line, const char * file) const<br>Log a message, use SW_STATUS macro.  |
| void | **[log_debug](../Classes/classshapeworks_1_1Logging.md#function-log-debug)**(const std::string & message, const int line, const char * file) const<br>Log a debug message, use SW_DEBUG macro.  |
| void | **[log_warning](../Classes/classshapeworks_1_1Logging.md#function-log-warning)**(const std::string & message, const int line, const char * file) const<br>Log a warning message, use SW_WARN macro.  |
| void | **[close_log](../Classes/classshapeworks_1_1Logging.md#function-close-log)**()<br>Close the log, use SW_CLOSE_LOG macro.  |
| void | **[set_error_callback](../Classes/classshapeworks_1_1Logging.md#function-set-error-callback)**(const std::function< void(std::string)> & callback)<br>Set an error callback function to be called whenever an error is raised.  |
| void | **[set_message_callback](../Classes/classshapeworks_1_1Logging.md#function-set-message-callback)**(const std::function< void(std::string)> & callback)<br>Set a message callback function to be called whenever an message is posted.  |
| void | **[set_warning_callback](../Classes/classshapeworks_1_1Logging.md#function-set-warning-callback)**(const std::function< void(std::string)> & callback)<br>Set a warning callback function to be called whenever a warning is posted.  |
| void | **[set_debug_callback](../Classes/classshapeworks_1_1Logging.md#function-set-debug-callback)**(const std::function< void(std::string)> & callback)<br>Set a debug messagecallback function to be called whenever a debug message is posted.  |

## Detailed Description

```cpp
class shapeworks::Logging;
```

ShapeWorks [Logging]() Library. 

The [Logging](../Classes/classshapeworks_1_1Logging.md) class supports univeral console and file logging.

To use the ShapeWorks logging library, use the pre-defined macros:

Normal log message: 

```cpp
SW_LOG("message")
```

Warning: 

```cpp
SW_WARN("warning")
```

Error: 

```cpp
SW_ERROR("error")
```

Debug: 

```cpp
SW_DEBUG("debug")
```

Output (assuming it was line 25 of Analyze.cpp and SW_LOG_LEVEL=debug): 

```cpp
[2022-08-18 11:40:01.021] [debug] [Analyze.cpp|25] debug
```

The logging macros using the fmt library. Read more here:

[https://github.com/fmtlib/fmt](https://github.com/fmtlib/fmt)

Example: 

```cpp
int mode = 10;
double eigen_value = 42.3;
SW_LOG("eigen value [{}]: {:.2f}", mode, eigen_value);
```

Output: 

```cpp
[2022-08-18 11:37:42.613] [info] eigen value [10]: 42.32
```

## Public Functions Documentation

### function Instance

```cpp
static Logging & Instance()
```

Return the singleton instance. 

### function open_file_log

```cpp
void open_file_log(
    const std::string & filename
)
```

Create a file log. 

### function check_log_open

```cpp
bool check_log_open() const
```

Return if the log is open. 

### function get_log_filename

```cpp
std::string get_log_filename() const
```

Return the log filename. 

### function log_message

```cpp
void log_message(
    const std::string & message,
    const int line,
    const char * file
) const
```

Log a message, use SW_LOG macro. 

### function log_stack

```cpp
void log_stack(
    const std::string & message
) const
```

Log a stack trace message, use SW_LOG_STACK macro. 

### function log_error

```cpp
void log_error(
    const std::string & message,
    const int line,
    const char * file
) const
```

Log an error, use SW_ERROR macro. 

### function show_message

```cpp
void show_message(
    const std::string & message,
    const int line,
    const char * file
) const
```

Log a message, use SW_MESSAGE macro. 

### function show_status

```cpp
void show_status(
    const std::string & message,
    const int line,
    const char * file
) const
```

Log a message, use SW_STATUS macro. 

### function log_debug

```cpp
void log_debug(
    const std::string & message,
    const int line,
    const char * file
) const
```

Log a debug message, use SW_DEBUG macro. 

### function log_warning

```cpp
void log_warning(
    const std::string & message,
    const int line,
    const char * file
) const
```

Log a warning message, use SW_WARN macro. 

### function close_log

```cpp
void close_log()
```

Close the log, use SW_CLOSE_LOG macro. 

### function set_error_callback

```cpp
void set_error_callback(
    const std::function< void(std::string)> & callback
)
```

Set an error callback function to be called whenever an error is raised. 

### function set_message_callback

```cpp
void set_message_callback(
    const std::function< void(std::string)> & callback
)
```

Set a message callback function to be called whenever an message is posted. 

### function set_warning_callback

```cpp
void set_warning_callback(
    const std::function< void(std::string)> & callback
)
```

Set a warning callback function to be called whenever a warning is posted. 

### function set_debug_callback

```cpp
void set_debug_callback(
    const std::function< void(std::string)> & callback
)
```

Set a debug messagecallback function to be called whenever a debug message is posted. 

-------------------------------

Updated on 2023-01-31 at 20:35:21 +0000