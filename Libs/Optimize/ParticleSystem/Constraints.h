#pragma once

#include "Constraint.h"
#include "PlaneConstraint.h"
#include "SphereConstraint.h"
#include "FreeFormConstraint.h"
#include <vector>
#include "vnl/vnl_inverse.h"
#include "vnl/vnl_cross.h"
#include "Eigen/Dense"
#include "itkPoint.h"

namespace itk
{

class Constraints{
public:
    Constraints(){
        planeConsts = new std::vector<PlaneConstraint>();
        sphereConsts = new std::vector<SphereConstraint>();
        freeFormConsts = new std::vector<FreeFormConstraint>();
    }

    ~Constraints(){
        delete planeConsts;
        delete sphereConsts;
        delete freeFormConsts;
    }

  // Set constraints
  void addPlane(const vnl_vector<double> &a, const vnl_vector<double> &b,const vnl_vector<double> &c);
  void addSphere(const vnl_vector_fixed<double, DIMENSION> &v, double r);
  void setFreeFormConstraint(std::string filename);

  // Transforms
  bool transformConstraints(const vnl_matrix_fixed<double, 4, 4> &Trans);
  bool transformPlanes(const vnl_matrix_fixed<double, 4, 4> &Trans);

  // Apply functions
  std::stringstream applyBoundaryConstraints(vnl_vector_fixed<double, 3> &gradE, const Point<double, 3> &pos);
  std::stringstream applyBoundaryConstraints(vnl_vector_fixed<float, 3> &gradE, const Point<double, 3> &pos);
  std::stringstream applyPlaneConstraints(vnl_vector_fixed<double, 3> &gradE, const Point<double, 3> &pos);

  // Write constraints
  bool writePlanes(std::string filename){return true;}
  bool writeSpheres(std::string filename){return true;}
  bool writeFreeFormConstraint(std::string filename){return true;}

  // Is defined? functions
  bool IsCuttingPlaneDefined() const {if(planeConsts->size() > 0) return true; return false;}
  bool IsCuttingSphereDefined() const {if(sphereConsts->size() > 0) return true; return false;}

  // Plane constraint
  std::vector<PlaneConstraint> *getPlaneConstraints(){return planeConsts;}
  std::vector<SphereConstraint> *GetSphereConstraints(){return sphereConsts;}

  // Is any constraint violated by point pos?
  bool IsAnyViolated(const Point<double, 3> &pos){
      Eigen::Vector3d pt; pt(0) = pos[0]; pt(1) = pos[1]; pt(2) = pos[2];
      for(size_t i = 0; i < planeConsts->size(); i++){
          if((*planeConsts)[i].isViolated(pt)){return true;}
      }
      for(size_t i = 0; i < sphereConsts->size(); i++){
          if((*sphereConsts)[i].isViolated(pt)){return true;}
      }
      for(size_t i = 0; i < freeFormConsts->size(); i++){
          if((*freeFormConsts)[i].isViolated(pt)){return true;}
      }
      return false;
  }

  // Constraint violations
  std::vector<int> planesViolated(Eigen::Vector3d pt){
      std::vector<int> planesViolated;
      for(size_t i = 0; i < planeConsts->size(); i++){
          if((*planeConsts)[i].isViolated(pt)){planesViolated.push_back(i);}
      }
      return planesViolated;
  }

  void PrintAll(){
      std::cout << "Cutting planes " << planeConsts->size() << std::endl;
      for(size_t i = 0; i < planeConsts->size(); i++){
          (*planeConsts)[i].printC();
      }
      std::cout << "Cutting spheres " << sphereConsts->size() << std::endl;
      for(size_t i = 0; i < sphereConsts->size(); i++){
          (*sphereConsts)[i].printC();
      }
      std::cout << "Cutting Free form constraints " << freeFormConsts->size() << std::endl;
      for(size_t i = 0; i < freeFormConsts->size(); i++){
          (*freeFormConsts)[i].printC();
      }
  }

  void ViolationReport(const Point<double, 3> &pos){
      Eigen::Vector3d pt; pt(0) = pos[0]; pt(1) = pos[1]; pt(2) = pos[2];
      std::stringstream stream;
      stream << "Cutting planes " << planeConsts->size() << std::endl;
      for(size_t i = 0; i < planeConsts->size(); i++){
          stream << "CuttingPlane " << i << ": " << (*planeConsts)[i].ConstraintEval(pt) << std::endl;
      }
      stream  << "Cutting spheres " << sphereConsts->size() << std::endl;
      for(size_t i = 0; i < sphereConsts->size(); i++){
          stream << "Sphere " << i << ": " << (*sphereConsts)[i].ConstraintEval(pt) << std::endl;
      }
      stream << "Cutting Free form constraints " << freeFormConsts->size() << std::endl;
      for(size_t i = 0; i < freeFormConsts->size(); i++){
          stream << "FreeForm " << i << ": " << (*freeFormConsts)[i].ConstraintEval(pt) << std::endl;
      }
      std::cout << stream.str();
  }

