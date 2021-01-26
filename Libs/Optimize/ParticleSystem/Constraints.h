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

  // ============================
  // Augmented Lagragian Fuctions
  // ============================
  // Energy gradient computations
  vnl_vector_fixed<double, 3> ConstraintsGradient(const Point<double, 3> &pos) const{
      Eigen::Vector3d pt; pt(0) = pos[0]; pt(1) = pos[1]; pt(2) = pos[2];
      Eigen::Vector3d grad;
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
  vnl_vector_fixed<double, 3> ConstraintsLagrangianGradient(const vnl_vector_fixed<float, 3> &pos, double C) const{
      Eigen::Vector3d pt; pt(0) = pos[0]; pt(1) = pos[1]; pt(2) = pos[2];
      Eigen::Vector3d grad;
      for(size_t i = 0; i < planeConsts->size(); i++){
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
      return gradE;
  }

  // Parameters mu and z initialization
  void InitializeLagrangianParameters(double mu, double z) const{
      for(size_t i = 0; i < planeConsts->size(); i++){
          (*planeConsts)[i].SetMu(mu);
          (*planeConsts)[i].SetZ(z);
      }
      for(size_t i = 0; i < sphereConsts->size(); i++){
          (*sphereConsts)[i].SetMu(mu);
          (*sphereConsts)[i].SetZ(z);
      }
      for(size_t i = 0; i < freeFormConsts->size(); i++){
          (*freeFormConsts)[i].SetMu(mu);
          (*freeFormConsts)[i].SetZ(z);
      }
  }

  void UpdateZs(const vnl_vector_fixed<float, 3> &pos, double C){
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

  void UpdateMus(const vnl_vector_fixed<float, 3> &pos, double C){
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

  // ============================

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

  vnl_vector_fixed<double, 3> augmented_lagrangian_grad(unsigned int idx, unsigned int d, const vnl_vector_fixed<float, 3> pos, vnl_vector_fixed<float, 3> h_grad, float hx){

      vnl_vector_fixed<double, 3> gradE;

      for (unsigned int n = 0; n < 3; n++) {
        gradE[n] = 0.0;
      }

      // Augmented Lagrangian Parameters
      //std::cout << "m_lambdas.size() " << this->GetLambdaI(d, idx) << " d " << d << " c_eq " << this->GetCEq() << std::endl;
      double c_eq = this->GetCEq(); // equalities: Surface constraints
      double c_in = this->GetCIn(); // inequalities/boundary: cutting plane, sphere or free form
      double lambda = this->GetLambdaI(d, idx);

      // Inequality constraint stuff
      this->UpdateZs(pos, c_in);
      vnl_vector_fixed<double, 3> ineq_constraint_energy = this->ConstraintsLagrangianGradient(pos, c_in);

      // Equality constraint stuff
      // Summing the gradient for computation
      vnl_vector_fixed<double, 3> posgrad;
      for (unsigned int n = 0; n < 3; n++)
        {
          posgrad[n] = pos[n] - gradE[n];
        }
      //system->GetDomain(d)->ApplyConstraints(posgrad);

      lambda = lambda + c_eq * hx; // lambda update before iteration
      vnl_vector_fixed<double, 3> eq_constraint_energy;
      for (unsigned int n = 0; n < 3; n++)
        {
            eq_constraint_energy[n] = lambda * h_grad[n] + c_eq * h_grad[n] * hx;
        }

      // std::cout << "pos " << pos << " Inequality " << ineq_constraint_energy << std::endl;
      std::stringstream stream;
      // debuggg
      stream << "d " << d << " idx " << idx << std::endl;
      stream << "gradE before adding eq" << gradE << std::endl;
      stream << "m_lambda " << lambda << " pos " << pos << " Equality " << eq_constraint_energy << std::endl;
      double pos_norm = sqrt(pos[0]*pos[0] + pos[1]*pos[1] + pos[2]*pos[2]);
      double eq_en_norm = sqrt(eq_constraint_energy[0]*eq_constraint_energy[0] + eq_constraint_energy[1]*eq_constraint_energy[1] + eq_constraint_energy[2]*eq_constraint_energy[2]);
      stream << "Coeff " << lambda + c_eq * hx << " c_eq " << c_eq << " lambda " << lambda << " h_grad " << h_grad << " hx " << hx << std::endl;
      stream << "pos_norm " << pos_norm << " pos_unit [" << pos[0]/pos_norm << " " << pos[1]/pos_norm << " " << pos[2]/pos_norm << "]" << std::endl <<
              "eq_en norm " << eq_en_norm << " eq_en unit [" << eq_constraint_energy[0]/eq_en_norm << " " << eq_constraint_energy[1]/eq_en_norm << " "<< eq_constraint_energy[2]/eq_en_norm << "] " << std::endl;
      for (unsigned int n = 0; n < 3; n++)
        {
          //gradE[n] += ineq_constraint_energy[n] + eq_constraint_energy[n];
          gradE[n] +=  eq_constraint_energy[n];
        }

      // Augmented lagrangian updates and scaling C updates
      this->UpdateMus(pos, c_in);
      //this->SetLambdaI(lambda + c_eq*hx, d, idx); // lambda update after iteration
      this->SetLambdaI(lambda, d, idx);
      this->SetCEq(c_eq*this->GetCEqFactor());
      this->SetCIn(c_in*this->GetCInFactor());

      // debuggg
      stream << "gradE " << gradE << std::endl;

      // Consider point bounds
      vnl_vector_fixed<double, 3> posUpd;
      for (unsigned int n = 0; n < 3; n++)
        {
          posUpd[n] = pos[n] + gradE[n];
        }
      /*
      system->GetDomain(d)->ApplyBoundaryConstraints(posUpd);
      for (unsigned int n = 0; n < VDimension; n++)
        {
          gradE[n] = posUpd[n] - pos[n];
        }
      */


      // debuggg
      //stream << "posUpd " << posUpd << " new gradE " << gradE << std::endl << std::endl;
      double dfrom0 = sqrt(posUpd[0]*posUpd[0] + posUpd[1]*posUpd[1] + posUpd[2]*posUpd[2]);
      if((std::fmod(dfrom0, 10) > 2 && std::fmod(dfrom0, 10) < 8) || dfrom0 > 50){
        stream << dfrom0 << std::endl << std::endl;
        //std::cerr << stream.str();
       }
      else{
      stream << dfrom0 << std::endl << std::endl;
        //std::cout << stream.str();
      }

      return gradE;
  }

    void SetLambdaI(double lambda, size_t domain, size_t id){m_lambdas[domain][id] = lambda;}
    double GetLambdaI(size_t domain, size_t id){
        //std::cout << "m_lambda " << m_lambdas.size() << " domain " << domain << std::endl;
        return m_lambdas[domain][id];
    }
    void SetLambdaVec(size_t size, double value){
        m_lambdas.clear();
        std::vector<double> lambdas;
        lambdas.push_back(value);
        for (size_t i = 0; i < size; i++){
            m_lambdas.push_back(lambdas);
        }
        std::cout << "size " << m_lambdas.size() << " value " << m_lambdas[0][0] << std::endl;
    }
    void CopyDoubleTheLambdas(){
        for (size_t i = 0; i < m_lambdas.size(); i++){
            size_t num = m_lambdas[i].size();
            for (size_t j = 0; j < num; j++){
              m_lambdas[i].push_back(m_lambdas[i][j]);
            }
        }
    }

    double GetCEq(){return m_c_eq;}
    double GetCIn(){return m_c_in;}
    void SetCEq(double eq){m_c_eq = eq;}
    void SetCIn(double in){m_c_in = in;}
    double GetCEqFactor(){return m_c_eq_factor;}
    double GetCInFactor(){return m_c_in_factor;}
    void SetCEqFactor(double factor){m_c_eq_factor = factor;}
    void SetCInFactor(double factor){m_c_in_factor = factor;}

protected:
  std::vector<PlaneConstraint> *planeConsts;
  std::vector<SphereConstraint> *sphereConsts;
  std::vector<FreeFormConstraint> *freeFormConsts;

    double m_c_eq; // equalities: Surface constraints
    double m_c_in; // inequalities/boundary: cutting plane, sphere or free form
    double m_c_eq_factor;
    double m_c_in_factor;

    std::vector<std::vector<double> > m_lambdas;

private:
  // Projections and intersects
  Eigen::Vector3d projectOntoLine(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d p);
  Eigen::Vector3d linePlaneIntersect(Eigen::Vector3d n, Eigen::Vector3d p0, Eigen::Vector3d l0, Eigen::Vector3d l);
  bool PlanePlaneIntersect(Eigen::Vector3d n1, Eigen::Vector3d p1, Eigen::Vector3d n2, Eigen::Vector3d p2, Eigen::Vector3d & l0_result, Eigen::Vector3d & l1_result);
};


}
