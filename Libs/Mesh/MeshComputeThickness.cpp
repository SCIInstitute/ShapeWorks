#include "MeshComputeThickness.h"

#include <itkGradientImageFilter.h>
#include <itkVectorLinearInterpolateImageFunction.h>
#include <tbb/parallel_for.h>
#include <vtkPointData.h>
#include <vtkStaticCellLocator.h>
#include <vtkStaticPointLocator.h>

#include "Logging.h"
namespace shapeworks::mesh {

using VectorPixelType = itk::CovariantVector<float, 3>;
using GradientImageType = itk::Image<VectorPixelType, 3>;
using GradientFilterType = itk::GradientImageFilter<Image::ImageType>;
using GradientInterpolatorType = itk::VectorLinearInterpolateImageFunction<GradientImageType, double>;

// mutex for cell locator (not thread safe)
static std::mutex cell_mutex;

//---------------------------------------------------------------------------
static double compute_average_edge_length(vtkSmartPointer<vtkPolyData> poly_data) {
  vtkPoints* points = poly_data->GetPoints();
  vtkCellArray* cells = poly_data->GetPolys();

  vtkIdType num_edges = 0;
  double total_edge_length = 0.0;

  vtkIdType cell_size;
  vtkIdType const* cell_points;
  for (cells->InitTraversal(); cells->GetNextCell(cell_size, cell_points);) {
    if (cell_size == 3 || cell_size == 4) {  // Assuming triangles or quads
      for (vtkIdType i = 0; i < cell_size; ++i) {
        vtkIdType start_point_id = cell_points[i];
        vtkIdType end_point_id = cell_points[(i + 1) % cell_size];

        double start_point[3];
        double end_point[3];
        points->GetPoint(start_point_id, start_point);
        points->GetPoint(end_point_id, end_point);

        double edge_length = vtkMath::Distance2BetweenPoints(start_point, end_point);
        total_edge_length += std::sqrt(edge_length);

        num_edges++;
      }
    }
  }

  if (num_edges > 0) {
    return total_edge_length / num_edges;
  } else {
    return 0.0;  // Return 0 if no edges found
  }
}

//---------------------------------------------------------------------------
static void median_smooth(vtkSmartPointer<vtkPolyData> poly_data, const char* scalar_name, double radius) {
  vtkSmartPointer<vtkDoubleArray> smoothed_scalars = vtkSmartPointer<vtkDoubleArray>::New();
  smoothed_scalars->SetName(scalar_name);

  auto locator = vtkSmartPointer<vtkStaticPointLocator>::New();
  locator->SetDataSet(poly_data);
  locator->BuildLocator();

  vtkSmartPointer<vtkIdList> point_ids = vtkSmartPointer<vtkIdList>::New();
  vtkSmartPointer<vtkIdList> neighbor_point_ids = vtkSmartPointer<vtkIdList>::New();

  for (vtkIdType point_id = 0; point_id < poly_data->GetNumberOfPoints(); ++point_id) {
    double queryPoint[3];
    poly_data->GetPoint(point_id, queryPoint);

    locator->FindPointsWithinRadius(radius, queryPoint, point_ids);

    int num_neighbors = point_ids->GetNumberOfIds();
    if (num_neighbors == 0) {
      smoothed_scalars->InsertNextValue(poly_data->GetPointData()->GetScalars(scalar_name)->GetComponent(point_id, 0));
      continue;
    }

    std::vector<double> neighbor_scalars;
    neighbor_scalars.reserve(num_neighbors);

    for (int i = 0; i < num_neighbors; ++i) {
      vtkIdType neighbor_id = point_ids->GetId(i);
      double neighbor_scalar = poly_data->GetPointData()->GetScalars(scalar_name)->GetComponent(neighbor_id, 0);
      neighbor_scalars.push_back(neighbor_scalar);
    }

    std::sort(neighbor_scalars.begin(), neighbor_scalars.end());

    int median_index = num_neighbors / 2;
    double smoothed_value = neighbor_scalars[median_index];

    smoothed_scalars->InsertNextValue(smoothed_value);
  }

  poly_data->GetPointData()->AddArray(smoothed_scalars);
}

//---------------------------------------------------------------------------
static std::vector<double> smooth_intensities(std::vector<double> intensities) {
  // smooth using average of neighbors
  std::vector<double> smoothed_intensities;

  for (int i = 0; i < intensities.size(); i++) {
    double sum = 0;
    int count = 0;
    for (int j = -2; j <= 2; j++) {
      if (i + j >= 0 && i + j < intensities.size()) {
        sum += intensities[i + j];
        count++;
      }
    }
    smoothed_intensities.push_back(sum / count);
  }

  return smoothed_intensities;
}

//---------------------------------------------------------------------------
static std::vector<double> median_smooth_signal_intensities(std::vector<double> intensities) {
  // smooth using average of neighbors
  std::vector<double> smoothed_intensities;

  for (int i = 0; i < intensities.size(); i++) {
    std::vector<double> local_intensities;
    for (int j = -2; j <= 2; j++) {
      local_intensities.push_back(intensities[i + j]);
    }
    // compute median
    std::sort(local_intensities.begin(), local_intensities.end());
    smoothed_intensities.push_back(local_intensities[2]);
  }

  return smoothed_intensities;
}

//---------------------------------------------------------------------------
static double get_distance_to_opposite_side(Mesh& mesh, int point_id) {
  vtkSmartPointer<vtkPolyData> poly_data = mesh.getVTKMesh();

  // Get the surface normal from the given point
  double normal[3];
  poly_data->GetPointData()->GetNormals()->GetTuple(point_id, normal);

  // Create a ray in the opposite direction of the normal
  double ray_start[3];
  double ray_end[3];
  poly_data->GetPoint(point_id, ray_start);
  for (int i = 0; i < 3; ++i) {
    ray_start[i] = ray_start[i] - normal[i] * 0.001;
    ray_end[i] = ray_start[i] - normal[i] * 100.0;
  }

  int result = 0;
  // Find the intersection point with the mesh
  double intersectionPoint[3];
  double t;
  int subId;
  double pcoords[3];
  {
    // lock mutex (IntersectWithLine is not thread safe)
    std::lock_guard<std::mutex> lock(cell_mutex);
    auto cellLocator = mesh.getCellLocator();
    result = cellLocator->IntersectWithLine(ray_start, ray_end, 0.0, t, intersectionPoint, pcoords, subId);
  }
  if (result == 0) {
    // return large number if no intersection found
    return std::numeric_limits<double>::max();
  }
  // Calculate the distance between the input point and the intersection point
  double distance = std::sqrt(vtkMath::Distance2BetweenPoints(ray_start, intersectionPoint));
  return distance;
}

double compute_thickness_from_signal(const std::vector<double>& intensities_input, double step_size) {
  auto smoothed = median_smooth_signal_intensities(intensities_input);
  for (int k = 0; k < 10; k++) {
    smoothed = median_smooth_signal_intensities(smoothed);
  }
  auto intensities = smoothed;

  // find the highest intensity and its index
  double max_intensity = 0;
  int point_a = 0;
  for (int j = 0; j < intensities.size(); j++) {
    if (intensities[j] > max_intensity) {
      max_intensity = intensities[j];
      point_a = j;
    }
  }

  // compute derivative of intensities
  std::vector<double> derivatives;
  for (int j = 0; j < intensities.size() - 1; j++) {
    derivatives.push_back((intensities[j + 1] - intensities[j]) / step_size);
  }

  // compute mean of DHU (derivative houndsfield unit) of the first 2mm (point x)
  int count_first_2mm = (1 / step_size) * 2.0;
  int point_x = count_first_2mm;
  int point_o = (1 / step_size) * 3.0;

  double mean_dhu = 0;
  for (int j = 0; j < count_first_2mm; j++) {
    mean_dhu += derivatives[j];
  }
  mean_dhu /= count_first_2mm;

  // compute standard deviation of DHU of the first 2mm
  double std_dhu = 0;
  for (int j = 0; j < count_first_2mm; j++) {
    std_dhu += (derivatives[j] - mean_dhu) * (derivatives[j] - mean_dhu);
  }

  std_dhu = std::sqrt(std_dhu / count_first_2mm);

  // equation 2
  double dhu_threshold = mean_dhu + 4.27 * std_dhu;

  // find the first index where the derivative is greater than the threshold (point b)
  int point_b = -1;
  for (int j = point_o; j < derivatives.size(); j++) {
    if (derivatives[j] > dhu_threshold) {
      point_b = j;
      break;
    }
  }

  bool not_found = false;
  if (point_b < 0) {
    not_found = true;
    point_b = 0;
  }
  double hu_a = intensities[point_a];
  double hu_c = intensities[point_b];

  // constant from paper
  const double k_cor = 0.605;

  double hu_threshold = (hu_a - hu_c) * k_cor + hu_c;

  // find the first and last point above hu_threshold
  int point_d = -1;
  for (int j = point_b; j < intensities.size(); j++) {
    if (intensities[j] > hu_threshold) {
      point_d = j;
      break;
    }
  }
  int point_e = -1;
  if (point_d != -1) {
    // find the next point below hu_threshold
    for (int j = point_d; j < intensities.size(); j++) {
      if (intensities[j] < hu_threshold) {
        point_e = j;
        break;
      }
    }
  }

  // compute the distance between the first and last point above the threshold
  double distance = (point_e - point_d) * step_size;

  if (point_d == -1 || point_e == -1 || not_found) {
    distance = 0;
  }

#if 0  // debug
    if (i < 10) {
      //    write out the intensities to a file named with the point id
      std::ofstream out("intensities_" + std::to_string(i) + ".txt");
      for (auto intensity : intensities) {
        out << intensity << std::endl;
      }
      std::ofstream out2("derivatives_" + std::to_string(i) + ".txt");
      for (auto derivative : derivatives) {
        out2 << derivative << std::endl;
      }

      std::ofstream out4("smoothed_" + std::to_string(i) + ".txt");
      for (auto intensity : smoothed) {
        out4 << intensity << std::endl;
      }

      // write out json file with the parameters
      std::ofstream out3("parameters_" + std::to_string(i) + ".json");
      out3 << "{\n";
      out3 << "  \"hu_a\": " << hu_a << ",\n";
      out3 << "  \"hu_c\": " << hu_c << ",\n";
      out3 << "  \"k_cor\": " << k_cor << ",\n";
      out3 << "  \"hu_threshold\": " << hu_threshold << ",\n";
      out3 << "  \"distance\": " << distance << ",\n";
      out3 << "  \"point_a\": " << point_a << ",\n";
      out3 << "  \"point_b\": " << point_b << ",\n";
      out3 << "  \"point_d\": " << point_d << ",\n";
      out3 << "  \"point_e\": " << point_e << ",\n";
      out3 << "  \"mean_dhu\": " << mean_dhu << ",\n";
      out3 << "  \"std_dhu\": " << std_dhu << ",\n";
      out3 << "  \"dhu_threshold\": " << dhu_threshold << "\n";
      out3 << "}\n";
    }
#endif

  return distance;
}

//---------------------------------------------------------------------------
void compute_thickness(Mesh& mesh, Image& image, Image* dt, double max_dist, double median_radius,
                       std::string distance_mesh) {
  SW_DEBUG("Computing thickness with max_dist {}", max_dist);

  bool use_dt = dt != nullptr;

  mesh.computeNormals();
  mesh.getCellLocator();

  auto poly_data = mesh.getVTKMesh();

  // create array of thickness values
  auto values = vtkSmartPointer<vtkDoubleArray>::New();
  values->SetNumberOfComponents(1);
  values->SetNumberOfTuples(poly_data->GetNumberOfPoints());
  values->SetName("thickness");

  auto interpolator = GradientInterpolatorType::New();

  if (use_dt) {
    // compute gradient image from distance transform
    auto gradient_filter = GradientFilterType::New();
    gradient_filter->SetInput(dt->getITKImage());
    gradient_filter->Update();
    auto gradient_map = gradient_filter->GetOutput();
    interpolator->SetInputImage(gradient_map);
  }

  auto check_inside = [&](Point3 point) -> bool {
    if (use_dt) {
      return image.isInside(point) && interpolator->IsInsideBuffer(point);
    } else {
      return image.isInside(point);
    }
  };

  auto get_gradient = [&](int i, Point3 point) -> VectorPixelType {
    if (use_dt) {
      return interpolator->Evaluate(point);
    } else {
      double normal[3];
      poly_data->GetPointData()->GetNormals()->GetTuple(i, normal);
      VectorPixelType gradient;
      gradient[0] = -normal[0];
      gradient[1] = -normal[1];
      gradient[2] = -normal[2];
      return gradient;
    }
  };

  // get minimum spacing
  double spacing = image.spacing()[0];
  for (int i = 1; i < 3; i++) {
    if (image.spacing()[i] < spacing) {
      spacing = image.spacing()[i];
    }
  }

  double step_size = spacing / 10.0;

  auto step = [&](Point3 point, VectorPixelType gradient, double multiplier) -> Point3 {
    // normalize the gradient
    float norm = std::sqrt(gradient[0] * gradient[0] + gradient[1] * gradient[1] + gradient[2] * gradient[2]);
    Point3 new_point;
    for (int i = 0; i < 3; i++) {
      gradient[i] /= norm;
      gradient[i] *= step_size;
      new_point[i] = point[i] + gradient[i] * multiplier;
    }
    return new_point;
  };

  const double distance_outside = 4.0;
  const double distance_inside = 12.0;

  // first establish average 'outside' values

  std::vector<double> average_intensities;
  for (int i = 0; i < mesh.numPoints(); i++) {
    Point3 point;
    poly_data->GetPoint(i, point.GetDataPointer());

    for (int j = 0; j <= distance_outside / step_size; j++) {
      double intensity = 0;
      // check if point is inside image
      if (check_inside(point)) {
        intensity = image.evaluate(point);
      }
      if (j < average_intensities.size()) {
        average_intensities[j] += intensity;
      } else {
        average_intensities.push_back(intensity);
      }

      VectorPixelType gradient = get_gradient(i, point);
      point = step(point, gradient, -1.0);
    }
  }

  // divide by mean to compute average
  for (int i = 0; i < average_intensities.size(); i++) {
    average_intensities[i] /= mesh.numPoints();
  }

  // mutex
  std::mutex mutex;

  unsigned long num_points = mesh.numPoints();

  // parallel loop over all points using TBB
  tbb::parallel_for(tbb::blocked_range<size_t>{0, num_points}, [&](const tbb::blocked_range<size_t>& r) {
    for (size_t i = r.begin(); i < r.end(); ++i) {
      Point3 point;
      poly_data->GetPoint(i, point.GetDataPointer());

      std::vector<double> intensities;

      for (int j = 0; j <= distance_outside / step_size; j++) {
        intensities.push_back(average_intensities[j]);
      }

      // reverse the intensities vector
      std::reverse(intensities.begin(), intensities.end());

      // drop the last intensity, since we're already there
      intensities.pop_back();

      // reset point position back to the surface
      poly_data->GetPoint(i, point.GetDataPointer());

      for (int j = 0; j < distance_inside / step_size; j++) {
        double intensity = 0;
        // check if point is inside image
        if (check_inside(point)) {
          intensity = image.evaluate(point);
        }

        intensities.push_back(intensity);

        // evaluate gradient
        VectorPixelType gradient = get_gradient(i, point);

        point = step(point, gradient, 1.0);
      }

      auto distance = compute_thickness_from_signal(intensities, step_size);

      distance = std::min<double>(distance, max_dist);
      distance = std::min<double>(distance, get_distance_to_opposite_side(mesh, i) / 2.0);

      {
        std::lock_guard<std::mutex> lock(mutex);
        values->InsertValue(i, distance);
      }
    }
  });

  mesh.setField("thickness", values, Mesh::Point);

  double average_edge_length = compute_average_edge_length(poly_data);
  SW_DEBUG("average edge length: {}", average_edge_length);

  median_smooth(mesh.getVTKMesh(), "thickness", average_edge_length * median_radius);
  values = vtkDoubleArray::SafeDownCast(mesh.getVTKMesh()->GetPointData()->GetArray("thickness"));

  if (distance_mesh != "") {
    // create a copy
    Mesh d_mesh = mesh;

    // for each vertex, move the particle the distance scalar
    vtkSmartPointer<vtkPolyData> poly_data = d_mesh.getVTKMesh();
    for (int i = 0; i < mesh.numPoints(); i++) {
      Point3 point;
      poly_data->GetPoint(i, point.GetDataPointer());

      double distance_travelled = 0;

      double distance = values->GetValue(i);

      int count = 500;
      while (distance_travelled < distance && count-- > 0) {
        if (!check_inside(point)) {
          break;
        }

        VectorPixelType gradient = get_gradient(i, point);

        // normalize the gradient
        float norm = std::sqrt(gradient[0] * gradient[0] + gradient[1] * gradient[1] + gradient[2] * gradient[2]);
        gradient[0] /= norm;
        gradient[1] /= norm;
        gradient[2] /= norm;

        gradient[0] *= step_size;
        gradient[1] *= step_size;
        gradient[2] *= step_size;

        point[0] += gradient[0];
        point[1] += gradient[1];
        point[2] += gradient[2];

        distance_travelled += step_size;
      }

      // modify the point position in the poly_data
      poly_data->GetPoints()->SetPoint(i, point.GetDataPointer());
    }

    d_mesh.write(distance_mesh);
  }
}
}  // namespace shapeworks::mesh
