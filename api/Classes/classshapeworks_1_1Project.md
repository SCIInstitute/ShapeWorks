---
title: shapeworks::Project
summary: Representation of a project. 

---

# shapeworks::Project



Representation of a project.  [More...](#detailed-description)


`#include <Project.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| | **[Project](../Classes/classshapeworks_1_1Project.md#function-project)**() |
| | **[~Project](../Classes/classshapeworks_1_1Project.md#function-~project)**() |
| bool | **[load](../Classes/classshapeworks_1_1Project.md#function-load)**(const std::string & filename)<br>Load from XLSX file.  |
| bool | **[save](../Classes/classshapeworks_1_1Project.md#function-save)**(const std::string & filename)<br>Save to XLSX file.  |
| std::string | **[get_filename](../Classes/classshapeworks_1_1Project.md#function-get-filename)**()<br>Return the filename.  |
| void | **[set_filename](../Classes/classshapeworks_1_1Project.md#function-set-filename)**(std::string filename)<br>Set project filename.  |
| std::vector< std::string > | **[get_headers](../Classes/classshapeworks_1_1Project.md#function-get-headers)**()<br>Return the headers of the subject sheet.  |
| std::vector< std::string > | **[get_string_column](../Classes/classshapeworks_1_1Project.md#function-get-string-column)**(const std::string & name) const<br>Return a column by name.  |
| int | **[get_number_of_subjects](../Classes/classshapeworks_1_1Project.md#function-get-number-of-subjects)**()<br>Return the number of subjects in the project.  |
| int | **[get_number_of_domains_per_subject](../Classes/classshapeworks_1_1Project.md#function-get-number-of-domains-per-subject)**()<br>Return the number of domains.  |
| std::vector< std::string > | **[get_domain_names](../Classes/classshapeworks_1_1Project.md#function-get-domain-names)**()<br>Return the domain names (e.g. femur, pelvis, etc)  |
| std::vector< std::shared_ptr< [Subject](../Classes/classshapeworks_1_1Subject.md) > > & | **[get_subjects](../Classes/classshapeworks_1_1Project.md#function-get-subjects)**()<br>Return the list of Subjects.  |
| bool | **[get_segmentations_present](../Classes/classshapeworks_1_1Project.md#function-get-segmentations-present)**() const<br>Return if segmentations are present.  |
| bool | **[get_groomed_present](../Classes/classshapeworks_1_1Project.md#function-get-groomed-present)**() const<br>Return if groomed files are present.  |
| bool | **[get_particles_present](../Classes/classshapeworks_1_1Project.md#function-get-particles-present)**() const<br>Return if particle files are present.  |
| bool | **[get_images_present](../Classes/classshapeworks_1_1Project.md#function-get-images-present)**()<br>Return if images are present (e.g. CT/MRI)  |
| std::vector< std::string > | **[get_feature_names](../Classes/classshapeworks_1_1Project.md#function-get-feature-names)**()<br>Get feature names.  |
| std::vector< std::string > | **[get_group_names](../Classes/classshapeworks_1_1Project.md#function-get-group-names)**()<br>Get group names.  |
| std::vector< std::string > | **[get_group_values](../Classes/classshapeworks_1_1Project.md#function-get-group-values)**(const std::string & group_name) const<br>Get possible group values.  |
| [Parameters](../Classes/classshapeworks_1_1Parameters.md) | **[get_parameters](../Classes/classshapeworks_1_1Project.md#function-get-parameters)**(const std::string & name, const std::string & domain_name ="")<br>Retrieve parameters based on key.  |
| void | **[set_parameters](../Classes/classshapeworks_1_1Project.md#function-set-parameters)**(const std::string & name, [Parameters](../Classes/classshapeworks_1_1Parameters.md) params, const std::string & domain_name ="")<br>Store parameters based on key.  |
| void | **[clear_parameters](../Classes/classshapeworks_1_1Project.md#function-clear-parameters)**(const std::string & name)<br>Clear parameters based on key.  |
| void | **[store_subjects](../Classes/classshapeworks_1_1Project.md#function-store-subjects)**()<br>Store from subject list to spreadsheet.  |
| int | **[get_supported_version](../Classes/classshapeworks_1_1Project.md#function-get-supported-version)**() const<br>Get the supported version (this version of the code)  |
| int | **[get_version](../Classes/classshapeworks_1_1Project.md#function-get-version)**() const<br>Get the version of the currently loaded project.  |

## Detailed Description

```cpp
class shapeworks::Project;
```

Representation of a project. 

The [Project](../Classes/classshapeworks_1_1Project.md) class encapsulates the spreadsheet based file format for storing project data. 

## Public Functions Documentation

### function Project

```cpp
Project()
```


### function ~Project

```cpp
~Project()
```


### function load

```cpp
bool load(
    const std::string & filename
)
```

Load from XLSX file. 

### function save

```cpp
bool save(
    const std::string & filename
)
```

Save to XLSX file. 

### function get_filename

```cpp
std::string get_filename()
```

Return the filename. 

### function set_filename

```cpp
void set_filename(
    std::string filename
)
```

Set project filename. 

### function get_headers

```cpp
std::vector< std::string > get_headers()
```

Return the headers of the subject sheet. 

### function get_string_column

```cpp
std::vector< std::string > get_string_column(
    const std::string & name
) const
```

Return a column by name. 

### function get_number_of_subjects

```cpp
int get_number_of_subjects()
```

Return the number of subjects in the project. 

### function get_number_of_domains_per_subject

```cpp
int get_number_of_domains_per_subject()
```

Return the number of domains. 

### function get_domain_names

```cpp
std::vector< std::string > get_domain_names()
```

Return the domain names (e.g. femur, pelvis, etc) 

### function get_subjects

```cpp
std::vector< std::shared_ptr< Subject > > & get_subjects()
```

Return the list of Subjects. 

### function get_segmentations_present

```cpp
bool get_segmentations_present() const
```

Return if segmentations are present. 

### function get_groomed_present

```cpp
bool get_groomed_present() const
```

Return if groomed files are present. 

### function get_particles_present

```cpp
bool get_particles_present() const
```

Return if particle files are present. 

### function get_images_present

```cpp
bool get_images_present()
```

Return if images are present (e.g. CT/MRI) 

### function get_feature_names

```cpp
std::vector< std::string > get_feature_names()
```

Get feature names. 

### function get_group_names

```cpp
std::vector< std::string > get_group_names()
```

Get group names. 

### function get_group_values

```cpp
std::vector< std::string > get_group_values(
    const std::string & group_name
) const
```

Get possible group values. 

### function get_parameters

```cpp
Parameters get_parameters(
    const std::string & name,
    const std::string & domain_name =""
)
```

Retrieve parameters based on key. 

### function set_parameters

```cpp
void set_parameters(
    const std::string & name,
    Parameters params,
    const std::string & domain_name =""
)
```

Store parameters based on key. 

### function clear_parameters

```cpp
void clear_parameters(
    const std::string & name
)
```

Clear parameters based on key. 

### function store_subjects

```cpp
void store_subjects()
```

Store from subject list to spreadsheet. 

### function get_supported_version

```cpp
int get_supported_version() const
```

Get the supported version (this version of the code) 

### function get_version

```cpp
int get_version() const
```

Get the version of the currently loaded project. 

-------------------------------

Updated on 2022-01-06 at 21:52:22 +0000