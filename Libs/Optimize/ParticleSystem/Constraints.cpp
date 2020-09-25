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

void Constraints::addSphere(const vnl_vector_fixed<double, DIMENSION> &v, double r){
    Eigen::Vector3d c; c(0) = v[0]; c(1) = v[1]; c(2) = v[2];
    SphereConstraint sphere_c;
    sphere_c.SetCenter(c);
    sphere_c.SetRadius(r);
}

bool Constraints::transformConstraints(const vnl_matrix_fixed<double, 4, 4> &Trans){
    return transformPlanes(Trans) & true;
}

bool Constraints::transformPlanes(const vnl_matrix_fixed<double, 4, 4> &Trans){

  std::cout << "Transforming planes" << Trans <<  std::endl;

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

std::stringstream Constraints::applyBoundaryConstraints(vnl_vector_fixed<double, 3> &gradE, const Point<double, 3> &pos){
    return applyPlaneConstraints(gradE, pos);
}

std::stringstream Constraints::applyBoundaryConstraints(vnl_vector_fixed<float, 3> &gradE, const Point<double, 3> &pos){
    vnl_vector_fixed<double, 3> gradD;
    gradD[0] = double(gradE[0]); gradD[1] = double(gradE[1]); gradD[2] = double(gradE[2]);
    std::stringstream  out = applyPlaneConstraints(gradD, pos);
    gradE[0] = float(gradD[0]); gradE[1] = float(gradD[1]); gradE[2] = float(gradD[2]);
    return out;
}

Eigen::Vector3d Constraints::projectOntoLine(Eigen::Vector3d a, Eigen::Vector3d b, Eigen::Vector3d p){
    Eigen::Vector3d ap = p-a;
    Eigen::Vector3d ab = b-a;
    return a+ap.dot(ab)/ab.dot(ab)*ab;
}

Eigen::Vector3d Constraints::linePlaneIntersect(Eigen::Vector3d n, Eigen::Vector3d p0, Eigen::Vector3d l0, Eigen::Vector3d l){
    double d = (p0-l0).dot(n)/l.dot(n);
    return l0+l*d;
}

bool Constraints::PlanePlaneIntersect(Eigen::Vector3d n1, Eigen::Vector3d p1, Eigen::Vector3d n2, Eigen::Vector3d p2, Eigen::Vector3d & l0_result, Eigen::Vector3d & l1_result){

    Eigen::Vector4d pl1;
    pl1(0)=n1(0); pl1(1)=n1(1); pl1(2)=n1(2);
    pl1(3) = - n1.dot(p1);
    Eigen::Vector4d pl2;
    pl2(0)=n2(0); pl2(1)=n2(1); pl2(2)=n2(2);
    pl2(3) = - n2.dot(p2);

    /*
    std::cout << "pl1 " << pl1.transpose() << " pl2 " << pl2.transpose() << std::endl;

    l0_result = SolveForPointOnPlaneIntersect(pl1, pl2, 0);
    l1_result = SolveForPointOnPlaneIntersect(pl1, pl2, 1);
    */
    Eigen::Vector3d n3 = n1.cross(n2);
    double det = n3.norm()*n3.norm();

    if (det != 0.0) {
        // calculate the final (point, normal)
        l0_result = ((n3.cross(n2) * pl1(3)) +
                   (n1.cross(n3) * pl2(3))) / det;
        l1_result = l0_result+n3;
        return true;
    }
    else {
        return false;
    }
}

// This function implementation performs a series of projections onto planes such that at the end, the gradient update is close to the original(<45 degrees)
// and the magnitude is less than or equal to the original. Read comments for more information.
std::stringstream Constraints::applyPlaneConstraints(vnl_vector_fixed<double, 3> &gradE, const Point<double, 3> &pos){

    // Error offset to account for precision error
    double eps = 1e-10;

    // Convert points and grads to eigen vectors
    Eigen::Vector3d l0; l0(0) = pos[0]; l0(1) = pos[1]; l0(2) = pos[2];
    Eigen::Vector3d l; l(0) = -gradE[0]; l(1) = -gradE[1]; l(2) = -gradE[2];

    std::stringstream stream;
    /*
    stream << "Constraint.cpp::ApplyPlaneConstraints(" << pos << ")" << std::endl
           << "Original point " << l0.transpose() << std::endl
           << "Original gradient " << l.transpose() << std::endl
          << "Original Updated point " << (l0+l).transpose() << std::endl;
    */

    // Check if original point violates any constraints
    // If it does, try to go towards the closest plane with the magnitude of the gradient
    std::vector<int> violatedPlanes = planesViolated(l0);
    if(violatedPlanes.size() > 0){
        std::stringstream stream;
        stream << "Violation! " << l0.transpose() << " violates constraints ";
        for(size_t i = 0; i < violatedPlanes.size(); i++){
            stream << violatedPlanes[i] << ", ";
        }
        stream << "." << std::endl;
        // debuggg
        // std::cerr << stream.str();

        /*
        Eigen::Vector3d n = (*planeConsts)[violatedPlanes[0]].GetPlaneNormal();
        Eigen::Vector3d p0 = (*planeConsts)[violatedPlanes[0]].GetPlanePoint();

        double minD = (p0-l0).dot(n)/l.dot(n);
        int minDInd = violatedPlanes[0];
        for(size_t i = 1; i < violatedPlanes.size(); i++){
            n = (*planeConsts)[violatedPlanes[i]].GetPlaneNormal();
            p0 = (*planeConsts)[violatedPlanes[i]].GetPlanePoint();
            double d = (p0-l0).dot(n)/l.dot(n);
            if(d < minD){
                minD = d;
                minDInd = violatedPlanes[i];
            }
        }

        Eigen::Vector3d updated_gradient = (*planeConsts)[minDInd].GetPlaneNormal()*l.norm();
        gradE[0] = -updated_gradient(0); gradE[1] = -updated_gradient(1); gradE[2] = -updated_gradient(2);
        return false;
        */
    }

    // Figure out dominant plane i.e. the one closest to pose in the direction of the gradient
    // Ranks the value of d when intersecting plane and line segment
    std::vector<double> Ds;
    double minD = -1.;
    int minDInd = -1;
    std::vector<size_t> violations;
    std::vector<double> distances;
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
            // if l.n==0, then the plane and the line are parallel. If (p0 - l0).n==0, the plane contains the line.

            double d = (p0-l0).dot(n)/l.dot(n);
            Ds.push_back(d);

            minD = d;
            minDInd = i;

            violations.push_back(i);
            distances.push_back(d);
        }
        else{
            Ds.push_back(-1.);
        }
    }

    bool run = true;
    // If exactly two planes are violated and the points are almost violating the plane, we project the updated point to the intersection between these planes
    if(violations.size() == 2){
        stream << "distance0 " << distances[0] << " distance1 " << distances[0] << std::endl;
        if(distances[0] <= 1e-4 && distances[1] <= 1e-4){
            // Plane 1
            Eigen::Vector3d n_d = (*planeConsts)[violations[0]].GetPlaneNormal();
            Eigen::Vector3d p0_d = (*planeConsts)[violations[0]].GetPlanePoint();
            // Plane 2
            Eigen::Vector3d n_d2 = (*planeConsts)[violations[1]].GetPlaneNormal();
            Eigen::Vector3d p0_d2 = (*planeConsts)[violations[1]].GetPlanePoint();
            // Compute plane plane intersect line
            Eigen::Vector3d a;
            Eigen::Vector3d b;
            run = !PlanePlaneIntersect(n_d+(n_d*eps), p0_d, n_d2+(n_d2*eps), p0_d2, a, b);

            Eigen::Vector3d new_point = projectOntoLine(a, b, l0+l);

            // Update gradient
            Eigen::Vector3d updated_gradient = (new_point-l0);
            gradE[0] = -updated_gradient(0); gradE[1] = -updated_gradient(1); gradE[2] = -updated_gradient(2);

            //if(l(0)*updated_gradient(0) < 0 || l(1)*updated_gradient(1) < 0 || l(2)*updated_gradient(2) < 0 || updated_gradient(1)/l(1) > 2){
            stream << "---------------------2 Constraints violated---------------------" << std::endl
                   << "Original point " << l0.transpose() << std::endl
                  << "Original Updated point " << (l0+l).transpose() << std::endl
                  << "a " << a.transpose() << " b " << b.transpose() << std::endl
                  << "Corrected point " << new_point.transpose() << std::endl
                       << "Original gradient " << l.transpose() << std::endl
                       << "Updated gradient " << updated_gradient.transpose() << std::endl << std::endl;
            //std::cout << stream.str();
        }
        else{
            run = true;
        }
    }
    // else if update violates at least one plane
    if(minDInd > -1 && run){

        // Project gradient-applied point onto dominant plane
        Eigen::Vector3d n_d = (*planeConsts)[minDInd].GetPlaneNormal();
        Eigen::Vector3d p0_d = (*planeConsts)[minDInd].GetPlanePoint();
        Eigen::Vector3d curr_updated_pt = linePlaneIntersect(n_d, p0_d+(n_d*eps), l0+l, n_d);

        // Projected point proj_grad_upd is curr_updated_pt projected to the gradient update vector segment l0 -> l0+l
        //Eigen::Vector3d proj_grad_upd = projectOntoLine(l0, l0+l, curr_updated_pt);
        // Projected point proj_grad_upd is the intersection bt the dominant plane and the original gradient update vector
        Eigen::Vector3d proj_grad_upd = linePlaneIntersect(n_d, p0_d+(n_d*eps), l0, l);

        stream << "---------------------Constraint violated---------------------" << std::endl
                << "Original point " << l0.transpose() << std::endl
               << "Original Updated point " << (l0+l).transpose() << std::endl
               << "Dominant plane id " << minDInd << ". Dominant plane normal: " << n_d.transpose() << ". Dominant plane point: " << p0_d.transpose() << std::endl
               << "Original intersection on dominant plane " << linePlaneIntersect(n_d, p0_d, l0+l, n_d).transpose() << std::endl
               << "proj_grad_update " << proj_grad_upd.transpose() << std::endl<< std::endl;

        // Now progressively check for how non-dominant planes affect the plane
        for(size_t i = 0; i < planeConsts->size(); i++){
            // If constraint is violated, update gradient
            stream << "----Checking plane " << i << std::endl;
            if((*planeConsts)[i].isViolated(curr_updated_pt)){
                // Get points
                Eigen::Vector3d n = (*planeConsts)[i].GetPlaneNormal();
                Eigen::Vector3d p0 = (*planeConsts)[i].GetPlanePoint();

                // Find intersection between violated plane and feasible range line segment i.e. the line between curr_updated_pt and its projection to the gradient update vector segment.
                curr_updated_pt = linePlaneIntersect(n, p0+(n*eps), curr_updated_pt, proj_grad_upd-curr_updated_pt);
                //proj_grad_upd = projectOntoLine(l0, l0+l, curr_updated_pt);
                stream << "Plane " << i << " violated" << " updatedpoint " << curr_updated_pt.transpose() << std::endl << std::endl;
            }
        }

        // Update gradient
        Eigen::Vector3d updated_gradient = (curr_updated_pt-l0);
        gradE[0] = -updated_gradient(0); gradE[1] = -updated_gradient(1); gradE[2] = -updated_gradient(2);

        //if(l(0)*updated_gradient(0) < 0 || l(1)*updated_gradient(1) < 0 || l(2)*updated_gradient(2) < 0 || updated_gradient(1)/l(1) > 2){
        stream << "Corrected point " << curr_updated_pt.transpose() << std::endl
                  << "Actual Corrected point " << (l0+updated_gradient).transpose() << std::endl
                   << "Original gradient " << l.transpose() << std::endl
                   << "Updated gradient " << updated_gradient.transpose() << std::endl << std::endl;
        //}
        std::vector<int> violatedPlanes = planesViolated(curr_updated_pt);
        if(violatedPlanes.size() > 0){
            stream << "Violation! " << curr_updated_pt.transpose() << " violates constraints ";
            for(size_t i = 0; i < violatedPlanes.size(); i++){
                stream << violatedPlanes[i] << ", ";
            }
            stream << "." << std::endl;
        }
    }

    //std::cout << stream.str();
    // CHECK: Is returning false correct? This is what the previous function returns
    return stream;
}

// Cutting plane constraints using truncated gradients. Replaced with dominant constraint above.
/*
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
*/
}
