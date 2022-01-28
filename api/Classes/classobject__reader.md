---
title: object_reader

---

# object_reader



 [More...](#detailed-description)


`#include <object_reader.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [object_reader](../Classes/classobject__reader.md) | **[Self](../Classes/classobject__reader.md#typedef-self)**  |
| typedef T | **[ObjectType](../Classes/classobject__reader.md#typedef-objecttype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| const std::vector< ObjectType > & | **[GetOutput](../Classes/classobject__reader.md#function-getoutput)**() const |
| std::vector< ObjectType > & | **[GetOutput](../Classes/classobject__reader.md#function-getoutput)**() |
| void | **[SetFileName](../Classes/classobject__reader.md#function-setfilename)**(const char * fn) |
| void | **[SetFileName](../Classes/classobject__reader.md#function-setfilename)**(const std::string & fn) |
| const std::string & | **[GetFileName](../Classes/classobject__reader.md#function-getfilename)**() const |
| void | **[Read](../Classes/classobject__reader.md#function-read)**() |
| void | **[Update](../Classes/classobject__reader.md#function-update)**() |
| | **[object_reader](../Classes/classobject__reader.md#function-object-reader)**() |
| virtual | **[~object_reader](../Classes/classobject__reader.md#function-~object-reader)**() |

## Detailed Description

```cpp
template <class T >
class object_reader;
```


Reads a std::vector of c++ objects. The first integer in the file is assumed to represent the number of transforms in the file. The size of each transform is determined by the templating. 

## Public Types Documentation

### typedef Self

```cpp
typedef object_reader object_reader< T >::Self;
```


Standard class typedefs 


### typedef ObjectType

```cpp
typedef T object_reader< T >::ObjectType;
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
void Update()
```


### function object_reader

```cpp
inline object_reader()
```


### function ~object_reader

```cpp
inline virtual ~object_reader()
```


-------------------------------

Updated on 2022-01-28 at 21:13:53 +0000