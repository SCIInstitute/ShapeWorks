/*=========================================================================
 *
 *  Copyright Insight Software Consortium
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0.txt
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *=========================================================================*/
#ifndef __itkPSMRBFCorrespondenceInterpolator_hxx
#define __itkPSMRBFCorrespondenceInterpolator_hxx
#include "itkPSMRBFCorrespondenceInterpolator.h"

namespace itk
{

template <unsigned int VDimension>
PSMRBFCorrespondenceInterpolator<VDimension>
::PSMRBFCorrespondenceInterpolator()
{

  //
  // TODO:  NOTE ! This function is currently only implemented for 3D
  //
  if (VDimension != 3)
    {
      itkExceptionMacro("This function is currently only implemented for 3D point sets.");
    }

}

template <unsigned int VDimension>
void
PSMRBFCorrespondenceInterpolator<VDimension>
::Initialize()
{ 
  // Make sure we have points on the input
  if (m_PointSetA.size() == 0 || m_PointSetB.size() == 0)
    {
      itkExceptionMacro("Surface point sets have not been specified.");
    }
  
  // Make sure the point set sizes match
  if (m_PointSetA.size() != m_PointSetB.size())
    {
      itkExceptionMacro("Surface point sets A and B are not the same size.");
    }

  // N is the number of points
  const unsigned int N = m_PointSetA.size();

  // TODO: Need algorithm references
  // TODO: Need to implement for at least 2D

  typedef vnl_vector<double> vectype;
  typedef vnl_matrix<double> matrixtype;
  
  // Transforms are from 1 to 2
  vectype b(N+4); // point data from file 2
  vectype x(N+4); // parameters to be solved for
  matrixtype A(N+4,N+4);
  matrixtype Phi(N,N);
  
  // Compute Phi values
  vectype Xi(3);
  vectype Xj(3);
  for (unsigned int i = 0; i < N; i++)
    {
    Xi[0] = (m_PointSetA[i])[0];
    Xi[1] = (m_PointSetA[i])[1];
    Xi[2] = (m_PointSetA[i])[2];
    
    for (unsigned int j = 0; j < N; j++)
      {
      Xj[0] = (m_PointSetA[j])[0];
      Xj[1] = (m_PointSetA[j])[1];
      Xj[2] = (m_PointSetA[j])[2];
      
      // Biharmonic Radial Basis Function
      Phi(i,j) = (Xi - Xj).magnitude();

      // Thin plate spline basis -- probably need some logic to switch on dimensionality
      // Phi(i,j) = dot_product(Xi - Xj,Xi - Xj) * log((Xi - Xj).magnitude() + 1.0e-6);
      }
    }
 
  // Construct the coefficient matrix A from PointSetA and Phi's
  for (unsigned int i = 0; i < 4; i++)
    {
    for (unsigned int j = 0; j < N +4; j++)
      {
      if      (j >= N) A(i,j) = 0.0;
      else if (i == 3) A(i,j) = 1.0;
      else             A(i,j) = (m_PointSetA[j])[i];
      }    
    }

  // Rest of the matrix A
  for (unsigned int i = 0; i < N; i++)
    {
    for (unsigned int j = 0; j < N +4; j++)
      {
      if      (j < N)    A(i+4,j) = Phi(i,j);
      else if (j == N+3) A(i+4,j) = 1.0;
      else               A(i+4,j) = (m_PointSetA[i])[2-(j-N)];
      }
    }

  // To keep things simple, solve in each dimension separately
  //  matrixtype P(4,3);
  //  matrixtype C(N,3);
  m_P.set_size(4,3);
  m_C.set_size(N,3);
  
  for (unsigned int D = 0; D < 3; D++)
    {
    // Construct the position data vector b from file 2 
    // [0 0 0 0 x1' x2' x3' ... xN' ]^T   
    b[0] = b[1] = b[2] = b[3] = 0.0;
    for(unsigned int i = 0; i < N; i++)
      {
      b[i+4] = (m_PointSetB[i])[D];
      }


    // Solve for parameters
    //    x = vnl_matrix_inverse<double>(A) * b;
    x = vnl_svd<double>(A).solve(b);

    // Store results
    for (unsigned int i = 0; i < N; i++)
      {
     m_C(i,D) = x[i];
      }
    
     m_P(0,D) = x[N+3];
     m_P(1,D) = x[N+2];
     m_P(2,D) = x[N+1];
     m_P(3,D) = x[N];

    } // end D

  m_Initialized = true;
}

template <unsigned int VDimension>
typename PSMRBFCorrespondenceInterpolator<VDimension>::PointType
PSMRBFCorrespondenceInterpolator<VDimension>
::Evaluate(const PointType &pt) const
{
  if (m_Initialized == false)
    {
      itkExceptionMacro("Function has not been initialized");
    }
  
 // N is the number of points
  const unsigned int N = m_PointSetA.size();

  // X is the input point
  vnl_vector<double> X(VDimension);
  for (unsigned int D = 0; D < VDimension; D++)
    {    X(D) = pt[D];    }

  // Convert PointSetA from points to vnl_vectors-- perhaps store this in the class?
  std::vector<vnl_vector<double> > points(N);
  for (unsigned int i = 0; i < N; i++)
    {
    points[i].set_size(VDimension);
    for (unsigned int D = 0; D < VDimension; D++)
      {
        points[i](D) = m_PointSetA[i][D];
      }

    }

  // Return value in which to accumulate
  PointType ret;
  
  // For each dimension ...
  for (unsigned int D = 0; D < VDimension; D++)
    {
    ret[D] = 0.0f; // initialize to zero
    
    // Compute RBF term in D
    for (unsigned int i = 0; i < N; i++)
      {      
        ret[D] += m_C(i,D) * (X - points[i]).magnitude();      
      }
    
    // Add the polynomial term
    for (unsigned int i = 0; i < VDimension; i++)
      {
        //  std::cout << "ret[D] += m_P(i+1,D) + X(i) ->" << ret[D] << " += " << m_P(i+1,D) << " + " << X(i) << std::endl;
        ret[D] += m_P(i+1,D) * X(i);
      }
    ret[D] += m_P(0,D);

    }

  return ret;
}

} // end namespace itk
#endif
