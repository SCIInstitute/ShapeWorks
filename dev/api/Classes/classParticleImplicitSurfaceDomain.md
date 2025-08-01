---
title: ParticleImplicitSurfaceDomain

---

# ParticleImplicitSurfaceDomain



 [More...](#detailed-description)


`#include <ImplicitSurfaceDomain.h>`

## Detailed Description

```cpp
class ParticleImplicitSurfaceDomain;
```


A 3D cartesian domain that constrains points so that they always lie an implicit surface. The implicit surface is defined as the zero isosurface of the given image. Constraints are applied using a Newton-Raphson iteration, and this class assumes it has a distance transform as an image. 

-------------------------------

Updated on 2025-08-01 at 02:40:46 +0000