#include "ShapeEvaluation.h"


#include <iostream>
#include <Eigen/Core>
#include <Eigen/SVD>
#include <Eigen/Dense>
#include <random>

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

        if(!saveScreePlotTo.empty()) {
            std::ofstream of(saveScreePlotTo);
            of << cumsum;
            of.close();
        }

        return cumsum(nModes - 1);
    }

    double ShapeEvaluation::ComputeGeneralization(const ParticleSystem &particleSystem, const int nModes, const std::string &saveTo) {
        const int N = particleSystem.N();
        const int D = particleSystem.D();
        const Eigen::MatrixXd &P = particleSystem.Particles();

        // Keep track of the reconstructions so we can visualize them later
        struct Reconstruction {
            double dist;
            int shapeIdx;
            Eigen::MatrixXd rec;
        };
        std::vector<Reconstruction> reconstructions;

        double totalDist = 0.0;
        for(int leave=0; leave<N; leave++) {

            Eigen::MatrixXd Y(D, N-1);
            Y.leftCols(leave) = P.leftCols(leave);
            Y.rightCols(N-leave-1) = P.rightCols(N-leave-1);

            const Eigen::VectorXd mu = Y.rowwise().mean();
            Y.colwise() -= mu;
            const Eigen::VectorXd Ytest = P.col(leave);

            Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y, Eigen::ComputeFullU);
            const auto epsi = svd.matrixU().block(0, 0, D, nModes);
            const auto betas = epsi.transpose() * (Ytest - mu);
            const Eigen::VectorXd rec = epsi * betas + mu;

            //TODO: This assumes 3-Dimensions
            const int numParticles = D / 3;
            const Eigen::Map<const RowMajorMatrix> Ytest_reshaped(Ytest.data(), numParticles, 3);
            const Eigen::Map<const RowMajorMatrix> rec_reshaped(rec.data(), numParticles, 3);
            const double dist = (rec_reshaped - Ytest_reshaped).rowwise().norm().sum() / numParticles;
            totalDist += dist;

            reconstructions.push_back({dist, leave, rec_reshaped});
        }
        const double generalization = totalDist / N;

        // Save the reconstructions if needed. Generates XML files that can be opened in
        // ShapeWorksView2
        if(!saveTo.empty()) {
            std::sort(reconstructions.begin(), reconstructions.end(),
                      [](const Reconstruction &l, const Reconstruction &r) { return l.dist < r.dist; });

            const auto &srcPaths = particleSystem.Paths();

            for(int i=0; i<reconstructions.size(); i++) {
                const int percentile = i == reconstructions.size() - 1 ? 100 : std::floor(((double)i / reconstructions.size()) * 100.0);

                // Save the reconstruction
                const std::string recPath = saveTo + "/" + std::to_string(percentile) + "perc.particles";
                std::ofstream recOF(recPath);
                if(!recOF) { throw std::runtime_error("Unable to open file: " + recPath); }
                recOF << reconstructions[i].rec << std::endl;
                recOF.close();

                // Create an XML file
                const std::string xmlPath = saveTo + "/" + std::to_string(percentile) + "perc.xml";
                std::ofstream xmlOF(xmlPath);
                if(!xmlOF) { throw std::runtime_error("Unable to open file: " + xmlPath); }

                xmlOF << "<point_files>"
                      << srcPaths[reconstructions[i].shapeIdx] << std::endl << recPath
                      << "</point_files>" << std::endl
                      << "<group_ids>"
                      << 1 << std::endl << 2
                      << "</group_ids>";
                xmlOF.close();
            }
        }

        return generalization;
    }

    //TODO: Implement Specificity

    struct multiVariateNormalRandom
    {
        Eigen::VectorXd mean;
        Eigen::MatrixXd transform;

        multiVariateNormalRandom(Eigen::MatrixXd const& covar)
            : multiVariateNormalRandom(Eigen::VectorXd::Zero(covar.rows()), covar)
        {}

        multiVariateNormalRandom(Eigen::VectorXd const& mean, Eigen::MatrixXd const& covar)
            : mean(mean)
        {
            Eigen::SelfAdjointEigenSolver<Eigen::MatrixXd> eigenSolver(covar);
            transform = eigenSolver.eigenvectors() * eigenSolver.eigenvalues().cwiseSqrt().asDiagonal();
        }

        Eigen::MatrixXd operator()() const
        {
            //static std::mt19937 gen{ std::random_device{}() };
            
            // seed set as constant 1 for test repeatability
            static std::mt19937 gen{ 1 }; 

            static std::normal_distribution<> dist;

            return mean + transform * Eigen::VectorXd{ mean.size() }.unaryExpr([&](auto x) { return dist(gen); });
        }
    };

    double ShapeEvaluation::ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes) {

        const int N = particleSystem.N();
        const int D = particleSystem.D();
        
        const int nSamples = 1000;

        Eigen::VectorXd meanSpecificity(nModes);
        Eigen::VectorXd stdSpecificity(nModes);
        Eigen::MatrixXd spec_store(nModes,4);

        // PCA calculations
        const Eigen::MatrixXd &ptsModels = particleSystem.Particles();
        const Eigen::VectorXd mu = ptsModels.rowwise().mean();
        Eigen::MatrixXd Y = ptsModels;

        Y.colwise() -= mu;

        Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y, Eigen::ComputeFullU);
        const auto epsi = svd.matrixU().block(0, 0, D, nModes);
        auto eigenValues = svd.singularValues();
        eigenValues = eigenValues.segment(0, nModes);
        
        Eigen::MatrixXd samplingBetas(nModes, nSamples);         
        multiVariateNormalRandom sampling{ eigenValues.asDiagonal() };
        for (int modeNumber = 0; modeNumber < nModes; modeNumber++) {
            for (int i = 0; i < nSamples; i++) {
                samplingBetas.col(i) = sampling();
            }

            Eigen::MatrixXd samplingPoints = (epsi * samplingBetas).colwise() + mu;

            //TODO: This assumes 3-Dimensions
            const int numParticles = D / 3;
            const int nTrain = ptsModels.cols();

            Eigen::VectorXd distanceToClosestTrainingSample(nSamples);

            for (int i = 0; i < nSamples; i++) {

                Eigen::VectorXd pts_m = samplingPoints.col(i);
                Eigen::MatrixXd ptsDistance_vec = ptsModels.colwise() - pts_m;
                Eigen::MatrixXd ptsDistance(Eigen::MatrixXd::Constant(1, nTrain, 0.0));

                for (int j = 0; j < nTrain; j++) {
                    Eigen::Map<const RowMajorMatrix> ptsDistance_vec_reshaped(ptsDistance_vec.col(j).data(), numParticles, 3);
                    ptsDistance(j) = (ptsDistance_vec_reshaped).rowwise().norm().sum();
                }

                distanceToClosestTrainingSample(i) = ptsDistance.minCoeff();
            }

            meanSpecificity(modeNumber) = distanceToClosestTrainingSample.mean();

            // TODO: fix stdSpecificity values,  returns incorect standard deviation
            //stdSpecificity(modeNumber) = std::sqrt((distanceToClosestTrainingSample.rowwise() - distanceToClosestTrainingSample.colwise().mean()).array().square().array().sum()) / std::sqrt(nTrain - 1);

            //auto checkItem = stdSpecificity(modeNumber)
            //std::cout << "\n \n" << checkItem .rows() << " x " << checkItem.cols() << std::endl << "row 1:" << std::endl << checkItem.row(1) << std::endl;
        }

        //TODO: This assumes 3-Dimensions
        const int numParticles = D / 3;
        const double specificity = meanSpecificity(nModes - 1) / numParticles;

        return specificity;
    }
}

