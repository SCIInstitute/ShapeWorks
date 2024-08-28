#include "MeshComputeThickness.h"

#include <itkGradientImageFilter.h>
#include <itkVectorLinearInterpolateImageFunction.h>
#include <tbb/parallel_for.h>
#include <vtkPointData.h>
#include <vtkSelectEnclosedPoints.h>
#include <vtkStaticCellLocator.h>
#include <vtkStaticPointLocator.h>

#include "Logging.h"
namespace shapeworks::mesh {

using VectorPixelType = itk::CovariantVector<float, 3>;
using GradientImageType = itk::Image<VectorPixelType, 3>;
using GradientFilterType = itk::GradientImageFilter<Image::ImageType>;
using GradientInterpolatorType = itk::VectorLinearInterpolateImageFunction<GradientImageType, double>;

//---------------------------------------------------------------------------
static bool getParticlesOnOuterMesh(Mesh& innerMesh, int point_id,, Mesh& outerMesh, std::vector<std::vector<double>>& projectedParticles, int numPointsProjected) {
  vtkSmartPointer<vtkPolyData> poly_data = innerMesh.getVTKMesh();
  projectedParticles.resize(numPointsProjected, vector<double>(3, 0.0));

  // Get the surface normal from the given point
  double normal[3]; // normal from inner mesh
  poly_data->GetPointData()->GetNormals()->GetTuple(point_id, normal);

  // Create a ray in the direction of the surface normal
  double ray_start[3];
  double ray_end[3];
  poly_data->GetPoint(point_id, ray_start);
  for (int i = 0; i < 3; ++i) {
    ray_start[i] = ray_start[i] + normal[i] * 0.001;
    ray_end[i] = ray_start[i] + normal[i] * 100.0;
  }

  int result = 0;
  // Find the first intersection point with the outer mesh
  double firstIntersectionPoint[3];
  double t;
  int subId;
  double pcoords[3];
  {
    // lock mutex (IntersectWithLine is not thread safe)
    std::lock_guard<std::mutex> lock(cell_mutex);
    auto cellLocator = outerMesh.getCellLocator();
    result = cellLocator->IntersectWithLine(ray_start, ray_end, 0.0, t, firstIntersectionPoint, pcoords, subId);
  }
  if (result == 0) return false;
  if (numPointsProjected == 1) return result;

  // now change the ray_start location now
  ray_start = firstIntersectionPoint;
  for (int i = 0; i < 3; ++i) {
    ray_start[i] = ray_start[i] + normal[i] * 0.001;
    ray_end[i] = ray_start[i] + normal[i] * 100.0;
  }

  int result = 0;
  // Find the last intersection point with the outer mesh
  double lastIntersectionPoint[3];
  double t2;
  int subId2;
  double pcoords2[3];
  {
    // lock mutex (IntersectWithLine is not thread safe)
    std::lock_guard<std::mutex> lock(cell_mutex);
    auto cellLocator = outerMesh.getCellLocator();
    result = cellLocator->IntersectWithLine(ray_start, ray_end, 0.0, t2, lastIntersectionPoint, pcoords2, subId2);
  }
  if (result == 0) return false;

  // interpolate 'numParticlesProjected' between start and end intersection Points
}





Eigen::MatrixXd project_particles(Mesh &inner_mesh, Mesh &outer_mesh, Eigen::MatrixXd &innerParticles)
{
    // coverage between inner and outer
    covered_area_mesh = inner_mesh.coverage(outer_mesh)

    //extract particles where covered_area = 1
    covered_particles = extract_particles(covered_mash)

    // project all covered particles from inner mesh to outer mesh
    // loop over all particles 
        getParticlesOnOuterMesh(innerMesh, point_id, outerMesh)
}

}  // namespace shapeworks::mesh
