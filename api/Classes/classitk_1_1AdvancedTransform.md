---
title: itk::AdvancedTransform
summary: Transform maps points, vectors and covariant vectors from an input space to an output space. 

---

# itk::AdvancedTransform



Transform maps points, vectors and covariant vectors from an input space to an output space.  [More...](#detailed-description)


`#include <itkAdvancedTransform.h>`

Inherits from Transform< TScalarType, 3, 3 >

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md) | **[Self](../Classes/classitk_1_1AdvancedTransform.md#typedef-self)**  |
| typedef Transform< TScalarType, NInputDimensions, NOutputDimensions > | **[Superclass](../Classes/classitk_1_1AdvancedTransform.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1AdvancedTransform.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1AdvancedTransform.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1AdvancedTransform.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1AdvancedTransform.md#typedef-constpointer)**  |
| typedef Superclass::ScalarType | **[ScalarType](../Classes/classitk_1_1AdvancedTransform.md#typedef-scalartype)**  |
| typedef Superclass::ParametersType | **[ParametersType](../Classes/classitk_1_1AdvancedTransform.md#typedef-parameterstype)**  |
| typedef Superclass::ParametersValueType | **[ParametersValueType](../Classes/classitk_1_1AdvancedTransform.md#typedef-parametersvaluetype)**  |
| typedef Superclass::NumberOfParametersType | **[NumberOfParametersType](../Classes/classitk_1_1AdvancedTransform.md#typedef-numberofparameterstype)**  |
| typedef Superclass::DerivativeType | **[DerivativeType](../Classes/classitk_1_1AdvancedTransform.md#typedef-derivativetype)**  |
| typedef Superclass::JacobianType | **[JacobianType](../Classes/classitk_1_1AdvancedTransform.md#typedef-jacobiantype)**  |
| typedef Superclass::InputVectorType | **[InputVectorType](../Classes/classitk_1_1AdvancedTransform.md#typedef-inputvectortype)**  |
| typedef Superclass::OutputVectorType | **[OutputVectorType](../Classes/classitk_1_1AdvancedTransform.md#typedef-outputvectortype)**  |
| typedef Superclass ::InputCovariantVectorType | **[InputCovariantVectorType](../Classes/classitk_1_1AdvancedTransform.md#typedef-inputcovariantvectortype)**  |
| typedef Superclass ::OutputCovariantVectorType | **[OutputCovariantVectorType](../Classes/classitk_1_1AdvancedTransform.md#typedef-outputcovariantvectortype)**  |
| typedef Superclass::InputVnlVectorType | **[InputVnlVectorType](../Classes/classitk_1_1AdvancedTransform.md#typedef-inputvnlvectortype)**  |
| typedef Superclass::OutputVnlVectorType | **[OutputVnlVectorType](../Classes/classitk_1_1AdvancedTransform.md#typedef-outputvnlvectortype)**  |
| typedef Superclass::InputPointType | **[InputPointType](../Classes/classitk_1_1AdvancedTransform.md#typedef-inputpointtype)**  |
| typedef Superclass::OutputPointType | **[OutputPointType](../Classes/classitk_1_1AdvancedTransform.md#typedef-outputpointtype)**  |
| typedef Superclass::InverseTransformBaseType | **[InverseTransformBaseType](../Classes/classitk_1_1AdvancedTransform.md#typedef-inversetransformbasetype)**  |
| typedef Superclass::InverseTransformBasePointer | **[InverseTransformBasePointer](../Classes/classitk_1_1AdvancedTransform.md#typedef-inversetransformbasepointer)**  |
| typedef Transform< TScalarType, NInputDimensions, NOutputDimensions > | **[TransformType](../Classes/classitk_1_1AdvancedTransform.md#typedef-transformtype)**  |
| typedef TransformType::Pointer | **[TransformTypePointer](../Classes/classitk_1_1AdvancedTransform.md#typedef-transformtypepointer)**  |
| typedef TransformType::ConstPointer | **[TransformTypeConstPointer](../Classes/classitk_1_1AdvancedTransform.md#typedef-transformtypeconstpointer)**  |
| typedef std::vector< unsigned long > | **[NonZeroJacobianIndicesType](../Classes/classitk_1_1AdvancedTransform.md#typedef-nonzerojacobianindicestype)**  |
| typedef Matrix< [ScalarType](../Classes/classitk_1_1AdvancedTransform.md#typedef-scalartype), OutputSpaceDimension, InputSpaceDimension > | **[SpatialJacobianType](../Classes/classitk_1_1AdvancedTransform.md#typedef-spatialjacobiantype)**  |
| typedef std::vector< SpatialJacobianType > | **[JacobianOfSpatialJacobianType](../Classes/classitk_1_1AdvancedTransform.md#typedef-jacobianofspatialjacobiantype)**  |
| typedef FixedArray< Matrix< [ScalarType](../Classes/classitk_1_1AdvancedTransform.md#typedef-scalartype), InputSpaceDimension, InputSpaceDimension >, OutputSpaceDimension > | **[SpatialHessianType](../Classes/classitk_1_1AdvancedTransform.md#typedef-spatialhessiantype)**  |
| typedef std::vector< SpatialHessianType > | **[JacobianOfSpatialHessianType](../Classes/classitk_1_1AdvancedTransform.md#typedef-jacobianofspatialhessiantype)**  |
| typedef SpatialJacobianType::InternalMatrixType | **[InternalMatrixType](../Classes/classitk_1_1AdvancedTransform.md#typedef-internalmatrixtype)**  |
| typedef OutputCovariantVectorType | **[MovingImageGradientType](../Classes/classitk_1_1AdvancedTransform.md#typedef-movingimagegradienttype)**  |
| typedef MovingImageGradientType::ValueType | **[MovingImageGradientValueType](../Classes/classitk_1_1AdvancedTransform.md#typedef-movingimagegradientvaluetype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1AdvancedTransform.md#function-itktypemacro)**([AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md) , Transform ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1AdvancedTransform.md#function-itkstaticconstmacro)**(InputSpaceDimension , unsigned int , NInputDimensions ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1AdvancedTransform.md#function-itkstaticconstmacro)**(OutputSpaceDimension , unsigned int , NOutputDimensions ) |
| virtual NumberOfParametersType | **[GetNumberOfNonZeroJacobianIndices](../Classes/classitk_1_1AdvancedTransform.md#function-getnumberofnonzerojacobianindices)**(void ) const |
| | **[itkGetConstMacro](../Classes/classitk_1_1AdvancedTransform.md#function-itkgetconstmacro)**(HasNonZeroSpatialHessian , bool ) |
| | **[itkGetConstMacro](../Classes/classitk_1_1AdvancedTransform.md#function-itkgetconstmacro)**(HasNonZeroJacobianOfSpatialHessian , bool ) |
| virtual void | **[GetJacobian](../Classes/classitk_1_1AdvancedTransform.md#function-getjacobian)**(const InputPointType & ipp, JacobianType & j, [NonZeroJacobianIndicesType](../Classes/classitk_1_1AdvancedTransform.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const =0 |
| virtual void | **[EvaluateJacobianWithImageGradientProduct](../Classes/classitk_1_1AdvancedTransform.md#function-evaluatejacobianwithimagegradientproduct)**(const InputPointType & ipp, const [MovingImageGradientType](../Classes/classitk_1_1AdvancedTransform.md#typedef-movingimagegradienttype) & movingImageGradient, DerivativeType & imageJacobian, [NonZeroJacobianIndicesType](../Classes/classitk_1_1AdvancedTransform.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const |
| virtual void | **[GetSpatialJacobian](../Classes/classitk_1_1AdvancedTransform.md#function-getspatialjacobian)**(const InputPointType & ipp, SpatialJacobianType & sj) const =0 |
| virtual void | **[ComputeJacobianWithRespectToParameters](../Classes/classitk_1_1AdvancedTransform.md#function-computejacobianwithrespecttoparameters)**(const InputPointType & itkNotUsedp, JacobianType & itkNotUsedj) const |
| virtual void | **[ComputeJacobianWithRespectToPosition](../Classes/classitk_1_1AdvancedTransform.md#function-computejacobianwithrespecttoposition)**(const InputPointType & itkNotUsedp, JacobianType & itkNotUsedj) const |
| virtual void | **[GetSpatialHessian](../Classes/classitk_1_1AdvancedTransform.md#function-getspatialhessian)**(const InputPointType & ipp, SpatialHessianType & sh) const =0 |
| virtual void | **[GetJacobianOfSpatialJacobian](../Classes/classitk_1_1AdvancedTransform.md#function-getjacobianofspatialjacobian)**(const InputPointType & ipp, JacobianOfSpatialJacobianType & jsj, [NonZeroJacobianIndicesType](../Classes/classitk_1_1AdvancedTransform.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const =0 |
| virtual void | **[GetJacobianOfSpatialJacobian](../Classes/classitk_1_1AdvancedTransform.md#function-getjacobianofspatialjacobian)**(const InputPointType & ipp, SpatialJacobianType & sj, JacobianOfSpatialJacobianType & jsj, [NonZeroJacobianIndicesType](../Classes/classitk_1_1AdvancedTransform.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const =0 |
| virtual void | **[GetJacobianOfSpatialHessian](../Classes/classitk_1_1AdvancedTransform.md#function-getjacobianofspatialhessian)**(const InputPointType & ipp, JacobianOfSpatialHessianType & jsh, [NonZeroJacobianIndicesType](../Classes/classitk_1_1AdvancedTransform.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const =0 |
| virtual void | **[GetJacobianOfSpatialHessian](../Classes/classitk_1_1AdvancedTransform.md#function-getjacobianofspatialhessian)**(const InputPointType & ipp, SpatialHessianType & sh, JacobianOfSpatialHessianType & jsh, [NonZeroJacobianIndicesType](../Classes/classitk_1_1AdvancedTransform.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const =0 |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md#function-advancedtransform)**() |
| | **[AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md#function-advancedtransform)**(NumberOfParametersType numberOfParameters) |
| virtual | **[~AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md#function-~advancedtransform)**() |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| bool | **[m_HasNonZeroSpatialHessian](../Classes/classitk_1_1AdvancedTransform.md#variable-m-hasnonzerospatialhessian)**  |
| bool | **[m_HasNonZeroJacobianOfSpatialHessian](../Classes/classitk_1_1AdvancedTransform.md#variable-m-hasnonzerojacobianofspatialhessian)**  |

## Detailed Description

```cpp
template <class TScalarType ,
unsigned int NInputDimensions =3,
unsigned int NOutputDimensions =3>
class itk::AdvancedTransform;
```

Transform maps points, vectors and covariant vectors from an input space to an output space. 

**Par**: Registration Framework Support

Typically a Transform class has several methods for setting its parameters. For use in the registration framework, the parameters must also be represented by an array of doubles to allow communication with generic optimizers. The Array of transformation parameters is set using the SetParameters() method.

This abstract class define the generic interface for a geometrical transformation from one space to another. The class provides methods for mapping points, vectors and covariant vectors from the input space to the output space.

Given that transformation are not necessarily invertible, this basic class does not provide the methods for back transformation. Back transform methods are implemented in derived classes where appropriate.


Another requirement of the registration framework is the computation of the Jacobian of the transform T. In general, an ImageToImageMetric requires the knowledge of this Jacobian in order to compute the metric derivatives. The Jacobian is a matrix whose element are the partial derivatives of the transformation with respect to the array of parameters mu that defines the transform, evaluated at a point p: dT/dmu(p).

If penalty terms are included in the registration, the transforms also need to implement other derivatives of T. Often, penalty terms are functions of the spatial derivatives of T. Therefore, e.g. the SpatialJacobian dT/dx and the SpatialHessian d^2T/dx_idx_j require implementation. The GetValueAndDerivative() requires the d/dmu of those terms. Therefore, we additionally define [GetJacobianOfSpatialJacobian()](../Classes/classitk_1_1AdvancedTransform.md#function-getjacobianofspatialjacobian) and [GetJacobianOfSpatialHessian()](../Classes/classitk_1_1AdvancedTransform.md#function-getjacobianofspatialhessian). 

## Public Types Documentation

### typedef Self

```cpp
typedef AdvancedTransform itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::Self;
```


Standard class typedefs. 


### typedef Superclass

```cpp
typedef Transform< TScalarType, NInputDimensions, NOutputDimensions > itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer< Self > itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer< const Self > itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::ConstPointer;
```


### typedef ScalarType

```cpp
typedef Superclass::ScalarType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::ScalarType;
```


Typedefs from the Superclass. 


### typedef ParametersType

```cpp
typedef Superclass::ParametersType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::ParametersType;
```


### typedef ParametersValueType

```cpp
typedef Superclass::ParametersValueType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::ParametersValueType;
```


### typedef NumberOfParametersType

```cpp
typedef Superclass::NumberOfParametersType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::NumberOfParametersType;
```


### typedef DerivativeType

```cpp
typedef Superclass::DerivativeType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::DerivativeType;
```


### typedef JacobianType

```cpp
typedef Superclass::JacobianType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::JacobianType;
```


### typedef InputVectorType

```cpp
typedef Superclass::InputVectorType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::InputVectorType;
```


### typedef OutputVectorType

```cpp
typedef Superclass::OutputVectorType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::OutputVectorType;
```


### typedef InputCovariantVectorType

```cpp
typedef Superclass ::InputCovariantVectorType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::InputCovariantVectorType;
```


### typedef OutputCovariantVectorType

```cpp
typedef Superclass ::OutputCovariantVectorType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::OutputCovariantVectorType;
```


### typedef InputVnlVectorType

```cpp
typedef Superclass::InputVnlVectorType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::InputVnlVectorType;
```


### typedef OutputVnlVectorType

```cpp
typedef Superclass::OutputVnlVectorType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::OutputVnlVectorType;
```


### typedef InputPointType

```cpp
typedef Superclass::InputPointType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::InputPointType;
```


### typedef OutputPointType

```cpp
typedef Superclass::OutputPointType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::OutputPointType;
```


### typedef InverseTransformBaseType

```cpp
typedef Superclass::InverseTransformBaseType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::InverseTransformBaseType;
```


### typedef InverseTransformBasePointer

```cpp
typedef Superclass::InverseTransformBasePointer itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::InverseTransformBasePointer;
```


### typedef TransformType

```cpp
typedef Transform< TScalarType, NInputDimensions, NOutputDimensions > itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::TransformType;
```


Transform typedefs for the from Superclass. 


### typedef TransformTypePointer

```cpp
typedef TransformType::Pointer itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::TransformTypePointer;
```


### typedef TransformTypeConstPointer

```cpp
typedef TransformType::ConstPointer itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::TransformTypeConstPointer;
```


### typedef NonZeroJacobianIndicesType

```cpp
typedef std::vector< unsigned long > itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::NonZeroJacobianIndicesType;
```


Types for the (Spatial)Jacobian/Hessian. Using an itk::FixedArray instead of an std::vector gives a performance gain for the SpatialHessianType. 


### typedef SpatialJacobianType

```cpp
typedef Matrix< ScalarType, OutputSpaceDimension, InputSpaceDimension > itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::SpatialJacobianType;
```


### typedef JacobianOfSpatialJacobianType

```cpp
typedef std::vector< SpatialJacobianType > itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::JacobianOfSpatialJacobianType;
```


### typedef SpatialHessianType

```cpp
typedef FixedArray< Matrix< ScalarType, InputSpaceDimension, InputSpaceDimension >, OutputSpaceDimension > itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::SpatialHessianType;
```


### typedef JacobianOfSpatialHessianType

```cpp
typedef std::vector< SpatialHessianType > itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::JacobianOfSpatialHessianType;
```


### typedef InternalMatrixType

```cpp
typedef SpatialJacobianType::InternalMatrixType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::InternalMatrixType;
```


### typedef MovingImageGradientType

```cpp
typedef OutputCovariantVectorType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::MovingImageGradientType;
```


Typedef for the moving image gradient type. This type is defined by the B-spline interpolator as typedef CovariantVector< RealType, ImageDimension > As we cannot access this type we simply re-construct it to be identical. 


### typedef MovingImageGradientValueType

```cpp
typedef MovingImageGradientType::ValueType itk::AdvancedTransform< TScalarType, NInputDimensions, NOutputDimensions >::MovingImageGradientValueType;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    AdvancedTransform ,
    Transform 
)
```


New method for creating an object using a factory. Run-time type information (and related methods). 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    InputSpaceDimension ,
    unsigned int ,
    NInputDimensions 
)
```


Dimension of the domain space. 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    OutputSpaceDimension ,
    unsigned int ,
    NOutputDimensions 
)
```


### function GetNumberOfNonZeroJacobianIndices

```cpp
virtual NumberOfParametersType GetNumberOfNonZeroJacobianIndices(
    void 
) const
```


Get the number of nonzero Jacobian indices. By default all. 


### function itkGetConstMacro

```cpp
itkGetConstMacro(
    HasNonZeroSpatialHessian ,
    bool 
)
```


Whether the advanced transform has nonzero matrices. 


### function itkGetConstMacro

```cpp
itkGetConstMacro(
    HasNonZeroJacobianOfSpatialHessian ,
    bool 
)
```


### function GetJacobian

```cpp
virtual void GetJacobian(
    const InputPointType & ipp,
    JacobianType & j,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const =0
```


**Reimplemented by**: [itk::KernelTransform2::GetJacobian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobian)


This returns a sparse version of the Jacobian of the transformation.

The Jacobian is expressed as a vector of partial derivatives of the transformation components with respect to the parameters \(\mu\) that define the transformation \(T\), evaluated at a point \(p\).

\[ J=\left[ \begin{array}{cccc} \frac{\partial T_{1}}{\partial \mu_{1}}(p) & \frac{\partial T_{1}}{\partial \mu_{2}}(p) & \cdots & \frac{\partial T_{1}}{\partial \mu_{m}}(p) \\ \frac{\partial T_{2}}{\partial \mu_{1}}(p) & \frac{\partial T_{2}}{\partial \mu_{2}}(p) & \cdots & \frac{\partial T_{2}}{\partial \mu_{m}}(p) \\ \vdots & \vdots & \ddots & \vdots \\ \frac{\partial T_{d}}{\partial \mu_{1}}(p) & \frac{\partial T_{d}}{\partial \mu_{2}}(p) & \cdots & \frac{\partial T_{d}}{\partial \mu_{m}}(p) \end{array}\right], \] with \(m\) the number of parameters, i.e. the size of \(\mu\), and \(d\) the dimension of the image. 


### function EvaluateJacobianWithImageGradientProduct

```cpp
virtual void EvaluateJacobianWithImageGradientProduct(
    const InputPointType & ipp,
    const MovingImageGradientType & movingImageGradient,
    DerivativeType & imageJacobian,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const
```


Compute the inner product of the Jacobian with the moving image gradient. The Jacobian is (partially) constructed inside this function, but not returned. 


### function GetSpatialJacobian

```cpp
virtual void GetSpatialJacobian(
    const InputPointType & ipp,
    SpatialJacobianType & sj
) const =0
```


**Reimplemented by**: [itk::KernelTransform2::GetSpatialJacobian](../Classes/classitk_1_1KernelTransform2.md#function-getspatialjacobian), [itk::KernelTransform2::GetSpatialJacobian](../Classes/classitk_1_1KernelTransform2.md#function-getspatialjacobian)


Compute the spatial Jacobian of the transformation.

The spatial Jacobian is expressed as a vector of partial derivatives of the transformation components with respect to the spatial position \(x\), evaluated at a point \(p\).

\[ sJ=\left[ \begin{array}{cccc} \frac{\partial T_{1}}{\partial x_{1}}(p) & \frac{\partial T_{1}}{\partial x_{2}}(p) & \cdots & \frac{\partial T_{1}}{\partial x_{m}}(p) \\ \frac{\partial T_{2}}{\partial x_{1}}(p) & \frac{\partial T_{2}}{\partial x_{2}}(p) & \cdots & \frac{\partial T_{2}}{\partial x_{m}}(p) \\ \vdots & \vdots & \ddots & \vdots \\ \frac{\partial T_{d}}{\partial x_{1}}(p) & \frac{\partial T_{d}}{\partial x_{2}}(p) & \cdots & \frac{\partial T_{d}}{\partial x_{m}}(p) \end{array}\right], \] with \(m\) the number of parameters, i.e. the size of \(\mu\), and \(d\) the dimension of the image. 


### function ComputeJacobianWithRespectToParameters

```cpp
inline virtual void ComputeJacobianWithRespectToParameters(
    const InputPointType & itkNotUsedp,
    JacobianType & itkNotUsedj
) const
```


Override some pure virtual ITK4 functions. 


### function ComputeJacobianWithRespectToPosition

```cpp
inline virtual void ComputeJacobianWithRespectToPosition(
    const InputPointType & itkNotUsedp,
    JacobianType & itkNotUsedj
) const
```


### function GetSpatialHessian

```cpp
virtual void GetSpatialHessian(
    const InputPointType & ipp,
    SpatialHessianType & sh
) const =0
```


**Reimplemented by**: [itk::KernelTransform2::GetSpatialHessian](../Classes/classitk_1_1KernelTransform2.md#function-getspatialhessian), [itk::KernelTransform2::GetSpatialHessian](../Classes/classitk_1_1KernelTransform2.md#function-getspatialhessian)


Compute the spatial Hessian of the transformation.

The spatial Hessian is the vector of matrices of partial second order derivatives of the transformation components with respect to the spatial position \(x\), evaluated at a point \(p\).

\[ sH=\left[ \begin{array}{cc} \frac{\partial^2 T_{i}}{\partial x_{1} \partial x_{1}}(p) & \frac{\partial^2 T_{i}}{\partial x_{1} \partial x_{2}}(p) \\ \frac{\partial^2 T_{i}}{\partial x_{1} \partial x_{2}}(p) & \frac{\partial^2 T_{i}}{\partial x_{2} \partial x_{2}}(p) \\ \end{array}\right], \] with i the i-th component of the transformation. 


### function GetJacobianOfSpatialJacobian

```cpp
virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp,
    JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const =0
```


**Reimplemented by**: [itk::KernelTransform2::GetJacobianOfSpatialJacobian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobianofspatialjacobian)


Compute the Jacobian of the spatial Jacobian of the transformation.

The Jacobian of the spatial Jacobian is the derivative of the spatial Jacobian to the transformation parameters \(\mu\), evaluated at a point \(p\). 


### function GetJacobianOfSpatialJacobian

```cpp
virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp,
    SpatialJacobianType & sj,
    JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const =0
```


**Reimplemented by**: [itk::KernelTransform2::GetJacobianOfSpatialJacobian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobianofspatialjacobian)


Compute both the spatial Jacobian and the Jacobian of the spatial Jacobian of the transformation. 


### function GetJacobianOfSpatialHessian

```cpp
virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp,
    JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const =0
```


**Reimplemented by**: [itk::KernelTransform2::GetJacobianOfSpatialHessian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobianofspatialhessian)


Compute the Jacobian of the spatial Hessian of the transformation.

The Jacobian of the spatial Hessian is the derivative of the spatial Hessian to the transformation parameters \(\mu\), evaluated at a point \(p\). 


### function GetJacobianOfSpatialHessian

```cpp
virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp,
    SpatialHessianType & sh,
    JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const =0
```


**Reimplemented by**: [itk::KernelTransform2::GetJacobianOfSpatialHessian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobianofspatialhessian)


Compute both the spatial Hessian and the Jacobian of the spatial Hessian of the transformation. 


## Protected Functions Documentation

### function AdvancedTransform

```cpp
AdvancedTransform()
```


### function AdvancedTransform

```cpp
AdvancedTransform(
    NumberOfParametersType numberOfParameters
)
```


### function ~AdvancedTransform

```cpp
inline virtual ~AdvancedTransform()
```


## Protected Attributes Documentation

### variable m_HasNonZeroSpatialHessian

```cpp
bool m_HasNonZeroSpatialHessian;
```


### variable m_HasNonZeroJacobianOfSpatialHessian

```cpp
bool m_HasNonZeroJacobianOfSpatialHessian;
```


-------------------------------

Updated on 2021-11-17 at 00:54:19 +0000