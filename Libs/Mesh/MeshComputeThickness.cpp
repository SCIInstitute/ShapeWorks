#include "MeshComputeThickness.h"

#include <itkGradientImageFilter.h>
#include <itkVectorLinearInterpolateImageFunction.h>
#include <vtkPointData.h>

#include "Logging.h"
namespace shapeworks::mesh {

using VectorPixelType = itk::CovariantVector<float, 3>;
using GradientImageType = itk::Image<VectorPixelType, 3>;
using GradientFilterType = itk::GradientImageFilter<Image::ImageType>;
using GradientInterpolatorType = itk::VectorLinearInterpolateImageFunction<GradientImageType, double>;

//---------------------------------------------------------------------------
std::vector<double> smoothIntensities(std::vector<double> intensities) {
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
void compute_thickness(Mesh& mesh, Image& image, Image* dt, double threshold, double min_dist, double max_dist,
                       std::string distance_mesh) {
  SW_DEBUG("Computing thickness with threshold {}, min_dist {}, max_dist {}", threshold, min_dist, max_dist);

  bool use_dt = dt != nullptr;

  mesh.computeNormals();

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

  for (int i = 0; i < mesh.numPoints(); i++) {
    Point3 point;
    poly_data->GetPoint(i, point.GetDataPointer());

    std::vector<double> intensities;
    double distance_outside = 4.0;
    double distance_inside = 12.0;

    for (int j = 0; j <= distance_outside / step_size; j++) {
      double intensity = 0;
      // check if point is inside image
      if (check_inside(point)) {
        intensity = image.evaluate(point);
      }
      intensities.push_back(intensity);

      VectorPixelType gradient = get_gradient(i, point);

      // normalize the gradient
      float norm = std::sqrt(gradient[0] * gradient[0] + gradient[1] * gradient[1] + gradient[2] * gradient[2]);
      gradient[0] /= norm;
      gradient[1] /= norm;
      gradient[2] /= norm;

      gradient[0] *= step_size;
      gradient[1] *= step_size;
      gradient[2] *= step_size;

      // take step
      point[0] -= gradient[0];
      point[1] -= gradient[1];
      point[2] -= gradient[2];
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

      // normalize the gradient
      float norm = std::sqrt(gradient[0] * gradient[0] + gradient[1] * gradient[1] + gradient[2] * gradient[2]);
      gradient[0] /= norm;
      gradient[1] /= norm;
      gradient[2] /= norm;

      gradient[0] *= step_size;
      gradient[1] *= step_size;
      gradient[2] *= step_size;

      // take step
      point[0] += gradient[0];
      point[1] += gradient[1];
      point[2] += gradient[2];
    }

    auto smoothed = smoothIntensities(intensities);
    for (int k = 0; k < 10; k++) {
      smoothed = smoothIntensities(smoothed);
    }
    intensities = smoothed;

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

    distance = std::min<double>(distance, max_dist);

    values->InsertValue(i, distance);
  }
  mesh.setField("thickness", values, Mesh::Point);

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
