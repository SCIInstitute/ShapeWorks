---
title: itk

---

# itk



## Namespaces

| Name           |
| -------------- |
| **[itk::Functor](../Namespaces/namespaceitk_1_1Functor.md)**  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[itk::AdvancedTransform](../Classes/classitk_1_1AdvancedTransform.md)** <br>Transform maps points, vectors and covariant vectors from an input space to an output space.  |
| class | **[itk::BSplineInterpolateImageFunctionWithDoubleCoefficents](../Classes/classitk_1_1BSplineInterpolateImageFunctionWithDoubleCoefficents.md)**  |
| class | **[itk::CompactlySupportedRBFSparseKernelTransform](../Classes/classitk_1_1CompactlySupportedRBFSparseKernelTransform.md)**  |
| class | **[itk::KernelTransform2](../Classes/classitk_1_1KernelTransform2.md)**  |
| class | **[itk::MultiplyByConstantImageFilter](../Classes/classitk_1_1MultiplyByConstantImageFilter.md)** <br>Multiply input pixels by a constant.  |
| class | **[itk::SparseKernelTransform](../Classes/classitk_1_1SparseKernelTransform.md)**  |
| class | **[itk::ThinPlateSplineKernelTransform2](../Classes/classitk_1_1ThinPlateSplineKernelTransform2.md)**  |
| class | **[itk::TPGACLevelSetImageFilter](../Classes/classitk_1_1TPGACLevelSetImageFilter.md)**  |

## Functions

|                | Name           |
| -------------- | -------------- |
| void | **[fillLocal6Neighbours](../Namespaces/namespaceitk.md#function-filllocal6neighbours)**(int * srcNbh, int * dstNbh, int centre, int i0, int i1, int i2, int i3) |
| void | **[label6Neighbours](../Namespaces/namespaceitk.md#function-label6neighbours)**(int * nbh, int * nbhlabels, int * nbhv, int curlabel, int idx) |
| void | **[label26Neighbours](../Namespaces/namespaceitk.md#function-label26neighbours)**(int * nbh, int * nbhlabels, int * nbhv, int curlabel, int idx) |
| int | **[connectedComponents](../Namespaces/namespaceitk.md#function-connectedcomponents)**(int * nbh, int * nbhLabels, void(*)(int *, int *, int *, int, int) labelNeighboursFunc) |

## Attributes

|                | Name           |
| -------------- | -------------- |
| int | **[nbh6Table](../Namespaces/namespaceitk.md#variable-nbh6table)**  |
| int | **[nbh26Table](../Namespaces/namespaceitk.md#variable-nbh26table)**  |


## Functions Documentation

### function fillLocal6Neighbours

```cpp
static inline void fillLocal6Neighbours(
    int * srcNbh,
    int * dstNbh,
    int centre,
    int i0,
    int i1,
    int i2,
    int i3
)
```


Checks if the centre'th element of srcNbh is on. If it is, activate that element in dstNbh and also i0 to i3 if they are active in srcNbh. This is used during the recursive 6 connectivity determination. 


### function label6Neighbours

```cpp
static void label6Neighbours(
    int * nbh,
    int * nbhlabels,
    int * nbhv,
    int curlabel,
    int idx
)
```


The idx'th voxel in nbh is ALREADY labeled. This checks for existing 6-neighbours and gives them label curlabel. 


### function label26Neighbours

```cpp
static void label26Neighbours(
    int * nbh,
    int * nbhlabels,
    int * nbhv,
    int curlabel,
    int idx
)
```


The idx'th voxel in nbh is ALREADY labeled. This checks for existing 26-neighbours and gives them label curlabel. 


### function connectedComponents

```cpp
static inline int connectedComponents(
    int * nbh,
    int * nbhLabels,
    void(*)(int *, int *, int *, int, int) labelNeighboursFunc
)
```



## Attributes Documentation

### variable nbh6Table

```cpp
static int nbh6Table = {
    {1, 3, 9, -1, -1, -1}, 
    {0, 2, 4, 10, -1, -1}, 
    {1, 5, 11, -1, -1, -1}, 
    {0, 4, 6, 12, -1, -1}, 
    {1, 3, 5, 7, 13, -1}, 
    {2, 4, 8, 14, -1, -1}, 
    {3, 7, 15, -1, -1, -1}, 
    {4, 6, 8, 16, -1, -1}, 
    {5, 7, 17, -1, -1, -1}, 
    {0, 10, 12, 18, -1, -1}, 
    {1, 9, 11, 13, 19, -1}, 
    {2, 10, 14, 20, -1, -1}, 
    {3, 9, 13, 15, 21, -1}, 
    {4, 10, 12, 14, 16, 22}, 
    {5, 11, 13, 17, 23, -1}, 
    {6, 12, 16, 24, -1, -1}, 
    {7, 13, 15, 17, 25, -1}, 
    {8, 14, 16, 26, -1, -1}, 
    {9, 19, 21, -1, -1, -1}, 
    {10, 18, 20, 22, -1, -1}, 
    {11, 19, 23, -1, -1, -1}, 
    {12, 18, 22, 24, -1, -1}, 
    {13, 19, 21, 23, 25, -1}, 
    {14, 20, 22, 26, -1, -1}, 
    {15, 21, 25, -1, -1, -1}, 
    {16, 22, 24, 26, -1, -1}, 
    {17, 23, 25, -1, -1, -1} 
};
```


### variable nbh26Table

```cpp
static int nbh26Table = {
    {1, 3, 4, 9, 10, 12, 13, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {0, 2, 3, 4, 5, 9, 10, 11, 12, 13, 14, -1, -1,-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {1, 4, 5, 10, 11, 13, 14, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {0, 1, 4, 6, 7, 9, 10, 12, 13, 15, 16, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {0, 1, 2, 3, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {1, 2, 4, 7, 8, 10, 11, 13, 14, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {3, 4, 7, 12, 13, 15, 16, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {3, 4, 5, 6, 8, 12, 13, 14, 15, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {4, 5, 7, 13, 14, 16, 17, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {0, 1, 3, 4, 10, 12, 13, 18, 19, 21, 22, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {0, 1, 2, 3, 4, 5, 9, 11, 12, 13, 14, 18, 19, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {1, 2, 4, 5, 10, 13, 14, 19, 20, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {0, 1, 3, 4, 6, 7, 9, 10, 13, 15, 16, 18, 19, 21, 22, 24, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26}, 
    {1, 2, 4, 5, 7, 8, 10, 11, 13, 16, 17, 19, 20, 22, 23, 25, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {3, 4, 6, 7, 12, 13, 16, 21, 22, 24, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {3, 4, 5, 6, 7, 8, 12, 13, 14, 15, 17, 21, 22, 23, 24, 25, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {4, 5, 7, 8, 13, 14, 16, 22, 23, 25, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {9, 10, 12, 13, 19, 21, 22, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {9, 10, 11, 12, 13, 14, 18, 20, 21, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {10, 11, 13, 14, 19, 22, 23, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {9, 10, 12, 13, 15, 16, 18, 19, 22, 24, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 23, 24, 25, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {10, 11, 13, 14, 16, 17, 19, 20, 22, 25, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {12, 13, 15, 16, 21, 22, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {12, 13, 14, 15, 16, 17, 21, 22, 23, 24, 26, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}, 
    {13, 14, 16, 17, 22, 23, 25, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1} 
};
```





-------------------------------

Updated on 2023-06-30 at 16:21:39 +0000