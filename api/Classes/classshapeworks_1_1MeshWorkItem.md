---
title: shapeworks::MeshWorkItem

---

# shapeworks::MeshWorkItem





## Public Attributes

|                | Name           |
| -------------- | -------------- |
| std::string | **[filename](../Classes/classshapeworks_1_1MeshWorkItem.md#variable-filename)**  |
| vnl_vector< double > | **[points](../Classes/classshapeworks_1_1MeshWorkItem.md#variable-points)**  |
| int | **[domain](../Classes/classshapeworks_1_1MeshWorkItem.md#variable-domain)**  |
| size_t | **[memory_size](../Classes/classshapeworks_1_1MeshWorkItem.md#variable-memory-size)**  |

## Friends

|                | Name           |
| -------------- | -------------- |
| bool | **[operator<](../Classes/classshapeworks_1_1MeshWorkItem.md#friend-operator<)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & a, const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & b)  |
| bool | **[operator==](../Classes/classshapeworks_1_1MeshWorkItem.md#friend-operator==)**(const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & a, const [MeshWorkItem](../Classes/classshapeworks_1_1MeshWorkItem.md) & b)  |

## Public Attributes Documentation

### variable filename

```cpp
std::string filename;
```


### variable points

```cpp
vnl_vector< double > points;
```


### variable domain

```cpp
int domain {0};
```


### variable memory_size

```cpp
size_t memory_size {0};
```


## Friends

### friend operator<

```cpp
friend bool operator<(
    const MeshWorkItem & a,

    const MeshWorkItem & b
);
```


### friend operator==

```cpp
friend bool operator==(
    const MeshWorkItem & a,

    const MeshWorkItem & b
);
```


-------------------------------

Updated on 2022-01-01 at 17:29:13 +0000