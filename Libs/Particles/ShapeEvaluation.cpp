#include "ShapeEvaluation.h"


#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include "ParticleSystem.h"

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMajorMatrix;

namespace shapeworks {
    double ShapeEvaluation::ComputeCompactness(const ParticleSystem &particleSystem, const int nModes, const std::string &saveScreePlotTo) {
        const int N = particleSystem.N();
        const int D = particleSystem.D();

        Eigen::MatrixXd Y = particleSystem.Particles();
        const Eigen::VectorXd mu = Y.rowwise().mean();
        Y.colwise() -= mu;

        Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y);
        const auto S = svd.singularValues().array().pow(2) / (N*D);

        // Compute cumulative sum
        Eigen::VectorXd cumsum(N);
        cumsum(0) = S(0);
        for(int i=1; i<N; i++) {
            cumsum(i) = cumsum(i-1) + S(i);
        }
        cumsum /= S.sum();

        if(saveScreePlotTo != "") {
            std::ofstream of(saveScreePlotTo);
            of << cumsum;
            of.close();
        }

        return cumsum(nModes - 1);
    }

    double ShapeEvaluation::ComputeGeneralization(const ParticleSystem &particleSystem, const int nModes) {
        const int N = particleSystem.N();
        const int D = particleSystem.D();

        const Eigen::MatrixXd &P = particleSystem.Particles();

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
    double ShapeEvaluation::ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes) {
        return -1.0;
    }
}
