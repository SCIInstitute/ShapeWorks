---
title: ObjectReader

---

# ObjectReader



 [More...](#detailed-description)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ObjectReader](../Classes/classObjectReader.md) | **[Self](../Classes/classObjectReader.md#typedef-self)**  |
| typedef T | **[ObjectType](../Classes/classObjectReader.md#typedef-objecttype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| const std::vector< ObjectType > & | **[GetOutput](../Classes/classObjectReader.md#function-getoutput)**() const |
| std::vector< ObjectType > & | **[GetOutput](../Classes/classObjectReader.md#function-getoutput)**() |
| void | **[SetFileName](../Classes/classObjectReader.md#function-setfilename)**(const char * fn) |
| void | **[SetFileName](../Classes/classObjectReader.md#function-setfilename)**(const std::string & fn) |
| const std::string & | **[GetFileName](../Classes/classObjectReader.md#function-getfilename)**() const |
| void | **[Read](../Classes/classObjectReader.md#function-read)**() |
| void | **[Update](../Classes/classObjectReader.md#function-update)**() |
| | **[ObjectReader](../Classes/classObjectReader.md#function-objectreader)**() |
| virtual | **[~ObjectReader](../Classes/classObjectReader.md#function-~objectreader)**() |

## Detailed Description

```cpp
template <class T >
class ObjectReader;
```

## Public Types Documentation

### typedef Self

```cpp
typedef ObjectReader ObjectReader< T >::Self;
```


Standard class typedefs 


### typedef ObjectType

```cpp
typedef T ObjectReader< T >::ObjectType;
```


## Public Functions Documentation

### function GetOutput

```cpp
inline const std::vector< ObjectType > & GetOutput() const
```


Get the output of the reader. The output is a std::vector of TransformType. 


### function GetOutput

```cpp
inline std::vector< ObjectType > & GetOutput()
```


### function SetFileName

```cpp
inline void SetFileName(
    const char * fn
)
```


### function SetFileName

```cpp
inline void SetFileName(
    const std::string & fn
)
```


### function GetFileName

```cpp
inline const std::string & GetFileName() const
```


### function Read

```cpp
inline void Read()
```


Read the file. 


### function Update

```cpp
inline void Update()
```


### function ObjectReader

```cpp
inline ObjectReader()
```


### function ~ObjectReader

```cpp
inline virtual ~ObjectReader()
```


-------------------------------

Updated on 2024-03-17 at 12:58:44 -0600