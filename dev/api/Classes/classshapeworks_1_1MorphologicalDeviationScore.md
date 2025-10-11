---
title: shapeworks::MorphologicalDeviationScore

---

# shapeworks::MorphologicalDeviationScore





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MorphologicalDeviationScore](../Classes/classshapeworks_1_1MorphologicalDeviationScore.md#function-morphologicaldeviationscore)**() |
| bool | **[SetControlShapes](../Classes/classshapeworks_1_1MorphologicalDeviationScore.md#function-setcontrolshapes)**(const Eigen::MatrixXd & X) |
| Eigen::VectorXd | **[GetMahalanobisDistance](../Classes/classshapeworks_1_1MorphologicalDeviationScore.md#function-getmahalanobisdistance)**(const Eigen::MatrixXd & X)<br>Get Mahalanobis-based deviation score for test samples (non-fixed shapes/domains)  |

## Public Functions Documentation

### function MorphologicalDeviationScore

```cpp
MorphologicalDeviationScore()
```


### function SetControlShapes

```cpp
bool SetControlShapes(
    const Eigen::MatrixXd & X
)
```


### function GetMahalanobisDistance

```cpp
Eigen::VectorXd GetMahalanobisDistance(
    const Eigen::MatrixXd & X
)
```

Get Mahalanobis-based deviation score for test samples (non-fixed shapes/domains) 

-------------------------------

Updated on 2025-10-11 at 18:16:33 +0000