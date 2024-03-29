#include <Common/Logging.h>
#include <Libs/Project/Project.h>
#include <pybind11/eigen.h>
#include <pybind11/embed.h>
#include <pybind11/stl.h>

namespace py = pybind11;
using namespace pybind11::literals;  // to bring in the `_a` literal

#include <Job/NetworkAnalysisJob.h>

namespace shapeworks {

//---------------------------------------------------------------------------
NetworkAnalysisJob::NetworkAnalysisJob(std::shared_ptr<Project> project, std::string target_group,
                                       std::string target_feature)
    : project_(project), target_group_(target_group), target_feature_(target_feature) {}

//---------------------------------------------------------------------------
void NetworkAnalysisJob::run() {
  SW_DEBUG("Running network analysis job");

  auto group_1_data = this->stats_.get_group1_matrix();
  auto group_2_data = this->stats_.get_group2_matrix();
  py::module sw = py::module::import("shapeworks");

  // auto stats = sw.attr("stats");

  py::type network_analysis_class = sw.attr("NetworkAnalysis");
  py::object network_analysis = network_analysis_class(project_);
  network_analysis.attr("set_target_group")(target_group_);
  network_analysis.attr("set_target_feature")(target_feature_);
  network_analysis.attr("set_pvalue_of_interest")(pvalue_of_interest_);
  network_analysis.attr("set_pvalue_threshold")(pvalue_threshold_);
  network_analysis.attr("set_num_iterations")(num_iterations_);

  py::object res = network_analysis.attr("run")();

  // py::object compute = network_analysis.attr("run");
  // Eigen::MatrixXd pvalues = compute(group_1_data, group_2_data, 100).cast<Eigen::MatrixXd>();

  Eigen::MatrixXd network_values = network_analysis.attr("particle_network_values").cast<Eigen::MatrixXd>();
  Eigen::MatrixXd spm_values = network_analysis.attr("particle_spm_values").cast<Eigen::MatrixXd>();

  // self.particle_fvalues_size = particle_fvalues_size
  // self.particle_fvalues_1d = particle_fvalues_1d

  tvalues_.resize(network_values.rows());
  for (int i = 0; i < network_values.rows(); i++) {
    tvalues_(i) = network_values(i, 0);
  }

  spm_values_.resize(spm_values.rows());
  for (int i = 0; i < spm_values.rows(); i++) {
    spm_values_(i) = spm_values(i, 0);
  }
  SW_DEBUG("End network analysis job");
}

//---------------------------------------------------------------------------
QString NetworkAnalysisJob::name() { return "Network Analysis"; }

//---------------------------------------------------------------------------
Eigen::VectorXd NetworkAnalysisJob::get_tvalues() { return tvalues_; }

//---------------------------------------------------------------------------
Eigen::VectorXd NetworkAnalysisJob::get_spm_values() { return spm_values_; }

//---------------------------------------------------------------------------
void NetworkAnalysisJob::set_pvalue_of_interest(double pvalue_of_interest) { pvalue_of_interest_ = pvalue_of_interest; }

//---------------------------------------------------------------------------
void NetworkAnalysisJob::set_pvalue_threshold(double pvalue_threshold) { pvalue_threshold_ = pvalue_threshold; }

//---------------------------------------------------------------------------
void NetworkAnalysisJob::set_num_iterations(int num_iterations) { num_iterations_ = num_iterations; }

//---------------------------------------------------------------------------
}  // namespace shapeworks
