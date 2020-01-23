#include "ShapeEvaluation.h"


#include <iostream>

namespace shapeworks {
    ShapeEvaluation::ShapeEvaluation() {

    }

    void ShapeEvaluation::ReadPointFiles(const std::string &flistPath) {
        this->particles.ReadPointFiles(flistPath.c_str());
    }

    void ShapeEvaluation::Evaluate() {
        std::cout << "Evaluation..." << std::endl;
    }
}
