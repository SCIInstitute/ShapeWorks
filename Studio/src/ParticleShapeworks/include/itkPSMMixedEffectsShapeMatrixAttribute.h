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

#ifndef __itkPSMMixedEffectsShapeMatrixAttribute_h
#define __itkPSMMixedEffectsShapeMatrixAttribute_h

#include "itkPSMShapeMatrixAttribute.h"
#include "vnl/vnl_vector.h"
#include "itkPSMParticleSystem.h"
#include "vnl/vnl_trace.h"

namespace itk
{
 /** \class PSMMixedEffectsShapeMatrixAttribute
  *
  *
  *
  */
 template <class T, unsigned int VDimension>
 class ITK_EXPORT PSMMixedEffectsShapeMatrixAttribute
   : public PSMShapeMatrixAttribute<T,VDimension>
 {
 public:
   /** Standard class typedefs */
   typedef T DataType;
   typedef PSMMixedEffectsShapeMatrixAttribute Self;
   typedef PSMShapeMatrixAttribute<T,VDimension> Superclass;
   typedef SmartPointer<Self>  Pointer;
   typedef SmartPointer<const Self>  ConstPointer;
   typedef WeakPointer<const Self>  ConstWeakPointer;

   /** Method for creation through the object factory. */
   itkNewMacro(Self);

   /** Run-time type information (and related methods). */
   itkTypeMacro(PSMMixedEffectsShapeMatrixAttribute, PSMShapeMatrixAttribute);

   inline vnl_vector<double> ComputeMean(double k) const
   {
     return m_Intercept + m_Slope * k;    
   }

   void ResizeParameters(unsigned int n)
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

   virtual void ResizeMeanMatrix(int rs, int cs)
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

   void ResizeExplanatory(unsigned int n)
   {
     if (n > m_Expl.size())
     {
       vnl_vector<double> tmp = m_Expl; // copy existing  matrix

       // Create new 
       m_Expl.set_size(n);
       m_Expl.fill(0.0);

       // Copy old data into new vector.
       for (unsigned int r = 0; r < tmp.size(); r++)
       {
	 m_Expl(r) = tmp(r);
       }
     }
   } 

   /** Callbacks that may be defined by a subclass.  If a subclass defines one
       of these callback methods, the corresponding flag in m_DefinedCallbacks
       should be set to true so that the PSMParticleSystem will know to register
       the appropriate event with this method. */
   virtual void DomainAddEventCallback(Object *, const EventObject &e)
   {
     const itk::ParticleDomainAddEvent &event
       = dynamic_cast<const itk::ParticleDomainAddEvent &>(e);
     unsigned int d = event.GetDomainIndex();

     if ( d % this->m_DomainsPerShape  == 0 )
     {
       this->ResizeMatrix(this->rows(), this->cols()+1);
       this->ResizeMeanMatrix(this->rows(), this->cols()+1);
       this->ResizeExplanatory(this->cols());
     }    
   }

   virtual void PositionAddEventCallback(Object *o, const EventObject &e) 
   {
     const itk::ParticlePositionAddEvent &event
       = dynamic_cast<const itk::ParticlePositionAddEvent &>(e);
     const itk::PSMParticleSystem<VDimension> *ps
       = dynamic_cast<const itk::PSMParticleSystem<VDimension> *>(o);
     const int d = event.GetDomainIndex();
     const unsigned int idx = event.GetPositionIndex();
     const typename itk::PSMParticleSystem<VDimension>::PointType pos
       = ps->GetTransformedPosition(idx, d);

     const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);

     // Make sure we have enough rows.
     if ( (ps->GetNumberOfParticles(d) * VDimension * this->m_DomainsPerShape) > this->rows() )
     {
       this->ResizeParameters(PointsPerDomain * VDimension * this->m_DomainsPerShape);
       this->ResizeMatrix(PointsPerDomain * VDimension * this->m_DomainsPerShape,
			  this->cols());
       this->ResizeMeanMatrix(PointsPerDomain * VDimension * this->m_DomainsPerShape,
			      this->cols());
     }

     // CANNOT ADD POSITION INFO UNTIL ALL POINTS PER DOMAIN IS KNOWN
     // Add position info to the matrix
     unsigned int k = ((d % this->m_DomainsPerShape) * PointsPerDomain * VDimension)
       + (idx * VDimension);
     for (unsigned int i = 0; i < VDimension; i++)
     {
       this->operator()(i+k, d / this->m_DomainsPerShape) = pos[i];
     }
   }

   virtual void PositionSetEventCallback(Object *o, const EventObject &e) 
   {
     const itk::ParticlePositionSetEvent &event
       = dynamic_cast <const itk::ParticlePositionSetEvent &>(e);

     const itk::PSMParticleSystem<VDimension> *ps
       = dynamic_cast<const itk::PSMParticleSystem<VDimension> *>(o);
     const int d = event.GetDomainIndex();
     const unsigned int idx = event.GetPositionIndex();
     const typename itk::PSMParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);
     const unsigned int PointsPerDomain = ps ->GetNumberOfParticles(d);

     // Modify matrix info
     unsigned int k = ((d % this->m_DomainsPerShape) * PointsPerDomain * VDimension)
       + (idx * VDimension);    

