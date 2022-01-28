---
title: Procrustes3D

---

# Procrustes3D





## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef double | **[RealType](../Classes/classProcrustes3D.md#typedef-realtype)**  |
| typedef vnl_vector_fixed< double, 3 > | **[PointType](../Classes/classProcrustes3D.md#typedef-pointtype)**  |
| typedef std::vector< PointType > | **[ShapeType](../Classes/classProcrustes3D.md#typedef-shapetype)**  |
| typedef ShapeType::iterator | **[ShapeIteratorType](../Classes/classProcrustes3D.md#typedef-shapeiteratortype)**  |
| typedef std::vector< ShapeType > | **[ShapeListType](../Classes/classProcrustes3D.md#typedef-shapelisttype)**  |
| typedef ShapeListType::iterator | **[ShapeListIteratorType](../Classes/classProcrustes3D.md#typedef-shapelistiteratortype)**  |
| typedef std::vector< [SimilarityTransform3D](../Classes/structSimilarityTransform3D.md) > | **[SimilarityTransformListType](../Classes/classProcrustes3D.md#typedef-similaritytransformlisttype)**  |
| typedef SimilarityTransformListType::iterator | **[SimilarityTransformListIteratorType](../Classes/classProcrustes3D.md#typedef-similaritytransformlistiteratortype)**  |
| typedef vnl_matrix_fixed< double, 3+1, 3+1 > | **[TransformMatrixType](../Classes/classProcrustes3D.md#typedef-transformmatrixtype)**  |
| typedef std::vector< TransformMatrixType > | **[TransformMatrixListType](../Classes/classProcrustes3D.md#typedef-transformmatrixlisttype)**  |
| typedef TransformMatrixListType::iterator | **[TransformMatrixIteratorType](../Classes/classProcrustes3D.md#typedef-transformmatrixiteratortype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Procrustes3D](../Classes/classProcrustes3D.md#function-procrustes3d)**() |
| | **[Procrustes3D](../Classes/classProcrustes3D.md#function-procrustes3d)**(bool do_scaling, bool do_rotation_translation) |
| bool | **[GetScaling](../Classes/classProcrustes3D.md#function-getscaling)**() const |
| void | **[ScalingOn](../Classes/classProcrustes3D.md#function-scalingon)**() |
| void | **[ScalingOff](../Classes/classProcrustes3D.md#function-scalingoff)**() |
| bool | **[GetRotationTranslation](../Classes/classProcrustes3D.md#function-getrotationtranslation)**() const |
| void | **[RotationTranslationOn](../Classes/classProcrustes3D.md#function-rotationtranslationon)**() |
| void | **[RotationTranslationOff](../Classes/classProcrustes3D.md#function-rotationtranslationoff)**() |
| void | **[AlignShapes](../Classes/classProcrustes3D.md#function-alignshapes)**(SimilarityTransformListType & transforms, ShapeListType & shapes) |
| void | **[RemoveTranslation](../Classes/classProcrustes3D.md#function-removetranslation)**(SimilarityTransformListType & transforms, ShapeListType & shapes) |
| void | **[ConstructTransformMatrices](../Classes/classProcrustes3D.md#function-constructtransformmatrices)**(SimilarityTransformListType & transforms, TransformMatrixListType & transformMatrices) |
| void | **[ConstructTransformMatrix](../Classes/classProcrustes3D.md#function-constructtransformmatrix)**([SimilarityTransform3D](../Classes/structSimilarityTransform3D.md) & transform, TransformMatrixType & transformMatrix) |
| void | **[ComputeMeanShape](../Classes/classProcrustes3D.md#function-computemeanshape)**(ShapeType & mean, ShapeListType & shapeList) |
| void | **[ComputeCenterOfMass](../Classes/classProcrustes3D.md#function-computecenterofmass)**(ShapeType & shape, PointType & center) |
| void | **[CenterShape](../Classes/classProcrustes3D.md#function-centershape)**(ShapeType & shape) |
| void | **[ComputeCommonCenter](../Classes/classProcrustes3D.md#function-computecommoncenter)**(SimilarityTransformListType & transforms, PointType & center) |
| void | **[AlignSourceToTarget](../Classes/classProcrustes3D.md#function-alignsourcetotarget)**([SimilarityTransform3D](../Classes/structSimilarityTransform3D.md) & transform, ShapeType & target, ShapeType & source) |
| int | **[ComputeMedianShape](../Classes/classProcrustes3D.md#function-computemedianshape)**(ShapeListType & shapeList) |
| void | **[TransformShape](../Classes/classProcrustes3D.md#function-transformshape)**(ShapeType & shape, [SimilarityTransform3D](../Classes/structSimilarityTransform3D.md) & transform) |
| void | **[TransformShapes](../Classes/classProcrustes3D.md#function-transformshapes)**(ShapeListType & shapes, SimilarityTransformListType & transforms) |
| RealType | **[ComputeSumOfSquares](../Classes/classProcrustes3D.md#function-computesumofsquares)**(ShapeListType & shapes) |

## Public Types Documentation

### typedef RealType

```cpp
typedef double Procrustes3D::RealType;
```


### typedef PointType

```cpp
typedef vnl_vector_fixed<double, 3> Procrustes3D::PointType;
```


### typedef ShapeType

```cpp
typedef std::vector<PointType> Procrustes3D::ShapeType;
```


### typedef ShapeIteratorType

```cpp
typedef ShapeType::iterator Procrustes3D::ShapeIteratorType;
```


### typedef ShapeListType

```cpp
typedef std::vector<ShapeType> Procrustes3D::ShapeListType;
```


### typedef ShapeListIteratorType

```cpp
typedef ShapeListType::iterator Procrustes3D::ShapeListIteratorType;
```


### typedef SimilarityTransformListType

```cpp
typedef std::vector<SimilarityTransform3D> Procrustes3D::SimilarityTransformListType;
```


### typedef SimilarityTransformListIteratorType

```cpp
typedef SimilarityTransformListType::iterator Procrustes3D::SimilarityTransformListIteratorType;
```


### typedef TransformMatrixType

```cpp
typedef vnl_matrix_fixed<double, 3+1, 3+1> Procrustes3D::TransformMatrixType;
```


### typedef TransformMatrixListType

```cpp
typedef std::vector<TransformMatrixType> Procrustes3D::TransformMatrixListType;
```


### typedef TransformMatrixIteratorType

```cpp
typedef TransformMatrixListType::iterator Procrustes3D::TransformMatrixIteratorType;
```


## Public Functions Documentation

### function Procrustes3D

```cpp
inline Procrustes3D()
```


### function Procrustes3D

```cpp
inline Procrustes3D(
    bool do_scaling,
    bool do_rotation_translation
)
```


### function GetScaling

```cpp
inline bool GetScaling() const
```


### function ScalingOn

```cpp
inline void ScalingOn()
```


### function ScalingOff

```cpp
inline void ScalingOff()
```


### function GetRotationTranslation

```cpp
inline bool GetRotationTranslation() const
```


### function RotationTranslationOn

```cpp
inline void RotationTranslationOn()
```


### function RotationTranslationOff

```cpp
inline void RotationTranslationOff()
```


### function AlignShapes

```cpp
void AlignShapes(
    SimilarityTransformListType & transforms,
    ShapeListType & shapes
)
```


### function RemoveTranslation

```cpp
void RemoveTranslation(
    SimilarityTransformListType & transforms,
    ShapeListType & shapes
)
```


### function ConstructTransformMatrices

```cpp
void ConstructTransformMatrices(
    SimilarityTransformListType & transforms,
    TransformMatrixListType & transformMatrices
)
```


### function ConstructTransformMatrix

```cpp
void ConstructTransformMatrix(
    SimilarityTransform3D & transform,
    TransformMatrixType & transformMatrix
)
```


### function ComputeMeanShape

```cpp
void ComputeMeanShape(
    ShapeType & mean,
    ShapeListType & shapeList
)
```


### function ComputeCenterOfMass

```cpp
void ComputeCenterOfMass(
    ShapeType & shape,
    PointType & center
)
```


### function CenterShape

```cpp
void CenterShape(
    ShapeType & shape
)
```


### function ComputeCommonCenter

```cpp
void ComputeCommonCenter(
    SimilarityTransformListType & transforms,
    PointType & center
)
```


### function AlignSourceToTarget

```cpp
void AlignSourceToTarget(
    SimilarityTransform3D & transform,
    ShapeType & target,
    ShapeType & source
)
```


### function ComputeMedianShape

```cpp
int ComputeMedianShape(
    ShapeListType & shapeList
)
```


### function TransformShape

```cpp
static void TransformShape(
    ShapeType & shape,
    SimilarityTransform3D & transform
)
```


### function TransformShapes

```cpp
static void TransformShapes(
    ShapeListType & shapes,
    SimilarityTransformListType & transforms
)
```


### function ComputeSumOfSquares

```cpp
static RealType ComputeSumOfSquares(
    ShapeListType & shapes
)
```


-------------------------------

Updated on 2022-01-28 at 21:13:53 +0000