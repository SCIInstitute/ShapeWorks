#include <pybind11/embed.h>
#include <pybind11/stl.h>
#include <pybind11/eigen.h>
namespace py = pybind11;
using namespace pybind11::literals; // to bring in the `_a` literal

#include <Job/RPPCAJob.h>

namespace shapeworks {

//---------------------------------------------------------------------------
GroupPvalueJob::RPPCAJob(ParticleShapeStatistics stats) : stats_(stats)
{}

//---------------------------------------------------------------------------
void RPPCAJob::run()
{
  auto group_1_data = this->stats_.get_group1_matrix();
  auto group_2_data = this->stats_.get_group2_matrix();
  py::module sw = py::module::import("shapeworks");
  py::object compute = sw.attr("stats").attr("compute_pvalues_for_group_difference_studio");
  Eigen::MatrixXd pvalues = compute(group_1_data, group_2_data, 100).cast<Eigen::MatrixXd>();

  this->group_pvalues_.resize(pvalues.rows());
  for (int i = 0; i < pvalues.rows(); i++) {
    this->group_pvalues_(i) = pvalues(i, 0);
  }
}

//---------------------------------------------------------------------------
QString GroupPvalueJob::name()
{
  return "Group p-values";
}
//---------------------------------------------------------------------------

Eigen::VectorXf GroupPvalueJob::get_group_pvalues()
{
  return this->group_pvalues_;
}
}
