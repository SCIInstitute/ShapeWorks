---
title: shapeworks::ColorMap

---

# shapeworks::ColorMap





## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[construct_lookup_table](../Classes/classshapeworks_1_1ColorMap.md#function-construct-lookup-table)**(vtkSmartPointer< vtkLookupTable > lut) |
| void | **[set_discrete_mode](../Classes/classshapeworks_1_1ColorMap.md#function-set-discrete-mode)**(bool discrete_mode) |
| void | **[set_reverse_mode](../Classes/classshapeworks_1_1ColorMap.md#function-set-reverse-mode)**(bool reverse_mode) |
| vtkColor3ub | **[convert](../Classes/classshapeworks_1_1ColorMap.md#function-convert)**(QColor color) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| QString | **[name_](../Classes/classshapeworks_1_1ColorMap.md#variable-name-)**  |
| vtkSmartPointer< vtkColorSeries > | **[color_series_](../Classes/classshapeworks_1_1ColorMap.md#variable-color-series-)**  |

## Public Functions Documentation

### function construct_lookup_table

```cpp
void construct_lookup_table(
    vtkSmartPointer< vtkLookupTable > lut
)
```


### function set_discrete_mode

```cpp
inline void set_discrete_mode(
    bool discrete_mode
)
```


### function set_reverse_mode

```cpp
inline void set_reverse_mode(
    bool reverse_mode
)
```


### function convert

```cpp
static vtkColor3ub convert(
    QColor color
)
```


## Public Attributes Documentation

### variable name_

```cpp
QString name_;
```


### variable color_series_

```cpp
vtkSmartPointer< vtkColorSeries > color_series_;
```


-------------------------------

Updated on 2024-07-31 at 07:37:35 +0000