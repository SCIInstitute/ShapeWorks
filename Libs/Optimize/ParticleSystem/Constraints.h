#pragma once

#include "Constraint.h"
#include "PlaneConstraint.h"
#include "SphereConstraint.h"
#include "FreeFormConstraint.h"
#include <vector>
#include "vnl/vnl_inverse.h"
#include "vnl/vnl_cross.h"

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
  void addPlane(const vnl_vector<double> &a, const vnl_vector<double> &b,
                const vnl_vector<double> &c){
                  // See http://mathworld.wolfram.com/Plane.html, for example
                  vnl_vector<double> q;
                  if (DIMENSION == 3)  q = vnl_cross_3d((b-a),(c-a));
                  else if (DIMENSION == 2)  q = vnl_cross_2d((b-a),(c-a));

                  if (q.magnitude() > 0.0)
                  {
                    Eigen::Vector4d qp;
                    q = q/q.magnitude();
                    qp(0) = q[0]; qp(1) = q[1]; qp(2) = q[2]; qp(3) = q.magnitude();
                    PlaneConstraint plane_c;
                    plane_c.SetPlane(qp);
                    plane_c.SetA(a);
                    plane_c.SetB(b);
                    plane_c.SetC(c);
                  }
                }
  void setSpheres(std::string filename);
  void setFreeFormConstraint(std::string filename);

  // Set & get defined
  void setCuttingPlaneDefined(bool value){cuttingPlaneDefined = value;}
  bool getCuttingPlaneDefined(){return cuttingPlaneDefined;}

  // Write constraints
  bool writePlanes(std::string filename){return true;}
  bool writeSpheres(std::string filename){return true;}
  bool writeFreeFormConstraint(std::string filename){return true;}

  // Is defined? functions
  bool IsCuttingPlaneDefined() const {if(planeConsts->size() > 0) return true; return false;}
  bool IsCuttingSphereDefined() const {if(sphereConsts->size() > 0) return true; return false;}

protected:
  std::vector<PlaneConstraint> *planeConsts;
  std::vector<SphereConstraint> *sphereConsts;
  std::vector<FreeFormConstraint> *freeFormConsts;

  bool cuttingPlaneDefined;
};


}
