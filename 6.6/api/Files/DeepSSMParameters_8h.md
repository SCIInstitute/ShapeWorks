---
title: Studio/DeepSSM/DeepSSMParameters.h

---

# Studio/DeepSSM/DeepSSMParameters.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::DeepSSMParameters](../Classes/classshapeworks_1_1DeepSSMParameters.md)**  |




## Source code

```cpp
#pragma once

#include <Project/Project.h>

namespace shapeworks {

class DeepSSMParameters {
  enum class SamplerTypeOption { gaussian, gaussian_mixture, kde };

 public:
  explicit DeepSSMParameters(ProjectHandle project);
  void save_to_project();

  int get_aug_num_samples();
  void set_aug_num_samples(int num_samples);

  int get_aug_num_dims();
  void set_aug_num_dims(int num_dims);

  int get_training_num_dims();
  void set_training_num_dims(int num_dims);

  double get_aug_percent_variability();
  void set_aug_percent_variability(double percent_variability);

  std::string get_aug_sampler_type();
  void set_aug_sampler_type(std::string sampler_type);

  int get_training_epochs();
  void set_training_epochs(int epochs);

  double get_training_learning_rate();
  void set_training_learning_rate(double rate);

  bool get_training_decay_learning_rate();
  void set_training_decay_learning_rate(bool decay);

  bool get_training_fine_tuning();
  void set_training_fine_tuning(bool fine_tuning);

  int get_training_fine_tuning_epochs();
  void set_training_fine_tuning_epochs(int epochs);

  double get_training_fine_tuning_learning_rate();
  void set_training_fine_tuning_learning_rate(double rate);

  int get_training_batch_size();
  void set_training_batch_size(int batch_size);

  double get_training_split();
  void set_training_split(double value);

  double get_validation_split();
  void set_validation_split(double value);

  double get_testing_split();
  void set_testing_split(double value);

  bool get_prep_step_complete();
  void set_prep_step_complete(bool value);

  int get_prep_stage();
  void set_prep_stage(int stage);

  bool get_aug_step_complete();
  void set_aug_step_complete(bool value);

  std::string get_aug_message();
  void set_aug_message(std::string message);

  bool get_training_step_complete();
  void set_training_step_complete(bool value);

  std::string get_training_message();
  void set_training_message(std::string message);

  std::vector<double> get_spacing();
  void set_spacing(std::vector<double> spacing);

  std::string get_loss_function();
  void set_loss_function(std::string loss_function);

  bool get_tl_net_enabled();
  void set_tl_net_enabled(bool enabled);

  int get_tl_net_ae_epochs();
  void set_tl_net_ae_epochs(int num_epochs);

  int get_tl_net_tf_epochs();
  void set_tl_net_tf_epochs(int num_epochs);

  int get_tl_net_joint_epochs();
  void set_tl_net_joint_epochs(int num_epochs);

  double get_tl_net_alpha();
  void set_tl_net_alpha(double alpha);

  double get_tl_net_a_ae();
  void set_tl_net_a_ae(double a_ae);

  double get_tl_net_c_ae();
  void set_tl_net_c_ae(double c_ae);

  double get_tl_net_a_lat();
  void set_tl_net_a_lat(double a_lat);

  double get_tl_net_c_lat();
  void set_tl_net_c_lat(double c_lat);

  void restore_split_defaults();
  void restore_augmentation_defaults();
  void restore_training_defaults();

  void restore_defaults();

  // constants
  const static std::string DEEPSSM_SAMPLER_GAUSSIAN_C;
  const static std::string DEEPSSM_SAMPLER_MIXTURE_C;
  const static std::string DEEPSSM_SAMPLER_KDE_C;

 private:
  Parameters params_;
  ProjectHandle project_;
};

}  // namespace shapeworks
```


-------------------------------

Updated on 2024-07-14 at 17:07:25 +0000
