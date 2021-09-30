#pragma once

#include <Eigen/Core>

#include "Shapeworks.h"

namespace shapeworks {

// Returns a 4x4 Eigen (col-major) from the given 3x4 (left-hand) itk::Transform.
Eigen::MatrixXd itkTransformToEigen(TransformPtr itk_xform);

// Returns a 3x4 itk::AffineTransform from the given (col-major) 4x4 Eigen::Matrix
TransformPtr eigen44ToItkTransform(const Eigen::Matrix<double, 4, 4> &eigen_mat);

template<typename T>
using VnlMatrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>;

/// Conversion (by copy) of itk matrix to Eigen::Matrix.
template<typename T, unsigned NRows, unsigned NCols>
Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor> itkToEigen(const itk::Matrix<T, NRows, NCols> &itk_mat)
{
  return Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor>(itk_mat.GetVnlMatrix().data_block());
}

/// Conversion (by copy) of Eigen::Matrix to itk (i.e., vnl) matrix.
// With any luck, rvalue assignment will be used here (though it's explicit in the vxl code).
template<typename T, int NRows, int NCols>
itk::Matrix<T, NRows, NCols> eigenToItk(const Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor> &eigen_mat)
{
  return itk::Matrix<T, NRows, NCols>(vnl_matrix_fixed<T, NRows, NCols>(eigen_mat.data()));
}

/// Wrap vnl matrix data to Eigen Matrix
template<typename T>
Eigen::Map<VnlMatrix<T>> vnlToEigen(const vnl_matrix<T> &vnl_mat)
{
  return Eigen::Map<VnlMatrix<T>>(const_cast<T*>(vnl_mat.data_block()), vnl_mat.rows(), vnl_mat.cols());
}

/// Wrap data pointer with Eigen::Matrix. Handy for efficiently going back and forth between Python numpy arrays.
/// WARNING: if data goes out of scope (e.g., numpy array size changes), matrix will point to invalid location.
template<int NRows, int NCols, typename T>
Eigen::Map<Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor>> wrapDataWithEigen(T *data)
{
  return Eigen::Map<Eigen::Matrix<T, NRows, NCols, Eigen::RowMajor>>(data);
}

/// Wrap a data pointer of the specified size with Eigen::Matrix.
template<typename T>
Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
wrapDataWithEigen(T *data, size_t nrows, size_t ncols)
{
  return Eigen::Map<Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>(data, nrows, ncols);
}

/// Wrap data pointer with itk::Matrix. Handy for efficiently going back and forth between Python numpy arrays.
/// WARNING: This function actually just copies data.
template<unsigned NRows, unsigned NCols, typename T>
itk::Matrix<T, NRows, NCols> wrapDataWithItk(T *data)
{
  std::cerr << "WARNING: vnl cannot wrap data, so this just copies input array\n";
  itk::Matrix<T, NRows, NCols> itk_mat;
  vnl_matrix_fixed<T, NRows, NCols> &vnl_mat = itk_mat.GetVnlMatrix();
  vnl_mat.copy_in(data);
  return itk_mat;
}

} // shapeworks
