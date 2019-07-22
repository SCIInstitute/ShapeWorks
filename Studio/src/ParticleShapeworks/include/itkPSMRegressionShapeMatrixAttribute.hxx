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
#ifndef __itkPSMRegressionShapeMatrixAttribute_hxx
#define __itkPSMRegressionShapeMatrixAttribute_hxx
#include "itkPSMRegressionShapeMatrixAttribute.h"

namespace itk
{

template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::UpdateMeanMatrix()
{
  // for each sample
  for (unsigned int i = 0; i < m_MeanMatrix.cols(); i++)
    {
      // compute the mean
      m_MeanMatrix.set_column(i, m_Intercept + m_Slope * m_Explanatory(i));
    }
}
  
template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::ResizeParameters(unsigned int n)
{
  vnl_vector<double> tmpA = m_Intercept; // copy existing  matrix
  vnl_vector<double> tmpB = m_Slope; // copy existing  matrix
  
  // Create new 
  m_Intercept.set_size(n);
  m_Slope.set_size(n);
  
  // Copy old data into new vector.
  for (unsigned int r = 0; r < tmpA.size(); r++)
    {
      m_Intercept(r) = tmpA(r);
      m_Slope(r) = tmpB(r);
    }
}
  
template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::ResizeMeanMatrix(int rs, int cs)
{
  vnl_matrix<T> tmp = m_MeanMatrix; // copy existing  matrix
  
  // Create new column (shape)
  m_MeanMatrix.set_size(rs, cs);
  
  m_MeanMatrix.fill(0.0);
  
  // Copy old data into new matrix.
  for (unsigned int c = 0; c < tmp.cols(); c++)
    {
      for (unsigned int r = 0; r < tmp.rows(); r++)
        {
          m_MeanMatrix(r,c) = tmp(r,c);
        }
    } 
}
  
template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::ResizeExplanatory(unsigned int n)
{
  if (n > m_Explanatory.size())
    {
      vnl_vector<double> tmp = m_Explanatory; // copy existing  matrix
      
      // Create new 
      m_Explanatory.set_size(n);
      m_Explanatory.fill(0.0);
      
      // Copy old data into new vector.
      for (unsigned int r = 0; r < tmp.size(); r++)
        {
          m_Explanatory(r) = tmp(r);
        }
    }
}  
  
template <class T, unsigned int VDimension>  
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::DomainAddEventCallback(Object *, const EventObject &e)
{
  const ParticleDomainAddEvent &event
    = dynamic_cast<const ParticleDomainAddEvent &>(e);
  unsigned int d = event.GetDomainIndex();
  
  if ( d % this->m_DomainsPerShape  == 0 )
    {
      this->ResizeMatrix(this->rows(), this->cols()+1);
      this->ResizeMeanMatrix(this->rows(), this->cols()+1);
      this->ResizeExplanatory(this->cols());
    }    
}
  
template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::PositionAddEventCallback(Object *o, const EventObject &e) 
{
  const ParticlePositionAddEvent &event
    = dynamic_cast<const ParticlePositionAddEvent &>(e);
  const PSMParticleSystem<VDimension> *ps
    = dynamic_cast<const PSMParticleSystem<VDimension> *>(o);
  const int d = event.GetDomainIndex();
  const unsigned int idx = event.GetPositionIndex();
  const typename PSMParticleSystem<VDimension>::PointType pos
    = ps->GetTransformedPosition(idx, d);
  
  const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);
  
  // Make sure we have enough rows.
  if ((ps->GetNumberOfParticles(d) * VDimension * this->m_DomainsPerShape)
      > this->rows())
    {
      this->ResizeParameters(PointsPerDomain * VDimension * this->m_DomainsPerShape);
      this->ResizeMatrix(PointsPerDomain * VDimension * this->m_DomainsPerShape,
                         this->cols());
      this->ResizeMeanMatrix(PointsPerDomain * VDimension * this->m_DomainsPerShape,
                             this->cols());
    }
  
  // ! CANNOT ADD POSITION INFO UNTIL ALL POINTS PER DOMAIN IS KNOWN !
  // Add position info to the matrix
  unsigned int k = ((d % this->m_DomainsPerShape) * PointsPerDomain * VDimension)
    + (idx * VDimension);
  for (unsigned int i = 0; i < VDimension; i++)
    {
      this->operator()(i+k, d / this->m_DomainsPerShape) = pos[i];
    }
}
  
