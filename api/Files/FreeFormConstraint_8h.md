---
title: Libs/Optimize/ParticleSystem/FreeFormConstraint.h

---

# Libs/Optimize/ParticleSystem/FreeFormConstraint.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::FreeFormConstraint](../Classes/classitk_1_1FreeFormConstraint.md)**  |




## Source code

```cpp
#pragma once

#include "Constraint.h"
#include "Eigen/Core"
#include "vnl/vnl_math.h"
#include <vector>

#include "Mesh.h"

namespace itk
{

class FreeFormConstraint: public Constraint{
public:
    FreeFormConstraint(){}

    void setMesh(std::shared_ptr<shapeworks::Mesh> mesh1){
        this->mesh = mesh1;
    }

    std::shared_ptr<shapeworks::Mesh> getMesh(){
        return this->mesh;
    }

  bool isViolated(const vnl_vector<double> &pt) const {
      return isViolated(Eigen::Vector3d(pt[0], pt[1], pt[2]));
  }

  bool isViolated(const Eigen::Vector3d &pt) const {
      if(ConstraintEval(pt) >= 0){
          return false;
      }
      else{
          return true;
      }

  }

  void printC() const{
      std::cout << "FF" << std::endl;
  }

  Eigen::Vector3d ConstraintGradient(const Eigen::Vector3d &pt) const{
      return mesh->getFFCGradient(pt);
    }

    double ConstraintEval(const Eigen::Vector3d &pt) const{
      return mesh->getFFCValue(pt);
    }

private:
    std::shared_ptr<shapeworks::Mesh> mesh;
};


}
```


-------------------------------

Updated on 2021-11-12 at 04:35:45 +0000
