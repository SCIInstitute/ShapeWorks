---
title: StudioLogger
summary: Handle logger callbacks via Qt for thread correctness. 

---

# StudioLogger



Handle logger callbacks via Qt for thread correctness. 


`#include <StudioLogger.h>`

Inherits from QObject

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[message](../Classes/classStudioLogger.md#signal-message)**(std::string str) |
| void | **[error](../Classes/classStudioLogger.md#signal-error)**(std::string str) |
| void | **[warning](../Classes/classStudioLogger.md#signal-warning)**(std::string str) |
| void | **[debug](../Classes/classStudioLogger.md#signal-debug)**(std::string str) |
| void | **[status](../Classes/classStudioLogger.md#signal-status)**(std::string str) |
| void | **[progress](../Classes/classStudioLogger.md#signal-progress)**(int value, std::string str) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[StudioLogger](../Classes/classStudioLogger.md#function-studiologger)**() |
| virtual | **[~StudioLogger](../Classes/classStudioLogger.md#function-~studiologger)**() |
| void | **[register_callbacks](../Classes/classStudioLogger.md#function-register-callbacks)**() |
| void | **[handle_message](../Classes/classStudioLogger.md#function-handle-message)**(std::string str) |
| void | **[handle_error](../Classes/classStudioLogger.md#function-handle-error)**(std::string str) |
| void | **[handle_warning](../Classes/classStudioLogger.md#function-handle-warning)**(std::string str) |
| void | **[handle_debug](../Classes/classStudioLogger.md#function-handle-debug)**(std::string str) |
| void | **[handle_status](../Classes/classStudioLogger.md#function-handle-status)**(std::string str) |
| void | **[handle_progress](../Classes/classStudioLogger.md#function-handle-progress)**(double value, std::string str) |

## Public Signals Documentation

### signal message

```cpp
void message(
    std::string str
)
```


### signal error

```cpp
void error(
    std::string str
)
```


### signal warning

```cpp
void warning(
    std::string str
)
```


### signal debug

```cpp
void debug(
    std::string str
)
```


### signal status

```cpp
void status(
    std::string str
)
```


### signal progress

```cpp
void progress(
    int value,
    std::string str
)
```


## Public Functions Documentation

### function StudioLogger

```cpp
inline StudioLogger()
```


### function ~StudioLogger

```cpp
virtual ~StudioLogger()
```


### function register_callbacks

```cpp
void register_callbacks()
```


### function handle_message

```cpp
void handle_message(
    std::string str
)
```


### function handle_error

```cpp
void handle_error(
    std::string str
)
```


### function handle_warning

```cpp
void handle_warning(
    std::string str
)
```


### function handle_debug

```cpp
void handle_debug(
    std::string str
)
```


### function handle_status

```cpp
void handle_status(
    std::string str
)
```


### function handle_progress

```cpp
void handle_progress(
    double value,
    std::string str
)
```


-------------------------------

Updated on 2024-08-08 at 17:20:00 +0000