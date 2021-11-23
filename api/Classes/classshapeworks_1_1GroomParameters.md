---
title: shapeworks::GroomParameters

---

# shapeworks::GroomParameters





## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[GroomParameters](../Classes/classshapeworks_1_1GroomParameters.md#function-groomparameters)**(ProjectHandle project, std::string domain_name ="") |
| void | **[save_to_project](../Classes/classshapeworks_1_1GroomParameters.md#function-save-to-project)**() |
| std::string | **[get_groom_output_prefix](../Classes/classshapeworks_1_1GroomParameters.md#function-get-groom-output-prefix)**() |
| void | **[set_groom_output_prefix](../Classes/classshapeworks_1_1GroomParameters.md#function-set-groom-output-prefix)**(std::string prefix) |
| bool | **[get_alignment_enabled](../Classes/classshapeworks_1_1GroomParameters.md#function-get-alignment-enabled)**() |
| void | **[set_alignment_enabled](../Classes/classshapeworks_1_1GroomParameters.md#function-set-alignment-enabled)**(bool value) |
| std::string | **[get_alignment_method](../Classes/classshapeworks_1_1GroomParameters.md#function-get-alignment-method)**() |
| void | **[set_alignment_method](../Classes/classshapeworks_1_1GroomParameters.md#function-set-alignment-method)**(std::string method) |
| bool | **[get_isolate_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-get-isolate-tool)**() |
| void | **[set_isolate_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-set-isolate-tool)**(bool value) |
| bool | **[get_fill_holes_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-get-fill-holes-tool)**() |
| void | **[set_fill_holes_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-set-fill-holes-tool)**(bool value) |
| bool | **[get_auto_pad_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-get-auto-pad-tool)**() |
| void | **[set_auto_pad_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-set-auto-pad-tool)**(bool value) |
| int | **[get_padding_amount](../Classes/classshapeworks_1_1GroomParameters.md#function-get-padding-amount)**() |
| void | **[set_padding_amount](../Classes/classshapeworks_1_1GroomParameters.md#function-set-padding-amount)**(int padding_amount) |
| bool | **[get_antialias_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-get-antialias-tool)**() |
| void | **[set_antialias_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-set-antialias-tool)**(bool value) |
| int | **[get_antialias_iterations](../Classes/classshapeworks_1_1GroomParameters.md#function-get-antialias-iterations)**() |
| void | **[set_antialias_iterations](../Classes/classshapeworks_1_1GroomParameters.md#function-set-antialias-iterations)**(int iterations) |
| bool | **[get_blur_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-get-blur-tool)**() |
| void | **[set_blur_tool](../Classes/classshapeworks_1_1GroomParameters.md#function-set-blur-tool)**(bool value) |
| double | **[get_blur_amount](../Classes/classshapeworks_1_1GroomParameters.md#function-get-blur-amount)**() |
| void | **[set_blur_amount](../Classes/classshapeworks_1_1GroomParameters.md#function-set-blur-amount)**(double blur_amount) |
| bool | **[get_fast_marching](../Classes/classshapeworks_1_1GroomParameters.md#function-get-fast-marching)**() |
| void | **[set_fast_marching](../Classes/classshapeworks_1_1GroomParameters.md#function-set-fast-marching)**(bool value) |
| bool | **[get_mesh_smooth](../Classes/classshapeworks_1_1GroomParameters.md#function-get-mesh-smooth)**() |
| void | **[set_mesh_smooth](../Classes/classshapeworks_1_1GroomParameters.md#function-set-mesh-smooth)**(bool value) |
| std::string | **[get_mesh_smoothing_method](../Classes/classshapeworks_1_1GroomParameters.md#function-get-mesh-smoothing-method)**() |
| void | **[set_mesh_smoothing_method](../Classes/classshapeworks_1_1GroomParameters.md#function-set-mesh-smoothing-method)**(std::string method) |
| int | **[get_mesh_vtk_laplacian_iterations](../Classes/classshapeworks_1_1GroomParameters.md#function-get-mesh-vtk-laplacian-iterations)**() |
| void | **[set_mesh_vtk_laplacian_iterations](../Classes/classshapeworks_1_1GroomParameters.md#function-set-mesh-vtk-laplacian-iterations)**(int iterations) |
| double | **[get_mesh_vtk_laplacian_relaxation](../Classes/classshapeworks_1_1GroomParameters.md#function-get-mesh-vtk-laplacian-relaxation)**() |
| void | **[set_mesh_vtk_laplacian_relaxation](../Classes/classshapeworks_1_1GroomParameters.md#function-set-mesh-vtk-laplacian-relaxation)**(double relaxation) |
| int | **[get_mesh_vtk_windowed_sinc_iterations](../Classes/classshapeworks_1_1GroomParameters.md#function-get-mesh-vtk-windowed-sinc-iterations)**() |
| void | **[set_mesh_vtk_windowed_sinc_iterations](../Classes/classshapeworks_1_1GroomParameters.md#function-set-mesh-vtk-windowed-sinc-iterations)**(int iterations) |
| double | **[get_mesh_vtk_windowed_sinc_passband](../Classes/classshapeworks_1_1GroomParameters.md#function-get-mesh-vtk-windowed-sinc-passband)**() |
| void | **[set_mesh_vtk_windowed_sinc_passband](../Classes/classshapeworks_1_1GroomParameters.md#function-set-mesh-vtk-windowed-sinc-passband)**(double passband) |
| bool | **[get_use_icp](../Classes/classshapeworks_1_1GroomParameters.md#function-get-use-icp)**() |
| bool | **[get_use_center](../Classes/classshapeworks_1_1GroomParameters.md#function-get-use-center)**() |
| void | **[restore_defaults](../Classes/classshapeworks_1_1GroomParameters.md#function-restore-defaults)**() |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| const std::string | **[GROOM_SMOOTH_VTK_LAPLACIAN_C](../Classes/classshapeworks_1_1GroomParameters.md#variable-groom-smooth-vtk-laplacian-c)**  |
| const std::string | **[GROOM_SMOOTH_VTK_WINDOWED_SINC_C](../Classes/classshapeworks_1_1GroomParameters.md#variable-groom-smooth-vtk-windowed-sinc-c)**  |
| const std::string | **[GROOM_ALIGNMENT_CENTER_C](../Classes/classshapeworks_1_1GroomParameters.md#variable-groom-alignment-center-c)**  |
| const std::string | **[GROOM_ALIGNMENT_ICP_C](../Classes/classshapeworks_1_1GroomParameters.md#variable-groom-alignment-icp-c)**  |

## Public Functions Documentation

### function GroomParameters

```cpp
explicit GroomParameters(
    ProjectHandle project,
    std::string domain_name =""
)
```


### function save_to_project

```cpp
void save_to_project()
```


### function get_groom_output_prefix

```cpp
std::string get_groom_output_prefix()
```


### function set_groom_output_prefix

```cpp
void set_groom_output_prefix(
    std::string prefix
)
```


### function get_alignment_enabled

```cpp
bool get_alignment_enabled()
```


### function set_alignment_enabled

```cpp
void set_alignment_enabled(
    bool value
)
```


### function get_alignment_method

```cpp
std::string get_alignment_method()
```


### function set_alignment_method

```cpp
void set_alignment_method(
    std::string method
)
```


### function get_isolate_tool

```cpp
bool get_isolate_tool()
```


### function set_isolate_tool

```cpp
void set_isolate_tool(
    bool value
)
```


### function get_fill_holes_tool

```cpp
bool get_fill_holes_tool()
```


### function set_fill_holes_tool

```cpp
void set_fill_holes_tool(
    bool value
)
```


### function get_auto_pad_tool

```cpp
bool get_auto_pad_tool()
```


### function set_auto_pad_tool

```cpp
void set_auto_pad_tool(
    bool value
)
```


### function get_padding_amount

```cpp
int get_padding_amount()
```


### function set_padding_amount

```cpp
void set_padding_amount(
    int padding_amount
)
```


### function get_antialias_tool

```cpp
bool get_antialias_tool()
```


### function set_antialias_tool

```cpp
void set_antialias_tool(
    bool value
)
```


### function get_antialias_iterations

```cpp
int get_antialias_iterations()
```


### function set_antialias_iterations

```cpp
void set_antialias_iterations(
    int iterations
)
```


### function get_blur_tool

```cpp
bool get_blur_tool()
```


### function set_blur_tool

```cpp
void set_blur_tool(
    bool value
)
```


### function get_blur_amount

```cpp
double get_blur_amount()
```


### function set_blur_amount

```cpp
void set_blur_amount(
    double blur_amount
)
```


### function get_fast_marching

```cpp
bool get_fast_marching()
```


### function set_fast_marching

```cpp
void set_fast_marching(
    bool value
)
```


### function get_mesh_smooth

```cpp
bool get_mesh_smooth()
```


### function set_mesh_smooth

```cpp
void set_mesh_smooth(
    bool value
)
```


### function get_mesh_smoothing_method

```cpp
std::string get_mesh_smoothing_method()
```


### function set_mesh_smoothing_method

```cpp
void set_mesh_smoothing_method(
    std::string method
)
```


### function get_mesh_vtk_laplacian_iterations

```cpp
int get_mesh_vtk_laplacian_iterations()
```


### function set_mesh_vtk_laplacian_iterations

```cpp
void set_mesh_vtk_laplacian_iterations(
    int iterations
)
```


### function get_mesh_vtk_laplacian_relaxation

```cpp
double get_mesh_vtk_laplacian_relaxation()
```


### function set_mesh_vtk_laplacian_relaxation

```cpp
void set_mesh_vtk_laplacian_relaxation(
    double relaxation
)
```


### function get_mesh_vtk_windowed_sinc_iterations

```cpp
int get_mesh_vtk_windowed_sinc_iterations()
```


### function set_mesh_vtk_windowed_sinc_iterations

```cpp
void set_mesh_vtk_windowed_sinc_iterations(
    int iterations
)
```


### function get_mesh_vtk_windowed_sinc_passband

```cpp
double get_mesh_vtk_windowed_sinc_passband()
```


### function set_mesh_vtk_windowed_sinc_passband

```cpp
void set_mesh_vtk_windowed_sinc_passband(
    double passband
)
```


### function get_use_icp

```cpp
bool get_use_icp()
```


### function get_use_center

```cpp
bool get_use_center()
```


### function restore_defaults

```cpp
void restore_defaults()
```


## Public Attributes Documentation

### variable GROOM_SMOOTH_VTK_LAPLACIAN_C

```cpp
static const std::string GROOM_SMOOTH_VTK_LAPLACIAN_C;
```


### variable GROOM_SMOOTH_VTK_WINDOWED_SINC_C

```cpp
static const std::string GROOM_SMOOTH_VTK_WINDOWED_SINC_C;
```


### variable GROOM_ALIGNMENT_CENTER_C

```cpp
static const std::string GROOM_ALIGNMENT_CENTER_C;
```


### variable GROOM_ALIGNMENT_ICP_C

```cpp
static const std::string GROOM_ALIGNMENT_ICP_C;
```


-------------------------------

Updated on 2021-11-23 at 22:14:02 +0000