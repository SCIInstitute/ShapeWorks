#pragma once

#include <Eigen/Core>

#include "Shapeworks.h"

namespace shapeworks {

/// Conversion (by copy) of itk (i.e., vnl) matrix to Eigen::Matrix.
template<typename T, unsigned NRows, unsigned NCols>
Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor> itkToEigen(const itk::Matrix<T, NRows, NCols> &itk_mat)
{
  return Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor>(itk_mat.GetVnlMatrix().data_block());
}

/// Conversion (by copy) of Eigen::Matrix to itk (i.e., vnl) matrix.
// With any luck, rvalue assignment will be used here (though it's explicit in the vxl code).
template<typename T, unsigned NRows, unsigned NCols>
itk::Matrix<T, NRows, NCols> eigenToItk(const Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor> &eigen_mat)
{
  return itk::Matrix<T, NRows, NCols>(vnl_matrix_fixed<T, NRows, NCols>(eigen_mat.data()));
}

/// Wrap data pointer with Eigen::Matrix. Handy for efficiently going back and forth between Python numpy arrays.
/// WARNING: if data goes out of scope (e.g., numpy array size changes), matrix will point to invalid location.
template<typename T, unsigned NRows, unsigned NCols>
Eigen::Map<Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor>> wrapDataWithEigen(T *data)
{
  return Eigen::Map<Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor>>(data);
}

/// Wrap data pointer with itk::Matrix. Handy for efficiently going back and forth between Python numpy arrays.
/// WARNING: This function actually just copies data.
template<typename T, unsigned NRows, unsigned NCols>
itk::Matrix<T, NRows, NCols> wrapDataWithItk(T *data)
{
  std::cerr << "WARNING: vnl cannot wrap data, so this just copies input array\n";
  itk::Matrix<T, NRows, NCols> itk_mat;
  auto vnl_mat = itk_mat.GetVnlMatrix();
  vnl_mat.copy_in(data);
  return itk_mat;
}

} // shapeworks
