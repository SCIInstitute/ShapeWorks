#include "ShapeEvaluation.h"
#include "EvaluationUtil.h"
#include "ParticleShapeStatistics.h"
#include <iostream>
#include <Eigen/Core>
#include <Eigen/SVD>

#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal


namespace shapeworks{


int RPPCA::ComputeRPPCAMode()
{
	Eigen::MatrixXd Y = particleSystem.Particles();
	const int N = particleSystem.N();
	const int D = particleSystem.D();
	const int num_modes = N-1; // the number of modes is one less than the number of samples
	
	py::module sw = py::module::import("shapeworks");
	py::object compute = sw.attr("stats").attr("robust_ppca");
	

	std::tie(Eigen::VectorXd m_rppcaVarByMode,Eigen::MatrixXd rppcaEigenVecs,Eigen::VectorXd mean,Eigen::VectorXd lambda) = compute(data,num_modes).cast(std::tuple<Eigen::VectorXd,Eigen::MatrixXd,Eigen::VectorXd,Eigen::VectorXd>);

	m_rppcspointsMinusMean = Y - mean;
	rppcaProjections = m_rppcspointsMinusMean * rppcaEigenVecs;

	// normalize those eigenvectors
  	for (unsigned int i = 0; i < N; i++) 
  	{
    double total = 0.0f;
    for (unsigned int j = 0; j < D; j++) {
      total += rppcaProjections(j, i) * rppcaProjections(j, i);
    }
    total = sqrt(total);

    for (unsigned int j = 0; j < D; j++) {
      rppcaProjections(j, i) = rppcaProjections(j, i) / (total + 1.0e-15);
    }

  	}

}






} //shapeworks