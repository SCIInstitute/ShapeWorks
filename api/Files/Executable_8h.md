---
title: Applications/shapeworks/Executable.h

---

# Applications/shapeworks/Executable.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Executable](../Classes/classshapeworks_1_1Executable.md)**  |




## Source code

```cpp
#pragma once

/*
 * Unified shapeworks executable.
 */

#include "Command.h"
#include <optparse.h>

namespace shapeworks {

class Executable {
public:
  Executable();

  void addCommand(Command &command);
  int run(int argc, char const *const *argv);

private:
  void buildParser();
  optparse::OptionParser parser;
  std::map<std::string, Command&> commands;
  std::map<std::string, std::map<std::string, std::string> > parser_epilog; // <command_type, <command_name, desc> >

  int run(std::vector<std::string> arguments, SharedCommandData &sharedData);
};

}; // shapeworks
```


-------------------------------

Updated on 2022-01-12 at 19:02:31 +0000
