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

  virtual Eigen::Vector3d ConstraintGradient(const Eigen::Vector3d &pt) const = 0;
  virtual double ConstraintEval(const Eigen::Vector3d &pt) const = 0;

  void UpdateZ(const Eigen::Vector3d &pt, double C){
    double a = 2*C;
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
    }
    //std::cout << "z: " << z << std::endl;
  }

  void UpdateMu(const Eigen::Vector3d &pt, double C){
      double eval = mu + C*ConstraintEval(pt);
      if(eval > 0){
          mu = eval;
      }
      else{
          mu = 0;
      }
      //std::cout << "mu: " << mu << std::endl;
  }

  Eigen::Vector3d LagragianGradient(const Eigen::Vector3d &pt, double C){
      // debuggg
      /*
      std::stringstream stream;
      Eigen::Vector3d lag_grad = mu*ConstraintGradient(pt) + C*(ConstraintEval(pt)*ConstraintGradient(pt) + Eigen::Vector3d(z*z,z*z,z*z));
      stream << "pt " << pt.transpose() << " lag grad " << lag_grad.transpose() << std::endl;
      stream << "\t ConstraintEval(pt) " << ConstraintEval(pt) << std::endl;
      stream << "\t ConstraintGradient(pt) " << ConstraintGradient(pt).transpose() << std::endl;
      stream << "\t z " << z << std::endl;
      stream << "\t z^2 " << z*z << std::endl;
      stream << "\t mu " << mu << std::endl;
      std::cout << stream.str();
      */
      return mu*ConstraintGradient(pt) + C*(ConstraintEval(pt)*ConstraintGradient(pt) + Eigen::Vector3d(z*z,z*z,z*z));
  }

private:
  shapeworks::ConstraintType type;

  // For augmented lagrangian
  double z;
  double mu;
};


}
