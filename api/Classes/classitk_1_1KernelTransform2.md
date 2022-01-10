---
title: itk::KernelTransform2

---

# itk::KernelTransform2



 [More...](#detailed-description)


`#include <itkKernelTransform2.h>`

Inherits from [itk::AdvancedTransform< TScalarType, NDimensions, NDimensions >](../Classes/classitk_1_1AdvancedTransform.md), Transform< TScalarType, 3, 3 >

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [KernelTransform2](../Classes/classitk_1_1KernelTransform2.md) | **[Self](../Classes/classitk_1_1KernelTransform2.md#typedef-self)**  |
| typedef [AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md)< TScalarType, NDimensions, NDimensions > | **[Superclass](../Classes/classitk_1_1KernelTransform2.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1KernelTransform2.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1KernelTransform2.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1KernelTransform2.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1KernelTransform2.md#typedef-constpointer)**  |
| typedef [Superclass::ScalarType](../Classes/classitk_1_1AdvancedTransform.md#typedef-scalartype) | **[ScalarType](../Classes/classitk_1_1KernelTransform2.md#typedef-scalartype)**  |
| typedef Superclass::ParametersType | **[ParametersType](../Classes/classitk_1_1KernelTransform2.md#typedef-parameterstype)**  |
| typedef Superclass::NumberOfParametersType | **[NumberOfParametersType](../Classes/classitk_1_1KernelTransform2.md#typedef-numberofparameterstype)**  |
| typedef Superclass::JacobianType | **[JacobianType](../Classes/classitk_1_1KernelTransform2.md#typedef-jacobiantype)**  |
| typedef Superclass::InputPointType | **[InputPointType](../Classes/classitk_1_1KernelTransform2.md#typedef-inputpointtype)**  |
| typedef Superclass::OutputPointType | **[OutputPointType](../Classes/classitk_1_1KernelTransform2.md#typedef-outputpointtype)**  |
| typedef Superclass::InputVectorType | **[InputVectorType](../Classes/classitk_1_1KernelTransform2.md#typedef-inputvectortype)**  |
| typedef Superclass::OutputVectorType | **[OutputVectorType](../Classes/classitk_1_1KernelTransform2.md#typedef-outputvectortype)**  |
| typedef Superclass::InputCovariantVectorType | **[InputCovariantVectorType](../Classes/classitk_1_1KernelTransform2.md#typedef-inputcovariantvectortype)**  |
| typedef Superclass::OutputCovariantVectorType | **[OutputCovariantVectorType](../Classes/classitk_1_1KernelTransform2.md#typedef-outputcovariantvectortype)**  |
| typedef Superclass::InputVnlVectorType | **[InputVnlVectorType](../Classes/classitk_1_1KernelTransform2.md#typedef-inputvnlvectortype)**  |
| typedef Superclass::OutputVnlVectorType | **[OutputVnlVectorType](../Classes/classitk_1_1KernelTransform2.md#typedef-outputvnlvectortype)**  |
| typedef [Superclass](../Classes/classitk_1_1AdvancedTransform.md) ::[NonZeroJacobianIndicesType](../Classes/classitk_1_1KernelTransform2.md#typedef-nonzerojacobianindicestype) | **[NonZeroJacobianIndicesType](../Classes/classitk_1_1KernelTransform2.md#typedef-nonzerojacobianindicestype)**  |
| typedef Superclass::SpatialJacobianType | **[SpatialJacobianType](../Classes/classitk_1_1KernelTransform2.md#typedef-spatialjacobiantype)**  |
| typedef [Superclass](../Classes/classitk_1_1AdvancedTransform.md) ::JacobianOfSpatialJacobianType | **[JacobianOfSpatialJacobianType](../Classes/classitk_1_1KernelTransform2.md#typedef-jacobianofspatialjacobiantype)**  |
| typedef Superclass::SpatialHessianType | **[SpatialHessianType](../Classes/classitk_1_1KernelTransform2.md#typedef-spatialhessiantype)**  |
| typedef [Superclass](../Classes/classitk_1_1AdvancedTransform.md) ::JacobianOfSpatialHessianType | **[JacobianOfSpatialHessianType](../Classes/classitk_1_1KernelTransform2.md#typedef-jacobianofspatialhessiantype)**  |
| typedef Superclass::InternalMatrixType | **[InternalMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-internalmatrixtype)**  |
| typedef DefaultStaticMeshTraits< TScalarType, NDimensions, NDimensions, TScalarType, TScalarType > | **[PointSetTraitsType](../Classes/classitk_1_1KernelTransform2.md#typedef-pointsettraitstype)**  |
| typedef PointSet< InputPointType, NDimensions, [PointSetTraitsType](../Classes/classitk_1_1KernelTransform2.md#typedef-pointsettraitstype) > | **[PointSetType](../Classes/classitk_1_1KernelTransform2.md#typedef-pointsettype)**  |
| typedef PointSetType::Pointer | **[PointSetPointer](../Classes/classitk_1_1KernelTransform2.md#typedef-pointsetpointer)**  |
| typedef PointSetType::PointsContainer | **[PointsContainer](../Classes/classitk_1_1KernelTransform2.md#typedef-pointscontainer)**  |
| typedef PointSetType::PointsContainerIterator | **[PointsIterator](../Classes/classitk_1_1KernelTransform2.md#typedef-pointsiterator)**  |
| typedef PointSetType::PointsContainerConstIterator | **[PointsConstIterator](../Classes/classitk_1_1KernelTransform2.md#typedef-pointsconstiterator)**  |
| typedef VectorContainer< unsigned long, InputVectorType > | **[VectorSetType](../Classes/classitk_1_1KernelTransform2.md#typedef-vectorsettype)**  |
| typedef VectorSetType::Pointer | **[VectorSetPointer](../Classes/classitk_1_1KernelTransform2.md#typedef-vectorsetpointer)**  |
| typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > | **[IMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-imatrixtype)**  |
| typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > | **[GMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-gmatrixtype)**  |
| typedef vnl_matrix< TScalarType > | **[LMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-lmatrixtype)**  |
| typedef vnl_matrix< TScalarType > | **[KMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-kmatrixtype)**  |
| typedef vnl_matrix< TScalarType > | **[PMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-pmatrixtype)**  |
| typedef vnl_matrix< TScalarType > | **[YMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-ymatrixtype)**  |
| typedef vnl_matrix< TScalarType > | **[WMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-wmatrixtype)**  |
| typedef vnl_matrix< TScalarType > | **[DMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-dmatrixtype)**  |
| typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > | **[AMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-amatrixtype)**  |
| typedef vnl_vector_fixed< TScalarType, NDimensions > | **[BMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-bmatrixtype)**  |
| typedef vnl_matrix_fixed< TScalarType, 1, NDimensions > | **[RowMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-rowmatrixtype)**  |
| typedef vnl_matrix_fixed< TScalarType, NDimensions, 1 > | **[ColumnMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-columnmatrixtype)**  |

## Protected Types

|                | Name           |
| -------------- | -------------- |
| typedef vnl_svd< [ScalarType](../Classes/classitk_1_1KernelTransform2.md#typedef-scalartype) > | **[SVDDecompositionType](../Classes/classitk_1_1KernelTransform2.md#typedef-svddecompositiontype)**  |
| typedef vnl_qr< [ScalarType](../Classes/classitk_1_1KernelTransform2.md#typedef-scalartype) > | **[QRDecompositionType](../Classes/classitk_1_1KernelTransform2.md#typedef-qrdecompositiontype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1KernelTransform2.md#function-itktypemacro)**([KernelTransform2](../Classes/classitk_1_1KernelTransform2.md) , [AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md) ) |
| | **[itkNewMacro](../Classes/classitk_1_1KernelTransform2.md#function-itknewmacro)**([Self](../Classes/classitk_1_1KernelTransform2.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1KernelTransform2.md#function-itkstaticconstmacro)**(SpaceDimension , unsigned int , NDimensions ) |
| virtual NumberOfParametersType | **[GetNumberOfParameters](../Classes/classitk_1_1KernelTransform2.md#function-getnumberofparameters)**(void ) const |
| | **[itkGetObjectMacro](../Classes/classitk_1_1KernelTransform2.md#function-itkgetobjectmacro)**(SourceLandmarks , PointSetType ) |
| virtual void | **[SetSourceLandmarks](../Classes/classitk_1_1KernelTransform2.md#function-setsourcelandmarks)**(PointSetType * ) |
| | **[itkGetObjectMacro](../Classes/classitk_1_1KernelTransform2.md#function-itkgetobjectmacro)**(TargetLandmarks , PointSetType ) |
| virtual void | **[SetTargetLandmarks](../Classes/classitk_1_1KernelTransform2.md#function-settargetlandmarks)**(PointSetType * ) |
| | **[itkGetObjectMacro](../Classes/classitk_1_1KernelTransform2.md#function-itkgetobjectmacro)**(Displacements , [VectorSetType](../Classes/classitk_1_1KernelTransform2.md#typedef-vectorsettype) ) |
| void | **[ComputeWMatrix](../Classes/classitk_1_1KernelTransform2.md#function-computewmatrix)**(void ) |
| void | **[ComputeLInverse](../Classes/classitk_1_1KernelTransform2.md#function-computelinverse)**(void ) |
| virtual OutputPointType | **[TransformPoint](../Classes/classitk_1_1KernelTransform2.md#function-transformpoint)**(const InputPointType & thisPoint) const |
| virtual OutputVectorType | **[TransformVector](../Classes/classitk_1_1KernelTransform2.md#function-transformvector)**(const InputVectorType & ) const |
| virtual OutputVnlVectorType | **[TransformVector](../Classes/classitk_1_1KernelTransform2.md#function-transformvector)**(const InputVnlVectorType & ) const |
| virtual OutputCovariantVectorType | **[TransformCovariantVector](../Classes/classitk_1_1KernelTransform2.md#function-transformcovariantvector)**(const InputCovariantVectorType & ) const |
| virtual void | **[GetJacobian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobian)**(const InputPointType & , JacobianType & , [NonZeroJacobianIndicesType](../Classes/classitk_1_1KernelTransform2.md#typedef-nonzerojacobianindicestype) & ) const |
| virtual void | **[SetIdentity](../Classes/classitk_1_1KernelTransform2.md#function-setidentity)**(void ) |
| virtual void | **[SetParameters](../Classes/classitk_1_1KernelTransform2.md#function-setparameters)**(const ParametersType & ) |
| virtual void | **[SetFixedParameters](../Classes/classitk_1_1KernelTransform2.md#function-setfixedparameters)**(const ParametersType & ) |
| virtual void | **[UpdateParameters](../Classes/classitk_1_1KernelTransform2.md#function-updateparameters)**(void ) |
| virtual const ParametersType & | **[GetParameters](../Classes/classitk_1_1KernelTransform2.md#function-getparameters)**(void ) const |
| virtual const ParametersType & | **[GetFixedParameters](../Classes/classitk_1_1KernelTransform2.md#function-getfixedparameters)**(void ) const |
| virtual void | **[SetStiffness](../Classes/classitk_1_1KernelTransform2.md#function-setstiffness)**(double stiffness) |
| | **[itkGetMacro](../Classes/classitk_1_1KernelTransform2.md#function-itkgetmacro)**(Stiffness , double ) |
| virtual void | **[SetAlpha](../Classes/classitk_1_1KernelTransform2.md#function-setalpha)**(TScalarType  itkNotUsedAlpha) |
| virtual TScalarType | **[GetAlpha](../Classes/classitk_1_1KernelTransform2.md#function-getalpha)**(void ) const |
| | **[itkSetMacro](../Classes/classitk_1_1KernelTransform2.md#function-itksetmacro)**(PoissonRatio , TScalarType ) |
| virtual const TScalarType | **[GetPoissonRatio](../Classes/classitk_1_1KernelTransform2.md#function-getpoissonratio)**(void ) const |
| | **[itkSetMacro](../Classes/classitk_1_1KernelTransform2.md#function-itksetmacro)**(MatrixInversionMethod , std::string ) |
| | **[itkGetConstReferenceMacro](../Classes/classitk_1_1KernelTransform2.md#function-itkgetconstreferencemacro)**(MatrixInversionMethod , std::string ) |
| virtual void | **[GetSpatialJacobian](../Classes/classitk_1_1KernelTransform2.md#function-getspatialjacobian)**(const InputPointType & ipp, SpatialJacobianType & sj) const |
| virtual void | **[GetSpatialHessian](../Classes/classitk_1_1KernelTransform2.md#function-getspatialhessian)**(const InputPointType & ipp, SpatialHessianType & sh) const |
| virtual void | **[GetJacobianOfSpatialJacobian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobianofspatialjacobian)**(const InputPointType & ipp, JacobianOfSpatialJacobianType & jsj, [NonZeroJacobianIndicesType](../Classes/classitk_1_1KernelTransform2.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const |
| virtual void | **[GetJacobianOfSpatialJacobian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobianofspatialjacobian)**(const InputPointType & ipp, SpatialJacobianType & sj, JacobianOfSpatialJacobianType & jsj, [NonZeroJacobianIndicesType](../Classes/classitk_1_1KernelTransform2.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const |
| virtual void | **[GetJacobianOfSpatialHessian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobianofspatialhessian)**(const InputPointType & ipp, JacobianOfSpatialHessianType & jsh, [NonZeroJacobianIndicesType](../Classes/classitk_1_1KernelTransform2.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const |
| virtual void | **[GetJacobianOfSpatialHessian](../Classes/classitk_1_1KernelTransform2.md#function-getjacobianofspatialhessian)**(const InputPointType & ipp, SpatialHessianType & sh, JacobianOfSpatialHessianType & jsh, [NonZeroJacobianIndicesType](../Classes/classitk_1_1KernelTransform2.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[KernelTransform2](../Classes/classitk_1_1KernelTransform2.md#function-kerneltransform2)**() |
| virtual | **[~KernelTransform2](../Classes/classitk_1_1KernelTransform2.md#function-~kerneltransform2)**() |
| void | **[PrintSelf](../Classes/classitk_1_1KernelTransform2.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual void | **[ComputeG](../Classes/classitk_1_1KernelTransform2.md#function-computeg)**(const InputVectorType & landmarkVector, [GMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-gmatrixtype) & GMatrix) const |
| virtual void | **[ComputeReflexiveG](../Classes/classitk_1_1KernelTransform2.md#function-computereflexiveg)**(PointsIterator , [GMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-gmatrixtype) & GMatrix) const |
| virtual void | **[ComputeDeformationContribution](../Classes/classitk_1_1KernelTransform2.md#function-computedeformationcontribution)**(const InputPointType & inputPoint, OutputPointType & result) const |
| void | **[ComputeK](../Classes/classitk_1_1KernelTransform2.md#function-computek)**(void ) |
| void | **[ComputeL](../Classes/classitk_1_1KernelTransform2.md#function-computel)**(void ) |
| void | **[ComputeP](../Classes/classitk_1_1KernelTransform2.md#function-computep)**(void ) |
| void | **[ComputeY](../Classes/classitk_1_1KernelTransform2.md#function-computey)**(void ) |
| void | **[ComputeD](../Classes/classitk_1_1KernelTransform2.md#function-computed)**(void ) |
| void | **[ReorganizeW](../Classes/classitk_1_1KernelTransform2.md#function-reorganizew)**(void ) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| PointSetPointer | **[m_SourceLandmarks](../Classes/classitk_1_1KernelTransform2.md#variable-m-sourcelandmarks)**  |
| PointSetPointer | **[m_TargetLandmarks](../Classes/classitk_1_1KernelTransform2.md#variable-m-targetlandmarks)**  |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| double | **[m_Stiffness](../Classes/classitk_1_1KernelTransform2.md#variable-m-stiffness)**  |
| VectorSetPointer | **[m_Displacements](../Classes/classitk_1_1KernelTransform2.md#variable-m-displacements)**  |
| [LMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-lmatrixtype) | **[m_LMatrix](../Classes/classitk_1_1KernelTransform2.md#variable-m-lmatrix)**  |
| [LMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-lmatrixtype) | **[m_LMatrixInverse](../Classes/classitk_1_1KernelTransform2.md#variable-m-lmatrixinverse)**  |
| [KMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-kmatrixtype) | **[m_KMatrix](../Classes/classitk_1_1KernelTransform2.md#variable-m-kmatrix)**  |
| [PMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-pmatrixtype) | **[m_PMatrix](../Classes/classitk_1_1KernelTransform2.md#variable-m-pmatrix)**  |
| [YMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-ymatrixtype) | **[m_YMatrix](../Classes/classitk_1_1KernelTransform2.md#variable-m-ymatrix)**  |
| [WMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-wmatrixtype) | **[m_WMatrix](../Classes/classitk_1_1KernelTransform2.md#variable-m-wmatrix)**  |
| [DMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-dmatrixtype) | **[m_DMatrix](../Classes/classitk_1_1KernelTransform2.md#variable-m-dmatrix)**  |
| [AMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-amatrixtype) | **[m_AMatrix](../Classes/classitk_1_1KernelTransform2.md#variable-m-amatrix)**  |
| [BMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-bmatrixtype) | **[m_BVector](../Classes/classitk_1_1KernelTransform2.md#variable-m-bvector)**  |
| bool | **[m_WMatrixComputed](../Classes/classitk_1_1KernelTransform2.md#variable-m-wmatrixcomputed)**  |
| bool | **[m_LMatrixComputed](../Classes/classitk_1_1KernelTransform2.md#variable-m-lmatrixcomputed)**  |
| bool | **[m_LInverseComputed](../Classes/classitk_1_1KernelTransform2.md#variable-m-linversecomputed)**  |
| bool | **[m_LMatrixDecompositionComputed](../Classes/classitk_1_1KernelTransform2.md#variable-m-lmatrixdecompositioncomputed)**  |
| [SVDDecompositionType](../Classes/classitk_1_1KernelTransform2.md#typedef-svddecompositiontype) * | **[m_LMatrixDecompositionSVD](../Classes/classitk_1_1KernelTransform2.md#variable-m-lmatrixdecompositionsvd)**  |
| QRDecompositionType * | **[m_LMatrixDecompositionQR](../Classes/classitk_1_1KernelTransform2.md#variable-m-lmatrixdecompositionqr)**  |
| [IMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-imatrixtype) | **[m_I](../Classes/classitk_1_1KernelTransform2.md#variable-m-i)**  |
| [NonZeroJacobianIndicesType](../Classes/classitk_1_1KernelTransform2.md#typedef-nonzerojacobianindicestype) | **[m_NonZeroJacobianIndices](../Classes/classitk_1_1KernelTransform2.md#variable-m-nonzerojacobianindices)**  |
| [NonZeroJacobianIndicesType](../Classes/classitk_1_1KernelTransform2.md#typedef-nonzerojacobianindicestype) | **[m_NonZeroJacobianIndicesTemp](../Classes/classitk_1_1KernelTransform2.md#variable-m-nonzerojacobianindicestemp)**  |
| bool | **[m_FastComputationPossible](../Classes/classitk_1_1KernelTransform2.md#variable-m-fastcomputationpossible)**  |

## Additional inherited members

**Public Types inherited from [itk::AdvancedTransform< TScalarType, NDimensions, NDimensions >](../Classes/classitk_1_1AdvancedTransform.md)**

|                | Name           |
| -------------- | -------------- |
| typedef Superclass::ParametersValueType | **[ParametersValueType](../Classes/classitk_1_1AdvancedTransform.md#typedef-parametersvaluetype)**  |
| typedef Superclass::DerivativeType | **[DerivativeType](../Classes/classitk_1_1AdvancedTransform.md#typedef-derivativetype)**  |
| typedef Superclass::InverseTransformBaseType | **[InverseTransformBaseType](../Classes/classitk_1_1AdvancedTransform.md#typedef-inversetransformbasetype)**  |
| typedef Superclass::InverseTransformBasePointer | **[InverseTransformBasePointer](../Classes/classitk_1_1AdvancedTransform.md#typedef-inversetransformbasepointer)**  |
| typedef Transform< TScalarType, NInputDimensions, NOutputDimensions > | **[TransformType](../Classes/classitk_1_1AdvancedTransform.md#typedef-transformtype)**  |
| typedef TransformType::Pointer | **[TransformTypePointer](../Classes/classitk_1_1AdvancedTransform.md#typedef-transformtypepointer)**  |
| typedef TransformType::ConstPointer | **[TransformTypeConstPointer](../Classes/classitk_1_1AdvancedTransform.md#typedef-transformtypeconstpointer)**  |
| typedef OutputCovariantVectorType | **[MovingImageGradientType](../Classes/classitk_1_1AdvancedTransform.md#typedef-movingimagegradienttype)**  |
| typedef MovingImageGradientType::ValueType | **[MovingImageGradientValueType](../Classes/classitk_1_1AdvancedTransform.md#typedef-movingimagegradientvaluetype)**  |

**Public Functions inherited from [itk::AdvancedTransform< TScalarType, NDimensions, NDimensions >](../Classes/classitk_1_1AdvancedTransform.md)**

|                | Name           |
| -------------- | -------------- |
| virtual NumberOfParametersType | **[GetNumberOfNonZeroJacobianIndices](../Classes/classitk_1_1AdvancedTransform.md#function-getnumberofnonzerojacobianindices)**(void ) const |
| | **[itkGetConstMacro](../Classes/classitk_1_1AdvancedTransform.md#function-itkgetconstmacro)**(HasNonZeroSpatialHessian , bool ) |
| | **[itkGetConstMacro](../Classes/classitk_1_1AdvancedTransform.md#function-itkgetconstmacro)**(HasNonZeroJacobianOfSpatialHessian , bool ) |
| virtual void | **[EvaluateJacobianWithImageGradientProduct](../Classes/classitk_1_1AdvancedTransform.md#function-evaluatejacobianwithimagegradientproduct)**(const InputPointType & ipp, const [MovingImageGradientType](../Classes/classitk_1_1AdvancedTransform.md#typedef-movingimagegradienttype) & movingImageGradient, DerivativeType & imageJacobian, [NonZeroJacobianIndicesType](../Classes/classitk_1_1AdvancedTransform.md#typedef-nonzerojacobianindicestype) & nonZeroJacobianIndices) const |
| virtual void | **[ComputeJacobianWithRespectToParameters](../Classes/classitk_1_1AdvancedTransform.md#function-computejacobianwithrespecttoparameters)**(const InputPointType & itkNotUsedp, JacobianType & itkNotUsedj) const |
| virtual void | **[ComputeJacobianWithRespectToPosition](../Classes/classitk_1_1AdvancedTransform.md#function-computejacobianwithrespecttoposition)**(const InputPointType & itkNotUsedp, JacobianType & itkNotUsedj) const |

**Protected Functions inherited from [itk::AdvancedTransform< TScalarType, NDimensions, NDimensions >](../Classes/classitk_1_1AdvancedTransform.md)**

|                | Name           |
| -------------- | -------------- |
| | **[AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md#function-advancedtransform)**() |
| | **[AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md#function-advancedtransform)**(NumberOfParametersType numberOfParameters) |
| virtual | **[~AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md#function-~advancedtransform)**() |

**Protected Attributes inherited from [itk::AdvancedTransform< TScalarType, NDimensions, NDimensions >](../Classes/classitk_1_1AdvancedTransform.md)**

|                | Name           |
| -------------- | -------------- |
| bool | **[m_HasNonZeroSpatialHessian](../Classes/classitk_1_1AdvancedTransform.md#variable-m-hasnonzerospatialhessian)**  |
| bool | **[m_HasNonZeroJacobianOfSpatialHessian](../Classes/classitk_1_1AdvancedTransform.md#variable-m-hasnonzerojacobianofspatialhessian)**  |


## Detailed Description

```cpp
template <class TScalarType ,
unsigned int NDimensions>
class itk::KernelTransform2;
```


Intended to be a base class for elastic body spline and thin plate spline. This is implemented in as straightforward a manner as possible from the IEEE TMI paper by Davis, Khotanzad, Flamig, and Harms, Vol. 16, No. 3 June 1997. Notation closely follows their paper, so if you have it in front of you, this code will make a lot more sense.

[KernelTransform2](../Classes/classitk_1_1KernelTransform2.md): Provides support for defining source and target landmarks Defines a number of data types used in the computations Defines the mathematical framework used to compute all splines, so that subclasses need only provide a kernel specific to that spline

This formulation allows the stiffness of the spline to be adjusted, allowing the spline to vary from interpolating the landmarks to approximating the landmarks. This part of the formulation is based on the short paper by R. Sprengel, K. Rohr, H. Stiehl. "Thin-Plate Spline Approximation for Image
Registration". In 18th International Conference of the IEEE Engineering in Medicine and Biology Society. 1996.

This class was modified to support its use in the ITK registration framework by Rupert Brooks, McGill Centre for Intelligent Machines, Montreal, Canada March 2007. See the Insight Journal Paper by Brooks, R., Arbel, T. "Improvements to the itk::KernelTransform and its subclasses."

Modified to include it in elastix:

* style
* make it inherit from [AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md)
* make it threadsafe, like was done in the itk as well.
* Support for matrix inversion by QR decomposition, instead of SVD. QR is much faster. Used in [SetParameters()](../Classes/classitk_1_1KernelTransform2.md#function-setparameters) and [SetFixedParameters()](../Classes/classitk_1_1KernelTransform2.md#function-setfixedparameters).
* Much faster Jacobian computation for some of the derived kernel transforms. 

## Public Types Documentation

### typedef Self

```cpp
typedef KernelTransform2 itk::KernelTransform2< TScalarType, NDimensions >::Self;
```


Standard class typedefs. 


### typedef Superclass

```cpp
typedef AdvancedTransform< TScalarType, NDimensions, NDimensions > itk::KernelTransform2< TScalarType, NDimensions >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer< Self > itk::KernelTransform2< TScalarType, NDimensions >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer< const Self > itk::KernelTransform2< TScalarType, NDimensions >::ConstPointer;
```


### typedef ScalarType

```cpp
typedef Superclass::ScalarType itk::KernelTransform2< TScalarType, NDimensions >::ScalarType;
```


Typedefs. 


### typedef ParametersType

```cpp
typedef Superclass::ParametersType itk::KernelTransform2< TScalarType, NDimensions >::ParametersType;
```


### typedef NumberOfParametersType

```cpp
typedef Superclass::NumberOfParametersType itk::KernelTransform2< TScalarType, NDimensions >::NumberOfParametersType;
```


### typedef JacobianType

```cpp
typedef Superclass::JacobianType itk::KernelTransform2< TScalarType, NDimensions >::JacobianType;
```


### typedef InputPointType

```cpp
typedef Superclass::InputPointType itk::KernelTransform2< TScalarType, NDimensions >::InputPointType;
```


### typedef OutputPointType

```cpp
typedef Superclass::OutputPointType itk::KernelTransform2< TScalarType, NDimensions >::OutputPointType;
```


### typedef InputVectorType

```cpp
typedef Superclass::InputVectorType itk::KernelTransform2< TScalarType, NDimensions >::InputVectorType;
```


### typedef OutputVectorType

```cpp
typedef Superclass::OutputVectorType itk::KernelTransform2< TScalarType, NDimensions >::OutputVectorType;
```


### typedef InputCovariantVectorType

```cpp
typedef Superclass::InputCovariantVectorType itk::KernelTransform2< TScalarType, NDimensions >::InputCovariantVectorType;
```


### typedef OutputCovariantVectorType

```cpp
typedef Superclass::OutputCovariantVectorType itk::KernelTransform2< TScalarType, NDimensions >::OutputCovariantVectorType;
```


### typedef InputVnlVectorType

```cpp
typedef Superclass::InputVnlVectorType itk::KernelTransform2< TScalarType, NDimensions >::InputVnlVectorType;
```


### typedef OutputVnlVectorType

```cpp
typedef Superclass::OutputVnlVectorType itk::KernelTransform2< TScalarType, NDimensions >::OutputVnlVectorType;
```


### typedef NonZeroJacobianIndicesType

```cpp
typedef Superclass ::NonZeroJacobianIndicesType itk::KernelTransform2< TScalarType, NDimensions >::NonZeroJacobianIndicesType;
```


[AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md) typedefs. 


### typedef SpatialJacobianType

```cpp
typedef Superclass::SpatialJacobianType itk::KernelTransform2< TScalarType, NDimensions >::SpatialJacobianType;
```


### typedef JacobianOfSpatialJacobianType

```cpp
typedef Superclass ::JacobianOfSpatialJacobianType itk::KernelTransform2< TScalarType, NDimensions >::JacobianOfSpatialJacobianType;
```


### typedef SpatialHessianType

```cpp
typedef Superclass::SpatialHessianType itk::KernelTransform2< TScalarType, NDimensions >::SpatialHessianType;
```


### typedef JacobianOfSpatialHessianType

```cpp
typedef Superclass ::JacobianOfSpatialHessianType itk::KernelTransform2< TScalarType, NDimensions >::JacobianOfSpatialHessianType;
```


### typedef InternalMatrixType

```cpp
typedef Superclass::InternalMatrixType itk::KernelTransform2< TScalarType, NDimensions >::InternalMatrixType;
```


### typedef PointSetTraitsType

```cpp
typedef DefaultStaticMeshTraits< TScalarType, NDimensions, NDimensions, TScalarType, TScalarType > itk::KernelTransform2< TScalarType, NDimensions >::PointSetTraitsType;
```


PointList typedef. This type is used for maintaining lists of points, specifically, the source and target landmark lists. 


### typedef PointSetType

```cpp
typedef PointSet< InputPointType, NDimensions, PointSetTraitsType > itk::KernelTransform2< TScalarType, NDimensions >::PointSetType;
```


### typedef PointSetPointer

```cpp
typedef PointSetType::Pointer itk::KernelTransform2< TScalarType, NDimensions >::PointSetPointer;
```


### typedef PointsContainer

```cpp
typedef PointSetType::PointsContainer itk::KernelTransform2< TScalarType, NDimensions >::PointsContainer;
```


### typedef PointsIterator

```cpp
typedef PointSetType::PointsContainerIterator itk::KernelTransform2< TScalarType, NDimensions >::PointsIterator;
```


### typedef PointsConstIterator

```cpp
typedef PointSetType::PointsContainerConstIterator itk::KernelTransform2< TScalarType, NDimensions >::PointsConstIterator;
```


### typedef VectorSetType

```cpp
typedef VectorContainer< unsigned long, InputVectorType > itk::KernelTransform2< TScalarType, NDimensions >::VectorSetType;
```


VectorSet typedef. 


### typedef VectorSetPointer

```cpp
typedef VectorSetType::Pointer itk::KernelTransform2< TScalarType, NDimensions >::VectorSetPointer;
```


### typedef IMatrixType

```cpp
typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > itk::KernelTransform2< TScalarType, NDimensions >::IMatrixType;
```


'I' (identity) matrix typedef. 


### typedef GMatrixType

```cpp
typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > itk::KernelTransform2< TScalarType, NDimensions >::GMatrixType;
```


'G' matrix typedef. 


### typedef LMatrixType

```cpp
typedef vnl_matrix< TScalarType > itk::KernelTransform2< TScalarType, NDimensions >::LMatrixType;
```


'L' matrix typedef. 


### typedef KMatrixType

```cpp
typedef vnl_matrix< TScalarType > itk::KernelTransform2< TScalarType, NDimensions >::KMatrixType;
```


'K' matrix typedef. 


### typedef PMatrixType

```cpp
typedef vnl_matrix< TScalarType > itk::KernelTransform2< TScalarType, NDimensions >::PMatrixType;
```


'P' matrix typedef. 


### typedef YMatrixType

```cpp
typedef vnl_matrix< TScalarType > itk::KernelTransform2< TScalarType, NDimensions >::YMatrixType;
```


'Y' matrix typedef. 


### typedef WMatrixType

```cpp
typedef vnl_matrix< TScalarType > itk::KernelTransform2< TScalarType, NDimensions >::WMatrixType;
```


'W' matrix typedef. 


### typedef DMatrixType

```cpp
typedef vnl_matrix< TScalarType > itk::KernelTransform2< TScalarType, NDimensions >::DMatrixType;
```


'D' matrix typedef. Deformation component 


### typedef AMatrixType

```cpp
typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > itk::KernelTransform2< TScalarType, NDimensions >::AMatrixType;
```


'A' matrix typedef. Rotational part of the Affine component 


### typedef BMatrixType

```cpp
typedef vnl_vector_fixed< TScalarType, NDimensions > itk::KernelTransform2< TScalarType, NDimensions >::BMatrixType;
```


'B' matrix typedef. Translational part of the Affine component 


### typedef RowMatrixType

```cpp
typedef vnl_matrix_fixed< TScalarType, 1, NDimensions > itk::KernelTransform2< TScalarType, NDimensions >::RowMatrixType;
```


Row matrix typedef. 


### typedef ColumnMatrixType

```cpp
typedef vnl_matrix_fixed< TScalarType, NDimensions, 1 > itk::KernelTransform2< TScalarType, NDimensions >::ColumnMatrixType;
```


Column matrix typedef. 


## Protected Types Documentation

### typedef SVDDecompositionType

```cpp
typedef vnl_svd< ScalarType > itk::KernelTransform2< TScalarType, NDimensions >::SVDDecompositionType;
```


Decompositions, needed for the L matrix. These decompositions are cached for performance reasons during registration. During registration, in every iteration [SetParameters()](../Classes/classitk_1_1KernelTransform2.md#function-setparameters) is called, which in turn calls [ComputeWMatrix()](../Classes/classitk_1_1KernelTransform2.md#function-computewmatrix). The L matrix is not changed however, and therefore it is not needed to redo the decomposition. 


### typedef QRDecompositionType

```cpp
typedef vnl_qr< ScalarType > itk::KernelTransform2< TScalarType, NDimensions >::QRDecompositionType;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    KernelTransform2 ,
    AdvancedTransform 
)
```


Run-time type information (and related methods). 


### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


New macro for creation of through a Smart Pointer. 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    SpaceDimension ,
    unsigned int ,
    NDimensions 
)
```


Dimension of the domain space. 


### function GetNumberOfParameters

```cpp
inline virtual NumberOfParametersType GetNumberOfParameters(
    void 
) const
```


Return the number of parameters that completely define the Transform. 


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
)
```


Compute W matrix. 


### function ComputeLInverse

```cpp
void ComputeLInverse(
    void 
)
```


Compute L matrix inverse. 


### function TransformPoint

```cpp
virtual OutputPointType TransformPoint(
    const InputPointType & thisPoint
) const
```


Compute the position of point in the new space 


### function TransformVector

```cpp
inline virtual OutputVectorType TransformVector(
    const InputVectorType & 
) const
```


These vector transforms are not implemented for this transform. 


### function TransformVector

```cpp
inline virtual OutputVnlVectorType TransformVector(
    const InputVnlVectorType & 
) const
```


### function TransformCovariantVector

```cpp
inline virtual OutputCovariantVectorType TransformCovariantVector(
    const InputCovariantVectorType & 
) const
```


### function GetJacobian

```cpp
virtual void GetJacobian(
    const InputPointType & ,
    JacobianType & ,
    NonZeroJacobianIndicesType & 
) const
```


Compute the Jacobian of the transformation. 


### function SetIdentity

```cpp
virtual void SetIdentity(
    void 
)
```


Set the Transformation Parameters to be an identity transform. 


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
)
```


Update the Parameters array from the landmarks coordinates. 


### function GetParameters

```cpp
virtual const ParametersType & GetParameters(
    void 
) const
```


Get the Transformation Parameters - Gets the source landmarks. 


### function GetFixedParameters

```cpp
virtual const ParametersType & GetFixedParameters(
    void 
) const
```


Get Transform Fixed Parameters - Gets the target landmarks. 


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


### function SetAlpha

```cpp
inline virtual void SetAlpha(
    TScalarType  itkNotUsedAlpha
)
```


This method makes only sense for the ElasticBody splines. Declare here, so that you can always call it if you don't know the type of kernel beforehand. It will be overridden in the ElasticBodySplineKernelTransform and in the ElasticBodyReciprocalSplineKernelTransform. 


### function GetAlpha

```cpp
inline virtual TScalarType GetAlpha(
    void 
) const
```


### function itkSetMacro

```cpp
itkSetMacro(
    PoissonRatio ,
    TScalarType 
)
```


This method makes only sense for the ElasticBody splines. Declare here, so that you can always call it if you don't know the type of kernel beforehand. It will be overridden in the ElasticBodySplineKernelTransform and in the ElasticBodyReciprocalSplineKernelTransform. 


### function GetPoissonRatio

```cpp
inline virtual const TScalarType GetPoissonRatio(
    void 
) const
```


### function itkSetMacro

```cpp
itkSetMacro(
    MatrixInversionMethod ,
    std::string 
)
```


Matrix inversion by SVD or QR decomposition. 


### function itkGetConstReferenceMacro

```cpp
itkGetConstReferenceMacro(
    MatrixInversionMethod ,
    std::string 
)
```


### function GetSpatialJacobian

```cpp
inline virtual void GetSpatialJacobian(
    const InputPointType & ipp,
    SpatialJacobianType & sj
) const
```


**Reimplements**: [itk::AdvancedTransform::GetSpatialJacobian](../Classes/classitk_1_1AdvancedTransform.md#function-getspatialjacobian)


Must be provided. 


### function GetSpatialHessian

```cpp
inline virtual void GetSpatialHessian(
    const InputPointType & ipp,
    SpatialHessianType & sh
) const
```


**Reimplements**: [itk::AdvancedTransform::GetSpatialHessian](../Classes/classitk_1_1AdvancedTransform.md#function-getspatialhessian)


Compute the spatial Hessian of the transformation.

The spatial Hessian is the vector of matrices of partial second order derivatives of the transformation components with respect to the spatial position \(x\), evaluated at a point \(p\).

\[ sH=\left[ \begin{array}{cc} \frac{\partial^2 T_{i}}{\partial x_{1} \partial x_{1}}(p) & \frac{\partial^2 T_{i}}{\partial x_{1} \partial x_{2}}(p) \\ \frac{\partial^2 T_{i}}{\partial x_{1} \partial x_{2}}(p) & \frac{\partial^2 T_{i}}{\partial x_{2} \partial x_{2}}(p) \\ \end{array}\right], \] with i the i-th component of the transformation. 


### function GetJacobianOfSpatialJacobian

```cpp
inline virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp,
    JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const
```


### function GetJacobianOfSpatialJacobian

```cpp
inline virtual void GetJacobianOfSpatialJacobian(
    const InputPointType & ipp,
    SpatialJacobianType & sj,
    JacobianOfSpatialJacobianType & jsj,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const
```


### function GetJacobianOfSpatialHessian

```cpp
inline virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp,
    JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const
```


### function GetJacobianOfSpatialHessian

```cpp
inline virtual void GetJacobianOfSpatialHessian(
    const InputPointType & ipp,
    SpatialHessianType & sh,
    JacobianOfSpatialHessianType & jsh,
    NonZeroJacobianIndicesType & nonZeroJacobianIndices
) const
```


## Protected Functions Documentation

### function KernelTransform2

```cpp
KernelTransform2()
```


### function ~KernelTransform2

```cpp
virtual ~KernelTransform2()
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
virtual void ComputeG(
    const InputVectorType & landmarkVector,
    GMatrixType & GMatrix
) const
```


**Reimplemented by**: [itk::ThinPlateSplineKernelTransform2::ComputeG](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-computeg)


Compute G(x) This is essentially the kernel of the transform. By overriding this method, we can obtain (among others): Elastic body spline Thin plate spline Volume spline. 


### function ComputeReflexiveG

```cpp
virtual void ComputeReflexiveG(
    PointsIterator ,
    GMatrixType & GMatrix
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


**Reimplemented by**: [itk::ThinPlateSplineKernelTransform2::ComputeDeformationContribution](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-computedeformationcontribution)


Compute the contribution of the landmarks weighted by the kernel function to the global deformation of the space. 


### function ComputeK

```cpp
void ComputeK(
    void 
)
```


Compute K matrix. 


### function ComputeL

```cpp
void ComputeL(
    void 
)
```


Compute L matrix. 


### function ComputeP

```cpp
void ComputeP(
    void 
)
```


Compute P matrix. 


### function ComputeY

```cpp
void ComputeY(
    void 
)
```


Compute Y matrix. 


### function ComputeD

```cpp
void ComputeD(
    void 
)
```


Compute displacements \( q_i - p_i \). 


### function ReorganizeW

```cpp
void ReorganizeW(
    void 
)
```


**Warning**: This method release the memory of the W Matrix. 

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


Stiffness parameter. 


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


The Deformation matrix. This is an auxiliary matrix that will hold the Deformation (non-affine) part of the transform. Those are the coefficients that will multiply the Kernel function. 


### variable m_AMatrix

```cpp
AMatrixType m_AMatrix;
```


Rotational/Shearing part of the Affine component of the Transformation. 


### variable m_BVector

```cpp
BMatrixType m_BVector;
```


Translational part of the Affine component of the Transformation. 


### variable m_WMatrixComputed

```cpp
bool m_WMatrixComputed;
```


The G matrix. It used to be mutable because m_GMatrix was made an ivar only to avoid copying the matrix at return time but this is not necessary. SK: we don't need this matrix anymore as a member. Has the W matrix been computed? 


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


### variable m_LMatrixDecompositionComputed

```cpp
bool m_LMatrixDecompositionComputed;
```


Has the L matrix decomposition been computed? 


### variable m_LMatrixDecompositionSVD

```cpp
SVDDecompositionType * m_LMatrixDecompositionSVD;
```


### variable m_LMatrixDecompositionQR

```cpp
QRDecompositionType * m_LMatrixDecompositionQR;
```


### variable m_I

```cpp
IMatrixType m_I;
```


Identity matrix. 


### variable m_NonZeroJacobianIndices

```cpp
NonZeroJacobianIndicesType m_NonZeroJacobianIndices;
```


Precomputed nonzero Jacobian indices (simply all params) 


### variable m_NonZeroJacobianIndicesTemp

```cpp
NonZeroJacobianIndicesType m_NonZeroJacobianIndicesTemp;
```


for old [GetJacobian()](../Classes/classitk_1_1KernelTransform2.md#function-getjacobian) method: 


### variable m_FastComputationPossible

```cpp
bool m_FastComputationPossible;
```


The Jacobian can be computed much faster for some of the derived kerbel transforms, most notably the TPS. 


-------------------------------

Updated on 2022-01-10 at 16:27:25 +0000