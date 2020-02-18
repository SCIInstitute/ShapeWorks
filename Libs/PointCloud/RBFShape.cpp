#include <RBFShape.h>

namespace shapeworks {

RBFShape::RBFShape(Eigen::Vector3d coeff,
                   Eigen::Vector3d pows,
                   Eigen::MatrixXd points)
: coeff_(coeff), pows_(pows),
id_(0), material_(0), points_(points) {
    if(this->points_.size() == 0)
        this->isEmpty_ = true;
    else
        this->isEmpty_ = false;
}

RBFShape::~RBFShape(){}

std::string RBFShape::ToDebugString() {
    return "name() = " + name() + ", id = " + std::to_string(getID()) + ", isEmtpy = " + std::to_string(isEmpty());
}

void RBFShape::setID(size_t id) { this->id_ = id; }

size_t RBFShape::getID() { return this->id_; }

double RBFShape::evaluate(const Eigen::Vector3d & point){
    CPURBFEvaluator eval = CPURBFEvaluator();
    RBFKernel * kernel = new ThinPlateKernel;
    return eval.evaluate(kernel, point, this->points_, this->TPSWeights, this->coeff_.head<3>(), this->coeff_(3));
}

Eigen::Vector3d RBFShape::gradient(const Eigen::Vector3d &point){
    return Eigen::Vector3d(1.,1.,1.);
}

void RBFShape::solve_system(Eigen::MatrixXd points){

}

} // shapeworks
