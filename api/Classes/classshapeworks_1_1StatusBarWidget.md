---
title: shapeworks::StatusBarWidget
summary: StatusBarWidget. 

---

# shapeworks::StatusBarWidget



[StatusBarWidget]().  [More...](#detailed-description)


`#include <StatusBarWidget.h>`

Inherits from QWidget

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[toggle_log_window](../Classes/classshapeworks_1_1StatusBarWidget.md#signal-toggle-log-window)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[StatusBarWidget](../Classes/classshapeworks_1_1StatusBarWidget.md#function-statusbarwidget)**(QMainWindow * parent =0) |
| void | **[set_message](../Classes/classshapeworks_1_1StatusBarWidget.md#function-set-message)**(MessageType message_type, QString message) |
| void | **[set_progress](../Classes/classshapeworks_1_1StatusBarWidget.md#function-set-progress)**(int value) |
| void | **[resizeEvent](../Classes/classshapeworks_1_1StatusBarWidget.md#function-resizeevent)**(QResizeEvent * event) override |

## Detailed Description

```cpp
class shapeworks::StatusBarWidget;
```

[StatusBarWidget](). 

This widget is used in place of the standard QStatusBar It supports a log window toggle, current message, with color, and progressbar 

## Public Signals Documentation

### signal toggle_log_window

```cpp
void toggle_log_window()
```


## Public Functions Documentation

### function StatusBarWidget

```cpp
StatusBarWidget(
    QMainWindow * parent =0
)
```


### function set_message

```cpp
void set_message(
    MessageType message_type,
    QString message
)
```


### function set_progress

```cpp
void set_progress(
    int value
)
```


### function resizeEvent

```cpp
void resizeEvent(
    QResizeEvent * event
) override
```


-------------------------------

Updated on 2022-01-12 at 19:02:30 +0000