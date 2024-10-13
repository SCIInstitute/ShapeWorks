---
title: shapeworks::AnalysisUtils

---

# shapeworks::AnalysisUtils





## Public Functions

|                | Name           |
| -------------- | -------------- |
| [ParticleSystemEvaluation](../Classes/classshapeworks_1_1ParticleSystemEvaluation.md) | **[get_local_particle_system](../Classes/classshapeworks_1_1AnalysisUtils.md#function-get-local-particle-system)**([Session](../Classes/classshapeworks_1_1Session.md) * session, int domain) |
| void | **[create_plot](../Classes/classshapeworks_1_1AnalysisUtils.md#function-create-plot)**(JKQTPlotter * plot, Eigen::VectorXd data, QString title, QString x_label, QString y_label) |
| void | **[create_box_plot](../Classes/classshapeworks_1_1AnalysisUtils.md#function-create-box-plot)**(JKQTPlotter * plot, Eigen::VectorXd data, QString title, QString x_label, QColor color =Qt::blue) |

## Public Functions Documentation

### function get_local_particle_system

```cpp
static ParticleSystemEvaluation get_local_particle_system(
    Session * session,
    int domain
)
```


### function create_plot

```cpp
static void create_plot(
    JKQTPlotter * plot,
    Eigen::VectorXd data,
    QString title,
    QString x_label,
    QString y_label
)
```


### function create_box_plot

```cpp
static void create_box_plot(
    JKQTPlotter * plot,
    Eigen::VectorXd data,
    QString title,
    QString x_label,
    QColor color =Qt::blue
)
```


-------------------------------

Updated on 2024-10-13 at 06:37:12 +0000