---
title: shapeworks::WheelEventForwarder

---

# shapeworks::WheelEventForwarder





Inherits from QObject

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[WheelEventForwarder](../Classes/classshapeworks_1_1WheelEventForwarder.md#function-wheeleventforwarder)**(QScrollBar * target) |
| | **[~WheelEventForwarder](../Classes/classshapeworks_1_1WheelEventForwarder.md#function-~wheeleventforwarder)**() |
| bool | **[eventFilter](../Classes/classshapeworks_1_1WheelEventForwarder.md#function-eventfilter)**(QObject * obj, QEvent * event) |

## Public Functions Documentation

### function WheelEventForwarder

```cpp
explicit WheelEventForwarder(
    QScrollBar * target
)
```


### function ~WheelEventForwarder

```cpp
~WheelEventForwarder()
```


### function eventFilter

```cpp
bool eventFilter(
    QObject * obj,
    QEvent * event
)
```


-------------------------------

Updated on 2022-03-05 at 23:20:34 +0000