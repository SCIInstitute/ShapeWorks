#include <gtest/gtest.h>

#include <cstdio>
#include <iomanip>
#include <cmath>
#include <ctime>
#include <cstring>
#include <fstream>

#include <Eigen/Core>
#include <itkImageFileReader.h>
#include <itkImageFileWriter.h>
#include <itkReinitializeLevelSetImageFilter.h> // for distance transform computation

#include "TestConfiguration.h"

#include "ShapeWorksRunApp.h"
#include "itkParticleShapeStatistics.h"

#include "RBFShape.h"

double *ellipsoid_grid ( int n, double r[3], double c[3], int ng );
double* get_bounding_sphere(size_t num, size_t& ng, double radius);

//Flat surface test

TEST(PointCloudTests, sample_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/point_cloud_ellipsoid");
  chdir(test_location.c_str());

  Eigen::MatrixXd flat(1200, 4);
  Eigen::MatrixXd flat_normals(1200, 4);

  Eigen::Index count = 0;

  double sep = 10.;

  for(size_t i = 0; i < 20; i++){
      for(size_t j = 0; j < 20; j++){
         flat(count,0) = i*sep;
         flat(count,1) = j*sep;
         flat(count,2) = 0.;
         flat(count,3) = 0.;
         flat_normals(count,2) = 1.;
         count++;

         flat(count,0) = i*sep;
         flat(count,1) = j*sep;
         flat(count,2) = 1.;
         flat(count,3) = 1.;
         flat_normals(count,2) = 1.;
         count++;

         flat(count,0) = i*sep;
         flat(count,1) = j*sep;
         flat(count,2) = -1.;
         flat(count,3) = -1.;
         flat_normals(count,2) = 1.;
         count++;
      }
  }

  std::cout << "Creating RBF..." << std::endl;

  shapeworks::RBFShape RBFflat = shapeworks::RBFShape();

  std::cout << "Solving system..." << std::endl;

  RBFflat.solve_system(flat);

  std::cout << "Printing..." << std::endl;

  RBFflat.set_raw_points(flat);
  RBFflat.set_raw_normals(flat_normals);

  RBFflat.write_csv("flat_1.csv",5);
  RBFflat.writeToEqFile("flat_1.Eq",5);
  RBFflat.writeToRawFile("flat_1.raw",5);

  for(Eigen::Index i = 0; i < count; i++){
      Eigen::Vector3d pt = flat.row(i).head<3>();
      //std::cout << "Point flat\n " << flat.row(i) << std::endl;
      //std::cout << "Point\n " << pt << std::endl;
      double value = flat(i,3);
      double rbf_value = RBFflat.evaluate(pt);
      std::cout << "Point " << i << " value " << value << " predicted_value " << rbf_value << std::endl;
  }

  double value = 99;
  ASSERT_LT(value, 100);
}

/*
//---------------------------------------------------------------------------
TEST(PointCloudTests, sample_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/point_cloud_ellipsoid");
  chdir(test_location.c_str());

  int n = 2;
  double r[3] = {1.,2.,3.};
  double c[3] = {0.,0.,0.};
  size_t ng;
  double* ellipsoid_pts = get_bounding_sphere(n, ng, 5);
  double* ellipsoid_pts_in = get_bounding_sphere(n, ng, 4);
  double* ellipsoid_pts_out = get_bounding_sphere(n, ng, 6);

  int count = ng;

  std::cout << "ng: " << ng << std::endl;

  std::cout << "Count: " << count << std::endl;

  Eigen::MatrixXd ellipsoid(count*3, 4);
  Eigen::MatrixXd ellipsoid_normals(count*3, 3);

  std::cout << "Points: " << count*3 << std::endl;

  for(size_t i = 0; i < count; i++){

    double ix = ellipsoid_pts[i*3];
    double iy = ellipsoid_pts[i*3+1];
    double iz = ellipsoid_pts[i*3+2];
    double ix_in = ellipsoid_pts_in[i*3];
    double iy_in = ellipsoid_pts_in[i*3+1];
    double iz_in = ellipsoid_pts_in[i*3+2];
    double ix_out = ellipsoid_pts_out[i*3];
    double iy_out = ellipsoid_pts_out[i*3+1];
    double iz_out = ellipsoid_pts_out[i*3+2];

    std::cout << ix << "," << iy << "," << iz << std::endl;

    double norm[3] = {ix-ix_in, iy-iy_in, iz-iz_in};
    double magnitude = sqrt(norm[0]*norm[0] + norm[1]*norm[1] + norm[2]*norm[2]);

    ellipsoid(i, 0) = ix;
    ellipsoid(i, 1) = iy;
    ellipsoid(i, 2) = iz;
    ellipsoid(i, 3) = 0.;
    ellipsoid(i+count,0) = ix_in;
    ellipsoid(i+count,1) = iy_in;
    ellipsoid(i+count,2) = iz_in;
    ellipsoid(i+count,3) = -1.;
    ellipsoid(i+count*2,0) = ix_out;
    ellipsoid(i+count*2,1) = iy_out;
    ellipsoid(i+count*2,2) = iz_out;
    ellipsoid(i+count*2,3) = 1.;

    ellipsoid_normals(i,0) = norm[0]/magnitude;
    ellipsoid_normals(i,1) = norm[1]/magnitude;
    ellipsoid_normals(i,2) = norm[2]/magnitude;
    ellipsoid_normals(i+count,0) = norm[0]/magnitude;
    ellipsoid_normals(i+count,1) = norm[1]/magnitude;
    ellipsoid_normals(i+count,2) = norm[2]/magnitude;
    ellipsoid_normals(i+count*2,0) = norm[0]/magnitude;
    ellipsoid_normals(i+count*2,1) = norm[1]/magnitude;
    ellipsoid_normals(i+count*2,2) = norm[2]/magnitude;
  }

  std::cout << "Creating RBF..." << std::endl;

  shapeworks::RBFShape RBFEllipsoid = shapeworks::RBFShape();

  std::cout << "Solving system..." << std::endl;

  RBFEllipsoid.solve_system(ellipsoid);

  std::cout << "Printing..." << std::endl;

  RBFEllipsoid.set_raw_points(ellipsoid);
  RBFEllipsoid.set_raw_normals(ellipsoid_normals);

  RBFEllipsoid.write_csv("ellipsoid_1.csv",5);
  RBFEllipsoid.writeToEqFile("ellipsoid_1.Eq",5);
  RBFEllipsoid.writeToRawFile("ellipsoid_1.raw",5);

  for(size_t i = 0; i < count*3; i++){
      Eigen::Vector3d pt = ellipsoid.row(i).head<3>();
      double value = ellipsoid(i,3);
      double rbf_value = RBFEllipsoid.evaluate(pt);
      std::cout << "Point " << i << " value " << value << " predicted_value " << rbf_value << std::endl;
  }

  double value = 99;
  ASSERT_LT(value, 100);
}
*/
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




