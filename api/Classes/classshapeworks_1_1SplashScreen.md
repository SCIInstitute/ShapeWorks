---
title: shapeworks::SplashScreen

---

# shapeworks::SplashScreen



 [More...](#detailed-description)


`#include <SplashScreen.h>`

Inherits from QDialog

## Public Signals

|                | Name           |
| -------------- | -------------- |
| void | **[open_project](../Classes/classshapeworks_1_1SplashScreen.md#signal-open-project)**(QString file_path)<br>signal to open a particular project  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[SplashScreen](../Classes/classshapeworks_1_1SplashScreen.md#function-splashscreen)**(QWidget * parent, [Preferences](../Classes/classPreferences.md) & preferences)<br>constructor  |
| virtual | **[~SplashScreen](../Classes/classshapeworks_1_1SplashScreen.md#function-~splashscreen)**()<br>destructor  |

## Detailed Description

```cpp
class shapeworks::SplashScreen;
```


The [SplashScreen](../Classes/classshapeworks_1_1SplashScreen.md) is a QDialog that implements the user interface for the Studio Splash Screen It supports a recent file list as well as loading other existing projects 

## Public Signals Documentation

### signal open_project

```cpp
void open_project(
    QString file_path
)
```

signal to open a particular project 

## Public Functions Documentation

### function SplashScreen

```cpp
SplashScreen(
    QWidget * parent,
    Preferences & preferences
)
```

constructor 

### function ~SplashScreen

```cpp
virtual ~SplashScreen()
```

destructor 

-------------------------------

Updated on 2022-01-01 at 17:29:13 +0000