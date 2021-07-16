#pragma once

#include <Libs/Project/Project.h>

namespace shapeworks {

class DeepSSMParameters {

  enum class SamplerTypeOption {
    gaussian,
    gaussian_mixture,
    kde
  };

public:

  explicit DeepSSMParameters(ProjectHandle project);
  void save_to_project();

  int get_num_samples();
  void set_num_samples(int num_samples);

  int get_num_dims();
  void set_num_dims(int num_dims);

  double get_percent_variability();
  void set_percent_variability(double percent_variability);

  std::string get_sampler_type();
  void set_sampler_type(std::string sampler_type);

  void restore_defaults();

  // constants
  const static std::string DEEPSSM_SAMPLER_GAUSSIAN_C;
  const static std::string DEEPSSM_SAMPLER_GAUSSIAN_MIXTURE_C;
  const static std::string DEEPSSM_SAMPLER_KDE_C;

private:

  Parameters params_;
  ProjectHandle project_;

};

}
