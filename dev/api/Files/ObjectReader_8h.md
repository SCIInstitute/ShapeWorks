---
title: Libs/Optimize/Utils/ObjectReader.h

---

# Libs/Optimize/Utils/ObjectReader.h



## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[ObjectReader](../Classes/classObjectReader.md)**  |




## Source code

```cpp
#pragma once

#include <fstream>
#include <string>
#include <vector>

template <class T>
class ObjectReader {
 public:
  typedef ObjectReader Self;
  typedef T ObjectType;

  const std::vector<ObjectType> &GetOutput() const { return m_Output; }
  std::vector<ObjectType> &GetOutput() { return m_Output; }

  void SetFileName(const char *fn) { m_FileName = fn; }
  void SetFileName(const std::string &fn) { m_FileName = fn; }
  const std::string &GetFileName() const { return m_FileName; }

  inline void Read() { this->Update(); }
  void Update() {
    // Open the output file.
    std::ifstream in(m_FileName.c_str(), std::ios::binary);

    if (!in) {
      std::cerr << "Could not open filename " << m_FileName << std::endl;
      throw 1;
    }
    // Read the number of transforms
    int N;
    in.read(reinterpret_cast<char *>(&N), sizeof(int));

    int sz = sizeof(ObjectType);
    // Read the transforms
    for (unsigned int i = 0; i < (unsigned int)N; i++) {
      ObjectType q;  // maybe not the most efficient, but safe
      in.read(reinterpret_cast<char *>(&q), sz);
      m_Output.push_back(q);
    }

    in.close();
  }

  ObjectReader() {}
  virtual ~ObjectReader(){};

 private:
  ObjectReader(const Self &);   // purposely not implemented
  void operator=(const Self &);  // purposely not implemented

  std::vector<ObjectType> m_Output;
  std::string m_FileName;
};
```


-------------------------------

Updated on 2025-10-11 at 18:16:34 +0000
