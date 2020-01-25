#include "ShapeEvaluation.h"


#include <iostream>

namespace shapeworks {
    ShapeEvaluation::ShapeEvaluation() {

    }

    void ShapeEvaluation::ReadPointFiles(const std::string &flistPath) {
        this->particles.ReadPointFiles(flistPath.c_str());
    }

    void ShapeEvaluation::Evaluate() {
        const int nModes = 1;
        std::cout << "Evaluating with nModes=" << nModes << "..." << std::endl;

        const auto compactness = this->ComputeCompactness(nModes);
        const auto generalizability = this->ComputeGeneralizability(nModes);
        const auto specificity = this->ComputeSpecificity(nModes);

        //TODO: Don't print out these things, return to caller or re-evaluate API
        // for whatever works best for the commands
        std::cout << "Done evaluating." << std::endl;
        std::cout << "Compactness:      " << compactness << std::endl;
        std::cout << "Generalizability: " << generalizability << std::endl;
        std::cout << "Specificity:      " << specificity << std::endl;
    }

    double ShapeEvaluation::ComputeCompactness(const int nModes) {
        this->particles.ComputeModes();

        const int N = this->particles.SampleSize();
        const int D = this->particles.NumberOfDimensions();

        auto eigenValues = this->particles.Eigenvalues(); // Makes a copy of the eigenvalues
        eigenValues.flip();
        eigenValues = eigenValues.apply([](const double &x) { return x * x; }); //TODO: Is there a better way to perform elem-wise square?
        eigenValues *= N - 1;

        eigenValues /= N * D;
        // Compute cumulative sum
        vnl_vector<double> cumsum(N);
        cumsum[0] = eigenValues[0];
        for(int i=1; i<N; i++) {
            cumsum[i] = cumsum[i-1] + eigenValues[i];
        }

        cumsum /= eigenValues.sum();
        return cumsum[nModes - 1];
    }

    //TODO: Implement
    double ShapeEvaluation::ComputeGeneralizability(const int nModes) {
        return -1.0;
    }

    //TODO: Implement
    double ShapeEvaluation::ComputeSpecificity(const int nModes) {
        return -1.0;
    }
}
