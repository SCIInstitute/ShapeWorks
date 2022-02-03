---
title: itk::SparseKernelTransform

---

# itk::SparseKernelTransform



 [More...](#detailed-description)


`#include <itkSparseKernelTransform.h>`

Inherits from Transform< TScalarType, NDimensions, NDimensions >

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md) | **[Self](../Classes/classitk_1_1SparseKernelTransform.md#typedef-self)**  |
| typedef Transform< TScalarType, NDimensions, NDimensions > | **[Superclass](../Classes/classitk_1_1SparseKernelTransform.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1SparseKernelTransform.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1SparseKernelTransform.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1SparseKernelTransform.md#typedef-constpointer)**  |
| typedef Superclass::ScalarType | **[ScalarType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-scalartype)**  |
| typedef Superclass::ParametersType | **[ParametersType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-parameterstype)**  |
| typedef Superclass::JacobianType | **[JacobianType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-jacobiantype)**  |
| typedef Superclass::InputPointType | **[InputPointType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputpointtype)**  |
| typedef Superclass::OutputPointType | **[OutputPointType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-outputpointtype)**  |
| typedef Superclass::InputVectorType | **[InputVectorType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputvectortype)**  |
| typedef Superclass::OutputVectorType | **[OutputVectorType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-outputvectortype)**  |
| typedef DefaultStaticMeshTraits< TScalarType, NDimensions, NDimensions, TScalarType, TScalarType > | **[PointSetTraitsType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsettraitstype)**  |
| typedef PointSet< [InputPointType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputpointtype), NDimensions, [PointSetTraitsType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsettraitstype) > | **[PointSetType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsettype)**  |
| typedef PointSetType::Pointer | **[PointSetPointer](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsetpointer)**  |
| typedef PointSetType::PointsContainer | **[PointsContainer](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointscontainer)**  |
| typedef PointSetType::PointsContainerIterator | **[PointsIterator](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsiterator)**  |
| typedef PointSetType::PointsContainerConstIterator | **[PointsConstIterator](../Classes/classitk_1_1SparseKernelTransform.md#typedef-pointsconstiterator)**  |
| typedef itk::VectorContainer< unsigned long, [InputVectorType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputvectortype) > | **[VectorSetType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-vectorsettype)**  |
| typedef VectorSetType::Pointer | **[VectorSetPointer](../Classes/classitk_1_1SparseKernelTransform.md#typedef-vectorsetpointer)**  |
| typedef Eigen::Matrix< TScalarType, NDimensions, NDimensions > | **[IMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-imatrixtype)**  |
| typedef Eigen::Triplet< TScalarType > | **[TripletType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-triplettype)**  |
| typedef Eigen::Matrix< TScalarType, NDimensions, NDimensions > | **[GMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-gmatrixtype)**  |
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

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1SparseKernelTransform.md#function-itktypemacro)**([SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md) , Transform ) |
| | **[itkNewMacro](../Classes/classitk_1_1SparseKernelTransform.md#function-itknewmacro)**([Self](../Classes/classitk_1_1SparseKernelTransform.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1SparseKernelTransform.md#function-itkstaticconstmacro)**(SpaceDimension , unsigned int , NDimensions ) |
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
| virtual void | **[ComputeJacobianWithRespectToParameters](../Classes/classitk_1_1SparseKernelTransform.md#function-computejacobianwithrespecttoparameters)**(const [InputPointType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputpointtype) & in, [JacobianType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-jacobiantype) & jacobian) const |
| virtual void | **[SetStiffness](../Classes/classitk_1_1SparseKernelTransform.md#function-setstiffness)**(double stiffness) |
| | **[itkGetMacro](../Classes/classitk_1_1SparseKernelTransform.md#function-itkgetmacro)**(Stiffness , double ) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md#function-sparsekerneltransform)**() |
| virtual | **[~SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md#function-~sparsekerneltransform)**() |
| void | **[PrintSelf](../Classes/classitk_1_1SparseKernelTransform.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual const [GMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-gmatrixtype) & | **[ComputeG](../Classes/classitk_1_1SparseKernelTransform.md#function-computeg)**(const [InputVectorType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputvectortype) & landmarkVector) const |
| virtual const [GMatrixType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-gmatrixtype) & | **[ComputeReflexiveG](../Classes/classitk_1_1SparseKernelTransform.md#function-computereflexiveg)**(PointsIterator ) const |
| virtual void | **[ComputeDeformationContribution](../Classes/classitk_1_1SparseKernelTransform.md#function-computedeformationcontribution)**(const [InputPointType](../Classes/classitk_1_1SparseKernelTransform.md#typedef-inputpointtype) & inputPoint, OutputPointType & result) const |
| void | **[ComputeK](../Classes/classitk_1_1SparseKernelTransform.md#function-computek)**() const |
| void | **[ComputeL](../Classes/classitk_1_1SparseKernelTransform.md#function-computel)**() const |
| void | **[ComputeP](../Classes/classitk_1_1SparseKernelTransform.md#function-computep)**() const |
| void | **[ComputeY](../Classes/classitk_1_1SparseKernelTransform.md#function-computey)**() const |
| void | **[ComputeD](../Classes/classitk_1_1SparseKernelTransform.md#function-computed)**() const |
| void | **[ReorganizeW](../Classes/classitk_1_1SparseKernelTransform.md#function-reorganizew)**(void ) const |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| PointSetPointer | **[m_SourceLandmarks](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-sourcelandmarks)**  |
| PointSetPointer | **[m_TargetLandmarks](../Classes/classitk_1_1SparseKernelTransform.md#variable-m-targetlandmarks)**  |

## Protected Attributes

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
unsigned int NDimensions>
class itk::SparseKernelTransform;
```


Intended to be a base class for elastic body spline and thin plate spline. This is implemented in as straightforward a manner as possible from the IEEE TMI paper by Davis, Khotanzad, Flamig, and Harms, Vol. 16, No. 3 June 1997. Notation closely follows their paper, so if you have it in front of you, this code will make a lot more sense.

[SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md): Provides support for defining source and target landmarks Defines a number of data types used in the computations Defines the mathematical framework used to compute all splines, so that subclasses need only provide a kernel specific to that spline

This formulation allows the stiffness of the spline to be adjusted, allowing the spline to vary from interpolating the landmarks to approximating the landmarks. This part of the formulation is based on the short paper by R. Sprengel, K. Rohr, H. Stiehl. "Thin-Plate Spline Approximation for Image
Registration". In 18th International Conference of the IEEE Engineering in Medicine and Biology Society. 1996.

This class was modified to support its use in the ITK registration framework by Rupert Brooks, McGill Centre for Intelligent Machines, Montreal, Canada March 2007. See the Insight Journal Paper by Brooks, R., Arbel, T. "Improvements to the itk::KernelTransform and its subclasses." 

## Public Types Documentation

### typedef Self

```cpp
typedef SparseKernelTransform itk::SparseKernelTransform< TScalarType, NDimensions >::Self;
```


Standard class typedefs. 


### typedef Superclass

```cpp
typedef Transform<TScalarType, NDimensions, NDimensions > itk::SparseKernelTransform< TScalarType, NDimensions >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::SparseKernelTransform< TScalarType, NDimensions >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::SparseKernelTransform< TScalarType, NDimensions >::ConstPointer;
```


### typedef ScalarType

```cpp
typedef Superclass::ScalarType itk::SparseKernelTransform< TScalarType, NDimensions >::ScalarType;
```


Scalar type. 


### typedef ParametersType

```cpp
typedef Superclass::ParametersType itk::SparseKernelTransform< TScalarType, NDimensions >::ParametersType;
```


Parameters type. 


### typedef JacobianType

```cpp
typedef Superclass::JacobianType itk::SparseKernelTransform< TScalarType, NDimensions >::JacobianType;
```


Jacobian type. 


### typedef InputPointType

```cpp
typedef Superclass::InputPointType itk::SparseKernelTransform< TScalarType, NDimensions >::InputPointType;
```


Standard coordinate point type for this class. 


### typedef OutputPointType

```cpp
typedef Superclass::OutputPointType itk::SparseKernelTransform< TScalarType, NDimensions >::OutputPointType;
```


### typedef InputVectorType

```cpp
typedef Superclass::InputVectorType itk::SparseKernelTransform< TScalarType, NDimensions >::InputVectorType;
```


Standard vector type for this class. 


### typedef OutputVectorType

```cpp
typedef Superclass::OutputVectorType itk::SparseKernelTransform< TScalarType, NDimensions >::OutputVectorType;
```


### typedef PointSetTraitsType

```cpp
typedef DefaultStaticMeshTraits<TScalarType, NDimensions, NDimensions, TScalarType, TScalarType> itk::SparseKernelTransform< TScalarType, NDimensions >::PointSetTraitsType;
```


PointList typedef. This type is used for maintaining lists of points, specifically, the source and target landmark lists. 


### typedef PointSetType

```cpp
typedef PointSet<InputPointType, NDimensions, PointSetTraitsType> itk::SparseKernelTransform< TScalarType, NDimensions >::PointSetType;
```


### typedef PointSetPointer

```cpp
typedef PointSetType::Pointer itk::SparseKernelTransform< TScalarType, NDimensions >::PointSetPointer;
```


### typedef PointsContainer

```cpp
typedef PointSetType::PointsContainer itk::SparseKernelTransform< TScalarType, NDimensions >::PointsContainer;
```


### typedef PointsIterator

```cpp
typedef PointSetType::PointsContainerIterator itk::SparseKernelTransform< TScalarType, NDimensions >::PointsIterator;
```


### typedef PointsConstIterator

```cpp
typedef PointSetType::PointsContainerConstIterator itk::SparseKernelTransform< TScalarType, NDimensions >::PointsConstIterator;
```


### typedef VectorSetType

```cpp
typedef itk::VectorContainer<unsigned long,InputVectorType> itk::SparseKernelTransform< TScalarType, NDimensions >::VectorSetType;
```


VectorSet typedef. 


### typedef VectorSetPointer

```cpp
typedef VectorSetType::Pointer itk::SparseKernelTransform< TScalarType, NDimensions >::VectorSetPointer;
```


### typedef IMatrixType

```cpp
typedef Eigen::Matrix<TScalarType, NDimensions, NDimensions> itk::SparseKernelTransform< TScalarType, NDimensions >::IMatrixType;
```


'I' (identity) matrix typedef. 


### typedef TripletType

```cpp
typedef Eigen::Triplet<TScalarType> itk::SparseKernelTransform< TScalarType, NDimensions >::TripletType;
```


triplets used to fill sparse matrices. 


### typedef GMatrixType

```cpp
typedef Eigen::Matrix<TScalarType, NDimensions, NDimensions> itk::SparseKernelTransform< TScalarType, NDimensions >::GMatrixType;
```


'G' matrix typedef. 


### typedef LMatrixType

```cpp
typedef Eigen::SparseMatrix<TScalarType> itk::SparseKernelTransform< TScalarType, NDimensions >::LMatrixType;
```


'L' matrix typedef. 


### typedef KMatrixType

```cpp
typedef Eigen::SparseMatrix<TScalarType> itk::SparseKernelTransform< TScalarType, NDimensions >::KMatrixType;
```


'K' matrix typedef. 


### typedef PMatrixType

```cpp
typedef Eigen::SparseMatrix<TScalarType> itk::SparseKernelTransform< TScalarType, NDimensions >::PMatrixType;
```


'P' matrix typedef. 


### typedef YMatrixType

```cpp
typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic> itk::SparseKernelTransform< TScalarType, NDimensions >::YMatrixType;
```


'Y' matrix typedef. 


### typedef WMatrixType

```cpp
typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic> itk::SparseKernelTransform< TScalarType, NDimensions >::WMatrixType;
```


'W' matrix typedef. 


### typedef DMatrixType

```cpp
typedef Eigen::Matrix<TScalarType, Eigen::Dynamic, Eigen::Dynamic> itk::SparseKernelTransform< TScalarType, NDimensions >::DMatrixType;
```


'D' matrix typedef. Deformation component 


### typedef AMatrixType

```cpp
typedef Eigen::Matrix<TScalarType, NDimensions, NDimensions> itk::SparseKernelTransform< TScalarType, NDimensions >::AMatrixType;
```


'A' matrix typedef. Rotational part of the Affine component 


### typedef BMatrixType

```cpp
typedef Eigen::Matrix<TScalarType,NDimensions,1> itk::SparseKernelTransform< TScalarType, NDimensions >::BMatrixType;
```


'B' matrix typedef. Translational part of the Affine component 


### typedef RowMatrixType

```cpp
typedef Eigen::Matrix<TScalarType,1,NDimensions> itk::SparseKernelTransform< TScalarType, NDimensions >::RowMatrixType;
```


Row matrix typedef. 


### typedef ColumnMatrixType

```cpp
typedef Eigen::Matrix<TScalarType,NDimensions,1> itk::SparseKernelTransform< TScalarType, NDimensions >::ColumnMatrixType;
```


Column matrix typedef. 


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    SparseKernelTransform ,
    Transform 
)
```


Run-time type information (and related methods). 


### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


New macro for creation of through a Smart Pointer 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    SpaceDimension ,
    unsigned int ,
    NDimensions 
)
```


Dimension of the domain space. 


### function itkGetObjectMacro

```cpp
itkGetObjectMacro(
    SourceLandmarks ,
    PointSetType 
)
```


Get the source landmarks list, which we will denote \( p \). 


### function SetSourceLandmarks

```cpp
virtual void SetSourceLandmarks(
    PointSetType * 
)
```


Set the source landmarks list. 


### function itkGetObjectMacro

```cpp
itkGetObjectMacro(
    TargetLandmarks ,
    PointSetType 
)
```


Get the target landmarks list, which we will denote \( q \). 


### function SetTargetLandmarks

```cpp
virtual void SetTargetLandmarks(
    PointSetType * 
)
```


Set the target landmarks list. 


### function itkGetObjectMacro

```cpp
itkGetObjectMacro(
    Displacements ,
    VectorSetType 
)
```


Get the displacements list, which we will denote \( d \), where \( d_i = q_i - p_i \). 


### function ComputeWMatrix

```cpp
void ComputeWMatrix(
    void 
) const
```


Compute W matrix. 


### function TransformPoint

```cpp
virtual OutputPointType TransformPoint(
    const InputPointType & thisPoint
) const
```


Compute L matrix inverse. Compute the position of point in the new space 


### function SetIdentity

```cpp
virtual void SetIdentity()
```


Compute the Jacobian Matrix of the transformation at one point Set the Transformation Parameters to be an identity transform 


### function SetParameters

```cpp
virtual void SetParameters(
    const ParametersType & 
)
```


Set the Transformation Parameters and update the internal transformation. The parameters represent the source landmarks. Each landmark point is represented by NDimensions doubles. All the landmarks are concatenated to form one flat Array<double>. 


### function SetFixedParameters

```cpp
virtual void SetFixedParameters(
    const ParametersType & 
)
```


Set Transform Fixed Parameters: To support the transform file writer this function was added to set the target landmarks similar to the SetParameters function setting the source landmarks 


### function UpdateParameters

```cpp
virtual void UpdateParameters(
    void 
) const
```


Update the Parameters array from the landmarks corrdinates. 


### function GetParameters

```cpp
virtual const ParametersType & GetParameters(
    void 
) const
```


Get the Transformation Parameters - Gets the Source Landmarks 


### function GetFixedParameters

```cpp
virtual const ParametersType & GetFixedParameters(
    void 
) const
```


Get Transform Fixed Parameters - Gets the Target Landmarks 


### function ComputeJacobianWithRespectToParameters

```cpp
virtual void ComputeJacobianWithRespectToParameters(
    const InputPointType & in,
    JacobianType & jacobian
) const
```


**Reimplemented by**: [itk::CompactlySupportedRBFSparseKernelTransform::ComputeJacobianWithRespectToParameters](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-computejacobianwithrespecttoparameters)


### function SetStiffness

```cpp
inline virtual void SetStiffness(
    double stiffness
)
```


Stiffness of the spline. A stiffness of zero results in the standard interpolating spline. A non-zero stiffness allows the spline to approximate rather than interpolate the landmarks. Stiffness values are usually rather small, typically in the range of 0.001 to 0.1. The approximating spline formulation is based on the short paper by R. Sprengel, K. Rohr, H. Stiehl. "Thin-Plate
Spline Approximation for Image Registration". In 18th International Conference of the IEEE Engineering in Medicine and Biology Society. 1996. 


### function itkGetMacro

```cpp
itkGetMacro(
    Stiffness ,
    double 
)
```


## Protected Functions Documentation

### function SparseKernelTransform

```cpp
SparseKernelTransform()
```


### function ~SparseKernelTransform

```cpp
virtual ~SparseKernelTransform()
```


### function PrintSelf

```cpp
void PrintSelf(
    std::ostream & os,
    Indent indent
) const
```


### function ComputeG

```cpp
virtual const GMatrixType & ComputeG(
    const InputVectorType & landmarkVector
) const
```


**Reimplemented by**: [itk::CompactlySupportedRBFSparseKernelTransform::ComputeG](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-computeg)


Compute G(x) This is essentially the kernel of the transform. By overriding this method, we can obtain (among others): Elastic body spline Thin plate spline Volume spline 


### function ComputeReflexiveG

```cpp
virtual const GMatrixType & ComputeReflexiveG(
    PointsIterator 
) const
```


Compute a G(x) for a point to itself (i.e. for the block diagonal elements of the matrix K. Parameter indicates for which landmark the reflexive G is to be computed. The default implementation for the reflexive contribution is a diagonal matrix where the diagonal elements are the stiffness of the spline. 


### function ComputeDeformationContribution

```cpp
virtual void ComputeDeformationContribution(
    const InputPointType & inputPoint,
    OutputPointType & result
) const
```


**Reimplemented by**: [itk::CompactlySupportedRBFSparseKernelTransform::ComputeDeformationContribution](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md#function-computedeformationcontribution)


Compute the contribution of the landmarks weighted by the kernel funcion to the global deformation of the space 


### function ComputeK

```cpp
void ComputeK() const
```


Compute K matrix. 


### function ComputeL

```cpp
void ComputeL() const
```


Compute L matrix. 


### function ComputeP

```cpp
void ComputeP() const
```


Compute P matrix. 


### function ComputeY

```cpp
void ComputeY() const
```


Compute Y matrix. 


### function ComputeD

```cpp
void ComputeD() const
```


Compute displacements \( q_i - p_i \). 


### function ReorganizeW

```cpp
void ReorganizeW(
    void 
) const
```


**Warning**: This method release the memory of the W Matrix 

Reorganize the components of W into D (deformable), A (rotation part of affine) and B (translational part of affine ) components. 


## Public Attributes Documentation

### variable m_SourceLandmarks

```cpp
PointSetPointer m_SourceLandmarks;
```


The list of source landmarks, denoted 'p'. 


### variable m_TargetLandmarks

```cpp
PointSetPointer m_TargetLandmarks;
```


The list of target landmarks, denoted 'q'. 


## Protected Attributes Documentation

### variable m_Stiffness

```cpp
double m_Stiffness;
```


Stiffness parameter 


### variable m_Displacements

```cpp
VectorSetPointer m_Displacements;
```


The list of displacements. d[i] = q[i] - p[i]; 


### variable m_LMatrix

```cpp
LMatrixType m_LMatrix;
```


The L matrix. 


### variable m_LMatrixInverse

```cpp
LMatrixType m_LMatrixInverse;
```


The inverse of L, which we also cache. 


### variable m_KMatrix

```cpp
KMatrixType m_KMatrix;
```


The K matrix. 


### variable m_PMatrix

```cpp
PMatrixType m_PMatrix;
```


The P matrix. 


### variable m_YMatrix

```cpp
YMatrixType m_YMatrix;
```


The Y matrix. 


### variable m_WMatrix

```cpp
WMatrixType m_WMatrix;
```


The W matrix. 


### variable m_DMatrix

```cpp
DMatrixType m_DMatrix;
```


The Deformation matrix. This is an auxiliary matrix that will hold the Deformation (non-affine) part of the transform. Those are the coefficients that will multiply the Kernel function 


### variable m_AMatrix

```cpp
AMatrixType m_AMatrix;
```


Rotatinoal/Shearing part of the Affine component of the Transformation 


### variable m_BVector

```cpp
BMatrixType m_BVector;
```


Translational part of the Affine component of the Transformation 


### variable m_GMatrix

```cpp
GMatrixType m_GMatrix;
```


The G matrix. It is made mutable because m_GMatrix was made an ivar only to avoid copying the matrix at return time 


### variable m_WMatrixComputed

```cpp
bool m_WMatrixComputed;
```


Has the W matrix been computed? 


### variable m_LMatrixComputed

```cpp
bool m_LMatrixComputed;
```


Has the L matrix been computed? 


### variable m_LInverseComputed

```cpp
bool m_LInverseComputed;
```


Has the L inverse matrix been computed? 


### variable m_I

```cpp
IMatrixType m_I;
```


Identity matrix. 


-------------------------------

Updated on 2022-02-03 at 23:25:51 +0000