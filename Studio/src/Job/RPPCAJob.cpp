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

  py::module sw = py::module::import("shapeworks");
  
  py::object geteigenVecs = sw.attr("stats").attr("get_rrpcaEigenVector");
  py::object geteigenVals = sw.attr("stats").attr("get_rppcaEigenValues");
  py::object getexpVar = sw.attr("stats").attr("get_expVar");
  py::object getmean = sw.attr("stats").attr("get_rppcamean");
  py::object getXminusMean = sw.attr("stats").attr("get_X_minsMean");
  
  Eigen::MatrixXd rppcaEigenVectors = geteigenVecs(data,num_modes);
  Eigen::VectorXd rppcaEigenValues = geteigenVecs(data,num_modes);
  Eigen::VectorXd rppcaExpVals = getexpVar(data,num_modes);
  Eigen::VectorXd rppcaMean = getmean(data,num_modes);
  Eigen::MatrixXd rppcaXminMean = getXminusMean(data,num_modes);
  this.stats_->set_rppcaEigenVecs(rppcaEigenValues);
  this.stats_->set_rppcaEigenVals(rppcaEigenValues);
  this.stats_->set_rppcaExpVar(rppcaExpVals);
  this.stats_->set_rppcaMean(rppcaMean);
  this.stats_->set_rppcaXminMean(rppcaXminMean);
  this.stats_->m_rppca_job_done = true;
}

//---------------------------------------------------------------------------
QString RPPCAJob::name()
{
  return "RPPCA Job";
}
//---------------------------------------------------------------------------



}
