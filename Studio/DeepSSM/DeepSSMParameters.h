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

  std::string get_prep_message();
  void set_prep_message(std::string message);

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
