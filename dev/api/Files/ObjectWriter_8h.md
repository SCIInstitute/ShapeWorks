---
title: Libs/Optimize/Utils/ObjectWriter.h

---

# Libs/Optimize/Utils/ObjectWriter.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[ObjectWriter](../Classes/classObjectWriter.md)**  |




## Source code

```cpp
#pragma once

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

template <class T>
class ObjectWriter {
 public:
  typedef ObjectWriter Self;

  typedef T ObjectType;

  void SetInput(const std::vector<ObjectType> &p) { m_Input = p; }

  void SetFileName(const char *fn) { m_FileName = fn; }
  void SetFileName(const std::string &fn) { m_FileName = fn; }
  const std::string &GetFileName() const { return m_FileName; }

  inline void Write() { this->Update(); }
  void Update() {
    // Open the output file.
    //  std::ofstream out( m_FileName.c_str(), std::ios::binary );
    std::ofstream out(m_FileName.c_str());

    if (!out) {
      std::cerr << "Could not open point file for output: " << m_FileName << std::endl;
      throw 1;
    }

    // Write the number of objects
    int sz = m_Input.size();
    out.write(reinterpret_cast<char *>(&sz), sizeof(int));

    sz = sizeof(ObjectType);
    // Write the objects
    for (typename std::vector<ObjectType>::const_iterator it = m_Input.begin(); it != m_Input.end(); it++) {
      ObjectType q = *it;  // maybe not the most efficient, but safe
      out.write(reinterpret_cast<char *>(&q), sz);
    }

    out.close();
  }

  ObjectWriter() {}
  virtual ~ObjectWriter(){};

 private:
  ObjectWriter(const Self &);   // purposely not implemented
  void operator=(const Self &);  // purposely not implemented

  std::vector<ObjectType> m_Input;
  std::string m_FileName;
};
```


-------------------------------

Updated on 2025-03-19 at 04:20:52 +0000