     for (unsigned int i = 0; i < VDimension; i++)
     {
       this->operator()(i+k, d / this->m_DomainsPerShape) =
	 pos[i] - m_MeanMatrix(i+k, d/ this->m_DomainsPerShape);
     }
   }

   virtual void PositionRemoveEventCallback(Object *, const EventObject &) 
   {
     // NEED TO IMPLEMENT THIS
   }

   /** Set/Get the number of domains per shape.  This can only be safely done
       before shapes are initialized with points! */
   void SetDomainsPerShape(int i)
   { 
     this->m_DomainsPerShape = i;
   }

   int GetDomainsPerShape() const
   { 
     return this->m_DomainsPerShape;
   }

   void SetTimePointsPerIndividual(const vnl_vector<int> & v)
   {
     (this->m_TimeptsPerIndividual).set_size(v.size());

     for (int i = 0; i < v.size(); i++)
     {
       (this->m_TimeptsPerIndividual)(i) = v(i);
     }
   }

   vnl_vector<int> &GetTimePointsPerIndividual() const
   {
     return this->m_TimeptsPerIndividual;
   }

   vnl_vector<int> &GetTimePointsPerIndividual()
   {
     return this->m_TimeptsPerIndividual;
   }

   void SetNumIndividuals(int i)
   {
     this->m_NumIndividuals = i;
   }

   int GetNumIndividuals()
   {
     return this->m_NumIndividuals;
   }

   void SetExplanatory(std::vector<double> v)
   {
     ResizeExplanatory(v.size());
     for (unsigned int i = 0; i < v.size(); i++)
     {
       m_Expl[i] = v[i];
     }
   }

   void SetExplanatory(unsigned int i, double q)
   { 
     m_Expl[i] = q;
   }

   const double &GetExplanatory(unsigned int i) const
   { 
     return m_Expl[i];
   }

   double &GetExplanatory(unsigned int i)
   { 
     return m_Expl[i];
   }

   const vnl_vector<double> &GetSlope() const
   { 
     return m_Slope;
   }

   const vnl_vector<double> &GetIntercept() const
   {
     return m_Intercept;
   }

   const vnl_matrix<double> &GetSlopeRandom() const
   {
     return m_SlopeRand;
   }

   const vnl_matrix<double> &GetInterceptRandom() const
   {
     return m_InterceptRand;
   }

   void SetSlope(const std::vector<double> &v)
   {
     ResizeParameters(v.size());
     for (unsigned int i = 0; i < v.size(); i++)
     {
       m_Slope[i] = v[i];
     }    
   }

   void SetIntercept(const std::vector<double> &v)
   {
     ResizeParameters(v.size());
     for (unsigned int i = 0; i < v.size(); i++)
     {
       m_Intercept[i] = v[i];
     }
   }

   // Initialize variables 
   void Initialize()
   {
     m_Intercept.fill(0.0);
     m_Slope.fill(0.0);
     m_MeanMatrix.fill(0.0);

     m_SlopeRand.fill(0.0);
     m_InterceptRand.fill(0.0);    
   }

   virtual void BeforeIteration()
   {
     m_UpdateCounter ++;
     if (m_UpdateCounter >= m_RegressionInterval)
     {
       m_UpdateCounter = 0;
       this->EstimateParameters();
       this->UpdateMeanMatrix();
     }
   }

   void SetRegressionInterval( int i)
   {
     m_RegressionInterval = i;
   }

   int GetRegressionInterval() const
   {
     return m_RegressionInterval;
   }

   virtual void UpdateMeanMatrix();
   
   virtual void EstimateParameters();
   
  protected:
   PSMMixedEffectsShapeMatrixAttribute()
   {
     this->m_DefinedCallbacks.DomainAddEvent = true;
     this->m_DefinedCallbacks.PositionAddEvent = true;
     this->m_DefinedCallbacks.PositionSetEvent = true;
     this->m_DefinedCallbacks.PositionRemoveEvent = true;
     m_UpdateCounter = 0;
     m_RegressionInterval = 1;
     m_NumIndividuals = 1;
     m_TimeptsPerIndividual.set_size(m_NumIndividuals);
     for(int i = 0; i < m_NumIndividuals; i++)
       m_TimeptsPerIndividual(i) = 2;
   }

   virtual ~PSMMixedEffectsShapeMatrixAttribute() {};

   void PrintSelf(std::ostream& os, Indent indent) const
   {
     Superclass::PrintSelf(os,indent);
   }

 private:
   PSMMixedEffectsShapeMatrixAttribute(const Self&); //purposely not implemented
   void operator=(const Self&); //purposely not implemented

   int m_UpdateCounter;
   int m_RegressionInterval;

   // Parameters for the linear model
   vnl_vector<double> m_Intercept;
   vnl_vector<double> m_Slope;

   // The explanatory variable value for each sample (matrix column)
   vnl_vector<double> m_Expl;

   // A matrix to store the mean estimated for each explanatory variable (each sample)
   vnl_matrix<double> m_MeanMatrix;

   vnl_matrix<double> m_InterceptRand; //added: AK , random intercepts for each group
   vnl_matrix<double> m_SlopeRand; //added: AK , random slopes for each group

   int m_NumIndividuals;
   // timepoints per individual
   vnl_vector<int> m_TimeptsPerIndividual;
 };

} // end namespace

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMMixedEffectsShapeMatrixAttribute.hxx"
#endif

#endif
