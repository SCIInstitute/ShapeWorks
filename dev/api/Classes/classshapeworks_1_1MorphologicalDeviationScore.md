---
title: shapeworks::MorphologicalDeviationScore

---

# shapeworks::MorphologicalDeviationScore





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MorphologicalDeviationScore](../Classes/classshapeworks_1_1MorphologicalDeviationScore.md#function-morphologicaldeviationscore)**() |
| bool | **[SetControlShapes](../Classes/classshapeworks_1_1MorphologicalDeviationScore.md#function-setcontrolshapes)**(const Eigen::MatrixXd & X) |
| Eigen::VectorXd | **[GetMorphoDevScore](../Classes/classshapeworks_1_1MorphologicalDeviationScore.md#function-getmorphodevscore)**(const Eigen::MatrixXd & X) |

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


### function GetMorphoDevScore

```cpp
Eigen::VectorXd GetMorphoDevScore(
    const Eigen::MatrixXd & X
)
```


Get Mahalanobis-based deviation score for test samples (non-fixed shapes/domains) 


-------------------------------

Updated on 2025-10-13 at 17:18:11 +0000