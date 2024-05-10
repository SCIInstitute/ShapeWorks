---
title: Libs/Optimize/OptimizeParameterFile.h

---

# Libs/Optimize/OptimizeParameterFile.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::OptimizeParameterFile](../Classes/classshapeworks_1_1OptimizeParameterFile.md)**  |




## Source code

```cpp
#pragma once

#include <exception>
#include <sstream>
#include <string>
#include <vector>

class TiXmlHandle;

namespace shapeworks {
class Optimize;

class OptimizeParameterFile {
 public:
  OptimizeParameterFile();

  bool load_parameter_file(std::string filename, Optimize* optimize);

 private:
  bool set_visualizer_parameters(TiXmlHandle* docHandle, Optimize* optimize);

  bool set_io_parameters(TiXmlHandle* docHandle, Optimize* optimize);

  bool set_optimization_parameters(TiXmlHandle* docHandle, Optimize* optimize);

  bool set_debug_parameters(TiXmlHandle* docHandle, Optimize* optimize);

  bool read_image_inputs(TiXmlHandle* docHandle, Optimize* optimize);

  bool read_mesh_inputs(TiXmlHandle* docHandle, Optimize* optimize);

  bool read_contour_inputs(TiXmlHandle* docHandle, Optimize* optimize);

  bool read_mesh_attributes(TiXmlHandle* docHandle, Optimize* optimize);

  bool read_point_files(TiXmlHandle* docHandle, Optimize* optimize);

  bool read_constraints(TiXmlHandle* doc_handle, Optimize* optimize);

  bool read_distribution_cutting_plane(TiXmlHandle* doc_handle, Optimize* optimize);

  bool read_cutting_planes(TiXmlHandle* doc_handle, Optimize* optimize);

  bool read_explanatory_variables(TiXmlHandle* doc_handle, Optimize* optimize);

  bool read_flag_particles(TiXmlHandle* doc_handle, Optimize* optimize);

  bool read_flag_domains(TiXmlHandle* doc_handle, Optimize* optimize);

  int get_num_inputs(TiXmlHandle* docHandle);

 private:
  std::vector<int> read_int_list(TiXmlHandle* doc_handle, std::string name);

  int verbosity_level_ = 0;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-05-10 at 16:10:36 +0000
