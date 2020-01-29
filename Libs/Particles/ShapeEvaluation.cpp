#include "ShapeEvaluation.h"


#include <iostream>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <Eigen/SVD>
#include "ParticleSystem.h"

typedef Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor> RowMajorMatrix;

namespace shapeworks {
    ShapeEvaluation::ShapeEvaluation() {

    }

    void ShapeEvaluation::ReadPointFiles(const std::string &flistPath) {
        std::vector<std::string> filepaths = {
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_0.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_1.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_10.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_11.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_12.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_13.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_14.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_15.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_16.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_17.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_18.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_19.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_2.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_20.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles",
                "TestEllipsoids/PointFiles/128/seg.ellipsoid_21.isores.pad.com.aligned.cropped.tpSmoothDT_world.particles"
        };
        this->particleSystem.LoadParticles(filepaths);
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

    double ShapeEvaluation::ComputeCompactness(const int nModes) const {
        const int N = this->particleSystem.N();
        const int D = this->particleSystem.D();

        Eigen::MatrixXd Y = this->particleSystem.Particles();
        const Eigen::VectorXd mu = Y.rowwise().mean();
        Y.colwise() -= mu;

        Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y);
        const auto S = svd.singularValues();
        const auto eigValues = S.array().pow(2) / (N*D); //TODO: eigValues might be a misnomer.

        // Compute cumulative sum
        Eigen::VectorXd cumsum(N);
        cumsum(0) = eigValues(0);
        for(int i=1; i<N; i++) {
            cumsum(i) = cumsum(i-1) + eigValues(i);
        }

        cumsum /= eigValues.sum();
        return cumsum(nModes - 1);
    }

    double ShapeEvaluation::ComputeGeneralizability(const int nModes) const {
        const int N = this->particleSystem.N();
        const int D = this->particleSystem.D();

        const Eigen::MatrixXd &P = this->particleSystem.Particles();

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
    double ShapeEvaluation::ComputeSpecificity(const int nModes) const {
        return -1.0;
    }
}
