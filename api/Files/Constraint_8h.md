---
title: Libs/Optimize/ParticleSystem/Constraint.h

---

# Libs/Optimize/ParticleSystem/Constraint.h



## Namespaces

| Name           |
| -------------- |
| **[itk](../Namespaces/namespaceitk.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::Constraint](../Classes/classitk_1_1Constraint.md)**  |




## Source code

```cpp
#pragma once

#include "ConstraintType.h"
#include <vnl_vector.h>
#include "vnl/vnl_math.h"
#include "Eigen/Core"
#include <iostream>

namespace itk
{

class Constraint{
public:

  virtual bool isViolated(const vnl_vector<double> &pt) const = 0;
  virtual bool isViolated(const Eigen::Vector3d &pt) const = 0;
  virtual void printC() const = 0;
  shapeworks::ConstraintType GetConstraintType(){ return type; }
  void setConstraintType(shapeworks::ConstraintType inType){ type = inType;}

  // For augmented lagrangian
    void SetZ(double inz){z = inz;}
    double GetZ(){return z;}
    void SetMu(double inmu){mu = inmu;}
    double GetMu(){return mu;}
    void SetLambda(double inLambda){lambda = inLambda;}
    double GetLambda(){return lambda;}

    virtual Eigen::Vector3d ConstraintGradient(const Eigen::Vector3d &pt) const = 0;
    virtual double ConstraintEval(const Eigen::Vector3d &pt) const = 0;

    void UpdateZ(const Eigen::Vector3d &pt, double C){
      /*double a = 2*C;
      double c = 2*mu + 2*C*ConstraintEval(pt);
      if(c >= 0){
          z = 0;
          //std::cout << "if    z: " << z << std::endl;
      }
      else{
          double z1 = std::sqrt(-c/a);
          double z2 = -z1;
          //std::cout << "else    z: " << z1 << std::endl;

          double pteval = ConstraintEval(pt);

          double res1 = mu*(pteval + z1*z1) + C/2*std::abs(pteval + z1*z1)*std::abs(pteval + z1*z1);
          double res2 = mu*(pteval + z2*z2) + C/2*std::abs(pteval + z2*z2)*std::abs(pteval + z2*z2);

          if(res1 < res2){
              z = z1;
          }
          else{
              z = z2;
          }
      }*/

    // Augmented lagrangian inequality equation: f(x) = mu*(g(x)+z^2) + C/2|g(x)+z^2|^2
    // f'(x) = mu*g'(x) + C*y' where by substitution
    // y = √(u^2) where by substitution
    // u = g(x) + z^2
    // u' = 2*z
    //
    // Then we compute y'
    // y' = (dy / du) (du / dx)
    //    = (1/2)*(2 * u) / √(u^2) (du / dx)
    //    = u * u' / | u |
    //    = sgn(u) * u'
    //
    // So we substitute
    // f'(x) = 2*mu*z + 2*C*sgn(g(x)+z^2)*z

      // z iterative update as explained above
      /*double update = 1000;
      size_t count = 1000;
      while(update < 0.1|| count > 0){
          std::cout << "pt: " << pt.transpose() << " count: " << count << " z: " << z << std::endl;
          update = 2*mu*z + 2*C*sgn(ConstraintEval(pt)+z*z)*z;
          z = z + update;
          count--;
      }*/
      //std::cout << "z: " << z << std::endl;
    }

    void UpdateMu(const Eigen::Vector3d &pt, double C){
        double eval = mu + C*ConstraintEval(pt);
        if(eval < 0){
            mu = 0;
        }
        else{
            mu = eval;
        }
        //std::cout << "mu: " << mu << std::endl;
    }

    Eigen::Vector3d LagragianGradient(const Eigen::Vector3d &pt, double C){
        // Augmented lagrangian inequality equation: f(x) = mu*(g(x)+z^2) + C/2|g(x)+z^2|^2
        // f'(x) = mu*g'(x) + C*y' where by substitution
        // y = √(u^2) where by substitution
        // u = g(x) + z^2
        //
        // Then we compute y'
        // y' = (dy / du) (du / dx)
        //    = (1/2)*(2 * u) / √(u^2) (du / dx)
        //    = u * u' / | u |
        //    = sgn(u) * u'
        //
        // So we substitute
        // f'(x) = mu*g'(x) + C*sgn(g(x)+z^2)*g'(x)
        /*
        Eigen::Vector3d constraint_grad = ConstraintGradient(pt);
        Eigen::Vector3d first_term = mu*constraint_grad;
        double eval = ConstraintEval(pt);
        Eigen::Vector3d second_term = C*constraint_grad*sgn(eval + z*z);
        return first_term+second_term;
        */
        Eigen::Vector3d constraint_grad = ConstraintGradient(pt);
        double eval = ConstraintEval(pt);
        double maxterm = mu + C*eval;
        if(maxterm < 0){
            return Eigen::Vector3d(0,0,0);
        }
        else{
            return maxterm*constraint_grad;
        }
    }


protected:
  shapeworks::ConstraintType type;

  int sgn(double val) {
      return (double(0) < val) - (val < double(0));
  }

  // For augmented lagrangian
    double mu;
    double z;
    double lambda;
};


}
```


-------------------------------

Updated on 2021-11-23 at 22:14:03 +0000
