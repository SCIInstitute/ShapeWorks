---
title: shapeworks::DeepSSMParameters

---

# shapeworks::DeepSSMParameters





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[DeepSSMParameters](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-deepssmparameters)**(ProjectHandle project) |
| void | **[save_to_project](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-save-to-project)**() |
| int | **[get_aug_num_samples](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-aug-num-samples)**() |
| void | **[set_aug_num_samples](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-aug-num-samples)**(int num_samples) |
| int | **[get_aug_num_dims](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-aug-num-dims)**() |
| void | **[set_aug_num_dims](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-aug-num-dims)**(int num_dims) |
| int | **[get_training_num_dims](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-training-num-dims)**() |
| void | **[set_training_num_dims](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-training-num-dims)**(int num_dims) |
| double | **[get_aug_percent_variability](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-aug-percent-variability)**() |
| void | **[set_aug_percent_variability](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-aug-percent-variability)**(double percent_variability) |
| std::string | **[get_aug_sampler_type](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-aug-sampler-type)**() |
| void | **[set_aug_sampler_type](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-aug-sampler-type)**(std::string sampler_type) |
| int | **[get_training_epochs](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-training-epochs)**() |
| void | **[set_training_epochs](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-training-epochs)**(int epochs) |
| double | **[get_training_learning_rate](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-training-learning-rate)**() |
| void | **[set_training_learning_rate](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-training-learning-rate)**(double rate) |
| bool | **[get_training_decay_learning_rate](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-training-decay-learning-rate)**() |
| void | **[set_training_decay_learning_rate](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-training-decay-learning-rate)**(bool decay) |
| bool | **[get_training_fine_tuning](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-training-fine-tuning)**() |
| void | **[set_training_fine_tuning](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-training-fine-tuning)**(bool fine_tuning) |
| int | **[get_training_fine_tuning_epochs](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-training-fine-tuning-epochs)**() |
| void | **[set_training_fine_tuning_epochs](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-training-fine-tuning-epochs)**(int epochs) |
| double | **[get_training_fine_tuning_learning_rate](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-training-fine-tuning-learning-rate)**() |
| void | **[set_training_fine_tuning_learning_rate](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-training-fine-tuning-learning-rate)**(double rate) |
| int | **[get_training_batch_size](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-training-batch-size)**() |
| void | **[set_training_batch_size](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-training-batch-size)**(int batch_size) |
| double | **[get_training_split](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-training-split)**() |
| void | **[set_training_split](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-training-split)**(double value) |
| double | **[get_validation_split](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-validation-split)**() |
| void | **[set_validation_split](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-validation-split)**(double value) |
| double | **[get_testing_split](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-get-testing-split)**() |
| void | **[set_testing_split](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-set-testing-split)**(double value) |
| void | **[restore_split_defaults](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-restore-split-defaults)**() |
| void | **[restore_augmentation_defaults](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-restore-augmentation-defaults)**() |
| void | **[restore_training_defaults](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-restore-training-defaults)**() |
| void | **[restore_defaults](../Classes/classshapeworks_1_1DeepSSMParameters.md#function-restore-defaults)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const std::string | **[DEEPSSM_SAMPLER_GAUSSIAN_C](../Classes/classshapeworks_1_1DeepSSMParameters.md#variable-deepssm-sampler-gaussian-c)**  |
| const std::string | **[DEEPSSM_SAMPLER_MIXTURE_C](../Classes/classshapeworks_1_1DeepSSMParameters.md#variable-deepssm-sampler-mixture-c)**  |
| const std::string | **[DEEPSSM_SAMPLER_KDE_C](../Classes/classshapeworks_1_1DeepSSMParameters.md#variable-deepssm-sampler-kde-c)**  |

## Public Functions Documentation

### function DeepSSMParameters

```cpp
explicit DeepSSMParameters(
    ProjectHandle project
)
```


### function save_to_project

```cpp
void save_to_project()
```


### function get_aug_num_samples

```cpp
int get_aug_num_samples()
```


### function set_aug_num_samples

```cpp
void set_aug_num_samples(
    int num_samples
)
```


### function get_aug_num_dims

```cpp
int get_aug_num_dims()
```


### function set_aug_num_dims

```cpp
void set_aug_num_dims(
    int num_dims
)
```


### function get_training_num_dims

```cpp
int get_training_num_dims()
```


### function set_training_num_dims

```cpp
void set_training_num_dims(
    int num_dims
)
```


### function get_aug_percent_variability

```cpp
double get_aug_percent_variability()
```


### function set_aug_percent_variability

```cpp
void set_aug_percent_variability(
    double percent_variability
)
```


### function get_aug_sampler_type

```cpp
std::string get_aug_sampler_type()
```


### function set_aug_sampler_type

```cpp
void set_aug_sampler_type(
    std::string sampler_type
)
```


### function get_training_epochs

```cpp
int get_training_epochs()
```


### function set_training_epochs

```cpp
void set_training_epochs(
    int epochs
)
```


### function get_training_learning_rate

```cpp
double get_training_learning_rate()
```


### function set_training_learning_rate

```cpp
void set_training_learning_rate(
    double rate
)
```


### function get_training_decay_learning_rate

```cpp
bool get_training_decay_learning_rate()
```


### function set_training_decay_learning_rate

```cpp
void set_training_decay_learning_rate(
    bool decay
)
```


### function get_training_fine_tuning

```cpp
bool get_training_fine_tuning()
```


### function set_training_fine_tuning

```cpp
void set_training_fine_tuning(
    bool fine_tuning
)
```


### function get_training_fine_tuning_epochs

```cpp
int get_training_fine_tuning_epochs()
```


### function set_training_fine_tuning_epochs

```cpp
void set_training_fine_tuning_epochs(
    int epochs
)
```


### function get_training_fine_tuning_learning_rate

```cpp
double get_training_fine_tuning_learning_rate()
```


### function set_training_fine_tuning_learning_rate

```cpp
void set_training_fine_tuning_learning_rate(
    double rate
)
```


### function get_training_batch_size

```cpp
int get_training_batch_size()
```


### function set_training_batch_size

```cpp
void set_training_batch_size(
    int batch_size
)
```


### function get_training_split

```cpp
double get_training_split()
```


### function set_training_split

```cpp
void set_training_split(
    double value
)
```


### function get_validation_split

```cpp
double get_validation_split()
```


### function set_validation_split

```cpp
void set_validation_split(
    double value
)
```


### function get_testing_split

```cpp
double get_testing_split()
```


### function set_testing_split

```cpp
void set_testing_split(
    double value
)
```


### function restore_split_defaults

```cpp
void restore_split_defaults()
```


### function restore_augmentation_defaults

```cpp
void restore_augmentation_defaults()
```


### function restore_training_defaults

```cpp
void restore_training_defaults()
```


### function restore_defaults

```cpp
void restore_defaults()
```


## Public Attributes Documentation

### variable DEEPSSM_SAMPLER_GAUSSIAN_C

```cpp
static const std::string DEEPSSM_SAMPLER_GAUSSIAN_C;
```


### variable DEEPSSM_SAMPLER_MIXTURE_C

```cpp
static const std::string DEEPSSM_SAMPLER_MIXTURE_C;
```


### variable DEEPSSM_SAMPLER_KDE_C

```cpp
static const std::string DEEPSSM_SAMPLER_KDE_C;
```


-------------------------------

Updated on 2022-01-01 at 17:59:58 +0000