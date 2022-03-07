---
title: FECVDDecimationModifier

---

# FECVDDecimationModifier



 [More...](#detailed-description)


`#include <FECVDDecimationModifier.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[FECVDDecimationModifier](../Classes/classFECVDDecimationModifier.md#function-fecvddecimationmodifier)**()<br>Constructor.  |
| [FEMesh](../Classes/classFEMesh.md) * | **[Apply](../Classes/classFECVDDecimationModifier.md#function-apply)**([FEMesh](../Classes/classFEMesh.md) * pm)<br>Apply the decimation modifier.  |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| double | **[m_pct](../Classes/classFECVDDecimationModifier.md#variable-m-pct)** <br>percentage of target number of clusters/vertices  |
| bool | **[m_bcvd](../Classes/classFECVDDecimationModifier.md#variable-m-bcvd)**  |
| double | **[m_sel_pct](../Classes/classFECVDDecimationModifier.md#variable-m-sel-pct)**  |
| double | **[m_gradient](../Classes/classFECVDDecimationModifier.md#variable-m-gradient)**  |

## Detailed Description

```cpp
class FECVDDecimationModifier;
```


This class implements a modifier that coarses a plygonal mesh using an approximated centroidal voronoi diagram. 

## Public Functions Documentation

### function FECVDDecimationModifier

```cpp
FECVDDecimationModifier()
```

Constructor. 

### function Apply

```cpp
FEMesh * Apply(
    FEMesh * pm
)
```

Apply the decimation modifier. 

## Public Attributes Documentation

### variable m_pct

```cpp
double m_pct;
```

percentage of target number of clusters/vertices 

### variable m_bcvd

```cpp
bool m_bcvd;
```


### variable m_sel_pct

```cpp
double m_sel_pct;
```


### variable m_gradient

```cpp
double m_gradient;
```


-------------------------------

Updated on 2022-03-07 at 00:21:28 +0000