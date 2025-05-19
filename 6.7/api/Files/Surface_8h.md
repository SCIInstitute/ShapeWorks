---
title: Libs/Optimize/Domain/Surface.h

---

# Libs/Optimize/Domain/Surface.h



## Namespaces

| Name           |
| -------------- |
| **[shapeworks](../Namespaces/namespaceshapeworks.md)** <br>User usage reporting (telemetry)  |

## Classes

|                | Name           |
| -------------- | -------------- |
| class | **[shapeworks::Surface](../Classes/classshapeworks_1_1Surface.md)**  |




## Source code

```cpp
#pragma once

#include <geometrycentral/surface/heat_method_distance.h>
#include <geometrycentral/surface/meshio.h>
#include <geometrycentral/surface/surface_mesh.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>

#include <unordered_set>

#include "Libs/Optimize/Domain/ParticleDomain.h"
#include "MeshGeoEntry.h"

class vtkCellLocator;

namespace shapeworks {
class Surface {
  public:
    using PointType = ParticleDomain::PointType;
    using GradNType = ParticleDomain::GradNType;

    using NormalType = vnl_vector_fixed<float, 3>;
    using VectorType = vnl_vector_fixed<double, 3>;

    explicit Surface(vtkSmartPointer<vtkPolyData> mesh,
                     bool geodesics_enabled = false,
                     size_t geodesics_cache_multiplier_size = 0); // 0 => MeshWrapper will choose a heuristic

    ~Surface() = default;

    double compute_distance(const PointType& pointa,
                            int idxa,
                            const PointType& pointb,
                            int idxb,
                            VectorType* out_grad = nullptr) const;

    bool is_within_distance(const PointType& pointa,
                            int idxa,
                            const PointType& pointb,
                            int idxb,
                            double test_dist,
                            double& dist) const;

    PointType geodesic_walk(PointType p, int idx, VectorType vector) const;

    VectorType project_vector_to_surface_tangent(const PointType& pointa, int idx, VectorType& vector) const;

    NormalType sample_normal_at_point(PointType p, int idx = -1) const;
    GradNType sample_gradient_normal_at_point(PointType p, int idx) const;

    PointType snap_to_mesh(PointType pointa, int idx) const;

    PointType get_point_on_mesh() const;

    inline const PointType& get_mesh_lower_bound() const { return mesh_lower_bound_; }

    inline const PointType& get_mesh_upper_bound() const { return mesh_upper_bound_; }

    virtual void invalidate_particle(int idx);

    inline vtkSmartPointer<vtkPolyData> get_polydata() const { return original_mesh_; }

    bool is_geodesics_enabled() const { return this->is_geodesics_enabled_; }

  private:
    int get_num_faces();
    void compute_mesh_bounds();
    void compute_grad_normals(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F) const;

    int get_triangle_for_point(const double pt[3], int idx, double closest_point[3]) const;

    Eigen::Vector3d project_vector_to_face(const Eigen::Vector3d& normal, const Eigen::Vector3d& vector) const;

    const Eigen::Vector3d get_face_normal(int face_index) const;

    bool is_in_triangle(const double pt[3], int face_index) const;

    Eigen::Vector3d compute_barycentric_coordinates(const Eigen::Vector3d& pt, int face) const;

    int compute_barycentric_coordinates(const Eigen::Vector3d& pt,
                                        int face,
                                        double& dist2,
                                        Eigen::Vector3d& bary) const;

    int compute_face_and_weights(const PointType& p, int idx, Eigen::Vector3d& weights) const;

    Eigen::Vector3d geodesic_walk_on_face(Eigen::Vector3d point_a,
                                          Eigen::Vector3d projected_vector,
                                          int face_index,
                                          int& ending_face) const;

    Eigen::Vector3d get_barycentric_intersection(Eigen::Vector3d start,
                                                 Eigen::Vector3d end,
                                                 int currentFace,
                                                 int edge) const;

    int get_across_edge(int face, int edge) const;

    int get_face_point_id(int face, int point_id) const;

    int slide_along_edge(Eigen::Vector3d& point, Eigen::Vector3d& remainingVector_, int face_, int edge_) const;

    Eigen::Vector3d get_vertex_coords(int vertex_id) const;

    Eigen::Vector3d rotate_vector_to_face(const Eigen::Vector3d& prev_normal,
                                          const Eigen::Vector3d& next_normal,
                                          const Eigen::Vector3d& vector) const;

    vtkSmartPointer<vtkPolyData> poly_data_;
    vtkSmartPointer<vtkPolyData> original_mesh_;

    NormalType calculate_normal_at_point(Surface::PointType p, int idx) const;

    // Caches of triangle, normal and position
    // Has to be mutable because all of the accessor APIs are const
    mutable std::vector<int> particle_triangles_;
    mutable std::vector<NormalType> particle_normals_;
    mutable std::vector<PointType> particle_positions_;
    mutable std::vector<double> particle_neighboorhood_;

    mutable std::vector<GradNType> grad_normals_;

    // bounds of the mesh plus some buffer
    PointType mesh_lower_bound_;
    PointType mesh_upper_bound_;

    // cell locator to find closest point on mesh
    vtkSmartPointer<vtkCellLocator> cell_locator_;

    // Geodesic distances

    bool is_geodesics_enabled_{false};

    // Geometry Central data structures
    std::unique_ptr<geometrycentral::surface::SurfaceMesh> gc_mesh_;
    std::unique_ptr<geometrycentral::surface::VertexPositionGeometry> gc_geometry_;
    std::unique_ptr<geometrycentral::surface::HeatMethodDistanceSolver> gc_heatsolver_;

    size_t geo_max_cache_entries_{0};
    mutable size_t geo_cache_size_{0};

    // Flattened version of libigl's gradient operator
    std::vector<Eigen::Matrix3d> face_grad_;

    std::vector<std::unordered_set<int> > face_kring_;

    // Cache for geodesic distances from a triangle
    mutable std::vector<MeshGeoEntry> geo_dist_cache_;

    // Returns true if face f_a is in the K-ring of face f_b
    bool are_faces_in_k_ring(int f_a, int f_b) const;
    const size_t kring_{1};

    // Convert the mesh to libigl data structures
    void get_igl_mesh(Eigen::MatrixXd& V, Eigen::MatrixXi& F) const;

    // Precompute heat data structures for faster geodesic lookups
    void precompute_geodesics(const Eigen::MatrixXd& V, const Eigen::MatrixXi& F);

    void compute_k_ring(int f, int k, std::unordered_set<int>& ring) const;

    const MeshGeoEntry& geodesics_from_triangle(int f,
                                                double max_dist = std::numeric_limits<double>::max(),
                                                int req_target_f = -1) const;
    const Eigen::Matrix3d geodesics_from_triangle_to_triangle(int f_a, int f_b) const;
    void clear_geodesic_cache() const;

    // Store some info about the last query. This accelerates the computation
    // because the optimizer generally asks for the distances _from_ the same
    // point as the previous query.
    mutable bool geo_lq_cached_{false};
    mutable PointType geo_lq_pt_a_{-1};
    mutable int geo_lq_face_{-1};
    mutable Eigen::Vector3d geo_lq_bary_;
    void fetch_and_cache_first_point(const PointType pt_a, int idx_a, int& face_a, Eigen::Vector3d& bary_a) const;

    Eigen::MatrixXd vertices_;
    Eigen::MatrixXi faces_;
};
} // namespace shapeworks
```


-------------------------------

Updated on 2025-05-19 at 18:59:08 +0000
