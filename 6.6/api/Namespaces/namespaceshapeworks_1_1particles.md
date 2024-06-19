---
title: shapeworks::particles

---

# shapeworks::particles



## Functions

|                | Name           |
| -------------- | -------------- |
| Eigen::VectorXd | **[read_particles](../Namespaces/namespaceshapeworks_1_1particles.md#function-read-particles)**(std::string filename) |
| std::vector< itk::Point< double, 3 > > | **[read_particles_as_vector](../Namespaces/namespaceshapeworks_1_1particles.md#function-read-particles-as-vector)**(std::string filename) |
| void | **[write_particles](../Namespaces/namespaceshapeworks_1_1particles.md#function-write-particles)**(std::string filename, const Eigen::VectorXd & points) |
| void | **[write_particles_from_vector](../Namespaces/namespaceshapeworks_1_1particles.md#function-write-particles-from-vector)**(std::string filename, std::vector< itk::Point< double, 3 > > points) |


## Functions Documentation

### function read_particles

```cpp
Eigen::VectorXd read_particles(
    std::string filename
)
```


### function read_particles_as_vector

```cpp
std::vector< itk::Point< double, 3 > > read_particles_as_vector(
    std::string filename
)
```


### function write_particles

```cpp
void write_particles(
    std::string filename,
    const Eigen::VectorXd & points
)
```


### function write_particles_from_vector

```cpp
void write_particles_from_vector(
    std::string filename,
    std::vector< itk::Point< double, 3 > > points
)
```






-------------------------------

Updated on 2024-06-19 at 02:57:34 +0000