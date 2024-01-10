#include "DeepSSMParameters.h"

namespace shapeworks {

const std::string DeepSSMParameters::DEEPSSM_SAMPLER_GAUSSIAN_C("Gaussian");
const std::string DeepSSMParameters::DEEPSSM_SAMPLER_MIXTURE_C("Mixture");
const std::string DeepSSMParameters::DEEPSSM_SAMPLER_KDE_C("KDE");

namespace Keys {
const std::string TRAINING_SPLIT = "training_split";
const std::string VALIDATION_SPLIT = "validation_split";
const std::string TESTING_SPLIT = "testing_split";

const std::string AUG_NUM_SAMPLES = "aug_num_samples";
const std::string AUG_NUM_DIMS = "aug_num_dims";
const std::string AUG_PERCENT_VARIABILITY = "aug_percent_variability";
const std::string AUG_SAMPLER_TYPE = "aug_sampler_type";

const std::string TRAIN_EPOCHS = "train_epochs";
const std::string TRAIN_LEARNING_RATE = "train_learning_rate";
const std::string TRAIN_DECAY_LEARNING_RATE = "train_decay_learning_rate";
const std::string TRAIN_FINE_TUNING = "train_fine_tuning";
const std::string TRAIN_NUM_DIMS = "train_num_dims";
const std::string TRAIN_BATCH_SIZE = "train_batch_size";
const std::string TRAIN_FINE_TUNING_EPOCHS = "train_fine_tuning_epochs";
const std::string TRAIN_FINE_TUNING_LEARNING_RATE = "train_fine_tuning_learning_rate";

const std::string PREP_STEP_COMPLETE = "prep_step_complete";
const std::string PREP_STEP_MESSAGE = "prep_step_message";
const std::string AUG_STEP_COMPLETE = "aug_step_complete";
const std::string AUG_STEP_MESSAGE = "aug_step_message";
const std::string TRAIN_STEP_COMPLETE = "train_step_complete";
const std::string TRAIN_STEP_MESSAGE = "train_step_message";

}  // namespace Keys

namespace Defaults {

// split defaults
const double TRAINING_SPLIT = 60;
const double VALIDATION_SPLIT = 20;
const double TESTING_SPLIT = 20;

// augmentation defaults
const int AUG_NUM_SAMPLES = 3;
const int AUG_NUM_DIMS = 3;
double AUG_PERCENT_VARIABILITY = 0.95;
const std::string AUG_SAMPLER_TYPE = DeepSSMParameters::DEEPSSM_SAMPLER_GAUSSIAN_C;

// training defaults
const int TRAIN_EPOCHS = 100;
const double TRAIN_LEARNING_RATE = 0.001;
const bool TRAIN_DECAY_LEARNING_RATE = true;
const bool TRAIN_FINE_TUNING = true;
const int TRAIN_NUM_DIMS = 0;
const int TRAIN_BATCH_SIZE = 8;
const int TRAIN_FINE_TUNING_EPOCHS = 2;
const double TRAIN_FINE_TUNING_LEARNING_RATE = 0.001;

// stage defaults
const bool PREP_STEP_COMPLETE = false;
const std::string PREP_STEP_MESSAGE = "";
const bool AUG_STEP_COMPLETE = false;
const std::string AUG_STEP_MESSAGE = "";
const bool TRAIN_STEP_COMPLETE = false;
const std::string TRAIN_STEP_MESSAGE = "";

}  // namespace Defaults

//---------------------------------------------------------------------------
DeepSSMParameters::DeepSSMParameters(ProjectHandle project) : project_(project) {
  params_ = project_->get_parameters(Parameters::DEEPSSM_PARAMS);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::restore_defaults() { params_.reset_parameters(); }

//---------------------------------------------------------------------------
void DeepSSMParameters::save_to_project() { project_->set_parameters(Parameters::DEEPSSM_PARAMS, params_); }

//---------------------------------------------------------------------------
int DeepSSMParameters::get_aug_num_samples() { return params_.get(Keys::AUG_NUM_SAMPLES, Defaults::AUG_NUM_SAMPLES); }

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_num_samples(int num_samples) { params_.set(Keys::AUG_NUM_SAMPLES, num_samples); }

//---------------------------------------------------------------------------
int DeepSSMParameters::get_aug_num_dims() { return params_.get(Keys::AUG_NUM_DIMS, Defaults::AUG_NUM_DIMS); }

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_num_dims(int num_dims) { params_.set(Keys::AUG_NUM_DIMS, num_dims); }

//---------------------------------------------------------------------------
int DeepSSMParameters::get_training_num_dims() { return params_.get(Keys::TRAIN_NUM_DIMS, Defaults::TRAIN_NUM_DIMS); }

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_num_dims(int num_dims) { params_.set(Keys::TRAIN_NUM_DIMS, num_dims); }

//---------------------------------------------------------------------------
double DeepSSMParameters::get_aug_percent_variability() {
  return params_.get(Keys::AUG_PERCENT_VARIABILITY, Defaults::AUG_PERCENT_VARIABILITY);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_percent_variability(double percent_variability) {
  params_.set(Keys::AUG_PERCENT_VARIABILITY, percent_variability);
}

//---------------------------------------------------------------------------
std::string DeepSSMParameters::get_aug_sampler_type() {
  return params_.get(Keys::AUG_SAMPLER_TYPE, Defaults::AUG_SAMPLER_TYPE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_sampler_type(std::string sampler_type) {
  params_.set(Keys::AUG_SAMPLER_TYPE, sampler_type);
}

//---------------------------------------------------------------------------
int DeepSSMParameters::get_training_epochs() { return params_.get(Keys::TRAIN_EPOCHS, Defaults::TRAIN_EPOCHS); }

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_epochs(int epochs) { params_.set(Keys::TRAIN_EPOCHS, epochs); }

//---------------------------------------------------------------------------
double DeepSSMParameters::get_training_learning_rate() {
  return params_.get(Keys::TRAIN_LEARNING_RATE, Defaults::TRAIN_LEARNING_RATE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_learning_rate(double rate) { params_.set(Keys::TRAIN_LEARNING_RATE, rate); }

//---------------------------------------------------------------------------
bool DeepSSMParameters::get_training_decay_learning_rate() {
  return params_.get(Keys::TRAIN_DECAY_LEARNING_RATE, Defaults::TRAIN_DECAY_LEARNING_RATE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_decay_learning_rate(bool decay) {
  params_.set(Keys::TRAIN_DECAY_LEARNING_RATE, decay);
}

//---------------------------------------------------------------------------
bool DeepSSMParameters::get_training_fine_tuning() {
  return params_.get(Keys::TRAIN_FINE_TUNING, Defaults::TRAIN_FINE_TUNING);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_fine_tuning(bool fine_tuning) {
  params_.set(Keys::TRAIN_FINE_TUNING, fine_tuning);
}

//---------------------------------------------------------------------------
int DeepSSMParameters::get_training_fine_tuning_epochs() {
  return params_.get(Keys::TRAIN_FINE_TUNING_EPOCHS, Defaults::TRAIN_FINE_TUNING_EPOCHS);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_fine_tuning_epochs(int epochs) {
  params_.set(Keys::TRAIN_FINE_TUNING_EPOCHS, epochs);
}

//---------------------------------------------------------------------------
double DeepSSMParameters::get_training_fine_tuning_learning_rate() {
  return params_.get(Keys::TRAIN_FINE_TUNING_LEARNING_RATE, Defaults::TRAIN_FINE_TUNING_LEARNING_RATE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_fine_tuning_learning_rate(double rate) {
  params_.set(Keys::TRAIN_FINE_TUNING_LEARNING_RATE, rate);
}

//---------------------------------------------------------------------------
int DeepSSMParameters::get_training_batch_size() {
  return params_.get(Keys::TRAIN_BATCH_SIZE, Defaults::TRAIN_BATCH_SIZE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_batch_size(int batch_size) { params_.set(Keys::TRAIN_BATCH_SIZE, batch_size); }

//---------------------------------------------------------------------------
double DeepSSMParameters::get_training_split() { return params_.get(Keys::TRAINING_SPLIT, Defaults::TRAINING_SPLIT); }

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_split(double value) { params_.set(Keys::TRAINING_SPLIT, value); }

//---------------------------------------------------------------------------
double DeepSSMParameters::get_validation_split() {
  return params_.get(Keys::VALIDATION_SPLIT, Defaults::VALIDATION_SPLIT);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_validation_split(double value) { params_.set(Keys::VALIDATION_SPLIT, value); }

//---------------------------------------------------------------------------
double DeepSSMParameters::get_testing_split() { return params_.get(Keys::TESTING_SPLIT, Defaults::TESTING_SPLIT); }

//---------------------------------------------------------------------------
void DeepSSMParameters::set_testing_split(double value) { params_.set(Keys::TESTING_SPLIT, value); }

//---------------------------------------------------------------------------
bool DeepSSMParameters::get_prep_step_complete() {
  return params_.get(Keys::PREP_STEP_COMPLETE, Defaults::PREP_STEP_COMPLETE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_prep_step_complete(bool value) { params_.set(Keys::PREP_STEP_COMPLETE, value); }

//---------------------------------------------------------------------------
std::string DeepSSMParameters::get_prep_message() {
  return params_.get(Keys::PREP_STEP_MESSAGE, Defaults::PREP_STEP_MESSAGE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_prep_message(std::string message) { params_.set(Keys::PREP_STEP_MESSAGE, message); }

//---------------------------------------------------------------------------
bool DeepSSMParameters::get_aug_step_complete() {
  return params_.get(Keys::AUG_STEP_COMPLETE, Defaults::AUG_STEP_COMPLETE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_step_complete(bool value) { params_.set(Keys::AUG_STEP_COMPLETE, value); }

//---------------------------------------------------------------------------
std::string DeepSSMParameters::get_aug_message() {
  return params_.get(Keys::AUG_STEP_MESSAGE, Defaults::AUG_STEP_MESSAGE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_aug_message(std::string message) { params_.set(Keys::AUG_STEP_MESSAGE, message); }

//---------------------------------------------------------------------------
bool DeepSSMParameters::get_training_step_complete() {
  return params_.get(Keys::TRAIN_STEP_COMPLETE, Defaults::TRAIN_STEP_COMPLETE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_step_complete(bool value) { params_.set(Keys::TRAIN_STEP_COMPLETE, value); }

//---------------------------------------------------------------------------
std::string DeepSSMParameters::get_training_message() {
  return params_.get(Keys::TRAIN_STEP_MESSAGE, Defaults::TRAIN_STEP_MESSAGE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::set_training_message(std::string message) { params_.set(Keys::TRAIN_STEP_MESSAGE, message); }

//---------------------------------------------------------------------------
void DeepSSMParameters::restore_split_defaults() {
  params_.remove_entry(Keys::TRAINING_SPLIT);
  params_.remove_entry(Keys::VALIDATION_SPLIT);
  params_.remove_entry(Keys::TESTING_SPLIT);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::restore_augmentation_defaults() {
  params_.remove_entry(Keys::AUG_NUM_SAMPLES);
  params_.remove_entry(Keys::AUG_NUM_DIMS);
  params_.remove_entry(Keys::AUG_PERCENT_VARIABILITY);
  params_.remove_entry(Keys::AUG_SAMPLER_TYPE);
}

//---------------------------------------------------------------------------
void DeepSSMParameters::restore_training_defaults() {
  params_.remove_entry(Keys::TRAIN_EPOCHS);
  params_.remove_entry(Keys::TRAIN_LEARNING_RATE);
  params_.remove_entry(Keys::TRAIN_DECAY_LEARNING_RATE);
  params_.remove_entry(Keys::TRAIN_FINE_TUNING);
  params_.remove_entry(Keys::TRAIN_FINE_TUNING_EPOCHS);
  params_.remove_entry(Keys::TRAIN_BATCH_SIZE);
  params_.remove_entry(Keys::TRAIN_FINE_TUNING_LEARNING_RATE);
}
}  // namespace shapeworks
