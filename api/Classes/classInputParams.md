---
title: InputParams

---

# InputParams





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[InputParams](../Classes/classInputParams.md#function-inputparams)**() |
| int | **[readParams](../Classes/classInputParams.md#function-readparams)**(char * infilename, int mode) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[number_of_particles](../Classes/classInputParams.md#variable-number-of-particles)**  |
| bool | **[do_procrustes](../Classes/classInputParams.md#variable-do-procrustes)**  |
| bool | **[do_procrustes_scaling](../Classes/classInputParams.md#variable-do-procrustes-scaling)**  |
| const float | **[pi](../Classes/classInputParams.md#variable-pi)**  |
| std::string | **[out_prefix](../Classes/classInputParams.md#variable-out-prefix)**  |
| std::string | **[out_path](../Classes/classInputParams.md#variable-out-path)**  |
| std::string | **[mean_prefix](../Classes/classInputParams.md#variable-mean-prefix)**  |
| std::string | **[mean_path](../Classes/classInputParams.md#variable-mean-path)**  |
| std::string | **[template_dense_mesh](../Classes/classInputParams.md#variable-template-dense-mesh)**  |
| std::string | **[template_sparse_points](../Classes/classInputParams.md#variable-template-sparse-points)**  |
| std::string | **[template_good_indices](../Classes/classInputParams.md#variable-template-good-indices)**  |
| bool | **[use_template_mesh](../Classes/classInputParams.md#variable-use-template-mesh)**  |
| bool | **[qcFixWinding](../Classes/classInputParams.md#variable-qcfixwinding)**  |
| bool | **[qcDoLaplacianSmoothingBeforeDecimation](../Classes/classInputParams.md#variable-qcdolaplaciansmoothingbeforedecimation)**  |
| bool | **[qcDoLaplacianSmoothingAfterDecimation](../Classes/classInputParams.md#variable-qcdolaplaciansmoothingafterdecimation)**  |
| float | **[qcSmoothingLambda](../Classes/classInputParams.md#variable-qcsmoothinglambda)**  |
| int | **[qcSmoothingIterations](../Classes/classInputParams.md#variable-qcsmoothingiterations)**  |
| float | **[qcDecimationPercentage](../Classes/classInputParams.md#variable-qcdecimationpercentage)**  |
| float | **[K](../Classes/classInputParams.md#variable-k)**  |
| int | **[number_of_correspondences_in_warp](../Classes/classInputParams.md#variable-number-of-correspondences-in-warp)**  |
| int | **[number_of_attributes](../Classes/classInputParams.md#variable-number-of-attributes)**  |
| std::string | **[attributes_prefix](../Classes/classInputParams.md#variable-attributes-prefix)**  |
| std::vector< std::string > | **[attribute_labels](../Classes/classInputParams.md#variable-attribute-labels)**  |
| float | **[levelsetValue](../Classes/classInputParams.md#variable-levelsetvalue)**  |
| float | **[targetReduction](../Classes/classInputParams.md#variable-targetreduction)**  |
| float | **[featureAngle](../Classes/classInputParams.md#variable-featureangle)**  |
| int | **[lsSmootherIterations](../Classes/classInputParams.md#variable-lssmootheriterations)**  |
| int | **[meshSmootherIterations](../Classes/classInputParams.md#variable-meshsmootheriterations)**  |
| bool | **[preserveTopology](../Classes/classInputParams.md#variable-preservetopology)**  |
| bool | **[display](../Classes/classInputParams.md#variable-display)**  |
| float | **[glyph_radius](../Classes/classInputParams.md#variable-glyph-radius)**  |
| float | **[maximum_variance_captured](../Classes/classInputParams.md#variable-maximum-variance-captured)**  |
| int | **[mode_index](../Classes/classInputParams.md#variable-mode-index)**  |
| int | **[number_of_modes](../Classes/classInputParams.md#variable-number-of-modes)**  |
| float | **[maximum_std_dev](../Classes/classInputParams.md#variable-maximum-std-dev)**  |
| int | **[number_of_samples_per_mode](../Classes/classInputParams.md#variable-number-of-samples-per-mode)**  |
| float | **[normalAngle](../Classes/classInputParams.md#variable-normalangle)**  |
| bool | **[usePairwiseNormalsDifferencesForGoodBad](../Classes/classInputParams.md#variable-usepairwisenormalsdifferencesforgoodbad)**  |
| bool | **[use_tps_transform](../Classes/classInputParams.md#variable-use-tps-transform)**  |
| bool | **[use_bspline_interpolation](../Classes/classInputParams.md#variable-use-bspline-interpolation)**  |
| std::vector< std::string > | **[localPointsFilenames](../Classes/classInputParams.md#variable-localpointsfilenames)**  |
| std::vector< std::string > | **[worldPointsFilenames](../Classes/classInputParams.md#variable-worldpointsfilenames)**  |
| std::vector< std::string > | **[distanceTransformFilenames](../Classes/classInputParams.md#variable-distancetransformfilenames)**  |
| std::vector< std::string > | **[attributeFilenames](../Classes/classInputParams.md#variable-attributefilenames)**  |

## Public Functions Documentation

### function InputParams

```cpp
inline InputParams()
```


### function readParams

```cpp
inline int readParams(
    char * infilename,
    int mode
)
```


## Public Attributes Documentation

### variable number_of_particles

```cpp
int number_of_particles;
```


### variable do_procrustes

```cpp
bool do_procrustes;
```


### variable do_procrustes_scaling

```cpp
bool do_procrustes_scaling;
```


### variable pi

```cpp
const float pi = std::acos(-1.0);
```


### variable out_prefix

```cpp
std::string out_prefix;
```


### variable out_path

```cpp
std::string out_path;
```


### variable mean_prefix

```cpp
std::string mean_prefix;
```


### variable mean_path

```cpp
std::string mean_path;
```


### variable template_dense_mesh

```cpp
std::string template_dense_mesh;
```


### variable template_sparse_points

```cpp
std::string template_sparse_points;
```


### variable template_good_indices

```cpp
std::string template_good_indices;
```


### variable use_template_mesh

```cpp
bool use_template_mesh;
```


### variable qcFixWinding

```cpp
bool qcFixWinding;
```


### variable qcDoLaplacianSmoothingBeforeDecimation

```cpp
bool qcDoLaplacianSmoothingBeforeDecimation;
```


### variable qcDoLaplacianSmoothingAfterDecimation

```cpp
bool qcDoLaplacianSmoothingAfterDecimation;
```


### variable qcSmoothingLambda

```cpp
float qcSmoothingLambda;
```


### variable qcSmoothingIterations

```cpp
int qcSmoothingIterations;
```


### variable qcDecimationPercentage

```cpp
float qcDecimationPercentage;
```


### variable K

```cpp
float K;
```


### variable number_of_correspondences_in_warp

```cpp
int number_of_correspondences_in_warp;
```


### variable number_of_attributes

```cpp
int number_of_attributes;
```


### variable attributes_prefix

```cpp
std::string attributes_prefix;
```


### variable attribute_labels

```cpp
std::vector< std::string > attribute_labels;
```


### variable levelsetValue

```cpp
float levelsetValue;
```


### variable targetReduction

```cpp
float targetReduction;
```


### variable featureAngle

```cpp
float featureAngle;
```


### variable lsSmootherIterations

```cpp
int lsSmootherIterations;
```


### variable meshSmootherIterations

```cpp
int meshSmootherIterations;
```


### variable preserveTopology

```cpp
bool preserveTopology;
```


### variable display

```cpp
bool display;
```


### variable glyph_radius

```cpp
float glyph_radius;
```


### variable maximum_variance_captured

```cpp
float maximum_variance_captured;
```


### variable mode_index

```cpp
int mode_index;
```


### variable number_of_modes

```cpp
int number_of_modes;
```


### variable maximum_std_dev

```cpp
float maximum_std_dev;
```


### variable number_of_samples_per_mode

```cpp
int number_of_samples_per_mode;
```


### variable normalAngle

```cpp
float normalAngle;
```


### variable usePairwiseNormalsDifferencesForGoodBad

```cpp
bool usePairwiseNormalsDifferencesForGoodBad;
```


### variable use_tps_transform

```cpp
bool use_tps_transform;
```


### variable use_bspline_interpolation

```cpp
bool use_bspline_interpolation;
```


### variable localPointsFilenames

```cpp
std::vector< std::string > localPointsFilenames;
```


### variable worldPointsFilenames

```cpp
std::vector< std::string > worldPointsFilenames;
```


### variable distanceTransformFilenames

```cpp
std::vector< std::string > distanceTransformFilenames;
```


### variable attributeFilenames

```cpp
std::vector< std::string > attributeFilenames;
```


-------------------------------

Updated on 2022-01-07 at 00:54:39 +0000