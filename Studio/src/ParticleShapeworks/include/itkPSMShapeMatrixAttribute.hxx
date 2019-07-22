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
#ifndef __itkPSMShapeMatrixAttribute_hxx
#define __itkPSMShapeMatrixAttribute_hxx
#include "itkPSMShapeMatrixAttribute.h"

namespace itk
{

template <class T, unsigned int VDimension>
void PSMShapeMatrixAttribute<T,VDimension>::DomainAddEventCallback(Object *, const EventObject &e)
{
  const ParticleDomainAddEvent &event = dynamic_cast<const ParticleDomainAddEvent &>(e);
  unsigned int d = event.GetDomainIndex();
  
  if ( d % m_DomainsPerShape  == 0 )
    {
      this->ResizeMatrix(this->rows(), this->cols()+1);
    }
}
 
template <class T, unsigned int VDimension> 
void PSMShapeMatrixAttribute<T,VDimension>::ResizeMatrix(int rs, int cs)
{
  vnl_matrix<T> tmp(*this); // copy existing  matrix
  
  // Create new column (shape)
  this->set_size(rs, cs);
  
  // Copy old data into new matrix.
  for (unsigned int c = 0; c < tmp.cols(); c++)
    {
      for (unsigned int r = 0; r < tmp.rows(); r++)
        {
          this->operator()(r,c) = tmp(r,c);
        }
    } 
}
 
template <class T, unsigned int VDimension> 
void PSMShapeMatrixAttribute<T,VDimension>::PositionAddEventCallback(Object *o, const EventObject &e) 
{
  const ParticlePositionAddEvent &event = dynamic_cast<const ParticlePositionAddEvent &>(e);
  const PSMParticleSystem<VDimension> *ps= dynamic_cast<const PSMParticleSystem<VDimension> *>(o);
  const int d = event.GetDomainIndex();
  const unsigned int idx = event.GetPositionIndex();
  const typename PSMParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);
  
  const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);
  
  // Make sure we have enough rows.
  if ((ps->GetNumberOfParticles(d) * VDimension * m_DomainsPerShape) > this->rows())
    {
      this->ResizeMatrix(PointsPerDomain * VDimension * m_DomainsPerShape, this->cols());
    }

  // CANNOT ADD POSITION INFO UNTIL ALL POINTS PER DOMAIN IS KNOWN
  // Add position info to the matrix
  unsigned int k = ((d % m_DomainsPerShape) * PointsPerDomain * VDimension)
    + (idx * VDimension);
  for (unsigned int i = 0; i < VDimension; i++)
    {
      this->operator()(i+k, d / m_DomainsPerShape) = pos[i];
    }
}
 
template <class T, unsigned int VDimension> 
void PSMShapeMatrixAttribute<T,VDimension>::PositionSetEventCallback(Object *o, const EventObject &e) 
{
  const ParticlePositionSetEvent &event = dynamic_cast<const ParticlePositionSetEvent &>(e);
  const PSMParticleSystem<VDimension> *ps= dynamic_cast<const PSMParticleSystem<VDimension> *>(o);
  const int d = event.GetDomainIndex();
  const unsigned int idx = event.GetPositionIndex();
  const typename PSMParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);
  const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);
  
  // Modify matrix info
  //    unsigned int k = VDimension * idx;
  unsigned int k = ((d % m_DomainsPerShape) * PointsPerDomain * VDimension)
    + (idx * VDimension);
  for (unsigned int i = 0; i < VDimension; i++)
    {
      this->operator()(i+k, d / m_DomainsPerShape) = pos[i];
    }
}

} // end namespace itk


#endif
