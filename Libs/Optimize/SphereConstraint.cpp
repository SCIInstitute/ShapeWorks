#include "SphereConstraint.h"

namespace shapeworks {

bool SphereConstraint::isViolated(const Eigen::Vector3d &pt) const {
  {
    double distance_from_center =
        sqrt(pow(pt(0) - center_(0), 2) + pow(pt(1) - center_(1), 2) + pow(pt(2) - center_(2), 2));

    if (distance_from_center < radius_) {
      return true;
    }
    return false;
  }
}

Eigen::Vector3d SphereConstraint::constraintGradientSphere(const Eigen::Vector3d &pt,
                                                           const Eigen::Vector3d &updpt) const {
  // Section computes the intersections between line segment and sphere and determines if they exist
  // <-----
  // Compute projection of center to line
  // proj = pt + dot(ptcenter, update) / dot(update, update) * update
  Eigen::Vector3d update = updpt - pt;
  Eigen::Vector3d ptcenter = center_ - pt;
  // If update is none, then return distance from center without considering the update
  if (update.dot(update) == 0) {
    return constraintGradient(updpt);
  }
  Eigen::Vector3d proj = pt + ptcenter.dot(update) / update.dot(update) * update;

  // Computes the distance between line and center
  double line_dist_from_center = (proj - center_).norm();

  if (line_dist_from_center < radius_) {
    // Use pithagorean theorem to figure out the intersections of line with sphere.
    double dist_from_projection_to_intersection =
        sqrt(radius_ * radius_ - line_dist_from_center * line_dist_from_center);
    Eigen::Vector3d unit_update = update / update.norm();

    Eigen::Vector3d intersection1 = proj + dist_from_projection_to_intersection * unit_update;
    Eigen::Vector3d intersection2 = proj - dist_from_projection_to_intersection * unit_update;
    if ((pt - intersection1).norm() > (pt - intersection2).norm()) {
      Eigen::Vector3d temp = intersection1;
      intersection1 = intersection2;
      intersection2 = temp;
    }

    // ----->
    if (dist_from_projection_to_intersection > 0 && update.norm() > (pt - intersection1).norm()) {
      // Center-repel method
      // return -(intersection1 - center)/(intersection1 - center).norm();
      // Projection-repel method
      return -(intersection1 - updpt) / (intersection1 - updpt).norm();
    }
  }

  // If intersections don't exist, then just return regular gradient.
  return constraintGradient(updpt);
}

double SphereConstraint::constraintEvalSphere(const Eigen::Vector3d &pt, const Eigen::Vector3d &updpt) const {
  // Section computes the intersections between line segment and sphere and determines if they exist
  // <-----
  // Compute projection of center to line
  // proj = pt + dot(ptcenter, update) / dot(update, update) * update
  Eigen::Vector3d update = updpt - pt;
  Eigen::Vector3d ptcenter = center_ - pt;
  // If update is none, then return distance from center without considering the update
  if (update.dot(update) == 0) {
    return constraintEval(updpt);
  }
  Eigen::Vector3d proj = pt + ptcenter.dot(update) / update.dot(update) * update;

  // Computes the distance between line and center
  double line_dist_from_center = (proj - center_).norm();

  if (line_dist_from_center < radius_) {
    // Use pithagorean theorem to figure out the intersections of line with sphere.
    double dist_from_projection_to_intersection =
        sqrt(radius_ * radius_ - line_dist_from_center * line_dist_from_center);
    Eigen::Vector3d unit_update = update / update.norm();

    Eigen::Vector3d intersection1 = proj + dist_from_projection_to_intersection * unit_update;
    Eigen::Vector3d intersection2 = proj - dist_from_projection_to_intersection * unit_update;
    if ((pt - intersection1).norm() > (pt - intersection2).norm()) {
      Eigen::Vector3d temp = intersection1;
      intersection1 = intersection2;
      intersection2 = temp;
    }

    // ----->
    if (dist_from_projection_to_intersection > 0 && update.norm() > (pt - intersection1).norm()) {
      // Center-repel method
      // return (intersection1 - center).norm();
      // Projection-repel method
      return -(intersection1 - updpt).norm();
    }
  }

  // If intersections don't exist, then just return regular gradient.
  return constraintEval(updpt);
}

Eigen::Vector3d SphereConstraint::lagragianGradient(const Eigen::Vector3d &pt, const Eigen::Vector3d &updpt,
                                                    double C) const {
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
  Eigen::Vector3d constraint_grad = constraintGradientSphere(pt, updpt);
  double eval = constraintEvalSphere(pt, updpt);
  double maxterm = mu_ + C * eval;
  if (maxterm < 0) {
    return Eigen::Vector3d(0, 0, 0);
  } else {
    return maxterm * constraint_grad;
  }
}

}  // namespace shapeworks
