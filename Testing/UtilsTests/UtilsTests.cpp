#include "Testing.h"

#include "EigenUtils.h"

using namespace shapeworks;

TEST(UtilsTests, itkToEigen)
{
  Matrix33 itk_mat;
  itk_mat.Fill(2);
  itk_mat(0,0) = 0;
  itk_mat(0,2) = 9;
  itk_mat(2,2) = 5;
  itk_mat(2,0) = 0;
  auto eigen_mat = itkToEigen(itk_mat);

  Eigen::Matrix<double, 3, 3, Eigen::RowMajor> ground_truth;
  ground_truth << 0, 2, 9,
                  2, 2, 2,
                  0, 2, 5;

  ASSERT_TRUE(eigen_mat == ground_truth);
}

TEST(UtilsTests, eigenToItk)
{
  Eigen::Matrix<double, 3, 3, Eigen::RowMajor> eigen_mat;
  eigen_mat << 0, 2, 9,
               2, 2, 2,
               0, 2, 5;
  Matrix33 itk_mat = eigenToItk(eigen_mat);

  Matrix33 ground_truth;
  ground_truth.Fill(2);
  ground_truth(0,0) = 0;
  ground_truth(0,2) = 9;
  ground_truth(2,2) = 5;
  ground_truth(2,0) = 0;

  ASSERT_TRUE(itk_mat == ground_truth);
}

TEST(UtilsTests, wrapDataWithEigen)
{
  std::vector<unsigned> data{
    0, 2, 9,
    2, 2, 2,
    0, 2, 5};
  auto eigen_mat = wrapDataWithEigen<3, 3>(data.data());

  Eigen::Matrix<unsigned, 3, 3, Eigen::RowMajor> ground_truth;
  ground_truth << 0, 2, 9,
                  2, 2, 2,
                  0, 2, 5;

  ASSERT_TRUE(eigen_mat == ground_truth);
}

TEST(UtilsTests, wrapDataWithItk)
{
  std::vector<float> data{
    0, 2, 9,
    2, 2, 2,
    0, 2, 5};
  auto itk_mat = wrapDataWithItk<3, 3>(data.data());

  itk::Matrix<float, 3, 3> ground_truth;
  ground_truth.Fill(2);
  ground_truth(0,0) = 0;
  ground_truth(0,2) = 9;
  ground_truth(2,2) = 5;
  ground_truth(2,0) = 0;

  ASSERT_TRUE(itk_mat == ground_truth);
}
