---
title: object_writer

---

# object_writer



 [More...](#detailed-description)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [object_writer](../Classes/classobject__writer.md) | **[Self](../Classes/classobject__writer.md#typedef-self)**  |
| typedef T | **[ObjectType](../Classes/classobject__writer.md#typedef-objecttype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[SetInput](../Classes/classobject__writer.md#function-setinput)**(const std::vector< ObjectType > & p) |
| void | **[SetFileName](../Classes/classobject__writer.md#function-setfilename)**(const char * fn) |
| void | **[SetFileName](../Classes/classobject__writer.md#function-setfilename)**(const std::string & fn) |
| const std::string & | **[GetFileName](../Classes/classobject__writer.md#function-getfilename)**() const |
| void | **[Write](../Classes/classobject__writer.md#function-write)**() |
| void | **[Update](../Classes/classobject__writer.md#function-update)**() |
| | **[object_writer](../Classes/classobject__writer.md#function-object-writer)**() |
| virtual | **[~object_writer](../Classes/classobject__writer.md#function-~object-writer)**() |

## Detailed Description

```cpp
template <class T >
class object_writer;
```

## Public Types Documentation

### typedef Self

```cpp
typedef object_writer object_writer< T >::Self;
```


Standard class typedefs 


### typedef ObjectType

```cpp
typedef T object_writer< T >::ObjectType;
```


## Public Functions Documentation

### function SetInput

```cpp
inline void SetInput(
    const std::vector< ObjectType > & p
)
```


Set the input vector. The input is a reference to a std::vector of ObjectType. 


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


### function Write

```cpp
inline void Write()
```


Write the file. 


### function Update

```cpp
void Update()
```


### function object_writer

```cpp
inline object_writer()
```


### function ~object_writer

```cpp
inline virtual ~object_writer()
```


-------------------------------

Updated on 2021-11-14 at 17:52:10 +0000