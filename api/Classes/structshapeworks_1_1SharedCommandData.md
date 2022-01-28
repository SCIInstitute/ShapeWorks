---
title: shapeworks::SharedCommandData

---

# shapeworks::SharedCommandData





## Public Functions

|                | Name           |
| -------------- | -------------- |
| bool | **[validImage](../Classes/structshapeworks_1_1SharedCommandData.md#function-validimage)**() const |
| bool | **[validMesh](../Classes/structshapeworks_1_1SharedCommandData.md#function-validmesh)**() const |
| bool | **[validParticleSystem](../Classes/structshapeworks_1_1SharedCommandData.md#function-validparticlesystem)**() const |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| [Image](../Classes/classshapeworks_1_1Image.md) | **[image](../Classes/structshapeworks_1_1SharedCommandData.md#variable-image)**  |
| std::unique_ptr< [Mesh](../Classes/classshapeworks_1_1Mesh.md) > | **[mesh](../Classes/structshapeworks_1_1SharedCommandData.md#variable-mesh)**  |
| [PhysicalRegion](../Classes/classshapeworks_1_1PhysicalRegion.md) | **[region](../Classes/structshapeworks_1_1SharedCommandData.md#variable-region)**  |
| [ParticleSystem](../Classes/classshapeworks_1_1ParticleSystem.md) | **[particleSystem](../Classes/structshapeworks_1_1SharedCommandData.md#variable-particlesystem)**  |
| Field | **[field](../Classes/structshapeworks_1_1SharedCommandData.md#variable-field)**  |

## Public Functions Documentation

### function validImage

```cpp
inline bool validImage() const
```


### function validMesh

```cpp
inline bool validMesh() const
```


### function validParticleSystem

```cpp
inline bool validParticleSystem() const
```


## Public Attributes Documentation

### variable image

```cpp
Image image;
```


### variable mesh

```cpp
std::unique_ptr< Mesh > mesh;
```


### variable region

```cpp
PhysicalRegion region;
```


### variable particleSystem

```cpp
ParticleSystem particleSystem;
```


### variable field

```cpp
Field field;
```


-------------------------------

Updated on 2022-01-28 at 21:13:53 +0000