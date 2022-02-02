---
title: shapeworks::MultiVariateNormalRandom

---

# shapeworks::MultiVariateNormalRandom





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MultiVariateNormalRandom](../Classes/structshapeworks_1_1MultiVariateNormalRandom.md#function-multivariatenormalrandom)**(Eigen::MatrixXd const & covar) |
| | **[MultiVariateNormalRandom](../Classes/structshapeworks_1_1MultiVariateNormalRandom.md#function-multivariatenormalrandom)**(Eigen::VectorXd const & mean, Eigen::MatrixXd const & covar) |
| Eigen::MatrixXd | **[operator()](../Classes/structshapeworks_1_1MultiVariateNormalRandom.md#function-operator())**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| Eigen::VectorXd | **[mean](../Classes/structshapeworks_1_1MultiVariateNormalRandom.md#variable-mean)**  |
| Eigen::MatrixXd | **[transform](../Classes/structshapeworks_1_1MultiVariateNormalRandom.md#variable-transform)**  |
| std::mt19937 | **[gen](../Classes/structshapeworks_1_1MultiVariateNormalRandom.md#variable-gen)**  |
| std::normal_distribution | **[dist](../Classes/structshapeworks_1_1MultiVariateNormalRandom.md#variable-dist)**  |

## Public Functions Documentation

### function MultiVariateNormalRandom

```cpp
inline MultiVariateNormalRandom(
    Eigen::MatrixXd const & covar
)
```


### function MultiVariateNormalRandom

```cpp
inline MultiVariateNormalRandom(
    Eigen::VectorXd const & mean,
    Eigen::MatrixXd const & covar
)
```


### function operator()

```cpp
inline Eigen::MatrixXd operator()()
```


## Public Attributes Documentation

### variable mean

```cpp
Eigen::VectorXd mean;
```


### variable transform

```cpp
Eigen::MatrixXd transform;
```


### variable gen

```cpp
std::mt19937 gen {42};
```


### variable dist

```cpp
std::normal_distribution dist;
```


-------------------------------

Updated on 2022-02-02 at 18:18:09 +0000