  // ============================
   // Augmented Lagragian Fuctions
   // ============================
   // Energy gradient computations
   vnl_vector_fixed<double, 3> ConstraintsGradient(const Point<double, 3> &pos) const{
       Eigen::Vector3d pt; pt(0) = pos[0]; pt(1) = pos[1]; pt(2) = pos[2];
       Eigen::Vector3d grad= Eigen::Vector3d(0,0,0);
       for(size_t i = 0; i < planeConsts->size(); i++){
           grad -= (*planeConsts)[i].ConstraintGradient(pt);
       }
       for(size_t i = 0; i < sphereConsts->size(); i++){
           grad -= (*sphereConsts)[i].ConstraintGradient(pt);
       }
       for(size_t i = 0; i < freeFormConsts->size(); i++){
           grad -= (*sphereConsts)[i].ConstraintGradient(pt);
       }
       vnl_vector_fixed<double, 3> gradE;
       for(size_t i = 0; i < 3; i++){
           gradE[i] = grad(i);
       }
       return gradE;
   }

   // Lagragian gradient computation
   vnl_vector_fixed<double, 3> ConstraintsLagrangianGradient(const Point<double, 3> &pos, double C) const{
       Eigen::Vector3d pt; pt(0) = pos[0]; pt(1) = pos[1]; pt(2) = pos[2];
       Eigen::Vector3d grad = Eigen::Vector3d(0,0,0);
       for(size_t i = 0; i < planeConsts->size(); i++){
           std::stringstream stream;
           stream << "auglag " << i << " : " << (*planeConsts)[i].LagragianGradient(pt, C).transpose() << std::endl;
           std::cout << stream.str();
           grad += (*planeConsts)[i].LagragianGradient(pt, C);
       }
       for(size_t i = 0; i < sphereConsts->size(); i++){
           grad += (*sphereConsts)[i].LagragianGradient(pt, C);
       }
       for(size_t i = 0; i < freeFormConsts->size(); i++){
           grad += (*freeFormConsts)[i].LagragianGradient(pt, C);
       }
       vnl_vector_fixed<double, 3> gradE;
       for(size_t i = 0; i < 3; i++){
           gradE[i] = grad(i);
       }
       //std::cout << "gradE " << gradE << std::endl;
       return gradE;
   }

   // Parameters lambda, mu and z initialization
   void InitializeLagrangianParameters(double lambda, double mu, double z) const{
       for(size_t i = 0; i < planeConsts->size(); i++){
           (*planeConsts)[i].SetLambda(lambda);
           (*planeConsts)[i].SetMu(mu);
           (*planeConsts)[i].SetZ(z);
       }
       for(size_t i = 0; i < sphereConsts->size(); i++){
           (*sphereConsts)[i].SetLambda(lambda);
           (*sphereConsts)[i].SetMu(mu);
           (*sphereConsts)[i].SetZ(z);
       }
       for(size_t i = 0; i < freeFormConsts->size(); i++){
           (*freeFormConsts)[i].SetLambda(lambda);
           (*freeFormConsts)[i].SetMu(mu);
           (*freeFormConsts)[i].SetZ(z);
       }
   }

   void UpdateZs(const Point<double, 3> &pos, double C){
       Eigen::Vector3d pt; pt(0) = pos[0]; pt(1) = pos[1]; pt(2) = pos[2];
       for(size_t i = 0; i < planeConsts->size(); i++){
           (*planeConsts)[i].UpdateZ(pt,C);
       }
       for(size_t i = 0; i < sphereConsts->size(); i++){
           (*sphereConsts)[i].UpdateZ(pt,C);
       }
       for(size_t i = 0; i < freeFormConsts->size(); i++){
           (*sphereConsts)[i].UpdateZ(pt,C);
       }
   }

   void UpdateMus(const Point<double, 3> &pos, double C){
       Eigen::Vector3d pt; pt(0) = pos[0]; pt(1) = pos[1]; pt(2) = pos[2];
       for(size_t i = 0; i < planeConsts->size(); i++){
           (*planeConsts)[i].UpdateMu(pt,C);
       }
       for(size_t i = 0; i < sphereConsts->size(); i++){
           (*sphereConsts)[i].UpdateMu(pt,C);
       }
       for(size_t i = 0; i < freeFormConsts->size(); i++){
           (*sphereConsts)[i].UpdateMu(pt,C);
       }
   }

protected:
  std::vector<PlaneConstraint> *planeConsts;
  std::vector<SphereConstraint> *sphereConsts;
  std::vector<FreeFormConstraint> *freeFormConsts;

private:
  // Projections and intersects
  Eigen::Vector3d projectOntoLine(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d p);
  Eigen::Vector3d linePlaneIntersect(Eigen::Vector3d n, Eigen::Vector3d p0, Eigen::Vector3d l0, Eigen::Vector3d l);
  bool PlanePlaneIntersect(Eigen::Vector3d n1, Eigen::Vector3d p1, Eigen::Vector3d n2, Eigen::Vector3d p2, Eigen::Vector3d & l0_result, Eigen::Vector3d & l1_result);
};


}
