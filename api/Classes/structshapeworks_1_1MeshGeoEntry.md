---
title: shapeworks::MeshGeoEntry

---

# shapeworks::MeshGeoEntry





## Public Types

|                | Name           |
| -------------- | -------------- |
| enum| **[Mode](../Classes/structshapeworks_1_1MeshGeoEntry.md#enum-mode)** { Full, Partial} |

## Public Functions

|                | Name           |
| -------------- | -------------- |
| void | **[clear](../Classes/structshapeworks_1_1MeshGeoEntry.md#function-clear)**() |
| bool | **[is_full_mode](../Classes/structshapeworks_1_1MeshGeoEntry.md#function-is-full-mode)**() const |
| void | **[update_max_dist](../Classes/structshapeworks_1_1MeshGeoEntry.md#function-update-max-dist)**() |
| bool | **[has_entry](../Classes/structshapeworks_1_1MeshGeoEntry.md#function-has-entry)**(int target) |

## Public Attributes

|                | Name           |
| -------------- | -------------- |
| Mode | **[mode](../Classes/structshapeworks_1_1MeshGeoEntry.md#variable-mode)**  |
| double | **[max_dist](../Classes/structshapeworks_1_1MeshGeoEntry.md#variable-max-dist)**  |
| robin_hood::unordered_flat_map< int, Eigen::Vector3d > | **[data_partial](../Classes/structshapeworks_1_1MeshGeoEntry.md#variable-data-partial)**  |
| std::array< Eigen::VectorXd, 3 > | **[data_full](../Classes/structshapeworks_1_1MeshGeoEntry.md#variable-data-full)**  |

## Public Types Documentation

### enum Mode

| Enumerator | Value | Description |
| ---------- | ----- | ----------- |
| Full | |   |
| Partial | |   |




## Public Functions Documentation

### function clear

```cpp
inline void clear()
```


### function is_full_mode

```cpp
inline bool is_full_mode() const
```


### function update_max_dist

```cpp
inline void update_max_dist()
```


### function has_entry

```cpp
inline bool has_entry(
    int target
)
```


## Public Attributes Documentation

### variable mode

```cpp
Mode mode {Mode::Partial};
```


### variable max_dist

```cpp
double max_dist {0.0};
```


### variable data_partial

```cpp
robin_hood::unordered_flat_map< int, Eigen::Vector3d > data_partial;
```


### variable data_full

```cpp
std::array< Eigen::VectorXd, 3 > data_full;
```


-------------------------------

Updated on 2021-11-15 at 18:13:24 +0000