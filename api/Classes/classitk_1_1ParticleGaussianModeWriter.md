---
title: itk::ParticleGaussianModeWriter

---

# itk::ParticleGaussianModeWriter



 [More...](#detailed-description)

Inherits from DataObject

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ParticleGaussianModeWriter](../Classes/classitk_1_1ParticleGaussianModeWriter.md) | **[Self](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-self)**  |
| typedef SmartPointer< [Self](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-self) > | **[Pointer](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-pointer)**  |
| typedef SmartPointer< const [Self](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-self) > | **[ConstPointer](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-constpointer)**  |
| typedef DataObject | **[Superclass](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-superclass)**  |
| typedef [ParticleSystem](../Classes/classitk_1_1ParticleSystem.md)< VDimension > | **[ParticleSystemType](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-particlesystemtype)**  |
| typedef [ParticleShapeMatrixAttribute](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md)< double, VDimension > | **[ShapeMatrixType](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-shapematrixtype)**  |
| typedef [ShapeMatrixType::DataType](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md#typedef-datatype) | **[DataType](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-datatype)**  |
| typedef [ParticleSystemType::PointType](../Classes/classitk_1_1ParticleSystem.md#typedef-pointtype) | **[PointType](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-pointtype)**  |
| typedef vnl_vector< DataType > | **[vnl_vector_type](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-vnl-vector-type)**  |
| typedef vnl_matrix< DataType > | **[vnl_matrix_type](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-vnl-matrix-type)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[itkTypeMacro](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-itktypemacro)**([ParticleGaussianModeWriter](../Classes/classitk_1_1ParticleGaussianModeWriter.md) , DataObject ) |
| | **[itkNewMacro](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-itknewmacro)**([Self](../Classes/classitk_1_1ParticleGaussianModeWriter.md#typedef-self) ) |
| | **[itkStaticConstMacro](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-itkstaticconstmacro)**(Dimension , unsigned int , VDimension ) |
| void | **[Update](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-update)**() const |
| int | **[GetNumberOfModes](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-getnumberofmodes)**() const |
| void | **[SetNumberOfModes](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-setnumberofmodes)**(int b) |
| void | **[SetShapeMatrix](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-setshapematrix)**([ShapeMatrixType](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md) * s) |
| [ShapeMatrixType](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md) * | **[GetShapeMatrix](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-getshapematrix)**() |
| const [ShapeMatrixType](../Classes/classitk_1_1ParticleShapeMatrixAttribute.md) * | **[GetShapeMatrix](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-getshapematrix)**() const |
| void | **[SetFileName](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-setfilename)**(const std::string & s) |
| void | **[SetFileName](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-setfilename)**(const char * s) |
| const std::string & | **[GetFileName](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-getfilename)**() const |

## Protected Functions

|                | Name           |
| -------------- | -------------- |
| | **[ParticleGaussianModeWriter](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-particlegaussianmodewriter)**() |
| virtual | **[~ParticleGaussianModeWriter](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-~particlegaussianmodewriter)**() |
| void | **[operator=](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-operator=)**(const [ParticleGaussianModeWriter](../Classes/classitk_1_1ParticleGaussianModeWriter.md) & ) |
| | **[ParticleGaussianModeWriter](../Classes/classitk_1_1ParticleGaussianModeWriter.md#function-particlegaussianmodewriter)**(const [ParticleGaussianModeWriter](../Classes/classitk_1_1ParticleGaussianModeWriter.md) & ) |

## Protected Attributes

|                | Name           |
| -------------- | -------------- |
| ShapeMatrixType::Pointer | **[m_ShapeMatrix](../Classes/classitk_1_1ParticleGaussianModeWriter.md#variable-m-shapematrix)**  |
| int | **[m_NumberOfModes](../Classes/classitk_1_1ParticleGaussianModeWriter.md#variable-m-numberofmodes)**  |
| std::string | **[m_FileName](../Classes/classitk_1_1ParticleGaussianModeWriter.md#variable-m-filename)**  |

## Detailed Description

```cpp
template <unsigned int VDimension>
class itk::ParticleGaussianModeWriter;
```

## Public Types Documentation

### typedef Self

```cpp
typedef ParticleGaussianModeWriter itk::ParticleGaussianModeWriter< VDimension >::Self;
```


Standard class typedefs. 


### typedef Pointer

```cpp
typedef SmartPointer<Self> itk::ParticleGaussianModeWriter< VDimension >::Pointer;
```


### typedef ConstPointer

```cpp
typedef SmartPointer<const Self> itk::ParticleGaussianModeWriter< VDimension >::ConstPointer;
```


### typedef Superclass

```cpp
typedef DataObject itk::ParticleGaussianModeWriter< VDimension >::Superclass;
```


### typedef ParticleSystemType

```cpp
typedef ParticleSystem<VDimension> itk::ParticleGaussianModeWriter< VDimension >::ParticleSystemType;
```


Type of particle system and associated classes. 


### typedef ShapeMatrixType

```cpp
typedef ParticleShapeMatrixAttribute<double, VDimension> itk::ParticleGaussianModeWriter< VDimension >::ShapeMatrixType;
```


### typedef DataType

```cpp
typedef ShapeMatrixType::DataType itk::ParticleGaussianModeWriter< VDimension >::DataType;
```


### typedef PointType

```cpp
typedef ParticleSystemType::PointType itk::ParticleGaussianModeWriter< VDimension >::PointType;
```


Vector & Point types. 


### typedef vnl_vector_type

```cpp
typedef vnl_vector<DataType> itk::ParticleGaussianModeWriter< VDimension >::vnl_vector_type;
```


### typedef vnl_matrix_type

```cpp
typedef vnl_matrix<DataType> itk::ParticleGaussianModeWriter< VDimension >::vnl_matrix_type;
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

Updated on 2021-11-15 at 01:30:07 +0000