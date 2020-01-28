#include "ShapeEvaluation.h"


#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SVD>

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMajorMatrix;

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
        const int N = this->particles.SampleSize();
        const int D = this->particles.NumberOfDimensions();
        const auto &_P = this->particles.ShapeMatrix();

        Eigen::Map<const RowMajorMatrix> P(_P.data_block(), _P.rows(), _P.cols());
        Eigen::MatrixXd Y = P;
        const Eigen::VectorXd mu = Y.rowwise().mean();
        Y.colwise() -= mu;

        Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y);
        const auto S = svd.singularValues();
        const auto eigValues = S.array().pow(2) / (N*D); //TODO: eigValues might be a misnomer.

        // Compute cumulative sum
        Eigen::VectorXd cumsum(N);
        cumsum[0] = eigValues(0);
        for(int i=1; i<N; i++) {
            cumsum(i) = cumsum(i-1) + eigValues(i);
        }

        cumsum /= eigValues.sum();
        return cumsum(nModes - 1);
    }

    double ShapeEvaluation::ComputeGeneralizability(const int nModes) {
        const int N = this->particles.SampleSize();
        const int D = this->particles.NumberOfDimensions();

        const auto &_P = this->particles.ShapeMatrix();
        const Eigen::Map<const RowMajorMatrix> P(_P.data_block(), _P.rows(), _P.cols());

        double totalDist = 0.0;
        for(int leave=0; leave<N; leave++) {
            Eigen::MatrixXd Y(D, N-1);
            for(int i=0; i<N; i++) {
                if(i < leave) {
                    Y.col(i) = P.col(i);
                } else if(i > leave) {
                    Y.col(i-1) = P.col(i);
                }
            }

            const Eigen::VectorXd mu = Y.rowwise().mean();
            Y.colwise() -= mu;
            const Eigen::VectorXd Ytest = P.col(leave) - mu;

            Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y, Eigen::ComputeFullU);
            const auto epsi = svd.matrixU().block(0, 0, D, nModes);
            const auto betas = epsi.transpose() * Ytest;
            const Eigen::VectorXd rec = epsi * betas;

            //TODO: This assumes 3-Dimensions
            const Eigen::Map<const RowMajorMatrix> Ytest_reshaped(Ytest.data(), D/3, 3);
            const Eigen::Map<const RowMajorMatrix> rec_reshaped(rec.data(), D/3, 3);
            const double dist = (rec_reshaped - Ytest_reshaped)
                    .array().pow(2)
                    .matrix().rowwise().sum()
                    .array().sqrt().sum();

            totalDist += dist;
        }

        const double generalizability = totalDist / N;
        return generalizability;
    }

    //TODO: Implement
    double ShapeEvaluation::ComputeSpecificity(const int nModes) {
        return -1.0;
    }
}
