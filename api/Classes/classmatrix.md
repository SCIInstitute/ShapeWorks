---
title: matrix

---

# matrix





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[matrix](../Classes/classmatrix.md#function-matrix)**(int r, int c) |
| | **[~matrix](../Classes/classmatrix.md#function-~matrix)**() |
| void | **[zero](../Classes/classmatrix.md#function-zero)**() |
| double * | **[operator[]](../Classes/classmatrix.md#function-operator[])**(int i) |
| double & | **[operator()](../Classes/classmatrix.md#function-operator())**(int i, int j) |
| bool | **[solve](../Classes/classmatrix.md#function-solve)**(vector< double > & x, vector< double > & b) |
| bool | **[lsq_solve](../Classes/classmatrix.md#function-lsq-solve)**(vector< double > & x, vector< double > & b) |
| bool | **[eigen_vectors](../Classes/classmatrix.md#function-eigen-vectors)**([matrix](../Classes/classmatrix.md) & Eigen, vector< double > & eigen_values) |
| int | **[Rows](../Classes/classmatrix.md#function-rows)**() |
| void | **[mult_transpose](../Classes/classmatrix.md#function-mult-transpose)**(vector< double > & x, vector< double > & y) |
| void | **[mult_transpose_self](../Classes/classmatrix.md#function-mult-transpose-self)**([matrix](../Classes/classmatrix.md) & AAt) |

## Public Functions Documentation

### function matrix

```cpp
matrix(
    int r,
    int c
)
```


### function ~matrix

```cpp
inline ~matrix()
```


### function zero

```cpp
void zero()
```


### function operator[]

```cpp
inline double * operator[](
    int i
)
```


### function operator()

```cpp
inline double & operator()(
    int i,
    int j
)
```


### function solve

```cpp
bool solve(
    vector< double > & x,
    vector< double > & b
)
```


### function lsq_solve

```cpp
bool lsq_solve(
    vector< double > & x,
    vector< double > & b
)
```


### function eigen_vectors

```cpp
bool eigen_vectors(
    matrix & Eigen,
    vector< double > & eigen_values
)
```


### function Rows

```cpp
inline int Rows()
```


### function mult_transpose

```cpp
void mult_transpose(
    vector< double > & x,
    vector< double > & y
)
```


### function mult_transpose_self

```cpp
void mult_transpose_self(
    matrix & AAt
)
```


-------------------------------

Updated on 2021-11-15 at 18:13:25 +0000