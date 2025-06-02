---
title: shapeworks::UpdateChecker
summary: Update Checker. 

---

# shapeworks::UpdateChecker



Update Checker.  [More...](#detailed-description)


`#include <UpdateChecker.h>`

Inherits from QDialog

## Public Slots

|                | Name           |
| -------------- | -------------- |
| void | **[handleNetworkReply](../Classes/classshapeworks_1_1UpdateChecker.md#slot-handlenetworkreply)**(QNetworkReply * reply) |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[UpdateChecker](../Classes/classshapeworks_1_1UpdateChecker.md#function-updatechecker)**([Preferences](../Classes/classPreferences.md) & prefs, QWidget * parent =nullptr) |
| | **[~UpdateChecker](../Classes/classshapeworks_1_1UpdateChecker.md#function-~updatechecker)**() |
| void | **[run_auto_update_check](../Classes/classshapeworks_1_1UpdateChecker.md#function-run-auto-update-check)**() |
| void | **[run_manual_update_check](../Classes/classshapeworks_1_1UpdateChecker.md#function-run-manual-update-check)**() |

## Detailed Description

```cpp
class shapeworks::UpdateChecker;
```

Update Checker. 

This class checks for new versions of ShapeWorks and informs the user with a dialog 

## Public Slots Documentation

### slot handleNetworkReply

```cpp
void handleNetworkReply(
    QNetworkReply * reply
)
```


## Public Functions Documentation

### function UpdateChecker

```cpp
explicit UpdateChecker(
    Preferences & prefs,
    QWidget * parent =nullptr
)
```


### function ~UpdateChecker

```cpp
~UpdateChecker()
```


### function run_auto_update_check

```cpp
void run_auto_update_check()
```


### function run_manual_update_check

```cpp
void run_manual_update_check()
```


-------------------------------

Updated on 2025-06-02 at 17:12:04 +0000