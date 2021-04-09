
#include "EigenUtils.h"

namespace shapeworks {

Matrix33 createMatrix(const Eigen::MatrixXd &mat)
{
  if (mat.rows() != 3 || mat.cols() != 3) {
    throw std::invalid_argument("input matrix must be 3x3");
  }

  Matrix33 m;
  for(int i=0; i<3; i++) {
    for(int j=0; j<3; j++) {
      m[i][j] = mat(i, j);
    }
  }

  return m;
}

} // shapeworks
