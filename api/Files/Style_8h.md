---
title: Studio/src/Interface/Style.h

---

# Studio/src/Interface/Style.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

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

Updated on 2022-02-04 at 06:14:24 +0000
