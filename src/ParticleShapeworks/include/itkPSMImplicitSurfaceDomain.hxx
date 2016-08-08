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
#ifndef __itkPSMImplicitSurfaceDomain_hxx
#define __itkPSMImplicitSurfaceDomain_hxx
#include "itkPSMImplicitSurfaceDomain.h"

#include "vnl/vnl_math.h"
#include "vnl/vnl_cross.h"

namespace itk
{

template<class T, unsigned int VDimension>
void
PSMImplicitSurfaceDomain<T, VDimension>::
SetCuttingPlane(const vnl_vector<double> &a, const vnl_vector<double> &b,
                const vnl_vector<double> &c)
{
  // See http://mathworld.wolfram.com/Plane.html, for example
  vnl_vector<double> q;
  if (VDimension == 3)  q = vnl_cross_3d((b-a),(c-a));
  else if (VDimension == 2)  q = vnl_cross_2d((b-a),(c-a));
  
  if (q.magnitude() > 0.0)
  {
    m_CuttingPlaneNormal = q / q.magnitude();
    m_CuttingPlanePoint = a;
    m_UseCuttingPlane = true;
  }
}

template<class T, unsigned int VDimension>
bool
PSMImplicitSurfaceDomain<T, VDimension>::
ApplyVectorConstraints(vnl_vector_fixed<double, VDimension> &gradE,
                       const PointType &pos,
                       double maxtimestep) const
{
  if (this->m_UseCuttingPlane == true)
    {    
    // See http://mathworld.wolfram.com/Point-PlaneDistance.html, for example
    vnl_vector_fixed<double, VDimension> x;
    vnl_vector_fixed<T, VDimension> grad = this->SampleGradientVnl(pos);
    for (unsigned int i = 0; i < VDimension; i++)
      { x[i] = pos[i]; }
    const double D = dot_product(m_CuttingPlaneNormal, x- m_CuttingPlanePoint);
    
    //    x = m_CuttingPlaneNormal * fabs(1.0 / (D + 1.0e-3));
    // x = m_CuttingPlaneNormal * lambda * exp(-lambda * fabs(D));
    
    // Gradient of simple force 1/D^2 to push away from cutting plane
    vnl_vector_fixed<double, VDimension> df;
    const double k = (-2.0 / (D * D * D));
    df[0] = k * grad[0] * m_CuttingPlaneNormal[0];
    df[1] = k * grad[1] * m_CuttingPlaneNormal[1];
    df[2] = k * grad[2] * m_CuttingPlaneNormal[2];

    gradE = gradE + df;

    // Make sure force is not huge relative to other forces.
    if (gradE.magnitude() > maxtimestep)
      {
      gradE.normalize();
      gradE = gradE * maxtimestep;
      }    
    }

  
  return Superclass::ApplyVectorConstraints(gradE,pos,maxtimestep);
}

template<class T, unsigned int VDimension>
bool
PSMImplicitSurfaceDomain<T, VDimension>::ApplyConstraints(PointType &p) const
{
  // First apply and constraints imposed by superclasses.  This will
  // guarantee the point starts in the correct image domain.
  bool flag = Superclass::ApplyConstraints(p);

  if (this->m_ConstraintsEnabled == true)
    {
  
    unsigned int k = 0;
    double mult = 1.0;
    
    const T epsilon = m_Tolerance * 0.001;
    T f = this->Sample(p);
    
    T gradmag = 1.0;
    while ( fabs(f) > (m_Tolerance * mult) || gradmag < epsilon)
      //  while ( fabs(f) > m_Tolerance || gradmag < epsilon)
      {
      vnl_vector_fixed<T, VDimension> grad = this->SampleGradientVnl(p);
      
      gradmag = grad.magnitude();
      vnl_vector_fixed<T, VDimension> vec   =  grad  * ( f / (gradmag + epsilon) );
      for (unsigned int i = 0; i < VDimension; i++)
        {
        p[i] -= vec[i];
        }
      
      f = this->Sample(p);
            
      // Raise the tolerance if we have done too many iterations.
      k++;
      if (k > 10000)
        {
        mult *= 2.0;
        k = 0;
        }
      } // end while
    } // end if m_ConstraintsEnabled == true

  return flag; 
}

template <class T, unsigned int VDimension>
double
PSMImplicitSurfaceDomain<T, VDimension>::Distance(const PointType &a, const PointType &b) const
{
  double sum = 0.0;
  for (unsigned int i = 0; i < VDimension; i++)
    {      sum += (b[i]-a[i]) * (b[i]-a[i]);      }
  return sqrt(sum);
}

} // end namespace
#endif
