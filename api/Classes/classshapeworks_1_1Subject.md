---
title: shapeworks::Subject
summary: Representation of a single subject. 

---

# shapeworks::Subject



Representation of a single subject.  [More...](#detailed-description)


`#include <Subject.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Subject](../Classes/classshapeworks_1_1Subject.md#function-subject)**() |
| | **[~Subject](../Classes/classshapeworks_1_1Subject.md#function-~subject)**() |
| void | **[set_segmentation_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-segmentation-filenames)**(std::vector< std::string > filenames)<br>Set segmentatation filenames (one per domain)  |
| std::vector< std::string > | **[get_segmentation_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-segmentation-filenames)**()<br>Get segmentation filenames.  |
| std::vector< DomainType > | **[get_domain_types](../Classes/classshapeworks_1_1Subject.md#function-get-domain-types)**(bool groomed =false)<br>Return the domain types.  |
| void | **[set_groomed_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-groomed-filenames)**(std::vector< std::string > filenames)<br>Set groomed filenames.  |
| std::vector< std::string > | **[get_groomed_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-groomed-filenames)**()<br>Get groomed filenames.  |
| void | **[set_local_particle_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-local-particle-filenames)**(std::vector< std::string > filenames)<br>Set local particle filenames (one per domain)  |
| std::vector< std::string > | **[get_local_particle_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-local-particle-filenames)**()<br>Get local particle filenames.  |
| void | **[set_world_particle_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-world-particle-filenames)**(std::vector< std::string > filenames)<br>Set the world particle filenames.  |
| std::vector< std::string > | **[get_world_particle_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-world-particle-filenames)**()<br>Get the world particle filenames.  |
| void | **[set_landmarks_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-landmarks-filenames)**(std::vector< std::string > filenames)<br>Get the landmarks filenames (one per domain)  |
| std::vector< std::string > | **[get_landmarks_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-landmarks-filenames)**()<br>Set the landmarks filenames.  |
| void | **[set_number_of_domains](../Classes/classshapeworks_1_1Subject.md#function-set-number-of-domains)**(int number_of_domains)<br>Set the number of domains.  |
| int | **[get_number_of_domains](../Classes/classshapeworks_1_1Subject.md#function-get-number-of-domains)**()<br>Get the number of domains.  |
| void | **[set_image_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-image-filenames)**(std::vector< std::string > filenames)<br>Set image filenames.  |
| std::vector< std::string > | **[get_image_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-image-filenames)**()<br>Get image filenames.  |
| std::map< std::string, std::string > | **[get_feature_filenames](../Classes/classshapeworks_1_1Subject.md#function-get-feature-filenames)**() const<br>Get the feature map filenames.  |
| void | **[set_feature_filenames](../Classes/classshapeworks_1_1Subject.md#function-set-feature-filenames)**(const std::map< std::string, std::string > & feature_filenames)<br>Set the feature map filenames.  |
| std::vector< std::vector< double > > | **[get_groomed_transforms](../Classes/classshapeworks_1_1Subject.md#function-get-groomed-transforms)**() const<br>Get the groomed transforms (one vector per domain)  |
| void | **[set_groomed_transforms](../Classes/classshapeworks_1_1Subject.md#function-set-groomed-transforms)**(std::vector< std::vector< double >> transforms)<br>Set the groomed transforms (one vector per domain)  |
| void | **[set_groomed_transform](../Classes/classshapeworks_1_1Subject.md#function-set-groomed-transform)**(int i, std::vector< double > transform)<br>Set the i-th groomed transform.  |
| std::vector< std::vector< double > > | **[get_procrustes_transforms](../Classes/classshapeworks_1_1Subject.md#function-get-procrustes-transforms)**() const<br>Get the procrustes transforms (one vector per domain)  |
| void | **[set_procrustes_transforms](../Classes/classshapeworks_1_1Subject.md#function-set-procrustes-transforms)**(std::vector< std::vector< double >> transforms)<br>Set the procrustes transforms (one vector per domain)  |
| std::map< std::string, std::string > | **[get_group_values](../Classes/classshapeworks_1_1Subject.md#function-get-group-values)**() const<br>Get the group values.  |
| std::string | **[get_group_value](../Classes/classshapeworks_1_1Subject.md#function-get-group-value)**(std::string group_name)<br>Get a specific group value.  |
| void | **[set_group_values](../Classes/classshapeworks_1_1Subject.md#function-set-group-values)**(const std::map< std::string, std::string > & group_values)<br>Set the group values.  |
| std::map< std::string, std::string > | **[get_extra_values](../Classes/classshapeworks_1_1Subject.md#function-get-extra-values)**() const<br>Get extra values (extra columns we don't interpret)  |
| void | **[set_extra_values](../Classes/classshapeworks_1_1Subject.md#function-set-extra-values)**(std::map< std::string, std::string > extra_values) |
| std::map< std::string, std::string > | **[get_table_values](../Classes/classshapeworks_1_1Subject.md#function-get-table-values)**() const<br>Get all table values.  |
| void | **[set_table_values](../Classes/classshapeworks_1_1Subject.md#function-set-table-values)**(std::map< std::string, std::string > table_values) |
| std::string | **[get_display_name](../Classes/classshapeworks_1_1Subject.md#function-get-display-name)**()<br>Get the display name.  |
| void | **[set_display_name](../Classes/classshapeworks_1_1Subject.md#function-set-display-name)**(std::string display_name)<br>Set the display name.  |

## Detailed Description

```cpp
class shapeworks::Subject;
```

Representation of a single subject. 

The [Subject](../Classes/classshapeworks_1_1Subject.md) class encapsulates one "sample" in the shapeworks system (e.g. one row in a spreadsheet project) A [Subject](../Classes/classshapeworks_1_1Subject.md) may have multiple segmentations/domains associated with it. 

## Public Functions Documentation

### function Subject

```cpp
Subject()
```


### function ~Subject

```cpp
~Subject()
```


### function set_segmentation_filenames

```cpp
void set_segmentation_filenames(
    std::vector< std::string > filenames
)
```

Set segmentatation filenames (one per domain) 

### function get_segmentation_filenames

```cpp
std::vector< std::string > get_segmentation_filenames()
```

Get segmentation filenames. 

### function get_domain_types

```cpp
std::vector< DomainType > get_domain_types(
    bool groomed =false
)
```

Return the domain types. 

### function set_groomed_filenames

```cpp
void set_groomed_filenames(
    std::vector< std::string > filenames
)
```

Set groomed filenames. 

### function get_groomed_filenames

```cpp
std::vector< std::string > get_groomed_filenames()
```

Get groomed filenames. 

### function set_local_particle_filenames

```cpp
void set_local_particle_filenames(
    std::vector< std::string > filenames
)
```

Set local particle filenames (one per domain) 

### function get_local_particle_filenames

```cpp
std::vector< std::string > get_local_particle_filenames()
```

Get local particle filenames. 

### function set_world_particle_filenames

```cpp
void set_world_particle_filenames(
    std::vector< std::string > filenames
)
```

Set the world particle filenames. 

### function get_world_particle_filenames

```cpp
std::vector< std::string > get_world_particle_filenames()
```

Get the world particle filenames. 

### function set_landmarks_filenames

```cpp
void set_landmarks_filenames(
    std::vector< std::string > filenames
)
```

Get the landmarks filenames (one per domain) 

### function get_landmarks_filenames

```cpp
std::vector< std::string > get_landmarks_filenames()
```

Set the landmarks filenames. 

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

### function set_image_filenames

```cpp
void set_image_filenames(
    std::vector< std::string > filenames
)
```

Set image filenames. 

### function get_image_filenames

```cpp
std::vector< std::string > get_image_filenames()
```

Get image filenames. 

### function get_feature_filenames

```cpp
std::map< std::string, std::string > get_feature_filenames() const
```

Get the feature map filenames. 

### function set_feature_filenames

```cpp
void set_feature_filenames(
    const std::map< std::string, std::string > & feature_filenames
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
    std::vector< std::vector< double >> transforms
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
    std::vector< std::vector< double >> transforms
)
```

Set the procrustes transforms (one vector per domain) 

### function get_group_values

```cpp
std::map< std::string, std::string > get_group_values() const
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
    const std::map< std::string, std::string > & group_values
)
```

Set the group values. 

### function get_extra_values

```cpp
std::map< std::string, std::string > get_extra_values() const
```

Get extra values (extra columns we don't interpret) 

### function set_extra_values

```cpp
void set_extra_values(
    std::map< std::string, std::string > extra_values
)
```


### function get_table_values

```cpp
std::map< std::string, std::string > get_table_values() const
```

Get all table values. 

### function set_table_values

```cpp
void set_table_values(
    std::map< std::string, std::string > table_values
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

-------------------------------

Updated on 2022-01-27 at 02:24:32 +0000