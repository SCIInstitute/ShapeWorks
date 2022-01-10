---
title: itk::Functor::MultiplyByConstant

---

# itk::Functor::MultiplyByConstant



 [More...](#detailed-description)

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[MultiplyByConstant](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md#function-multiplybyconstant)**() |
| | **[~MultiplyByConstant](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md#function-~multiplybyconstant)**() |
| bool | **[operator!=](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md#function-operator!=)**(const [MultiplyByConstant](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md) & other) const |
| bool | **[operator==](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md#function-operator==)**(const [MultiplyByConstant](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md) & other) const |
| TOutput | **[operator()](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md#function-operator())**(const TInput & A) const |
| void | **[SetConstant](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md#function-setconstant)**(TConstant ct) |
| const TConstant & | **[GetConstant](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md#function-getconstant)**() const |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| TConstant | **[m_Constant](../Classes/classitk_1_1Functor_1_1MultiplyByConstant.md#variable-m-constant)**  |

## Detailed Description

```cpp
template <class TInput ,
class TConstant ,
class TOutput >
class itk::Functor::MultiplyByConstant;
```

## Public Functions Documentation

### function MultiplyByConstant

```cpp
inline MultiplyByConstant()
```


### function ~MultiplyByConstant

```cpp
inline ~MultiplyByConstant()
```


### function operator!=

```cpp
inline bool operator!=(
    const MultiplyByConstant & other
) const
```


### function operator==

```cpp
inline bool operator==(
    const MultiplyByConstant & other
) const
```


### function operator()

```cpp
inline TOutput operator()(
    const TInput & A
) const
```


### function SetConstant

```cpp
inline void SetConstant(
    TConstant ct
)
```


### function GetConstant

```cpp
inline const TConstant & GetConstant() const
```


## Public Attributes Documentation

### variable m_Constant

```cpp
TConstant m_Constant;
```


-------------------------------

Updated on 2022-01-10 at 16:27:26 +0000