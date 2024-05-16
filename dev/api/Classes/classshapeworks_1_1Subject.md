---
title: shapeworks::Subject
summary: Representation of a single subject. 

---

# shapeworks::Subject



Representation of a single subject.  [More...](#detailed-description)


`#include <Subject.h>`

## Public Types

|                | Name           |
| -------------- | -------------- |
| using project::types::StringMap | **[StringMap](../Classes/classshapeworks_1_1Subject.md#using-stringmap)**  |
| using project::types::StringList | **[StringList](../Classes/classshapeworks_1_1Subject.md#using-stringlist)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Subject](../Classes/classshapeworks_1_1Subject.md#function-subject)**() |
| | **[~Subject](../Classes/classshapeworks_1_1Subject.md#function-~subject)**() |
| void | **[set_original_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-original-filenames)**(StringList filenames)<br>Set original filenames (one per domain)  |
| StringList | **[get_original_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-original-filenames)**()<br>Get original filenames.  |
| void | **[set_groomed_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-groomed-filenames)**(StringList filenames)<br>Set groomed filenames.  |
| StringList | **[get_groomed_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-groomed-filenames)**()<br>Get groomed filenames.  |
| void | **[set_local_particle_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-local-particle-filenames)**(StringList filenames)<br>Set local particle filenames (one per domain)  |
| StringList | **[get_local_particle_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-local-particle-filenames)**()<br>Get local particle filenames.  |
| void | **[set_world_particle_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-world-particle-filenames)**(StringList filenames)<br>Set the world particle filenames.  |
| StringList | **[get_world_particle_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-world-particle-filenames)**()<br>Get the world particle filenames.  |
| void | **[set_landmarks_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-landmarks-filenames)**(StringList filenames)<br>Get the landmarks filenames (one per domain)  |
| StringList | **[get_landmarks_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-landmarks-filenames)**()<br>Set the landmarks filenames.  |
| void | **[set_constraints_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-constraints-filenames)**(StringList filenames)<br>Get the constraints filenames (one per domain)  |
| StringList | **[get_constraints_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-constraints-filenames)**()<br>Set the constratins filenames.  |
| void | **[set_number_of_domains](../Classes/classshapeworks_1_1Subject.md#function-set-number-of-domains)**(int number_of_domains)<br>Set the number of domains.  |
| int | **[get_number_of_domains](../Classes/classshapeworks_1_1Subject.md#function-get-number-of-domains)**()<br>Get the number of domains.  |
| StringMap | **[get_feature_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-feature-filenames)**() const<br>Get the feature map filenames.  |
| void | **[set_feature_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-feature-filenames)**(const StringMap & feature_filenames)<br>Set the feature map filenames.  |
| std::vector< std::vector< double > > | **[get_groomed_transforms](../Classes/classshapeworks_1_1Subject.md#function-get-groomed-transforms)**() const<br>Get the groomed transforms (one vector per domain)  |
| void | **[set_groomed_transforms](../Classes/classshapeworks_1_1Subject.md#function-set-groomed-transforms)**(std::vector< std::vector< double > > transforms)<br>Set the groomed transforms (one vector per domain)  |
| void | **[set_groomed_transform](../Classes/classshapeworks_1_1Subject.md#function-set-groomed-transform)**(int i, std::vector< double > transform)<br>Set the i-th groomed transform.  |
| std::vector< std::vector< double > > | **[get_procrustes_transforms](../Classes/classshapeworks_1_1Subject.md#function-get-procrustes-transforms)**() const<br>Get the procrustes transforms (one vector per domain)  |
| void | **[set_procrustes_transforms](../Classes/classshapeworks_1_1Subject.md#function-set-procrustes-transforms)**(std::vector< std::vector< double > > transforms)<br>Set the procrustes transforms (one vector per domain)  |
| StringMap | **[get_group_values](../Classes/classshapeworks_1_1Subject.md#function-get-group-values)**() const<br>Get the group values.  |
| std::string | **[get_group_value](../Classes/classshapeworks_1_1Subject.md#function-get-group-value)**(std::string group_name)<br>Get a specific group value.  |
| void | **[set_group_values](../Classes/classshapeworks_1_1Subject.md#function-set-group-values)**(const StringMap & group_values)<br>Set the group values.  |
| StringMap | **[get_extra_values](../Classes/classshapeworks_1_1Subject.md#function-get-extra-values)**() const<br>Get extra values (extra columns we don't interpret)  |
| void | **[set_extra_values](../Classes/classshapeworks_1_1Subject.md#function-set-extra-values)**(StringMap extra_values) |
| StringMap | **[get_table_values](../Classes/classshapeworks_1_1Subject.md#function-get-table-values)**() const<br>Get all table values.  |
| void | **[set_table_values](../Classes/classshapeworks_1_1Subject.md#function-set-table-values)**(StringMap table_values) |
| std::string | **[get_display_name](../Classes/classshapeworks_1_1Subject.md#function-get-display-name)**()<br>Get the display name.  |
| void | **[set_display_name](../Classes/classshapeworks_1_1Subject.md#function-set-display-name)**(std::string display_name)<br>Set the display name.  |
| bool | **[is_fixed](../Classes/classshapeworks_1_1Subject.md#function-is-fixed)**()<br>Get if this subject is fixed or not.  |
| void | **[set_fixed](../Classes/classshapeworks_1_1Subject.md#function-set-fixed)**(bool fixed)<br>Set if this subject is fixed or not.  |
| bool | **[is_excluded](../Classes/classshapeworks_1_1Subject.md#function-is-excluded)**()<br>Get if this subject is excluded or not.  |
| void | **[set_excluded](../Classes/classshapeworks_1_1Subject.md#function-set-excluded)**(bool excluded)<br>Set if this subject is excluded or not.  |
| std::string | **[get_notes](../Classes/classshapeworks_1_1Subject.md#function-get-notes)**()<br>Get the notes.  |
| void | **[set_notes](../Classes/classshapeworks_1_1Subject.md#function-set-notes)**(std::string notes)<br>Set the notes.  |

## Detailed Description

```cpp
class shapeworks::Subject;
```

Representation of a single subject. 

The [Subject](../Classes/classshapeworks_1_1Subject.md) class encapsulates one "sample" in the shapeworks system (e.g. one row in a spreadsheet project) A [Subject](../Classes/classshapeworks_1_1Subject.md) may have multiple segmentations/domains associated with it. 

## Public Types Documentation

### using StringMap

```cpp
using shapeworks::Subject::StringMap =  project::types::StringMap;
```


### using StringList

```cpp
using shapeworks::Subject::StringList =  project::types::StringList;
```


## Public Functions Documentation

### function Subject

```cpp
Subject()
```


### function ~Subject

```cpp
~Subject()
```


### function set_original_filenames

```cpp
void set_original_filenames(
    StringList filenames
)
```

Set original filenames (one per domain) 

### function get_original_filenames

```cpp
StringList get_original_filenames()
```

Get original filenames. 

### function set_groomed_filenames

```cpp
void set_groomed_filenames(
    StringList filenames
)
```

Set groomed filenames. 

### function get_groomed_filenames

```cpp
StringList get_groomed_filenames()
```

Get groomed filenames. 

### function set_local_particle_filenames

```cpp
void set_local_particle_filenames(
    StringList filenames
)
```

Set local particle filenames (one per domain) 

### function get_local_particle_filenames

```cpp
StringList get_local_particle_filenames()
```

Get local particle filenames. 

### function set_world_particle_filenames

```cpp
void set_world_particle_filenames(
    StringList filenames
)
```

Set the world particle filenames. 

### function get_world_particle_filenames

```cpp
StringList get_world_particle_filenames()
```

Get the world particle filenames. 

### function set_landmarks_filenames

```cpp
void set_landmarks_filenames(
    StringList filenames
)
```

Get the landmarks filenames (one per domain) 

### function get_landmarks_filenames

```cpp
StringList get_landmarks_filenames()
```

Set the landmarks filenames. 

### function set_constraints_filenames

```cpp
void set_constraints_filenames(
    StringList filenames
)
```

Get the constraints filenames (one per domain) 

### function get_constraints_filenames

```cpp
StringList get_constraints_filenames()
```

Set the constratins filenames. 

### function set_number_of_domains

```cpp
void set_number_of_domains(
    int number_of_domains
)
```

Set the number of domains. 

### function get_number_of_domains

```cpp
int get_number_of_domains()
```

Get the number of domains. 

### function get_feature_filenames

```cpp
StringMap get_feature_filenames() const
```

Get the feature map filenames. 

### function set_feature_filenames

```cpp
void set_feature_filenames(
    const StringMap & feature_filenames
)
```

Set the feature map filenames. 

### function get_groomed_transforms

```cpp
std::vector< std::vector< double > > get_groomed_transforms() const
```

Get the groomed transforms (one vector per domain) 

### function set_groomed_transforms

```cpp
void set_groomed_transforms(
    std::vector< std::vector< double > > transforms
)
```

Set the groomed transforms (one vector per domain) 

### function set_groomed_transform

```cpp
void set_groomed_transform(
    int i,
    std::vector< double > transform
)
```

Set the i-th groomed transform. 

### function get_procrustes_transforms

```cpp
std::vector< std::vector< double > > get_procrustes_transforms() const
```

Get the procrustes transforms (one vector per domain) 

### function set_procrustes_transforms

```cpp
void set_procrustes_transforms(
    std::vector< std::vector< double > > transforms
)
```

Set the procrustes transforms (one vector per domain) 

### function get_group_values

```cpp
StringMap get_group_values() const
```

Get the group values. 

### function get_group_value

```cpp
std::string get_group_value(
    std::string group_name
)
```

Get a specific group value. 

### function set_group_values

```cpp
void set_group_values(
    const StringMap & group_values
)
```

Set the group values. 

### function get_extra_values

```cpp
StringMap get_extra_values() const
```

Get extra values (extra columns we don't interpret) 

### function set_extra_values

```cpp
void set_extra_values(
    StringMap extra_values
)
```


### function get_table_values

```cpp
StringMap get_table_values() const
```

Get all table values. 

### function set_table_values

```cpp
void set_table_values(
    StringMap table_values
)
```


### function get_display_name

```cpp
std::string get_display_name()
```

Get the display name. 

### function set_display_name

```cpp
void set_display_name(
    std::string display_name
)
```

Set the display name. 

### function is_fixed

```cpp
bool is_fixed()
```

Get if this subject is fixed or not. 

### function set_fixed

```cpp
void set_fixed(
    bool fixed
)
```

Set if this subject is fixed or not. 

### function is_excluded

```cpp
bool is_excluded()
```

Get if this subject is excluded or not. 

### function set_excluded

```cpp
void set_excluded(
    bool excluded
)
```

Set if this subject is excluded or not. 

### function get_notes

```cpp
std::string get_notes()
```

Get the notes. 

### function set_notes

```cpp
void set_notes(
    std::string notes
)
```

Set the notes. 

-------------------------------

Updated on 2024-05-16 at 20:36:01 +0000