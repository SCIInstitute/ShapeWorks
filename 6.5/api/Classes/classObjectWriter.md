---
title: ObjectWriter

---

# ObjectWriter



 [More...](#detailed-description)

## Public Types

|                | Name           |
| -------------- | -------------- |
| typedef [ObjectWriter](../Classes/classObjectWriter.md) | **[Self](../Classes/classObjectWriter.md#typedef-self)**  |
| typedef T | **[ObjectType](../Classes/classObjectWriter.md#typedef-objecttype)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[SetInput](../Classes/classObjectWriter.md#function-setinput)**(const std::vector< ObjectType > & p) |
| void | **[SetFileName](../Classes/classObjectWriter.md#function-setfilename)**(const char * fn) |
| void | **[SetFileName](../Classes/classObjectWriter.md#function-setfilename)**(const std::string & fn) |
| const std::string & | **[GetFileName](../Classes/classObjectWriter.md#function-getfilename)**() const |
| void | **[Write](../Classes/classObjectWriter.md#function-write)**() |
| void | **[Update](../Classes/classObjectWriter.md#function-update)**() |
| | **[ObjectWriter](../Classes/classObjectWriter.md#function-objectwriter)**() |
| virtual | **[~ObjectWriter](../Classes/classObjectWriter.md#function-~objectwriter)**() |

## Detailed Description

```cpp
template <class T >
class ObjectWriter;
```

## Public Types Documentation

### typedef Self

```cpp
typedef ObjectWriter ObjectWriter< T >::Self;
```


Standard class typedefs 


### typedef ObjectType

```cpp
typedef T ObjectWriter< T >::ObjectType;
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
inline void Update()
```


### function ObjectWriter

```cpp
inline ObjectWriter()
```


### function ~ObjectWriter

```cpp
inline virtual ~ObjectWriter()
```


-------------------------------

Updated on 2023-07-11 at 23:59:10 +0000