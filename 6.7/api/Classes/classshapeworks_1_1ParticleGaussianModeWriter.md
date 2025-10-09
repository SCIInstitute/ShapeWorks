---
title: shapeworks::ParticleGaussianModeWriter

---

# shapeworks::ParticleGaussianModeWriter



 [More...](#detailed-description)

Inherits from itk::DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleGaussianModeWriter](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md) | **[Self](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-self)**  |
| typedef itk::SmartPointer< [Self](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-self) > | **[Pointer](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-pointer)**  |
| typedef itk::SmartPointer< const [Self](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-self) > | **[ConstPointer](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-constpointer)**  |
| typedef DataObject | **[Superclass](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-superclass)**  |
| typedef [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) | **[ParticleSystemType](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-particlesystemtype)**  |
| typedef [LegacyShapeMatrix](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) | **[ShapeMatrixType](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-shapematrixtype)**  |
| typedef [ShapeMatrixType::DataType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md#typedef-datatype) | **[DataType](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-datatype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classshapeworks_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-pointtype)**  |
| typedef vnl_vector< DataType > | **[vnl_vector_type](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-vnl-vector-type)**  |
| typedef vnl_matrix< DataType > | **[vnl_matrix_type](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-vnl-matrix-type)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-itktypemacro)**([ParticleGaussianModeWriter](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md) , DataObject ) |
| | **[itkNewMacro](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-itknewmacro)**([Self](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| void | **[Update](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-update)**() const |
| int | **[GetNumberOfModes](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-getnumberofmodes)**() const |
| void | **[SetNumberOfModes](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-setnumberofmodes)**(int b) |
| void | **[SetShapeMatrix](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-setshapematrix)**([ShapeMatrixType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * s) |
| [ShapeMatrixType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * | **[GetShapeMatrix](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-getshapematrix)**() |
| const [ShapeMatrixType](../Classes/classshapeworks_1_1LegacyShapeMatrix.md) * | **[GetShapeMatrix](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-getshapematrix)**() const |
| void | **[SetFileName](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-setfilename)**(const std::string & s) |
| void | **[SetFileName](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-setfilename)**(const char * s) |
| const std::string & | **[GetFileName](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-getfilename)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleGaussianModeWriter](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-particlegaussianmodewriter)**() |
| virtual | **[~ParticleGaussianModeWriter](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-~particlegaussianmodewriter)**() |
| void | **[operator=](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-operator=)**(const [ParticleGaussianModeWriter](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md) & ) |
| | **[ParticleGaussianModeWriter](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#function-particlegaussianmodewriter)**(const [ParticleGaussianModeWriter](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md) & ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| ShapeMatrixType::Pointer | **[m_ShapeMatrix](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#variable-m-shapematrix)**  |
| int | **[m_NumberOfModes](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#variable-m-numberofmodes)**  |
| std::string | **[m_FileName](../Classes/classshapeworks_1_1ParticleGaussianModeWriter.md#variable-m-filename)**  |

## Detailed Description

```cpp
template <unsigned int VDimension>
class shapeworks::ParticleGaussianModeWriter;
```

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleGaussianModeWriter shapeworks::ParticleGaussianModeWriter< VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef itk::SmartPointer<Self> shapeworks::ParticleGaussianModeWriter< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef itk::SmartPointer<const Self> shapeworks::ParticleGaussianModeWriter< VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef DataObject shapeworks::ParticleGaussianModeWriter< VDimension >::Superclass;
```


### typedef ParticleSystemType

```cpp
typedef ParticleSystem shapeworks::ParticleGaussianModeWriter< VDimension >::ParticleSystemType;
```


Type of particle system and associated classes. 


### typedef ShapeMatrixType

```cpp
typedef LegacyShapeMatrix shapeworks::ParticleGaussianModeWriter< VDimension >::ShapeMatrixType;
```


### typedef DataType

```cpp
typedef ShapeMatrixType::DataType shapeworks::ParticleGaussianModeWriter< VDimension >::DataType;
```


### typedef PointType

```cpp
typedef ParticleSystemType::PointType shapeworks::ParticleGaussianModeWriter< VDimension >::PointType;
```


Vector & Point types. 


### typedef vnl_vector_type

```cpp
typedef vnl_vector<DataType> shapeworks::ParticleGaussianModeWriter< VDimension >::vnl_vector_type;
```


### typedef vnl_matrix_type

```cpp
typedef vnl_matrix<DataType> shapeworks::ParticleGaussianModeWriter< VDimension >::vnl_matrix_type;
```


## Public Functions Documentation

### function itkTypeMacro

```cpp
itkTypeMacro(
    ParticleGaussianModeWriter ,
    DataObject 
)
```


### function itkNewMacro

```cpp
itkNewMacro(
    Self 
)
```


Method for creation through the object factory. 


### function itkStaticConstMacro

```cpp
itkStaticConstMacro(
    Dimension ,
    unsigned int ,
    VDimension 
)
```


Dimensionality of the domain of the particle system. 


### function Update

```cpp
void Update() const
```


Write the first n modes to +- 3 std dev and the mean of the model described by the covariance matrix. 


### function GetNumberOfModes

```cpp
inline int GetNumberOfModes() const
```


### function SetNumberOfModes

```cpp
inline void SetNumberOfModes(
    int b
)
```


### function SetShapeMatrix

```cpp
inline void SetShapeMatrix(
    ShapeMatrixType * s
)
```


Access the shape matrix. 


### function GetShapeMatrix

```cpp
inline ShapeMatrixType * GetShapeMatrix()
```


### function GetShapeMatrix

```cpp
inline const ShapeMatrixType * GetShapeMatrix() const
```


### function SetFileName

```cpp
inline void SetFileName(
    const std::string & s
)
```


Set/Get the filename prefix. 


### function SetFileName

```cpp
inline void SetFileName(
    const char * s
)
```


### function GetFileName

```cpp
inline const std::string & GetFileName() const
```


## Protected Functions Documentation

### function ParticleGaussianModeWriter

```cpp
inline ParticleGaussianModeWriter()
```


### function ~ParticleGaussianModeWriter

```cpp
inline virtual ~ParticleGaussianModeWriter()
```


### function operator=

```cpp
void operator=(
    const ParticleGaussianModeWriter & 
)
```


### function ParticleGaussianModeWriter

```cpp
ParticleGaussianModeWriter(
    const ParticleGaussianModeWriter & 
)
```


## Protected Attributes Documentation

### variable m_ShapeMatrix

```cpp
ShapeMatrixType::Pointer m_ShapeMatrix;
```


### variable m_NumberOfModes

```cpp
int m_NumberOfModes;
```


### variable m_FileName

```cpp
std::string m_FileName;
```


-------------------------------

Updated on 2025-10-09 at 04:04:22 +0000