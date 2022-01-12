---
title: itk::CompactlySupportedRBFSparseKernelTransform

---

# itk::CompactlySupportedRBFSparseKernelTransform



 [More...](#detailed-description)

Inherits from [itk::SparseKernelTransform< TScalarType, 3 >](../Classes/classitk_1_1SparseKernelTransform.md), Transform< TScalarType, NDimensions, NDimensions >

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [CompactlySupportedRBFSparseKernelTransform](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md) | **[Self](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-self)**  |
| typedef [SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md)< TScalarType, NDimensions > | **[Superclass](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-constpointer)**  |
| typedef [Superclass::ScalarType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-scalartype) | **[ScalarType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-scalartype)**  |
| typedef [Superclass::ParametersType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-parameterstype) | **[ParametersType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-parameterstype)**  |
| typedef [Superclass::JacobianType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-jacobiantype) | **[JacobianType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-jacobiantype)**  |
| typedef [Superclass::InputPointType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputpointtype) | **[InputPointType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-inputpointtype)**  |
| typedef Superclass::OutputPointType | **[OutputPointType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-outputpointtype)**  |
| typedef [Superclass::InputVectorType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputvectortype) | **[InputVectorType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-inputvectortype)**  |
| typedef Superclass::OutputVectorType | **[OutputVectorType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-outputvectortype)**  |
| typedef Superclass::InputCovariantVectorType | **[InputCovariantVectorType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-inputcovariantvectortype)**  |
| typedef Superclass::OutputCovariantVectorType | **[OutputCovariantVectorType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-outputcovariantvectortype)**  |
| typedef Superclass::PointsIterator | **[PointsIterator](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-pointsiterator)**  |

## Protected Types

|                | Name           |
| -------------- | -------------- |
| typedef [Superclass::GMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-gmatrixtype) | **[GMatrixType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-gmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-itknewmacro)**([Self](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-itktypemacro)**([CompactlySupportedRBFSparseKernelTransform](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md) , [SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-itkstaticconstmacro)**(SpaceDimension , unsigned int , Superclass::SpaceDimension ) |
| void | **[SetSigma](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-setsigma)**(double sigma) |
| virtual void | **[ComputeJacobianWithRespectToParameters](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-computejacobianwithrespecttoparameters)**(const [InputPointType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-inputpointtype) & in, [JacobianType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-jacobiantype) & jacobian) const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[CompactlySupportedRBFSparseKernelTransform](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-compactlysupportedrbfsparsekerneltransform)**() |
| virtual | **[~CompactlySupportedRBFSparseKernelTransform](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-~compactlysupportedrbfsparsekerneltransform)**() |
| virtual const [GMatrixType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-gmatrixtype) & | **[ComputeG](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-computeg)**(const InputVectorType & landmarkVector) const override |
| virtual void | **[ComputeDeformationContribution](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-computedeformationcontribution)**(const [InputPointType](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#typedef-inputpointtype) & inputPoint, OutputPointType & result) const override |

## Additional inherited members

**Public Types inherited from [itk::SparseKernelTransform< TScalarType, 3 >](../Classes/classitk_1_1SparseKernelTransform.md)**

|                | Name           |
| -------------- | -------------- |
| typedef DefaultStaticMeshTraits< TScalarType, NDimensions, NDimensions, TScalarType, TScalarType > | **[PointSetTraitsType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsettraitstype)**  |
| typedef PointSet< [InputPointType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputpointtype), NDimensions, [PointSetTraitsType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsettraitstype) > | **[PointSetType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsettype)**  |
| typedef PointSetType::Pointer | **[PointSetPointer](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsetpointer)**  |
| typedef PointSetType::PointsContainer | **[PointsContainer](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointscontainer)**  |
| typedef PointSetType::PointsContainerConstIterator | **[PointsConstIterator](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsconstiterator)**  |
| typedef itk::VectorContainer< unsigned long, [InputVectorType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputvectortype) > | **[VectorSetType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-vectorsettype)**  |
| typedef VectorSetType::Pointer | **[VectorSetPointer](../Classes/classitk_1_1SparseKernelTransform.md#typedef-vectorsetpointer)**  |
| typedef Eigen::Matrix< TScalarType, NDimensions, NDimensions > | **[IMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-imatrixtype)**  |
| typedef Eigen::Triplet< TScalarType > | **[TripletType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-triplettype)**  |
| typedef Eigen::SparseMatrix< TScalarType > | **[LMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-lmatrixtype)**  |
| typedef Eigen::SparseMatrix< TScalarType > | **[KMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-kmatrixtype)**  |
| typedef Eigen::SparseMatrix< TScalarType > | **[PMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pmatrixtype)**  |
| typedef Eigen::Matrix< TScalarType, Eigen::Dynamic, Eigen::Dynamic > | **[YMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-ymatrixtype)**  |
| typedef Eigen::Matrix< TScalarType, Eigen::Dynamic, Eigen::Dynamic > | **[WMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-wmatrixtype)**  |
| typedef Eigen::Matrix< TScalarType, Eigen::Dynamic, Eigen::Dynamic > | **[DMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-dmatrixtype)**  |
| typedef Eigen::Matrix< TScalarType, NDimensions, NDimensions > | **[AMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-amatrixtype)**  |
| typedef Eigen::Matrix< TScalarType, NDimensions, 1 > | **[BMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-bmatrixtype)**  |
| typedef Eigen::Matrix< TScalarType, 1, NDimensions > | **[RowMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-rowmatrixtype)**  |
| typedef Eigen::Matrix< TScalarType, NDimensions, 1 > | **[ColumnMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-columnmatrixtype)**  |

**Public Functions inherited from [itk::SparseKernelTransform< TScalarType, 3 >](../Classes/classitk_1_1SparseKernelTransform.md)**

|                | Name           |
| -------------- | -------------- |
| | **[itkGetObjectMacro](../Classes/classitk_1_1SparseKernelTransform.md#function-itkgetobjectmacro)**(SourceLandmarks , PointSetType ) |
| virtual void | **[SetSourceLandmarks](../Classes/classitk_1_1SparseKernelTransform.md#function-setsourcelandmarks)**(PointSetType * ) |
| | **[itkGetObjectMacro](../Classes/classitk_1_1SparseKernelTransform.md#function-itkgetobjectmacro)**(TargetLandmarks , PointSetType ) |
| virtual void | **[SetTargetLandmarks](../Classes/classitk_1_1SparseKernelTransform.md#function-settargetlandmarks)**(PointSetType * ) |
| | **[itkGetObjectMacro](../Classes/classitk_1_1SparseKernelTransform.md#function-itkgetobjectmacro)**(Displacements , [VectorSetType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-vectorsettype) ) |
| void | **[ComputeWMatrix](../Classes/classitk_1_1SparseKernelTransform.md#function-computewmatrix)**(void ) const |
| virtual OutputPointType | **[TransformPoint](../Classes/classitk_1_1SparseKernelTransform.md#function-transformpoint)**(const [InputPointType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputpointtype) & thisPoint) const |
| virtual void | **[SetIdentity](../Classes/classitk_1_1SparseKernelTransform.md#function-setidentity)**() |
| virtual void | **[SetParameters](../Classes/classitk_1_1SparseKernelTransform.md#function-setparameters)**(const [ParametersType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-parameterstype) & ) |
| virtual void | **[SetFixedParameters](../Classes/classitk_1_1SparseKernelTransform.md#function-setfixedparameters)**(const [ParametersType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-parameterstype) & ) |
| virtual void | **[UpdateParameters](../Classes/classitk_1_1SparseKernelTransform.md#function-updateparameters)**(void ) const |
| virtual const [ParametersType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-parameterstype) & | **[GetParameters](../Classes/classitk_1_1SparseKernelTransform.md#function-getparameters)**(void ) const |
| virtual const [ParametersType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-parameterstype) & | **[GetFixedParameters](../Classes/classitk_1_1SparseKernelTransform.md#function-getfixedparameters)**(void ) const |
| virtual void | **[SetStiffness](../Classes/classitk_1_1SparseKernelTransform.md#function-setstiffness)**(double stiffness) |
| | **[itkGetMacro](../Classes/classitk_1_1SparseKernelTransform.md#function-itkgetmacro)**(Stiffness , double ) |

**Protected Functions inherited from [itk::SparseKernelTransform< TScalarType, 3 >](../Classes/classitk_1_1SparseKernelTransform.md)**

|                | Name           |
| -------------- | -------------- |
| | **[SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md#function-sparsekerneltransform)**() |
| virtual | **[~SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md#function-~sparsekerneltransform)**() |
| void | **[PrintSelf](../Classes/classitk_1_1SparseKernelTransform.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual const [GMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-gmatrixtype) & | **[ComputeReflexiveG](../Classes/classitk_1_1SparseKernelTransform.md#function-computereflexiveg)**(PointsIterator ) const |
| void | **[ComputeK](../Classes/classitk_1_1SparseKernelTransform.md#function-computek)**() const |
| void | **[ComputeL](../Classes/classitk_1_1SparseKernelTransform.md#function-computel)**() const |
| void | **[ComputeP](../Classes/classitk_1_1SparseKernelTransform.md#function-computep)**() const |
| void | **[ComputeY](../Classes/classitk_1_1SparseKernelTransform.md#function-computey)**() const |
| void | **[ComputeD](../Classes/classitk_1_1SparseKernelTransform.md#function-computed)**() const |
| void | **[ReorganizeW](../Classes/classitk_1_1SparseKernelTransform.md#function-reorganizew)**(void ) const |

**Public Attributes inherited from [itk::SparseKernelTransform< TScalarType, 3 >](../Classes/classitk_1_1SparseKernelTransform.md)**

|                | Name           |
| -------------- | -------------- |
| PointSetPointer | **[m_SourceLandmarks](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-sourcelandmarks)**  |
| PointSetPointer | **[m_TargetLandmarks](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-targetlandmarks)**  |

**Protected Attributes inherited from [itk::SparseKernelTransform< TScalarType, 3 >](../Classes/classitk_1_1SparseKernelTransform.md)**

|                | Name           |
| -------------- | -------------- |
| double | **[m_Stiffness](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-stiffness)**  |
| VectorSetPointer | **[m_Displacements](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-displacements)**  |
| [LMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-lmatrixtype) | **[m_LMatrix](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-lmatrix)**  |
| [LMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-lmatrixtype) | **[m_LMatrixInverse](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-lmatrixinverse)**  |
| [KMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-kmatrixtype) | **[m_KMatrix](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-kmatrix)**  |
| [PMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pmatrixtype) | **[m_PMatrix](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-pmatrix)**  |
| [YMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-ymatrixtype) | **[m_YMatrix](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-ymatrix)**  |
| [WMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-wmatrixtype) | **[m_WMatrix](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-wmatrix)**  |
| [DMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-dmatrixtype) | **[m_DMatrix](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-dmatrix)**  |
| [AMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-amatrixtype) | **[m_AMatrix](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-amatrix)**  |
| [BMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-bmatrixtype) | **[m_BVector](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-bvector)**  |
| [GMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-gmatrixtype) | **[m_GMatrix](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-gmatrix)**  |
| bool | **[m_WMatrixComputed](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-wmatrixcomputed)**  |
| bool | **[m_LMatrixComputed](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-lmatrixcomputed)**  |
| bool | **[m_LInverseComputed](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-linversecomputed)**  |
| [IMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-imatrixtype) | **[m_I](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-i)**  |


## Detailed Description

```cpp
template <class TScalarType ,
unsigned int NDimensions =3>
class itk::CompactlySupportedRBFSparseKernelTransform;
```

## Public Types Documentation

### typedef Self

```cpp
typedef CompactlySupportedRBFSparseKernelTransform itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::Self;
```


Standard class typedefs. 


### typedef Superclass

```cpp
typedef SparseKernelTransform< TScalarType, NDimensions> itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::ConstPointer;
```


### typedef ScalarType

```cpp
typedef Superclass::ScalarType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::ScalarType;
```


Scalar type. 


### typedef ParametersType

```cpp
typedef Superclass::ParametersType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::ParametersType;
```


Parameters type. 


### typedef JacobianType

```cpp
typedef Superclass::JacobianType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::JacobianType;
```


Jacobian Type 


### typedef InputPointType

```cpp
typedef Superclass::InputPointType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::InputPointType;
```


These (rather redundant) typedefs are needed because on SGI, typedefs are not inherited 


### typedef OutputPointType

```cpp
typedef Superclass::OutputPointType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::OutputPointType;
```


### typedef InputVectorType

```cpp
typedef Superclass::InputVectorType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::InputVectorType;
```


### typedef OutputVectorType

```cpp
typedef Superclass::OutputVectorType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::OutputVectorType;
```


### typedef InputCovariantVectorType

```cpp
typedef Superclass::InputCovariantVectorType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::InputCovariantVectorType;
```


### typedef OutputCovariantVectorType

```cpp
typedef Superclass::OutputCovariantVectorType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::OutputCovariantVectorType;
```


### typedef PointsIterator

```cpp
typedef Superclass::PointsIterator itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::PointsIterator;
```


## Protected Types Documentation

### typedef GMatrixType

```cpp
typedef Superclass::GMatrixType itk::CompactlySupportedRBFSparseKernelTransform< TScalarType, NDimensions >::GMatrixType;
```


These (rather redundant) typedefs are needed because on SGI, typedefs are not inherited. 


## Public Functions Documentation

### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


New macro for creation of through a Smart Pointer 


### function itkTypeMacro

```cpp
itkTypeMacro(
    CompactlySupportedRBFSparseKernelTransform ,
    SparseKernelTransform 
)
```


Run-time type information (and related methods). 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    SpaceDimension ,
    unsigned int ,
    Superclass::SpaceDimension 
)
```


Dimension of the domain space. 


### function SetSigma

```cpp
inline void SetSigma(
    double sigma
)
```


### function ComputeJacobianWithRespectToParameters

```cpp
virtual void ComputeJacobianWithRespectToParameters(
    const InputPointType & in,
    JacobianType & jacobian
) const
```


**Reimplements**: [itk::SparseKernelTransform::ComputeJacobianWithRespectToParameters](../Classes/classitk_1_1SparseKernelTransform.md#function-computejacobianwithrespecttoparameters)


## Protected Functions Documentation

### function CompactlySupportedRBFSparseKernelTransform

```cpp
inline CompactlySupportedRBFSparseKernelTransform()
```


### function ~CompactlySupportedRBFSparseKernelTransform

```cpp
inline virtual ~CompactlySupportedRBFSparseKernelTransform()
```


### function ComputeG

```cpp
virtual const GMatrixType & ComputeG(
    const InputVectorType & landmarkVector
) const override
```


**Reimplements**: [itk::SparseKernelTransform::ComputeG](../Classes/classitk_1_1SparseKernelTransform.md#function-computeg)


Compute G(x) This is essentially the kernel of the transform. By overriding this method, we can obtain (among others): Elastic body spline Thin plate spline Volume spline 


### function ComputeDeformationContribution

```cpp
virtual void ComputeDeformationContribution(
    const InputPointType & inputPoint,
    OutputPointType & result
) const override
```


**Reimplements**: [itk::SparseKernelTransform::ComputeDeformationContribution](../Classes/classitk_1_1SparseKernelTransform.md#function-computedeformationcontribution)


Compute the contribution of the landmarks weighted by the kernel funcion to the global deformation of the space 


-------------------------------

Updated on 2022-01-12 at 19:06:12 +0000