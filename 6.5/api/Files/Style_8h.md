---
title: Studio/Interface/Style.h

---

# Studio/Interface/Style.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Style](../Classes/classshapeworks_1_1Style.md)**  |




## Source code

```cpp
#pragma once

class QPushButton;

namespace shapeworks {

class Style
{
public:
  Style();

  static void apply_normal_button_style(QPushButton *button);
  static void apply_abort_button_style(QPushButton *button);
};


}
```


-------------------------------

Updated on 2024-01-17 at 16:55:16 +0000