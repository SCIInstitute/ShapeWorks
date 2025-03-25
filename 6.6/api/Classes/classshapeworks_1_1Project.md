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
| bool | **[load](../Classes/classshapeworks_1_1Project.md#function-load)**(const std::string & filename)<br>Load from project file.  |
| bool | **[save](../Classes/classshapeworks_1_1Project.md#function-save)**(const std::string & filename ="")<br>Save to project file.  |
| void | **[set_project_path](../Classes/classshapeworks_1_1Project.md#function-set-project-path)**(const std::string & path)<br>Set the project path.  |
| std::string | **[get_project_path](../Classes/classshapeworks_1_1Project.md#function-get-project-path)**()<br>Return the project path.  |
| std::string | **[get_filename](../Classes/classshapeworks_1_1Project.md#function-get-filename)**()<br>Return the filename.  |
| void | **[set_filename](../Classes/classshapeworks_1_1Project.md#function-set-filename)**(std::string filename)<br>Set project filename.  |
| std::vector< std::string > | **[get_headers](../Classes/classshapeworks_1_1Project.md#function-get-headers)**()<br>Return the headers of the subject sheet.  |
| std::vector< std::string > | **[get_string_column](../Classes/classshapeworks_1_1Project.md#function-get-string-column)**(const std::string & name) const<br>Return a column by name.  |
| int | **[get_number_of_subjects](../Classes/classshapeworks_1_1Project.md#function-get-number-of-subjects)**()<br>Return the number of subjects in the project.  |
| int | **[get_number_of_domains_per_subject](../Classes/classshapeworks_1_1Project.md#function-get-number-of-domains-per-subject)**()<br>Return the number of domains.  |
| std::vector< std::string > | **[get_domain_names](../Classes/classshapeworks_1_1Project.md#function-get-domain-names)**()<br>Return the domain names (e.g. femur, pelvis, etc)  |
| void | **[set_domain_names](../Classes/classshapeworks_1_1Project.md#function-set-domain-names)**(std::vector< std::string > domain_names)<br>Set the domain names.  |
| std::vector< std::shared_ptr< [Subject](../Classes/classshapeworks_1_1Subject.md) > > & | **[get_subjects](../Classes/classshapeworks_1_1Project.md#function-get-subjects)**()<br>Return the Subjects (as a reference)  |
| std::vector< std::shared_ptr< [Subject](../Classes/classshapeworks_1_1Subject.md) > > | **[get_non_excluded_subjects](../Classes/classshapeworks_1_1Project.md#function-get-non-excluded-subjects)**()<br>Return the non-excluded Subjects (as a reference)  |
| void | **[set_subjects](../Classes/classshapeworks_1_1Project.md#function-set-subjects)**(const std::vector< std::shared_ptr< [Subject](../Classes/classshapeworks_1_1Subject.md) > > & subjects)<br>Set the Subjects.  |
| void | **[update_subjects](../Classes/classshapeworks_1_1Project.md#function-update-subjects)**() |
| bool | **[get_originals_present](../Classes/classshapeworks_1_1Project.md#function-get-originals-present)**() const<br>Return if originals are present.  |
| bool | **[get_groomed_present](../Classes/classshapeworks_1_1Project.md#function-get-groomed-present)**() const<br>Return if groomed files are present.  |
| bool | **[get_particles_present](../Classes/classshapeworks_1_1Project.md#function-get-particles-present)**() const<br>Return if particle files are present.  |
| bool | **[get_images_present](../Classes/classshapeworks_1_1Project.md#function-get-images-present)**()<br>Return if images are present (e.g. CT/MRI)  |
| bool | **[get_fixed_subjects_present](../Classes/classshapeworks_1_1Project.md#function-get-fixed-subjects-present)**()<br>Return if there are fixed subjects present.  |
| bool | **[get_excluded_subjects_present](../Classes/classshapeworks_1_1Project.md#function-get-excluded-subjects-present)**()<br>Return if there are excluded subjects present.  |
| std::vector< std::string > | **[get_feature_names](../Classes/classshapeworks_1_1Project.md#function-get-feature-names)**()<br>Get feature names.  |
| std::vector< std::string > | **[get_image_names](../Classes/classshapeworks_1_1Project.md#function-get-image-names)**()<br>Get image names.  |
| std::vector< std::string > | **[get_group_names](../Classes/classshapeworks_1_1Project.md#function-get-group-names)**()<br>Get group names.  |
| std::vector< std::string > | **[get_group_values](../Classes/classshapeworks_1_1Project.md#function-get-group-values)**(const std::string & group_name) const<br>Get possible group values.  |
| [Parameters](../Classes/classshapeworks_1_1Parameters.md) | **[get_parameters](../Classes/classshapeworks_1_1Project.md#function-get-parameters)**(const std::string & name, std::string domain_name ="")<br>Retrieve parameters based on key.  |
| std::map< std::string, [Parameters](../Classes/classshapeworks_1_1Parameters.md) > | **[get_parameter_map](../Classes/classshapeworks_1_1Project.md#function-get-parameter-map)**(const std::string & name)<br>Retrieve full parameter map for a given name.  |
| void | **[set_parameter_map](../Classes/classshapeworks_1_1Project.md#function-set-parameter-map)**(const std::string & name, std::map< std::string, [Parameters](../Classes/classshapeworks_1_1Parameters.md) > map)<br>Set parameter map for a given name.  |
| void | **[set_parameters](../Classes/classshapeworks_1_1Project.md#function-set-parameters)**(const std::string & name, [Parameters](../Classes/classshapeworks_1_1Parameters.md) params, std::string domain_name ="")<br>Store parameters based on key.  |
| void | **[clear_parameters](../Classes/classshapeworks_1_1Project.md#function-clear-parameters)**(const std::string & name)<br>Clear parameters based on key.  |
| int | **[get_supported_version](../Classes/classshapeworks_1_1Project.md#function-get-supported-version)**() const<br>Get the supported version (this version of the code)  |
| int | **[get_version](../Classes/classshapeworks_1_1Project.md#function-get-version)**() const<br>Get the version of the currently loaded project.  |
| std::vector< [LandmarkDefinition](../Classes/classshapeworks_1_1LandmarkDefinition.md) > | **[get_landmarks](../Classes/classshapeworks_1_1Project.md#function-get-landmarks)**(int domain_id)<br>Return the set of landmarks definitions for a particular domain.  |
| std::vector< std::vector< [LandmarkDefinition](../Classes/classshapeworks_1_1LandmarkDefinition.md) > > | **[get_all_landmark_definitions](../Classes/classshapeworks_1_1Project.md#function-get-all-landmark-definitions)**()<br>Return all landmark definitions.  |
| void | **[set_landmark_definitions](../Classes/classshapeworks_1_1Project.md#function-set-landmark-definitions)**(std::vector< std::vector< [LandmarkDefinition](../Classes/classshapeworks_1_1LandmarkDefinition.md) > > defs)<br>Set all landmark definitions.  |
| bool | **[get_landmarks_present](../Classes/classshapeworks_1_1Project.md#function-get-landmarks-present)**()<br>Return if landmarks are present.  |
| void | **[set_landmarks](../Classes/classshapeworks_1_1Project.md#function-set-landmarks)**(int domain_id, std::vector< [LandmarkDefinition](../Classes/classshapeworks_1_1LandmarkDefinition.md) > landmarks)<br>Set landmarks for this project.  |
| void | **[new_landmark](../Classes/classshapeworks_1_1Project.md#function-new-landmark)**(int domain_id)<br>Add a new landmark.  |
| std::vector< DomainType > | **[get_original_domain_types](../Classes/classshapeworks_1_1Project.md#function-get-original-domain-types)**()<br>Return the original domain types.  |
| std::vector< DomainType > | **[get_groomed_domain_types](../Classes/classshapeworks_1_1Project.md#function-get-groomed-domain-types)**()<br>Return the groomed domain types.  |
| void | **[set_original_domain_types](../Classes/classshapeworks_1_1Project.md#function-set-original-domain-types)**(std::vector< DomainType > domain_types)<br>Set the original domain types.  |
| void | **[set_groomed_domain_types](../Classes/classshapeworks_1_1Project.md#function-set-groomed-domain-types)**(std::vector< DomainType > domain_types)<br>Get the groomed domain types.  |

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

Load from project file. 

### function save

```cpp
bool save(
    const std::string & filename =""
)
```

Save to project file. 

### function set_project_path

```cpp
void set_project_path(
    const std::string & path
)
```

Set the project path. 

### function get_project_path

```cpp
std::string get_project_path()
```

Return the project path. 

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

### function set_domain_names

```cpp
void set_domain_names(
    std::vector< std::string > domain_names
)
```

Set the domain names. 

### function get_subjects

```cpp
std::vector< std::shared_ptr< Subject > > & get_subjects()
```

Return the Subjects (as a reference) 

### function get_non_excluded_subjects

```cpp
std::vector< std::shared_ptr< Subject > > get_non_excluded_subjects()
```

Return the non-excluded Subjects (as a reference) 

### function set_subjects

```cpp
void set_subjects(
    const std::vector< std::shared_ptr< Subject > > & subjects
)
```

Set the Subjects. 

### function update_subjects

```cpp
void update_subjects()
```


### function get_originals_present

```cpp
bool get_originals_present() const
```

Return if originals are present. 

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

### function get_fixed_subjects_present

```cpp
bool get_fixed_subjects_present()
```

Return if there are fixed subjects present. 

### function get_excluded_subjects_present

```cpp
bool get_excluded_subjects_present()
```

Return if there are excluded subjects present. 

### function get_feature_names

```cpp
std::vector< std::string > get_feature_names()
```

Get feature names. 

### function get_image_names

```cpp
std::vector< std::string > get_image_names()
```

Get image names. 

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
    std::string domain_name =""
)
```

Retrieve parameters based on key. 

### function get_parameter_map

```cpp
std::map< std::string, Parameters > get_parameter_map(
    const std::string & name
)
```

Retrieve full parameter map for a given name. 

### function set_parameter_map

```cpp
void set_parameter_map(
    const std::string & name,
    std::map< std::string, Parameters > map
)
```

Set parameter map for a given name. 

### function set_parameters

```cpp
void set_parameters(
    const std::string & name,
    Parameters params,
    std::string domain_name =""
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

### function get_landmarks

```cpp
std::vector< LandmarkDefinition > get_landmarks(
    int domain_id
)
```

Return the set of landmarks definitions for a particular domain. 

### function get_all_landmark_definitions

```cpp
std::vector< std::vector< LandmarkDefinition > > get_all_landmark_definitions()
```

Return all landmark definitions. 

### function set_landmark_definitions

```cpp
void set_landmark_definitions(
    std::vector< std::vector< LandmarkDefinition > > defs
)
```

Set all landmark definitions. 

### function get_landmarks_present

```cpp
bool get_landmarks_present()
```

Return if landmarks are present. 

### function set_landmarks

```cpp
void set_landmarks(
    int domain_id,
    std::vector< LandmarkDefinition > landmarks
)
```

Set landmarks for this project. 

### function new_landmark

```cpp
void new_landmark(
    int domain_id
)
```

Add a new landmark. 

### function get_original_domain_types

```cpp
std::vector< DomainType > get_original_domain_types()
```

Return the original domain types. 

### function get_groomed_domain_types

```cpp
std::vector< DomainType > get_groomed_domain_types()
```

Return the groomed domain types. 

### function set_original_domain_types

```cpp
void set_original_domain_types(
    std::vector< DomainType > domain_types
)
```

Set the original domain types. 

### function set_groomed_domain_types

```cpp
void set_groomed_domain_types(
    std::vector< DomainType > domain_types
)
```

Get the groomed domain types. 

-------------------------------

Updated on 2025-03-25 at 08:57:11 +0000