---
title: shapeworks::ShapeWorksUtils

---

# shapeworks::ShapeWorksUtils





## Public Functions

|                | Name           |
| -------------- | -------------- |
| time_point | **[now](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-now)**()<br>get the current time  |
| double | **[elapsed](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-elapsed)**(time_point start, time_point end, bool print_elapsed =true)<br>return elapsed time in seconds, optionally printing to console  |
| unsigned | **[get_rng_seed](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-get-rng-seed)**()<br>returns random seed  |
| void | **[set_rng_seed](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-set-rng-seed)**(const unsigned seed)<br>sets random seed  |
| unsigned | **[get_random_number](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-get-random-number)**()<br>generates random number  |
| bool | **[is_directory](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-is-directory)**(const std::string & pathname)<br>returns true if pathname is a directory  |
| bool | **[file_exists](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-file-exists)**(const std::string & filename)<br>returns true if filename exists  |
| void | **[setup_console_logging](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-setup-console-logging)**(bool show_progress, bool xml_status)<br>Set up the console logging options.  |
| void | **[set_progress_callback](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-set-progress-callback)**(void * ptr)<br>Set up a progress callback.  |
| void | **[setup_threads](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-setup-threads)**()<br>Set up the number of threads from TBB_NUM_THREADS env variable.  |
| Matrix33 | **[convert_matrix](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-convert-matrix)**(const vtkSmartPointer< vtkMatrix4x4 > & mat)<br>converts a vtkMatrix4x4 to a Matrix33 and corresponding translationVector  |
| Vector3 | **[get_offset](../Classes/classshapeworks_1_1ShapeWorksUtils.md#function-get-offset)**(const vtkSmartPointer< vtkMatrix4x4 > & mat) |

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

### function get_rng_seed

```cpp
static inline unsigned get_rng_seed()
```

returns random seed 

### function set_rng_seed

```cpp
static void set_rng_seed(
    const unsigned seed
)
```

sets random seed 

### function get_random_number

```cpp
static inline unsigned get_random_number()
```

generates random number 

### function is_directory

```cpp
static bool is_directory(
    const std::string & pathname
)
```

returns true if pathname is a directory 

### function file_exists

```cpp
static bool file_exists(
    const std::string & filename
)
```

returns true if filename exists 

### function setup_console_logging

```cpp
static void setup_console_logging(
    bool show_progress,
    bool xml_status
)
```

Set up the console logging options. 

### function set_progress_callback

```cpp
static void set_progress_callback(
    void * ptr
)
```

Set up a progress callback. 

### function setup_threads

```cpp
static void setup_threads()
```

Set up the number of threads from TBB_NUM_THREADS env variable. 

### function convert_matrix

```cpp
static Matrix33 convert_matrix(
    const vtkSmartPointer< vtkMatrix4x4 > & mat
)
```

converts a vtkMatrix4x4 to a Matrix33 and corresponding translationVector 

### function get_offset

```cpp
static Vector3 get_offset(
    const vtkSmartPointer< vtkMatrix4x4 > & mat
)
```


-------------------------------

Updated on 2024-07-10 at 19:31:21 +0000