---
title: shapeworks::OptimizeParameters

---

# shapeworks::OptimizeParameters





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[OptimizeParameters](../Classes/classshapeworks_1_1OptimizeParameters.md#function-optimizeparameters)**(ProjectHandle project) |
| void | **[save_to_project](../Classes/classshapeworks_1_1OptimizeParameters.md#function-save-to-project)**() |
| std::string | **[get_optimize_output_prefix](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-optimize-output-prefix)**() |
| void | **[set_optimize_output_prefix](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-optimize-output-prefix)**(std::string prefix) |
| bool | **[set_up_optimize](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-up-optimize)**([Optimize](../Classes/classshapeworks_1_1Optimize.md) * optimize) |
| std::vector< int > | **[get_number_of_particles](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-number-of-particles)**() |
| void | **[set_number_of_particles](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-number-of-particles)**(std::vector< int > number_of_particles) |
| double | **[get_initial_relative_weighting](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-initial-relative-weighting)**() |
| void | **[set_initial_relative_weighting](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-initial-relative-weighting)**(double value) |
| double | **[get_relative_weighting](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-relative-weighting)**() |
| void | **[set_relative_weighting](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-relative-weighting)**(double value) |
| double | **[get_starting_regularization](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-starting-regularization)**() |
| void | **[set_starting_regularization](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-starting-regularization)**(double value) |
| double | **[get_ending_regularization](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-ending-regularization)**() |
| void | **[set_ending_regularization](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-ending-regularization)**(double value) |
| int | **[get_iterations_per_split](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-iterations-per-split)**() |
| void | **[set_iterations_per_split](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-iterations-per-split)**(int value) |
| int | **[get_optimization_iterations](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-optimization-iterations)**() |
| void | **[set_optimization_iterations](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-optimization-iterations)**(int value) |
| bool | **[get_use_geodesic_distance](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-use-geodesic-distance)**() |
| void | **[set_use_geodesic_distance](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-use-geodesic-distance)**(bool value) |
| int | **[get_geodesic_cache_multiplier](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-geodesic-cache-multiplier)**() |
| void | **[set_geodesic_cache_multiplier](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-geodesic-cache-multiplier)**(int value) |
| std::vector< bool > | **[get_use_normals](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-use-normals)**() |
| void | **[set_use_normals](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-use-normals)**(std::vector< bool > use_normals) |
| double | **[get_normals_strength](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-normals-strength)**() |
| void | **[set_normals_strength](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-normals-strength)**(double value) |
| bool | **[get_use_procrustes](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-use-procrustes)**() |
| void | **[set_use_procrustes](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-use-procrustes)**(bool value) |
| bool | **[get_use_procrustes_scaling](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-use-procrustes-scaling)**() |
| void | **[set_use_procrustes_scaling](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-use-procrustes-scaling)**(bool value) |
| int | **[get_procrustes_interval](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-procrustes-interval)**() |
| void | **[set_procrustes_interval](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-procrustes-interval)**(int value) |
| bool | **[get_use_multiscale](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-use-multiscale)**() |
| void | **[set_use_multiscale](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-use-multiscale)**(bool value) |
| int | **[get_multiscale_particles](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-multiscale-particles)**() |
| void | **[set_multiscale_particles](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-multiscale-particles)**(int value) |
| double | **[get_narrow_band](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-narrow-band)**() |
| void | **[set_narrow_band](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-narrow-band)**(double value) |
| int | **[get_verbosity](../Classes/classshapeworks_1_1OptimizeParameters.md#function-get-verbosity)**() |
| void | **[set_verbosity](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-verbosity)**(int value) |
| void | **[set_abort_load](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-abort-load)**(bool value) |
| void | **[set_load_callback](../Classes/classshapeworks_1_1OptimizeParameters.md#function-set-load-callback)**(const std::function< void(int)> & f) |

## Public Functions Documentation

### function OptimizeParameters

```cpp
explicit OptimizeParameters(
    ProjectHandle project
)
```


### function save_to_project

```cpp
void save_to_project()
```


### function get_optimize_output_prefix

```cpp
std::string get_optimize_output_prefix()
```


### function set_optimize_output_prefix

```cpp
void set_optimize_output_prefix(
    std::string prefix
)
```


### function set_up_optimize

```cpp
bool set_up_optimize(
    Optimize * optimize
)
```


### function get_number_of_particles

```cpp
std::vector< int > get_number_of_particles()
```


### function set_number_of_particles

```cpp
void set_number_of_particles(
    std::vector< int > number_of_particles
)
```


### function get_initial_relative_weighting

```cpp
double get_initial_relative_weighting()
```


### function set_initial_relative_weighting

```cpp
void set_initial_relative_weighting(
    double value
)
```


### function get_relative_weighting

```cpp
double get_relative_weighting()
```


### function set_relative_weighting

```cpp
void set_relative_weighting(
    double value
)
```


### function get_starting_regularization

```cpp
double get_starting_regularization()
```


### function set_starting_regularization

```cpp
void set_starting_regularization(
    double value
)
```


### function get_ending_regularization

```cpp
double get_ending_regularization()
```


### function set_ending_regularization

```cpp
void set_ending_regularization(
    double value
)
```


### function get_iterations_per_split

```cpp
int get_iterations_per_split()
```


### function set_iterations_per_split

```cpp
void set_iterations_per_split(
    int value
)
```


### function get_optimization_iterations

```cpp
int get_optimization_iterations()
```


### function set_optimization_iterations

```cpp
void set_optimization_iterations(
    int value
)
```


### function get_use_geodesic_distance

```cpp
bool get_use_geodesic_distance()
```


### function set_use_geodesic_distance

```cpp
void set_use_geodesic_distance(
    bool value
)
```


### function get_geodesic_cache_multiplier

```cpp
int get_geodesic_cache_multiplier()
```


### function set_geodesic_cache_multiplier

```cpp
void set_geodesic_cache_multiplier(
    int value
)
```


### function get_use_normals

```cpp
std::vector< bool > get_use_normals()
```


### function set_use_normals

```cpp
void set_use_normals(
    std::vector< bool > use_normals
)
```


### function get_normals_strength

```cpp
double get_normals_strength()
```


### function set_normals_strength

```cpp
void set_normals_strength(
    double value
)
```


### function get_use_procrustes

```cpp
bool get_use_procrustes()
```


### function set_use_procrustes

```cpp
void set_use_procrustes(
    bool value
)
```


### function get_use_procrustes_scaling

```cpp
bool get_use_procrustes_scaling()
```


### function set_use_procrustes_scaling

```cpp
void set_use_procrustes_scaling(
    bool value
)
```


### function get_procrustes_interval

```cpp
int get_procrustes_interval()
```


### function set_procrustes_interval

```cpp
void set_procrustes_interval(
    int value
)
```


### function get_use_multiscale

```cpp
bool get_use_multiscale()
```


### function set_use_multiscale

```cpp
void set_use_multiscale(
    bool value
)
```


### function get_multiscale_particles

```cpp
int get_multiscale_particles()
```


### function set_multiscale_particles

```cpp
void set_multiscale_particles(
    int value
)
```


### function get_narrow_band

```cpp
double get_narrow_band()
```


### function set_narrow_band

```cpp
void set_narrow_band(
    double value
)
```


### function get_verbosity

```cpp
int get_verbosity()
```


### function set_verbosity

```cpp
void set_verbosity(
    int value
)
```


### function set_abort_load

```cpp
void set_abort_load(
    bool value
)
```


### function set_load_callback

```cpp
void set_load_callback(
    const std::function< void(int)> & f
)
```


-------------------------------

Updated on 2022-01-01 at 17:59:58 +0000