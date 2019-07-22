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
#ifndef __itkPSMMixedEffectsShapeMatrixAttribute_hxx
#define __itkPSMMixedEffectsShapeMatrixAttribute_hxx
#include "itkPSMMixedEffectsShapeMatrixAttribute.h"
#include <iostream>

namespace itk
{

template <class T, unsigned int VDimension>
void PSMMixedEffectsShapeMatrixAttribute<T,VDimension>
::UpdateMeanMatrix()
{
  // For each sample
  vnl_vector<double> tempvect;
  int indexSum = 0, group_indx = -1;
  tempvect.set_size(m_MeanMatrix.rows());
  tempvect.fill(0.0);
  for (unsigned int i = 0; i < m_MeanMatrix.cols(); i++)
  {
    if(i == indexSum)
    {

      //// Convert m_TimePointsPerIndividual to an array
      //  int group_indx = i / m_TimePointsPerIndividual;
      //  tempvect = this->GetIntercept() + this->GetSlope() * expl(i);
      //  tempvect = tempvect + m_InterceptRand.get_row(group_indx);
      //  tempvect = tempvect + m_SlopeRand.get_row(group_indx) * expl(i);
      //// ... compute the mean.
      //this->GetMeanMatrix().set_column(i, tempvect);
      
      group_indx = group_indx + 1;
      indexSum += m_TimeptsPerIndividual(group_indx);
    }
      
    tempvect = m_Intercept + m_Slope * m_Expl(i);
    tempvect = tempvect + m_InterceptRand.get_row(group_indx);
    tempvect = tempvect + m_SlopeRand.get_row(group_indx) * m_Expl(i);
    // Compute the mean
    m_MeanMatrix.set_column(i, tempvect);
  }
}

  
template <class T, unsigned int VDimension>
void PSMMixedEffectsShapeMatrixAttribute<T,VDimension>::
EstimateParameters()
{
  std::cout << "Estimating params" << std::endl;
  
  vnl_matrix<double> X = *this + m_MeanMatrix;
  
  // Number of samples
  int num_shapes = static_cast<double>(X.cols());
  
  int nr = X.rows(); //number of points*3
  
  // set the sizes of random slope and intercept matrix
  m_SlopeRand.set_size(m_NumIndividuals, nr); // num_individuals X num_points*3
  m_InterceptRand.set_size(m_NumIndividuals, nr); // num_individuals X num_points*3
  
  vnl_matrix<double> fixed; // slopes + intercepts for all points
  vnl_matrix<double> random; // slopes + intercepts for all groups, for all points
  fixed.set_size(2, nr);
  random.set_size(2, nr * m_NumIndividuals);
  
  vnl_matrix<double> Ds(2, 2); // covariance matrix of random parameters (2x2)
  Ds.set_identity();  // initialize to identity
  vnl_matrix<double> identity_2;
  identity_2.set_size(2, 2);
  identity_2.set_identity();
  
  double sigma2s = 1;  // variance of error
  vnl_matrix<double> * Ws = NULL, * Vs = NULL, * identity_n = NULL, * Xp = NULL;
  Ws = new vnl_matrix<double>[m_NumIndividuals];
  Vs = new vnl_matrix<double>[m_NumIndividuals];
  identity_n = new vnl_matrix<double>[m_NumIndividuals];
  Xp = new vnl_matrix<double>[m_NumIndividuals];  // Xp: Design matrix. Stores all time points.
  
  vnl_vector<double> residual, y;
  y.set_size(m_TimeptsPerIndividual(0));
  y.fill(0.0);
  residual.set_size(m_TimeptsPerIndividual(0));
  residual.fill(0.0);
  
  for (int i = 0; i < m_NumIndividuals; i++)
  {
    Vs[i].set_size(m_TimeptsPerIndividual(i), m_TimeptsPerIndividual(i));
    Ws[i].set_size(m_TimeptsPerIndividual(i), m_TimeptsPerIndividual(i));
    
    identity_n[i].set_size(m_TimeptsPerIndividual(i), m_TimeptsPerIndividual(i));
    identity_n[i].set_identity();
    
    Xp[i].set_size(m_TimeptsPerIndividual(i), 2);
  }
  
  vnl_matrix<double> sum_mat1(2, 2, 0);
  vnl_vector<double> sum_mat2(2); sum_mat2.fill(0.0);
  double ecorr = 0.0;
  double tracevar = 0.0;
  vnl_matrix<double> bscorr(2, 2, 0.0);
  vnl_matrix<double> bsvar(2, 2, 0.0);
  vnl_vector<double> tempvect; tempvect.set_size(2);
  unsigned int indexSum = 0;
  
  for (int i = 0; i < nr; i++) //for all points (x,y,z coordinates)
  {
    sigma2s = 0.01;
    Ds.set_identity();
    Ds *= sigma2s;
    for (int j = 0; j < 100; j++) //EM iterations
    {
      sum_mat1.fill(0.0); sum_mat2.fill(0.0);
      indexSum = 0;
      
      for (int k = 0; k < m_NumIndividuals; k++)
      {
        y.clear();
        y.set_size(m_TimeptsPerIndividual(k));
        y.fill(0.0);
        
        for (int l = 0; l < m_TimeptsPerIndividual(k); l++)
        {
          Xp[k](l, 0) = m_Expl(indexSum + l);
          Xp[k](l, 1) = 1;
          y(l) = X(i, indexSum + l);
        }
        
        indexSum += m_TimeptsPerIndividual(k);
        
        Vs[k] = (identity_n[k] * sigma2s) + Xp[k] * Ds * vnl_transpose(Xp[k]);
        
        if(Vs[k].rows() > 4)
        {
          Ws[k] = vnl_matrix_inverse<double>(Vs[k]);
        }
        else
        {
          Ws[k] = vnl_inverse<double>(Vs[k]);
        }
        sum_mat1 = sum_mat1 + vnl_transpose(Xp[k]) * Ws[k] * Xp[k];
        sum_mat2 = sum_mat2 + vnl_transpose(Xp[k]) * Ws[k] * y;
      }
      tempvect = vnl_inverse<double>(sum_mat1) * sum_mat2;
      fixed.set_column(i, tempvect);
      
      indexSum = 0; // re-initializing
      ecorr = 0.0;
      tracevar = 0.0;
      bscorr.fill(0.0);
      bsvar.fill(0.0);
      
      for (int k = 0; k < m_NumIndividuals; k++)
      {
        residual.clear(); y.clear();
        residual.set_size(m_TimeptsPerIndividual(k));
        y.set_size(m_TimeptsPerIndividual(k));
        residual.fill(0.0); y.fill(0.0);
        
        for (int l = 0; l < m_TimeptsPerIndividual[k]; l++)
        {
          Xp[k](l,0) = m_Expl(indexSum + l);
          Xp[k](l,1) = 1;
          y(l) = X(i, indexSum + l);
        }
        
        indexSum += m_TimeptsPerIndividual(k);
        
        tempvect = Ds * vnl_transpose(Xp[k]) * Ws[k] * (y - (Xp[k] * fixed.get_column(i)));
        random.set_column(i * m_NumIndividuals + k, tempvect);
        residual = y - (Xp[k] * fixed.get_column(i)) - (Xp[k] * random.get_column(i * m_NumIndividuals + k));
        ecorr = ecorr + dot_product(residual, residual);
        tracevar = tracevar + (m_TimeptsPerIndividual(k) - sigma2s * vnl_trace(Ws[k]));
        bscorr = bscorr + outer_product(random.get_column(i * m_NumIndividuals + k), random.get_column(i * m_NumIndividuals + k));
        bsvar = bsvar + (identity_2 - (vnl_transpose(Xp[k]) * Ws[k] * Xp[k] * Ds));
      }
      
      indexSum = 0; // re-initializing
      sigma2s = (ecorr + sigma2s * tracevar) / (num_shapes);
      Ds = (bscorr + Ds * bsvar) / m_NumIndividuals;
    } //endfor EM iterations
  } //endfor all points on shape (x,y & z)
  
  m_Slope = fixed.get_row(0);
  m_Intercept = fixed.get_row(1);
  for (int i = 0; i < m_NumIndividuals; i++)
  {
    for (int j = 0; j < nr; j++) //for all points * 3
    {
      m_SlopeRand(i, j) = random(0, j * m_NumIndividuals + i);
      m_InterceptRand(i, j) = random(1, j * m_NumIndividuals + i);
    }
  }
  
  delete [] Vs;
  delete [] Ws;
  delete [] identity_n;
  delete [] Xp;
}
  
} // end namespace itk

#endif
