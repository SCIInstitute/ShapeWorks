---
title: Libs/Optimize/Global.h

---

# Libs/Optimize/Global.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[Global](../Classes/classGlobal.md)**  |




## Source code

```cpp
#pragma once

#include <vector>
#include <itkPoint.h>
#include <tbb/mutex.h>

class Global {
public:

  static Global& global();

  std::vector<int> flipped_particles;
  std::vector<int> flipped_domains;
  std::vector<itk::Point<double>> targets;
  std::vector<int> neighbors;

  tbb::mutex mutex_;

};
```


-------------------------------

Updated on 2022-06-13 at 01:32:51 -0600
