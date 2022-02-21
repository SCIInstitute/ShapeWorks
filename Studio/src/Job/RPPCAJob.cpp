#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

#include <Job/RPPCAJob.h>

namespace shapeworks {
//---------------------------------------------------------------------------
RPPCAJob::RPPCAJob(ParticleShapeStatistics stats) : stats_(stats)
{}

//---------------------------------------------------------------------------
void RPPCAJob::run()
{ 
  std::cout << "Inside the job" << std::endl;

  auto data = this->stats_.ShapeMatrix();
  int numSamples = data.cols();
  int numDimensions = data.rows();
  auto num_modes = this->stats_.SampleSize();
  // std::cout << this->stats_->ShapeMatrix().cols() << std::endl;
  py::module sw = py::module::import("shapeworks");
  
  py::object geteigenVecs = sw.attr("stats").attr("get_rrpcaEigenVector");
  py::object geteigenVals = sw.attr("stats").attr("get_rppcaEigenValues");
  py::object getexpVar = sw.attr("stats").attr("get_expVar");
  py::object getmean = sw.attr("stats").attr("get_rppcamean");
  
  
  Eigen::MatrixXd rppcaEigenVectors = geteigenVecs(data,num_modes).cast<Eigen::MatrixXd>();
  Eigen::VectorXd rppcaEigenValues = geteigenVals(data,num_modes).cast<Eigen::VectorXd>();
  Eigen::VectorXd rppcaExpVals = getexpVar(data,num_modes).cast<Eigen::VectorXd>();
  Eigen::VectorXd rppcaMean = getmean(data,num_modes).cast<Eigen::VectorXd>();
  
  // for (unsigned int i = 0; i < numSamples; i++) {
  //   double total = 0.0f;
  //   for (unsigned int j = 0; j < numDimensions; j++) {
  //     total += rppcaEigenVectors(j, i) * rppcaEigenVectors(j, i);
  //   }
  //   total = sqrt(total);

  //   for (unsigned int j = 0; j < numDimensions; j++) {
  //     rppcaEigenVectors(j, i) = rppcaEigenVectors(j, i) / (total + 1.0e-15);
  //   }

  // }
  
  

  this->m_rppcaeigenvalues.resize(rppcaEigenValues.size());
    for(int i = 0; i<rppcaEigenValues.size(); i++){
      this->m_rppcaeigenvalues[i] = rppcaEigenValues[i];
    }

  this->m_rppcapercentVarByMode.resize(rppcaExpVals.size());
    for(int i = 0; i<rppcaExpVals.size(); i++){
      this->m_rppcapercentVarByMode[i] = rppcaExpVals[i];
    }
  this->m_rppcaeigenvectors = rppcaEigenVectors;
  this->m_rppcamean = rppcaMean;
}

//---------------------------------------------------------------------------
QString RPPCAJob::name()
{
  return "RPPCA Job";
}
//---------------------------------------------------------------------------


}
