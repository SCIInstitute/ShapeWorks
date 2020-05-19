#include <gtest/gtest.h>

#include <cstdio>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include "RBFShape.h"

#include <Eigen/Core>

#include "TestConfiguration.h"

double* get_bounding_sphere(size_t num, size_t& ng, double radius);

bool print_files = true;

//Flat surface test
std::vector<double> flat_test(int side_length, int sep){

    std::cout << "\n----------------------\nFlat Surface Testing RBFShape\n----------------------" << std::endl;

    Eigen::MatrixXd flat(side_length*side_length*3, 4);
    Eigen::MatrixXd flat_normals(side_length*side_length*3, 4);

    Eigen::Index count = 0;

    std::ofstream file;
    file.open("flat_surface.ev");
    file << std::fixed;
    file << std::setprecision(5);

    std::ofstream file2;
    file2.open("sample_flat_surface.ptnrms");
    file2 << std::fixed;
    file2 << std::setprecision(5);

    for(size_t i = 0; i < side_length; i++){
        for(size_t j = 0; j < side_length; j++){
           flat(count,0) = i*sep;
           flat(count,1) = j*sep;
           flat(count,2) = 0.;
           flat(count,3) = 0.;
           flat_normals(count,2) = 1.;
           count++;

           file << float(i*sep) << " " << float(j*sep) << " " << 0.0 << " " << i+1 << " " << j+1 << "\n";

           file2 << float(i*sep) << " " << float(j*sep) << " " << 0.0 << " " << 0. << " " << 0. <<  " " << 1. << "\n";

           flat(count,0) = i*sep;
           flat(count,1) = j*sep;
           flat(count,2) = 10.;
           flat(count,3) = 10.;
           flat_normals(count,2) = 1.;
           count++;

           flat(count,0) = i*sep;
           flat(count,1) = j*sep;
           flat(count,2) = -10.;
           flat(count,3) = -10.;
           flat_normals(count,2) = 1.;
           count++;
        }
    }

    file.close();
    file2.close();

    std::cout << "Points: " << count/3 << std::endl;

    std::cout << "Points + dipoles: " << count << std::endl;

    std::cout << "Creating RBF..." << std::endl;

    shapeworks::RBFShape RBFflat = shapeworks::RBFShape();

    std::cout << "Solving system..." << std::endl;

    RBFflat.solve_system(flat);

    std::cout << "Printing..." << std::endl;

    if(print_files){
        RBFflat.set_raw_points(flat);
        RBFflat.set_raw_normals(flat_normals);
        RBFflat.compute_normals();

        RBFflat.write_csv("flat_1.csv",5);
        RBFflat.writeToEqFile("flat_1.Eq",5);
        RBFflat.writeToRawFile("flat_1.raw",5);
    }

    double sum_absolute_differences = 0.;
    std::vector<double> absolute_differences;

    for(Eigen::Index i = 0; i < flat.rows(); i++){
        Eigen::Vector3d pt = flat.row(i).head<3>();
        //std::cout << "Point flat\n " << flat.row(i) << std::endl;
        //std::cout << "Point\n " << pt << std::endl;
        double value = flat(i,3);
        double rbf_value = RBFflat.evaluate(pt);
        double mag_grad = RBFflat.mag_gradient(pt);
        double value_over_mag = rbf_value/mag_grad;
        //std::cout << "Point " << i << " value " << value << " predicted_value " << value_over_mag << std::endl;
        sum_absolute_differences += abs(value_over_mag - value);
        absolute_differences.push_back(value_over_mag - value);
    }

    double mean_abs_differences = sum_absolute_differences/count;

    std::cout << "Mean absolute differences: " << mean_abs_differences << std::endl;

    return absolute_differences;
}

