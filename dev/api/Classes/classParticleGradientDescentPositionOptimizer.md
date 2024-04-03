---
title: ParticleGradientDescentPositionOptimizer

---

# ParticleGradientDescentPositionOptimizer



 [More...](#detailed-description)


`#include <GradientDescentOptimizer.h>`

## Detailed Description

```cpp
class ParticleGradientDescentPositionOptimizer;
```


This class optimizes a list of particle system positions with respect to a specified energy function using a simple gradient descent strategy. A function which computes the gradient of the function with respect to particle position must be specified. The optimization performs Jacobi updates (each particle position is changed as soon as its new position is computed). 

-------------------------------

Updated on 2024-04-03 at 19:47:52 +0000