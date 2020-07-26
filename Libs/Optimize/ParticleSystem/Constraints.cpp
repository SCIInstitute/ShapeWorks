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

bool Constraints::transformConstraints(const vnl_matrix_fixed<double, 4, 4> &Trans){
    return transformPlanes(Trans) & true;
}

bool Constraints::transformPlanes(const vnl_matrix_fixed<double, 4, 4> &Trans){

  // Convert vnl_matrix to Eigen Matrix
  Eigen::Matrix4d trans;
  for(unsigned int i = 0; i < 4; i++){
    for(unsigned int j = 0; j < 4; j++){
      trans(i,j) = Trans[i][j];
    }
  }

  // Maybe TODO Check if transformation matrix is invertible. Return false if not.

  // Transform each plane to new format
  for(unsigned int i = 0; i < planeConsts->size(); i++){
    //Get points
    Eigen::Vector3d norm = (*planeConsts)[i].GetPlaneNormal();
    Eigen::Vector3d point = (*planeConsts)[i].GetPlanePoint();

    // Convert to homogeneous coordinates i.e. add an extra coordinate
    Eigen::Vector4d norm_homogeneous; norm_homogeneous(0) = norm(0); norm_homogeneous(1) = norm(1); norm_homogeneous(2) = norm(2); norm_homogeneous(3) = 0;
    Eigen::Vector4d point_homogeneous; point_homogeneous(0) = point(0); point_homogeneous(1) = point(1); point_homogeneous(2) = point(2); point_homogeneous(3) = 1;

    // Apply transformation
    // N = M^{-1}^{T} * N
    // O = M * O
    Eigen::Vector4d new_point_homogeneous = trans*point_homogeneous;
    Eigen::Vector4d new_norm_homogeneous = trans.inverse().transpose()*norm_homogeneous;
    new_point_homogeneous = new_point_homogeneous/new_point_homogeneous(3);
    new_norm_homogeneous = new_norm_homogeneous/new_norm_homogeneous(3);

    // Return to 3D coordinates
    Eigen::Vector3d new_point = new_point_homogeneous.head<3>();
    Eigen::Vector3d new_norm = new_norm_homogeneous.head<3>();
    new_norm = new_norm/new_norm.norm();

    // Set transformed planes
    (*planeConsts)[i].SetPlaneNormal(new_norm);
    (*planeConsts)[i].SetPlanePoint(new_point);
  }

  return true;

}

bool Constraints::applyBoundaryConstraints(vnl_vector_fixed<double, 3> &gradE, const Point<double, 3> &pos){
    return applyPlaneConstraints(gradE, pos);
}

bool Constraints::applyPlaneConstraints(vnl_vector_fixed<double, 3> &gradE, const Point<double, 3> &pos){

    // Convert points and grads to eigen vectors
    Eigen::Vector3d l0; l0(0) = pos[0]; l0(1) = pos[1]; l0(2) = pos[2];
    Eigen::Vector3d l; l(0) = -gradE[0]; l(1) = -gradE[1]; l(2) = -gradE[2];

    // Apply each cutting plane constraint
    for(size_t i = 0; i < planeConsts->size(); i++){

        // If constraint is violated, update gradient
        if((*planeConsts)[i].isViolated(l0+l)){

            // Get points
            Eigen::Vector3d n = (*planeConsts)[i].GetPlaneNormal();
            Eigen::Vector3d p0 = (*planeConsts)[i].GetPlanePoint();

            // Find intersection between cutting plane (p0, n) and update line (l0, l)
            // Eq of plane:             (p - p0).n = 0
            // Eq of line segment:      p = l0 + ld
            // Solving for d, we have d = (p0 - l0).n / l.n
            double d = (p0-l0).dot(n)/l.dot(n);
            Eigen::Vector3d p = l0 + l*d;

            l = p - l0;
        }
    }

    // Update gradient
    gradE[0] = -l(0); gradE[1] = -l(1); gradE[2] = -l(2);

    // CHECK: Is returning false correct? This is what the previous function returns
    return false;
}

}