//Sphere test
std::vector<double> sphere_test() {
  std::cout << "\n----------------------\nSphere Testing RBFShape\n----------------------" << std::endl;

  int n = 5;
  double radius = 100;
  double true_mag = 1.;
  double r[3] = {1.,2.,3.};
  double c[3] = {0.,0.,0.};
  size_t ng;
  double* sphere_pts = get_bounding_sphere(n, ng, radius);
  double* sphere_pts_in = get_bounding_sphere(n, ng, radius - true_mag);
  double* sphere_pts_out = get_bounding_sphere(n, ng, radius + true_mag);

  int count = ng;

  std::cout << "Points: " << count << std::endl;

  Eigen::MatrixXd sphere(count*3, 4);
  Eigen::MatrixXd sphere_normals(count*3, 3);

  std::cout << "Points + dipoles: " << count*3 << std::endl;

  std::ofstream file;
  file.open("sample_sphere.ptnrms");
  file << std::fixed;
  file << std::setprecision(5);

  for(size_t i = 0; i < count; i++){

    double ix = sphere_pts[i*3];
    double iy = sphere_pts[i*3+1];
    double iz = sphere_pts[i*3+2];
    double ix_in = sphere_pts_in[i*3];
    double iy_in = sphere_pts_in[i*3+1];
    double iz_in = sphere_pts_in[i*3+2];
    double ix_out = sphere_pts_out[i*3];
    double iy_out = sphere_pts_out[i*3+1];
    double iz_out = sphere_pts_out[i*3+2];

    //std::cout << ix << "," << iy << "," << iz << std::endl;

    double norm[3] = {ix-ix_in, iy-iy_in, iz-iz_in};
    double magnitude = sqrt(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]);

    if(abs(magnitude - true_mag) > 1e-5)
        std::cout << magnitude << std::endl;

    double norm2[3] = {ix-ix_out, iy-iy_out, iz-iz_out};
    double magnitude2 = sqrt(norm2[0]*norm2[0] + norm2[1]*norm2[1] + norm2[2]*norm2[2]);

    if(abs(magnitude2 - true_mag) > 1e-5)
        std::cout << magnitude2 << std::endl;

    file << ix << " " << iy << " " << iz << " " << norm[0] << " " << norm[1] <<  " " << norm[2] << "\n";

    sphere(i, 0) = ix;
    sphere(i, 1) = iy;
    sphere(i, 2) = iz;
    sphere(i, 3) = 0.;
    sphere(i+count,0) = ix_in;
    sphere(i+count,1) = iy_in;
    sphere(i+count,2) = iz_in;
    sphere(i+count,3) = -true_mag;
    sphere(i+count*2,0) = ix_out;
    sphere(i+count*2,1) = iy_out;
    sphere(i+count*2,2) = iz_out;
    sphere(i+count*2,3) = true_mag;

    sphere_normals(i,0) = norm[0]/magnitude;
    sphere_normals(i,1) = norm[1]/magnitude;
    sphere_normals(i,2) = norm[2]/magnitude;
    sphere_normals(i+count,0) = norm[0]/magnitude;
    sphere_normals(i+count,1) = norm[1]/magnitude;
    sphere_normals(i+count,2) = norm[2]/magnitude;
    sphere_normals(i+count*2,0) = norm[0]/magnitude;
    sphere_normals(i+count*2,1) = norm[1]/magnitude;
    sphere_normals(i+count*2,2) = norm[2]/magnitude;
  }

  file.close();

  std::cout << "Creating RBF..." << std::endl;

  shapeworks::RBFShape RBFsphere = shapeworks::RBFShape();

  std::cout << "Solving system..." << std::endl;

  RBFsphere.solve_system(sphere);

  std::cout << "Printing..." << std::endl;

  if(print_files){
      RBFsphere.set_raw_points(sphere);
      RBFsphere.set_raw_normals(sphere_normals);

      RBFsphere.write_csv("sphere_1.csv",5);
      RBFsphere.writeToEqFile("sphere_1.Eq",5);
      RBFsphere.writeToRawFile("sphere_1.raw",5);
  }

  double sum_absolute_differences = 0.;
  std::vector<double> absolute_differences;

  for(size_t i = 0; i < count*3; i++){
      Eigen::Vector3d pt = sphere.row(i).head<3>();
      double value = sphere(i,3);
      double rbf_value = RBFsphere.evaluate(pt);
      double mag_grad = RBFsphere.mag_gradient(pt);
      double value_over_mag = rbf_value/mag_grad;
      //std::cout << "Point " << i << " value " << value << " predicted_value " << value_over_mag << std::endl;
      sum_absolute_differences += abs(value_over_mag - value);
      absolute_differences.push_back(value_over_mag - value);
  }

  double mean_abs_differences = sum_absolute_differences/count/3;

  std::cout << "Mean absolute differences: " << mean_abs_differences << std::endl;

  return absolute_differences;
}

TEST(PointCloudTests, point_cloud_functions_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/point_cloud_tests");
  chdir(test_location.c_str());

  //Flat surface test
  //Params: width and height of the flat surface, spacing between points
  std::vector<double> flat_test_output = flat_test(20, 10);

  double sum_absolute_differences = 0.;

  for(size_t i = 0; i < flat_test_output.size(); i++){
      sum_absolute_differences += abs(flat_test_output[i]);
  }

  double flat_mean_abs_differences = sum_absolute_differences/flat_test_output.size();
  std::cout << "Flat Difference between true and evaluated values: " << flat_mean_abs_differences << std::endl;

  //Sphere test
  std::vector<double> sphere_test_output = sphere_test();

  double sph_sum_absolute_differences = 0.;

  for(size_t i = 0; i < sphere_test_output.size(); i++){
      sph_sum_absolute_differences += abs(sphere_test_output[i]);
  }

  double sph_mean_abs_differences = sph_sum_absolute_differences/sphere_test_output.size();
  std::cout << "Sphere Difference between true and evaluated values: " << sph_mean_abs_differences << std::endl;

  ASSERT_LT(flat_mean_abs_differences, 0.2);
  ASSERT_LT(sph_mean_abs_differences, 0.2);

}

double* get_bounding_sphere(size_t num, size_t& ng, double radius){
    size_t num2 = num*4;

    double interval = 2*3.1416 / num2;

    double * sphere_pts = new double[num2*num2*3];

    size_t pt_count = 0;

    for(size_t i = 0; i < num2; i++){
        for(size_t j = 0; j < num2; j++){
            double theta = interval*i;
            double phi = interval*j;
            double x_ = radius* sin(theta) * cos(phi);
            double y_ = radius* sin(theta) * sin(phi);
            double z_ = radius * cos(theta);
            sphere_pts[pt_count*3] = x_;
            sphere_pts[pt_count*3+1] = y_;
            sphere_pts[pt_count*3+2] = z_;
            pt_count += 1;
        }
    }
    ng = pt_count;

    return sphere_pts;
}
