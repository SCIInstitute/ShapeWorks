---
title: shapeworks::ProjectUtils

---

# shapeworks::ProjectUtils





## Public Types

|                | Name           |
| -------------- | -------------- |
| using project::types::StringList | **[StringList](../Classes/classshapeworks_1_1ProjectUtils.md#using-stringlist)**  |
| using project::types::StringMap | **[StringMap](../Classes/classshapeworks_1_1ProjectUtils.md#using-stringmap)**  |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| vtkSmartPointer< vtkTransform > | **[convert_transform](../Classes/classshapeworks_1_1ProjectUtils.md#function-convert-transform)**(std::vector< double > list)<br>convert a list of doubles from a spreadsheet to a vtkTransform  |
| std::vector< double > | **[convert_transform](../Classes/classshapeworks_1_1ProjectUtils.md#function-convert-transform)**(vtkSmartPointer< vtkTransform > transform)<br>convert a vtkTransform to a list of doubles (e.g. for project spreadsheet)  |
| StringList | **[determine_domain_names](../Classes/classshapeworks_1_1ProjectUtils.md#function-determine-domain-names)**(StringList keys)<br>determine domain names from a subject's keys  |
| void | **[determine_domain_types](../Classes/classshapeworks_1_1ProjectUtils.md#function-determine-domain-types)**([Project](../Classes/classshapeworks_1_1Project.md) * project, StringMap key_map)<br>determine and set domain types  |
| StringList | **[get_input_prefixes](../Classes/classshapeworks_1_1ProjectUtils.md#function-get-input-prefixes)**()<br>return a list of the input prefixes  |
| StringList | **[get_groomed_prefixes](../Classes/classshapeworks_1_1ProjectUtils.md#function-get-groomed-prefixes)**()<br>return a list of the groomed prefixes  |
| StringList | **[get_original_keys](../Classes/classshapeworks_1_1ProjectUtils.md#function-get-original-keys)**(StringList domain_names, StringMap key_map)<br>extract original keys from key_map given an ordered list of domain names  |
| StringList | **[get_values](../Classes/classshapeworks_1_1ProjectUtils.md#function-get-values)**(StringList prefixes, StringList domain_names, StringMap key_map)<br>get the values for a list of prefixes from key_map, ordered by domain_names  |
| std::vector< std::vector< double > > | **[get_transforms](../Classes/classshapeworks_1_1ProjectUtils.md#function-get-transforms)**(std::string prefix, StringList domain_names, StringMap key_map)<br>get the transforms for a prefix from key_map, ordered by domain_names  |
| StringMap | **[get_value_map](../Classes/classshapeworks_1_1ProjectUtils.md#function-get-value-map)**(std::vector< std::string > prefix, StringMap key_map)<br>get a map<string,string> for a list of prefixes from a key_map (e.g. group_, or feature_)  |
| StringMap | **[get_extra_columns](../Classes/classshapeworks_1_1ProjectUtils.md#function-get-extra-columns)**(StringMap key_map)<br>get any extra columns that don't match prefixes  |
| DomainType | **[determine_domain_type](../Classes/classshapeworks_1_1ProjectUtils.md#function-determine-domain-type)**(std::string filename)<br>determine the domain type, given a filename  |
| bool | **[starts_with](../Classes/classshapeworks_1_1ProjectUtils.md#function-starts-with)**(std::string str, std::string prefix) |
| std::string | **[transform_to_string](../Classes/classshapeworks_1_1ProjectUtils.md#function-transform-to-string)**(std::vector< double > transform) |
| std::vector< std::string > | **[convert_domain_types](../Classes/classshapeworks_1_1ProjectUtils.md#function-convert-domain-types)**(std::vector< DomainType > domain_types) |
| std::vector< std::string > | **[convert_groomed_domain_types](../Classes/classshapeworks_1_1ProjectUtils.md#function-convert-groomed-domain-types)**(std::vector< DomainType > domain_types) |
| StringMap | **[convert_subject_to_map](../Classes/classshapeworks_1_1ProjectUtils.md#function-convert-subject-to-map)**([Project](../Classes/classshapeworks_1_1Project.md) * project, [Subject](../Classes/classshapeworks_1_1Subject.md) * subject) |

## Public Types Documentation

### using StringList

```cpp
using shapeworks::ProjectUtils::StringList =  project::types::StringList;
```


### using StringMap

```cpp
using shapeworks::ProjectUtils::StringMap =  project::types::StringMap;
```


## Public Functions Documentation

### function convert_transform

```cpp
static vtkSmartPointer< vtkTransform > convert_transform(
    std::vector< double > list
)
```

convert a list of doubles from a spreadsheet to a vtkTransform 

### function convert_transform

```cpp
static std::vector< double > convert_transform(
    vtkSmartPointer< vtkTransform > transform
)
```

convert a vtkTransform to a list of doubles (e.g. for project spreadsheet) 

### function determine_domain_names

```cpp
static StringList determine_domain_names(
    StringList keys
)
```

determine domain names from a subject's keys 

### function determine_domain_types

```cpp
static void determine_domain_types(
    Project * project,
    StringMap key_map
)
```

determine and set domain types 

### function get_input_prefixes

```cpp
static StringList get_input_prefixes()
```

return a list of the input prefixes 

### function get_groomed_prefixes

```cpp
static StringList get_groomed_prefixes()
```

return a list of the groomed prefixes 

### function get_original_keys

```cpp
static StringList get_original_keys(
    StringList domain_names,
    StringMap key_map
)
```

extract original keys from key_map given an ordered list of domain names 

### function get_values

```cpp
static StringList get_values(
    StringList prefixes,
    StringList domain_names,
    StringMap key_map
)
```

get the values for a list of prefixes from key_map, ordered by domain_names 

### function get_transforms

```cpp
static std::vector< std::vector< double > > get_transforms(
    std::string prefix,
    StringList domain_names,
    StringMap key_map
)
```

get the transforms for a prefix from key_map, ordered by domain_names 

### function get_value_map

```cpp
static StringMap get_value_map(
    std::vector< std::string > prefix,
    StringMap key_map
)
```

get a map<string,string> for a list of prefixes from a key_map (e.g. group_, or feature_) 

### function get_extra_columns

```cpp
static StringMap get_extra_columns(
    StringMap key_map
)
```

get any extra columns that don't match prefixes 

### function determine_domain_type

```cpp
static DomainType determine_domain_type(
    std::string filename
)
```

determine the domain type, given a filename 

### function starts_with

```cpp
static bool starts_with(
    std::string str,
    std::string prefix
)
```


### function transform_to_string

```cpp
static std::string transform_to_string(
    std::vector< double > transform
)
```


### function convert_domain_types

```cpp
static std::vector< std::string > convert_domain_types(
    std::vector< DomainType > domain_types
)
```


### function convert_groomed_domain_types

```cpp
static std::vector< std::string > convert_groomed_domain_types(
    std::vector< DomainType > domain_types
)
```


### function convert_subject_to_map

```cpp
static StringMap convert_subject_to_map(
    Project * project,
    Subject * subject
)
```


-------------------------------

Updated on 2023-10-24 at 22:16:02 +0000