---
title: shapeworks::Parameters
summary: Parameter settings. 

---

# shapeworks::Parameters



Parameter settings.  [More...](#detailed-description)


`#include <Parameters.h>`

## Public Functions

|                | Name           |
| -------------- | -------------- |
| [Variant](../Classes/classshapeworks_1_1Variant.md) | **[get](../Classes/classshapeworks_1_1Parameters.md#function-get)**(std::string key, [Variant](../Classes/classshapeworks_1_1Variant.md) default_value)<br>get a parameter based on a key, return default if it doesn't exist  |
| bool | **[key_exists](../Classes/classshapeworks_1_1Parameters.md#function-key-exists)**(std::string key)<br>return if a key exists or not  |
| void | **[set](../Classes/classshapeworks_1_1Parameters.md#function-set)**(std::string key, [Variant](../Classes/classshapeworks_1_1Variant.md) value)<br>set a parameter based on a key  |
| void | **[remove_entry](../Classes/classshapeworks_1_1Parameters.md#function-remove-entry)**(std::string key)<br>remove an entry  |
| void | **[set_map](../Classes/classshapeworks_1_1Parameters.md#function-set-map)**(std::map< std::string, std::string > map)<br>set underlying map  |
| std::map< std::string, std::string > | **[get_map](../Classes/classshapeworks_1_1Parameters.md#function-get-map)**()<br>get underlying map  |
| void | **[reset_parameters](../Classes/classshapeworks_1_1Parameters.md#function-reset-parameters)**()<br>reset parameters to blank  |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| constexpr const char * | **[ANALYSIS_PARAMS](../Classes/classshapeworks_1_1Parameters.md#variable-analysis-params)**  |
| constexpr const char * | **[GROOM_PARAMS](../Classes/classshapeworks_1_1Parameters.md#variable-groom-params)**  |
| constexpr const char * | **[OPTIMIZE_PARAMS](../Classes/classshapeworks_1_1Parameters.md#variable-optimize-params)**  |
| constexpr const char * | **[STUDIO_PARAMS](../Classes/classshapeworks_1_1Parameters.md#variable-studio-params)**  |
| constexpr const char * | **[PROJECT_PARAMS](../Classes/classshapeworks_1_1Parameters.md#variable-project-params)**  |
| constexpr const char * | **[DEEPSSM_PARAMS](../Classes/classshapeworks_1_1Parameters.md#variable-deepssm-params)**  |

## Detailed Description

```cpp
class shapeworks::Parameters;
```

Parameter settings. 

Store key/variant combinations Used to store parameters for various tools 

## Public Functions Documentation

### function get

```cpp
Variant get(
    std::string key,
    Variant default_value
)
```

get a parameter based on a key, return default if it doesn't exist 

### function key_exists

```cpp
bool key_exists(
    std::string key
)
```

return if a key exists or not 

### function set

```cpp
void set(
    std::string key,
    Variant value
)
```

set a parameter based on a key 

### function remove_entry

```cpp
void remove_entry(
    std::string key
)
```

remove an entry 

### function set_map

```cpp
void set_map(
    std::map< std::string, std::string > map
)
```

set underlying map 

### function get_map

```cpp
std::map< std::string, std::string > get_map()
```

get underlying map 

### function reset_parameters

```cpp
void reset_parameters()
```

reset parameters to blank 

## Public Attributes Documentation

### variable ANALYSIS_PARAMS

```cpp
static constexpr const char * ANALYSIS_PARAMS = "analysis";
```


### variable GROOM_PARAMS

```cpp
static constexpr const char * GROOM_PARAMS = "groom";
```


### variable OPTIMIZE_PARAMS

```cpp
static constexpr const char * OPTIMIZE_PARAMS = "optimize";
```


### variable STUDIO_PARAMS

```cpp
static constexpr const char * STUDIO_PARAMS = "studio";
```


### variable PROJECT_PARAMS

```cpp
static constexpr const char * PROJECT_PARAMS = "project";
```


### variable DEEPSSM_PARAMS

```cpp
static constexpr const char * DEEPSSM_PARAMS = "deepssm";
```


-------------------------------

Updated on 2021-11-23 at 22:14:02 +0000