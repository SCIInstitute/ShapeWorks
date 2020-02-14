#include "ShapeEvaluation.h"


#include <iostream>
#include <Eigen/Core>
#include <Eigen/SVD>
//#include <Eigen/Dense>

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

    //TODO: Implement
    double ShapeEvaluation::ComputeSpecificity(const ParticleSystem &particleSystem, const int nModes) {

        const int N = particleSystem.N();
        const int D = particleSystem.D();
        
        const int nSamples = 1000;

        Eigen::VectorXd meanSpecificity(nModes);
        Eigen::VectorXd stdSpecificity(nModes);
        Eigen::MatrixXd spec_store(nModes,4);

        std::cout << "Computing specificity..." << std::endl;

        //DoPCA
        const Eigen::MatrixXd &ptsModels = particleSystem.Particles();
        const Eigen::VectorXd mu = ptsModels.rowwise().mean();
        Eigen::MatrixXd Y = ptsModels;

        Y.colwise() -= mu;

        Eigen::JacobiSVD<Eigen::MatrixXd> svd(Y, Eigen::ComputeFullU);
        const auto epsi = svd.matrixU().block(0, 0, D, nModes);
        auto eigenValues = svd.singularValues();
        eigenValues = eigenValues.array().pow(2);
        //eigenValues = eigenValues.row( (0, nModes);
        //eigenValues(


        auto checkItem = eigenValues.segment(0,nModes);
        std::cout << checkItem.rows() << " x " << checkItem.cols() << std::endl << "row 1:" << std::endl << checkItem << std::endl;

        //Eigen::MatrixXd a(4, 3);
        //a << 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12;
        //Eigen::JacobiSVD<Eigen::MatrixXd> svd(a, Eigen::ComputeFullU);
        
        //const auto s = svd.matrixU().block(0, 0,4,nModes);
        //const auto v = svd.matrixV();
        //const auto d = svd.singularValues().block(0, 0, 4, nModes);
        //auto checkItem = s;
        //std::cout << checkItem.rows() << " x " << checkItem.cols() << std::endl << "row 1:" << std::endl << checkItem << std::endl;
        ////auto checkItem2 = v;
        ////std::cout << checkItem2.rows() << " x " << checkItem2.cols() << std::endl << "row 1:" << std::endl << checkItem2.row(0) << std::endl;
        //auto checkItem3 = d;
        //std::cout << checkItem3.rows() << " x " << checkItem3.cols() << std::endl << "row 1:" << std::endl << checkItem3 << std::endl;

        //Eigen::MatrixXd a(3,3);
        //Eigen::MatrixXd b;

        //a = Eigen::MatrixXd::Random(3,3);
        //b = Eigen::MatrixXd::Constant(3, 1, 1.0);

        //std::cout << "a:" << std::endl << a << std::endl;
        //std::cout << "b:" << std::endl << b << std::endl;

        //Eigen::MatrixXd c = a * b;
        //std::cout << "c:" << std::endl << c << std::endl;

        ////a = Eigen::MatrixXd::Random(2,2);
        ////std::cout << "a:" << std::endl << a << std::endl;
        ////std::cout << a.colwise()-b << std::endl;
        ////a = a.isZero

        ////for
        //a = a.array + 1;
        //b(1) += .5;
        //std::cout << b << std::endl;


        //std::cout << std::endl;
        return -1.0;
    }
}
