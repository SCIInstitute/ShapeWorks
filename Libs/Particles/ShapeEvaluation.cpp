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
        // Its probably best to just Compute SVD here, the ComputeModes function
        // seems to be doing a lot of weird things...
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

    double ShapeEvaluation::ComputeGeneralizability(const int nModes) {
        const int N = this->particles.SampleSize();
        const int D = this->particles.NumberOfDimensions();

        const auto & P = this->particles.RecenteredShape();

        for(int leave=0; leave<N; leave++) {
            vnl_matrix<double> Y(D, N-1); // Store all particles except one.
            //TODO: Copy these without extra memory... Perhaps using `update` and/or `extract`
            // see https://vxl.github.io/doc/release/core/vnl/html/classvnl__matrix.html#adc6f9aab16a597e0addbab45a023e986
            for(int i=0; i<N; i++) {
                if(i < leave) {
                    Y.set_column(i, P.get_column(i));
                } else if(i > leave) {
                    Y.set_column(i-1, P.get_column(i));
                }
            }

            const auto Ytest = P.get_column(leave);

            //TODO: This is _slightly_ off by the MATLAB SVD result. Why?
            // C++ S[0] = 7841.8507, MATLAB S(1) = 7794.4
            vnl_svd<double> svd(Y);
            const auto S = svd.W().diagonal().apply([](const double &x) { return x * x; }); //TODO: Is there a better way to perform elem-wise square?
            const auto epsi = svd.U().extract(D, nModes);
            const auto betas = epsi.transpose() * Ytest;
            const auto rec = epsi * betas;

            const auto recDist = (rec - Ytest).two_norm();
            std::cout << recDist << std::endl;
        }


        return -1.0;
    }

    //TODO: Implement
    double ShapeEvaluation::ComputeSpecificity(const int nModes) {
        return -1.0;
    }
}
