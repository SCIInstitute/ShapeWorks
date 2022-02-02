---
title: itk::ThinPlateSplineKernelTransform2

---

# itk::ThinPlateSplineKernelTransform2



 [More...](#detailed-description)


`#include <itkThinPlateSplineKernelTransform2.h>`

Inherits from [itk::KernelTransform2< TScalarType, 3 >](../Classes/classitk_1_1KernelTransform2.md), [itk::AdvancedTransform< TScalarType, NDimensions, NDimensions >](../Classes/classitk_1_1AdvancedTransform.md), Transform< TScalarType, 3, 3 >

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ThinPlateSplineKernelTransform2](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md) | **[Self](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-self)**  |
| typedef [KernelTransform2](../Classes/classitk_1_1KernelTransform2.md)< TScalarType, NDimensions > | **[Superclass](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-superclass)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-constpointer)**  |
| typedef [Superclass::ScalarType](../Classes/classitk_1_1KernelTransform2.md#typedef-scalartype) | **[ScalarType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-scalartype)**  |
| typedef Superclass::ParametersType | **[ParametersType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-parameterstype)**  |
| typedef Superclass::JacobianType | **[JacobianType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-jacobiantype)**  |
| typedef Superclass::InputPointType | **[InputPointType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-inputpointtype)**  |
| typedef Superclass::OutputPointType | **[OutputPointType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-outputpointtype)**  |
| typedef Superclass::InputVectorType | **[InputVectorType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-inputvectortype)**  |
| typedef Superclass::OutputVectorType | **[OutputVectorType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-outputvectortype)**  |
| typedef Superclass::InputCovariantVectorType | **[InputCovariantVectorType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-inputcovariantvectortype)**  |
| typedef Superclass::OutputCovariantVectorType | **[OutputCovariantVectorType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-outputcovariantvectortype)**  |
| typedef Superclass::PointsIterator | **[PointsIterator](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-pointsiterator)**  |

## Protected Types

|                | Name           |
| -------------- | -------------- |
| typedef [Superclass::GMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-gmatrixtype) | **[GMatrixType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-gmatrixtype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkNewMacro](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-self) ) |
| | **[itkTypeMacro](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-itktypemacro)**([ThinPlateSplineKernelTransform2](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md) , [KernelTransform2](../Classes/classitk_1_1KernelTransform2.md) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-itkstaticconstmacro)**(SpaceDimension , unsigned int , Superclass::SpaceDimension ) |
| void | **[SetSigma](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-setsigma)**(double sigma) |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ThinPlateSplineKernelTransform2](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-thinplatesplinekerneltransform2)**() |
| virtual | **[~ThinPlateSplineKernelTransform2](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-~thinplatesplinekerneltransform2)**() |
| virtual void | **[ComputeG](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-computeg)**(const InputVectorType & x, [GMatrixType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-gmatrixtype) & GMatrix) const |
| virtual void | **[ComputeDeformationContribution](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#function-computedeformationcontribution)**(const [InputPointType](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md#typedef-inputpointtype) & inputPoint, OutputPointType & result) const |

## Additional inherited members

**Public Types inherited from [itk::KernelTransform2< TScalarType, 3 >](../Classes/classitk_1_1KernelTransform2.md)**

|                | Name           |
| -------------- | -------------- |
| typedef Superclass::NumberOfParametersType | **[NumberOfParametersType](../Classes/classitk_1_1KernelTransform2.md#typedef-numberofparameterstype)**  |
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
| typedef PointSetType::PointsContainerConstIterator | **[PointsConstIterator](../Classes/classitk_1_1KernelTransform2.md#typedef-pointsconstiterator)**  |
| typedef VectorContainer< unsigned long, InputVectorType > | **[VectorSetType](../Classes/classitk_1_1KernelTransform2.md#typedef-vectorsettype)**  |
| typedef VectorSetType::Pointer | **[VectorSetPointer](../Classes/classitk_1_1KernelTransform2.md#typedef-vectorsetpointer)**  |
| typedef vnl_matrix_fixed< TScalarType, NDimensions, NDimensions > | **[IMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-imatrixtype)**  |
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

**Protected Types inherited from [itk::KernelTransform2< TScalarType, 3 >](../Classes/classitk_1_1KernelTransform2.md)**

|                | Name           |
| -------------- | -------------- |
| typedef vnl_svd< [ScalarType](../Classes/classitk_1_1KernelTransform2.md#typedef-scalartype) > | **[SVDDecompositionType](../Classes/classitk_1_1KernelTransform2.md#typedef-svddecompositiontype)**  |
| typedef vnl_qr< [ScalarType](../Classes/classitk_1_1KernelTransform2.md#typedef-scalartype) > | **[QRDecompositionType](../Classes/classitk_1_1KernelTransform2.md#typedef-qrdecompositiontype)**  |

**Public Functions inherited from [itk::KernelTransform2< TScalarType, 3 >](../Classes/classitk_1_1KernelTransform2.md)**

|                | Name           |
| -------------- | -------------- |
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

**Protected Functions inherited from [itk::KernelTransform2< TScalarType, 3 >](../Classes/classitk_1_1KernelTransform2.md)**

|                | Name           |
| -------------- | -------------- |
| | **[KernelTransform2](../Classes/classitk_1_1KernelTransform2.md#function-kerneltransform2)**() |
| virtual | **[~KernelTransform2](../Classes/classitk_1_1KernelTransform2.md#function-~kerneltransform2)**() |
| void | **[PrintSelf](../Classes/classitk_1_1KernelTransform2.md#function-printself)**(std::ostream & os, Indent indent) const |
| virtual void | **[ComputeReflexiveG](../Classes/classitk_1_1KernelTransform2.md#function-computereflexiveg)**(PointsIterator , [GMatrixType](../Classes/classitk_1_1KernelTransform2.md#typedef-gmatrixtype) & GMatrix) const |
| void | **[ComputeK](../Classes/classitk_1_1KernelTransform2.md#function-computek)**(void ) |
| void | **[ComputeL](../Classes/classitk_1_1KernelTransform2.md#function-computel)**(void ) |
| void | **[ComputeP](../Classes/classitk_1_1KernelTransform2.md#function-computep)**(void ) |
| void | **[ComputeY](../Classes/classitk_1_1KernelTransform2.md#function-computey)**(void ) |
| void | **[ComputeD](../Classes/classitk_1_1KernelTransform2.md#function-computed)**(void ) |
| void | **[ReorganizeW](../Classes/classitk_1_1KernelTransform2.md#function-reorganizew)**(void ) |

**Public Attributes inherited from [itk::KernelTransform2< TScalarType, 3 >](../Classes/classitk_1_1KernelTransform2.md)**

|                | Name           |
| -------------- | -------------- |
| PointSetPointer | **[m_SourceLandmarks](../Classes/classitk_1_1KernelTransform2.md#variable-m-sourcelandmarks)**  |
| PointSetPointer | **[m_TargetLandmarks](../Classes/classitk_1_1KernelTransform2.md#variable-m-targetlandmarks)**  |

**Protected Attributes inherited from [itk::KernelTransform2< TScalarType, 3 >](../Classes/classitk_1_1KernelTransform2.md)**

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

**Public Types inherited from [itk::AdvancedTransform< TScalarType, NDimensions, NDimensions >](../Classes/classitk_1_1AdvancedTransform.md)**

|                | Name           |
| -------------- | -------------- |
| typedef Superclass::ParametersValueType | **[ParametersValueType](../Classes/classitk_1_1AdvancedTransform.md#typedef-parametersvaluetype)**  |
| typedef Superclass::NumberOfParametersType | **[NumberOfParametersType](../Classes/classitk_1_1AdvancedTransform.md#typedef-numberofparameterstype)**  |
| typedef Superclass::DerivativeType | **[DerivativeType](../Classes/classitk_1_1AdvancedTransform.md#typedef-derivativetype)**  |
| typedef Superclass::InputVnlVectorType | **[InputVnlVectorType](../Classes/classitk_1_1AdvancedTransform.md#typedef-inputvnlvectortype)**  |
| typedef Superclass::OutputVnlVectorType | **[OutputVnlVectorType](../Classes/classitk_1_1AdvancedTransform.md#typedef-outputvnlvectortype)**  |
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

**Public Functions inherited from [itk::AdvancedTransform< TScalarType, NDimensions, NDimensions >](../Classes/classitk_1_1AdvancedTransform.md)**

|                | Name           |
| -------------- | -------------- |
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
unsigned int NDimensions =3>
class itk::ThinPlateSplineKernelTransform2;
```


This class defines the thin plate spline (TPS) transformation. It is implemented in as straightforward a manner as possible from the IEEE TMI paper by Davis, Khotanzad, Flamig, and Harms, Vol. 16 No. 3 June 1997 

## Public Types Documentation

### typedef Self

```cpp
typedef ThinPlateSplineKernelTransform2 itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::Self;
```


Standard class typedefs. 


### typedef Superclass

```cpp
typedef KernelTransform2< TScalarType, NDimensions > itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::Superclass;
```


### typedef Pointer

```cpp
typedef SmartPointer< Self > itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer< const Self > itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::ConstPointer;
```


### typedef ScalarType

```cpp
typedef Superclass::ScalarType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::ScalarType;
```


Scalar type. 


### typedef ParametersType

```cpp
typedef Superclass::ParametersType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::ParametersType;
```


Parameters type. 


### typedef JacobianType

```cpp
typedef Superclass::JacobianType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::JacobianType;
```


Jacobian Type 


### typedef InputPointType

```cpp
typedef Superclass::InputPointType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::InputPointType;
```


These (rather redundant) typedefs are needed because on SGI, typedefs are not inherited. 


### typedef OutputPointType

```cpp
typedef Superclass::OutputPointType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::OutputPointType;
```


### typedef InputVectorType

```cpp
typedef Superclass::InputVectorType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::InputVectorType;
```


### typedef OutputVectorType

```cpp
typedef Superclass::OutputVectorType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::OutputVectorType;
```


### typedef InputCovariantVectorType

```cpp
typedef Superclass::InputCovariantVectorType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::InputCovariantVectorType;
```


### typedef OutputCovariantVectorType

```cpp
typedef Superclass::OutputCovariantVectorType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::OutputCovariantVectorType;
```


### typedef PointsIterator

```cpp
typedef Superclass::PointsIterator itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::PointsIterator;
```


## Protected Types Documentation

### typedef GMatrixType

```cpp
typedef Superclass::GMatrixType itk::ThinPlateSplineKernelTransform2< TScalarType, NDimensions >::GMatrixType;
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
    ThinPlateSplineKernelTransform2 ,
    KernelTransform2 
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


## Protected Functions Documentation

### function ThinPlateSplineKernelTransform2

```cpp
inline ThinPlateSplineKernelTransform2()
```


### function ~ThinPlateSplineKernelTransform2

```cpp
inline virtual ~ThinPlateSplineKernelTransform2()
```


### function ComputeG

```cpp
virtual void ComputeG(
    const InputVectorType & x,
    GMatrixType & GMatrix
) const
```


**Reimplements**: [itk::KernelTransform2::ComputeG](../Classes/classitk_1_1KernelTransform2.md#function-computeg)


Compute G(x) For the thin plate spline, this is: G(x) = r(x)*I \( G(x) = r(x)*I \) where r(x) = Euclidean norm = sqrt[x1^2 + x2^2 + x3^2] \[ r(x) = \sqrt{ x_1^2 + x_2^2 + x_3^2 } \] I = identity matrix. 


### function ComputeDeformationContribution

```cpp
virtual void ComputeDeformationContribution(
    const InputPointType & inputPoint,
    OutputPointType & result
) const
```


**Reimplements**: [itk::KernelTransform2::ComputeDeformationContribution](../Classes/classitk_1_1KernelTransform2.md#function-computedeformationcontribution)


Compute the contribution of the landmarks weighted by the kernel function to the global deformation of the space. 


-------------------------------

Updated on 2022-02-02 at 18:18:08 +0000