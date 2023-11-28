---
title: shapeworks::LogWindow
summary: Log Window. 

---

# shapeworks::LogWindow



Log Window.  [More...](#detailed-description)


`#include <LogWindow.h>`

Inherits from QDialog

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[LogWindow](../Classes/classshapeworks_1_1LogWindow.md#function-logwindow)**(QWidget * parent =0) |
| void | **[add_message](../Classes/classshapeworks_1_1LogWindow.md#function-add-message)**(MessageType message_type, QString message)<br>add a message to the history  |

## Detailed Description

```cpp
class shapeworks::LogWindow;
```

Log Window. 

This dialog displays the history of messages (e.g. status bar messages) 

## Public Functions Documentation

### function LogWindow

```cpp
LogWindow(
    QWidget * parent =0
)
```


### function add_message

```cpp
void add_message(
    MessageType message_type,
    QString message
)
```

add a message to the history 

-------------------------------

Updated on 2023-11-28 at 04:34:29 +0000