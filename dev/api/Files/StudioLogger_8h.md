---
title: Studio/src/Interface/StudioLogger.h

---

# Studio/src/Interface/StudioLogger.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[StudioLogger](../Classes/classStudioLogger.md)** <br>Handle logger callbacks via Qt for thread correctness.  |




## Source code

```cpp
#pragma once

#include <QObject>

class StudioLogger : public QObject {
  Q_OBJECT

 public:
  StudioLogger() {}
  virtual ~StudioLogger() {}

  void register_callbacks();

  // callbacks from logger
  void handle_message(std::string str);
  void handle_error(std::string str);
  void handle_warning(std::string str);
  void handle_debug(std::string str);

 Q_SIGNALS:
  void message(std::string str);
  void error(std::string str);
  void warning(std::string str);
  void debug(std::string str);
};
```


-------------------------------

Updated on 2022-12-02 at 16:59:30 +0000
