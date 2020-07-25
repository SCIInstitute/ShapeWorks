#include "Constraints.h"

namespace itk
{

void Constraints::addPlane(const vnl_vector<double> &a, const vnl_vector<double> &b,const vnl_vector<double> &c){
    // See http://mathworld.wolfram.com/Plane.html, for example
    vnl_vector<double> q;
    if (DIMENSION == 3)  q = vnl_cross_3d((b-a),(c-a));
    else if (DIMENSION == 2)  q = vnl_cross_2d((b-a),(c-a));

    if (q.magnitude() > 0.0)
    {
      Eigen::Vector3d qp;
      q = q/q.magnitude();
      qp(0) = q[0]; qp(1) = q[1]; qp(2) = q[2];
      PlaneConstraint plane_c;
      plane_c.SetPlaneNormal(qp);
      plane_c.SetPlanePoint(a);
      planeConsts->push_back(plane_c);
    }
  }

}
