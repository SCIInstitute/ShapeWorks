---
title: shapeworks::CompareWidget

---

# shapeworks::CompareWidget





Inherits from QWidget

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[settings_changed](../Classes/classshapeworks_1_1CompareWidget.md#signal-settings-changed)**() |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[CompareWidget](../Classes/classshapeworks_1_1CompareWidget.md#function-comparewidget)**(QWidget * parent =nullptr) |
| | **[~CompareWidget](../Classes/classshapeworks_1_1CompareWidget.md#function-~comparewidget)**() |
| [CompareSettings](../Classes/classshapeworks_1_1CompareSettings.md) | **[get_settings](../Classes/classshapeworks_1_1CompareWidget.md#function-get-settings)**() |
| void | **[set_available](../Classes/classshapeworks_1_1CompareWidget.md#function-set-available)**(DisplayMode mode, bool available) |
| void | **[set_mean_shape_available](../Classes/classshapeworks_1_1CompareWidget.md#function-set-mean-shape-available)**(bool available) |
| bool | **[check_any_available](../Classes/classshapeworks_1_1CompareWidget.md#function-check-any-available)**() |

## Public Signals Documentation

### signal settings_changed

```cpp
void settings_changed()
```


## Public Functions Documentation

### function CompareWidget

```cpp
explicit CompareWidget(
    QWidget * parent =nullptr
)
```


### function ~CompareWidget

```cpp
~CompareWidget()
```


### function get_settings

```cpp
CompareSettings get_settings()
```


### function set_available

```cpp
void set_available(
    DisplayMode mode,
    bool available
)
```


### function set_mean_shape_available

```cpp
void set_mean_shape_available(
    bool available
)
```


### function check_any_available

```cpp
bool check_any_available()
```


-------------------------------

Updated on 2023-05-04 at 20:03:03 +0000