template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::PositionSetEventCallback(Object *o, const EventObject &e) 
{
  const ParticlePositionSetEvent &event
    = dynamic_cast <const ParticlePositionSetEvent &>(e);
  
  const PSMParticleSystem<VDimension> *ps
    = dynamic_cast<const PSMParticleSystem<VDimension> *>(o);
  const int d = event.GetDomainIndex();
  const unsigned int idx = event.GetPositionIndex();
  const typename PSMParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);
  const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);
  
  // Modify matrix info
  //    unsigned int k = VDimension * idx;
  unsigned int k = ((d % this->m_DomainsPerShape) * PointsPerDomain * VDimension)
    + (idx * VDimension);    
  
  for (unsigned int i = 0; i < VDimension; i++)
    {
      this->operator()(i+k, d / this->m_DomainsPerShape) =
        pos[i] - m_MeanMatrix(i+k, d/ this->m_DomainsPerShape);
    }
}
  
template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::SetExplanatory(const std::vector<double> &v)
{
  this->ResizeExplanatory(v.size());
  for (unsigned int i = 0; i < v.size(); i++)
    {
      m_Explanatory[i] = v[i];
    }
}

template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::SetSlope(const std::vector<double> &v)
{
  ResizeParameters(v.size());
  for (unsigned int i = 0; i < v.size(); i++)
    {
      m_Slope[i] = v[i];
    }    
}

template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::SetIntercept(const std::vector<double> &v)
{
  ResizeParameters(v.size());
  for (unsigned int i = 0; i < v.size(); i++)
    {
      m_Intercept[i] = v[i];
    }
  
}
  
template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::EstimateParameters()
{
  const double tol = 1.0e-12;
  vnl_matrix<double> X = *this + m_MeanMatrix;
  
  // Number of samples
  double n = (double)(X.cols());
  
  vnl_vector<double> sumtx = m_Explanatory[0] * X.get_column(0);
  vnl_vector<double> sumx = X.get_column(0);
  double sumt = m_Explanatory[0];
  double sumt2 = m_Explanatory[0] * m_Explanatory[0];    
  for (unsigned int k = 1; k < X.cols(); k++)      // k is the sample number
    {
      sumtx += m_Explanatory[k] * X.get_column(k);
      sumx  += X.get_column(k);
      sumt  += m_Explanatory[k];
      sumt2 += m_Explanatory[k] * m_Explanatory[k];      
    }
  
  m_Slope = (n * sumtx - (sumx * sumt)) / ((n * sumt2 - (sumt*sumt)) + tol);
  
  vnl_vector<double> sumbt = m_Slope * m_Explanatory[0];
  for (unsigned int k = 1; k < X.cols(); k++)
    {
      sumbt += m_Slope * m_Explanatory[k];
    }
  
  m_Intercept = (sumx - sumbt) / n;
}
  
template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::Initialize()
{
  //  if (m_Explanatory.size() != this->columns())
  //  {
  //    itkExceptionMacro("The number of columns (shapes) does not match the number of explanatory variables -- or no explanatory variables have been set.");
  //  }
  // Check that the paramters are not all equal.  This is likely
  // because the user did not set the parameters.  In any case, it
  // will result in undefined results (nan) for the slope and
  // intercept.
  bool ok = false;
  for (unsigned int i = 1; i < m_Explanatory.size(); i++)
    {
      if (m_Explanatory[i] != m_Explanatory[0]) ok = true;
    }
  if (!ok)
    {
      itkExceptionMacro("All explanatory variables are the same value.  Please set these values before running the filter.");
    }
  m_Intercept.fill(0.0);
  m_Slope.fill(0.0);
  m_MeanMatrix.fill(0.0);
}
  
template <class T, unsigned int VDimension>
void PSMRegressionShapeMatrixAttribute<T,VDimension>
::BeforeIteration()
{
  m_UpdateCounter ++;
  if (m_UpdateCounter >= m_RegressionInterval)
    {
      m_UpdateCounter = 0;
      this->EstimateParameters();
      this->UpdateMeanMatrix();
    }
}
  
} // end namespace itk


#endif
