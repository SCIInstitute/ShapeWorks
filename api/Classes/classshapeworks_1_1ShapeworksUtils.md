---
title: shapeworks::ShapeworksUtils

---

# shapeworks::ShapeworksUtils





## Public Functions

|                | Name           |
| -------------- | -------------- |
| time_point | **[now](../Classes/classshapeworks_1_1ShapeworksUtils.md#function-now)**()<br>get the current time  |
| double | **[elapsed](../Classes/classshapeworks_1_1ShapeworksUtils.md#function-elapsed)**(time_point start, time_point end, bool print_elapsed =true)<br>return elapsed time in seconds, optionally printing to console  |
| unsigned | **[rngSeed](../Classes/classshapeworks_1_1ShapeworksUtils.md#function-rngseed)**()<br>returns random seed  |
| void | **[setRngSeed](../Classes/classshapeworks_1_1ShapeworksUtils.md#function-setrngseed)**(const unsigned seed)<br>sets random seed  |
| unsigned | **[generateNumber](../Classes/classshapeworks_1_1ShapeworksUtils.md#function-generatenumber)**()<br>generates random number  |
| bool | **[is_directory](../Classes/classshapeworks_1_1ShapeworksUtils.md#function-is-directory)**(const std::string & pathname)<br>returns true if pathname is a directory  |
| bool | **[exists](../Classes/classshapeworks_1_1ShapeworksUtils.md#function-exists)**(const std::string & filename)<br>returns true if filename exists  |
| Matrix33 | **[getMatrix](../Classes/classshapeworks_1_1ShapeworksUtils.md#function-getmatrix)**(const vtkSmartPointer< vtkMatrix4x4 > & mat)<br>converts a vtkMatrix4x4 to a Matrix33 and corresponding translationVector  |
| Vector3 | **[getOffset](../Classes/classshapeworks_1_1ShapeworksUtils.md#function-getoffset)**(const vtkSmartPointer< vtkMatrix4x4 > & mat) |

## Public Functions Documentation

### function now

```cpp
static inline time_point now()
```

get the current time 

### function elapsed

```cpp
static double elapsed(
    time_point start,
    time_point end,
    bool print_elapsed =true
)
```

return elapsed time in seconds, optionally printing to console 

### function rngSeed

```cpp
static inline unsigned rngSeed()
```

returns random seed 

### function setRngSeed

```cpp
static void setRngSeed(
    const unsigned seed
)
```

sets random seed 

### function generateNumber

```cpp
static inline unsigned generateNumber()
```

generates random number 

### function is_directory

```cpp
static bool is_directory(
    const std::string & pathname
)
```

returns true if pathname is a directory 

### function exists

```cpp
static bool exists(
    const std::string & filename
)
```

returns true if filename exists 

### function getMatrix

```cpp
static Matrix33 getMatrix(
    const vtkSmartPointer< vtkMatrix4x4 > & mat
)
```

converts a vtkMatrix4x4 to a Matrix33 and corresponding translationVector 

### function getOffset

```cpp
static Vector3 getOffset(
    const vtkSmartPointer< vtkMatrix4x4 > & mat
)
```


-------------------------------

Updated on 2022-01-01 at 17:29:13 +0000