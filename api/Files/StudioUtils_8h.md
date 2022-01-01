---
title: Studio/src/Utils/StudioUtils.h

---

# Studio/src/Utils/StudioUtils.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::StudioUtils](../Classes/classshapeworks_1_1StudioUtils.md)**  |




## Source code

```cpp
#pragma once

#include <Libs/Project/Project.h>
class QWidget;

#include <QStringList>

namespace shapeworks {

class StudioUtils {
public:

  static bool ask_multiple_domains_as_single(QWidget *parent, std::shared_ptr<Project> project);

  static QStringList to_string_list(std::vector<std::string> vector);

};

} // namespace shapeworks
```


-------------------------------

Updated on 2022-01-01 at 17:33:55 +0000
