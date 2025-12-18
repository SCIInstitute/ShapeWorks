#pragma once

#include "Eigen/Core"
#include "Eigen/Dense"
#include "Mesh.h"
#include "ShapeworksUtils.h"

class vtkActor;

namespace shapeworks {
/**
 * \class MeshUtils
 * \ingroup Group-Mesh
 *
 * This class provides helper functions for meshes
 *
 */
class MeshUtils {
  public:
    /// computes a rigid transformation from source to target using vtkIterativeClosestPointTransform
    static const vtkSmartPointer<vtkMatrix4x4> createICPTransform(const Mesh source,
                                                                  const Mesh target,
                                                                  Mesh::AlignmentType align,
                                                                  const unsigned iterations = 20,
                                                                  bool meshTransform = false);

    /// Mesh from mesh or image file
    static Mesh create_mesh_from_file(std::string filename, double iso_value = 0.5);

    /// Thread safe reading of a mesh, uses a lock
    static Mesh threadSafeReadMesh(std::string filename);

    /// Thread safe writing of a mesh, uses a lock
    static void threadSafeWriteMesh(std::string filename, Mesh mesh);

    /// calculate bounding box incrementally for meshes
    static PhysicalRegion boundingBox(const std::vector<std::string>& filenames, bool center = false);

    /// calculate bounding box incrementally for meshes
    static PhysicalRegion boundingBox(const std::vector<std::reference_wrapper<const Mesh> >& meshes,
                                      bool center = false);

    /// calculate bounding box incrementally for meshes
    static PhysicalRegion boundingBox(const std::vector<Mesh>& meshes, bool center = false);

    /// determine the reference mesh
    static int findReferenceMesh(std::vector<Mesh>& meshes, int random_subset_size = -1);

    /// boundary loop extractor for a given mesh
    static Mesh extract_boundary_loop(Mesh mesh);

    /// shared boundary extractor for the left and right mesh
    static std::array<Mesh, 3> shared_boundary_extractor(const Mesh& mesh_l, const Mesh& mesh_r, double tol);

    /// generates and adds normals for points and faces for each mesh in given set of meshes
    static void generateNormals(const std::vector<std::reference_wrapper<Mesh> >& meshes, bool forceRegen = false);

    /// computes average normals for each point in given set of meshes
    static Field computeMeanNormals(const std::vector<std::string>& filenames, bool autoGenerateNormals = true);

    /// computes average normals for each point in given set of meshes
    static Field computeMeanNormals(const std::vector<std::reference_wrapper<const Mesh> >& meshes);

    /// This function visualizes vector and scalar fields for FFCs
    void visualizeVectorFieldForFFCs(std::shared_ptr<Mesh> mesh);

    /// Used as an auxiliary function for vector field visualizations
    vtkSmartPointer<vtkActor> getArrow(Eigen::Vector3d start, Eigen::Vector3d end);

    static int evaluate_triangle_position(const double x[3],
                                          double closestPoint[3],
                                          int& subId,
                                          double pcoords[3],
                                          double& dist2,
                                          double weights[],
                                          double pt3[3],
                                          double pt1[3],
                                          double pt2[3]);


};
} // namespace shapeworks
