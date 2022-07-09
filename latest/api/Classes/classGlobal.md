---
title: Global

---

# Global





## Public Functions

|                | Name           |
| -------------- | -------------- |
| [Global](../Classes/classGlobal.md) & | **[global](../Classes/classGlobal.md#function-global)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| std::vector< int > | **[flipped_particles](../Classes/classGlobal.md#variable-flipped-particles)**  |
| std::vector< int > | **[flipped_domains](../Classes/classGlobal.md#variable-flipped-domains)**  |
| std::vector< itk::Point< double > > | **[targets](../Classes/classGlobal.md#variable-targets)**  |
| std::vector< int > | **[neighbors](../Classes/classGlobal.md#variable-neighbors)**  |
| tbb::mutex | **[mutex_](../Classes/classGlobal.md#variable-mutex-)**  |

## Public Functions Documentation

### function global

```cpp
static Global & global()
```


## Public Attributes Documentation

### variable flipped_particles

```cpp
std::vector< int > flipped_particles;
```


### variable flipped_domains

```cpp
std::vector< int > flipped_domains;
```


### variable targets

```cpp
std::vector< itk::Point< double > > targets;
```


### variable neighbors

```cpp
std::vector< int > neighbors;
```


### variable mutex_

```cpp
tbb::mutex mutex_;
```


-------------------------------

Updated on 2022-07-09 at 13:02:49 -0600