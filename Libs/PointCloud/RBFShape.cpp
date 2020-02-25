#include <RBFShape.h>

namespace shapeworks {

RBFShape::RBFShape(Eigen::Vector4d coeff,
                   Eigen::Vector3d pows,
                   Eigen::MatrixXd points)
: coeff_(coeff), pows_(pows),
id_(0), material_(0), points_(points) {
    if(this->points_.size() == 0)
        this->isEmpty_ = true;
    else
        this->isEmpty_ = false;
    this->kernel = new ThinPlateKernel();
}

RBFShape::~RBFShape(){}

std::string RBFShape::ToDebugString() {
    return "name() = " + name() + ", id = " + std::to_string(getID()) + ", isEmtpy = " + std::to_string(isEmpty());
}

void RBFShape::setID(size_t id) { this->id_ = id; }

size_t RBFShape::getID() { return this->id_; }

double RBFShape::evaluate(const Eigen::Vector3d & point){
    CPURBFEvaluator eval = CPURBFEvaluator();

    return eval.evaluate(this->kernel, point, this->points_, this->TPSWeights, this->coeff_.head<3>(), this->coeff_(3));
}

Eigen::Vector3d RBFShape::gradient(const Eigen::Vector3d &point){

    Eigen::Vector3d grad = Eigen::Vector3d(0.,0.,0.);

    for (size_t i = 0; i < this->points_.size(); i++) {
      Eigen::Vector3d p = this->points_.row(i).head<3>();
      Eigen::Vector3d s = (point - p);
      double r = sqrt(pow(s(0), 2) + pow(s(1), 2) + pow(s(2), 2));
      grad = grad + (s * (this->TPSWeights[i] * (this->kernel)->compute_gradient(r)));
    }

    return Eigen::Vector3d(1.,1.,1.);
}

void RBFShape::solve_system(Eigen::MatrixXd points){
    EigenRBFSolver solver = EigenRBFSolver();

    this->TPSWeights = solver.solveForCoefficients(this->kernel, this->points_);
}

} // shapeworks
