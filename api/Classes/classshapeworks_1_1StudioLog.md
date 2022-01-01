---
title: shapeworks::StudioLog

---

# shapeworks::StudioLog





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[StudioLog](../Classes/classshapeworks_1_1StudioLog.md#function-studiolog)**() |
| void | **[new_log](../Classes/classshapeworks_1_1StudioLog.md#function-new-log)**()<br>Create a new session.  |
| void | **[log_message](../Classes/classshapeworks_1_1StudioLog.md#function-log-message)**(QString message, const int line, const char * file)<br>Log a message, use STUDIO_LOG_MESSAGE macro.  |
| void | **[log_stack](../Classes/classshapeworks_1_1StudioLog.md#function-log-stack)**(std::string message)<br>Log a stack trace message, use STUDIO_LOG_STACK macro.  |
| void | **[log_error](../Classes/classshapeworks_1_1StudioLog.md#function-log-error)**(QString message, const int line, const char * file)<br>Log an error, use STUDIO_LOG_ERROR macro.  |
| void | **[log_debug](../Classes/classshapeworks_1_1StudioLog.md#function-log-debug)**(QString message, const int line, const char * file)<br>Log a debug message, use STUDIO_LOG_DEBUG macro.  |
| void | **[close_log](../Classes/classshapeworks_1_1StudioLog.md#function-close-log)**()<br>Close the log, use STUDIO_CLOSE_LOG macro.  |
| void | **[flush_log](../Classes/classshapeworks_1_1StudioLog.md#function-flush-log)**()<br>Flush log to disk.  |
| bool | **[check_log_open](../Classes/classshapeworks_1_1StudioLog.md#function-check-log-open)**()<br>Return if the log is open.  |
| QString | **[get_log_filename](../Classes/classshapeworks_1_1StudioLog.md#function-get-log-filename)**()<br>Return the log filename.  |
| [StudioLog](../Classes/classshapeworks_1_1StudioLog.md) & | **[Instance](../Classes/classshapeworks_1_1StudioLog.md#function-instance)**()<br>Return the singleton instance.  |

## Public Functions Documentation

### function StudioLog

```cpp
StudioLog()
```


### function new_log

```cpp
void new_log()
```

Create a new session. 

### function log_message

```cpp
void log_message(
    QString message,
    const int line,
    const char * file
)
```

Log a message, use STUDIO_LOG_MESSAGE macro. 

### function log_stack

```cpp
void log_stack(
    std::string message
)
```

Log a stack trace message, use STUDIO_LOG_STACK macro. 

### function log_error

```cpp
void log_error(
    QString message,
    const int line,
    const char * file
)
```

Log an error, use STUDIO_LOG_ERROR macro. 

### function log_debug

```cpp
void log_debug(
    QString message,
    const int line,
    const char * file
)
```

Log a debug message, use STUDIO_LOG_DEBUG macro. 

### function close_log

```cpp
void close_log()
```

Close the log, use STUDIO_CLOSE_LOG macro. 

### function flush_log

```cpp
void flush_log()
```

Flush log to disk. 

### function check_log_open

```cpp
bool check_log_open()
```

Return if the log is open. 

### function get_log_filename

```cpp
QString get_log_filename()
```

Return the log filename. 

### function Instance

```cpp
static StudioLog & Instance()
```

Return the singleton instance. 

-------------------------------

Updated on 2022-01-01 at 17:59:58 +0000