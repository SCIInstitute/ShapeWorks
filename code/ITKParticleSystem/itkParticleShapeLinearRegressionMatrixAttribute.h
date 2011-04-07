/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkParticleShapeLinearRegressionMatrixAttribute.h,v $

  Date:      $Date: 2011/03/24 01:17:34 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkParticleShapeLinearRegressionMatrixAttribute_h

#define __itkParticleShapeLinearRegressionMatrixAttribute_h



#include "itkParticleShapeMatrixAttribute.h"

#include "vnl/vnl_vector.h"

#include "itkParticleSystem.h"



namespace itk

{

/** \class ParticleShapeLinearRegressionMatrixAttribute

 *

 *

 *

 */

template <class T, unsigned int VDimension>

class ITK_EXPORT ParticleShapeLinearRegressionMatrixAttribute

  : public ParticleShapeMatrixAttribute<T,VDimension>

{

public:

  /** Standard class typedefs */

  typedef T DataType;

  typedef ParticleShapeLinearRegressionMatrixAttribute Self;

  typedef ParticleShapeMatrixAttribute<T,VDimension> Superclass;

  typedef SmartPointer<Self>  Pointer;

  typedef SmartPointer<const Self>  ConstPointer;

  typedef WeakPointer<const Self>  ConstWeakPointer;

  

  /** Method for creation through the object factory. */

  itkNewMacro(Self);

  

  /** Run-time type information (and related methods). */

  itkTypeMacro(ParticleShapeLinearRegressionMatrixAttribute, ParticleShapeMatrixAttribute);

  

  

  void UpdateMeanMatrix()

  {

    // for each sample

    for (unsigned int i = 0; i < m_MeanMatrix.cols(); i++)

      {

      // compute the mean

      m_MeanMatrix.set_column(i, m_Intercept + m_Slope * m_Expl(i));

      }

  }

  

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

      should be set to true so that the ParticleSystem will know to register

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

    const itk::ParticleSystem<VDimension> *ps

      = dynamic_cast<const itk::ParticleSystem<VDimension> *>(o);

    const int d = event.GetDomainIndex();

    const unsigned int idx = event.GetPositionIndex();

    const typename itk::ParticleSystem<VDimension>::PointType pos

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

    

    // CANNOT ADD POSITION INFO UNTIL ALL POINTS PER DOMAIN IS KNOWN

    // Add position info to the matrix

    unsigned int k = ((d % this->m_DomainsPerShape) * PointsPerDomain * VDimension)

      + (idx * VDimension);

    for (unsigned int i = 0; i < VDimension; i++)

      {

      this->operator()(i+k, d / this->m_DomainsPerShape) = pos[i];

      }

    

    //   std::cout << "Row " << k << " Col " << d / this->m_DomainsPerShape << " = " << pos << std::endl;

  }

  

  virtual void PositionSetEventCallback(Object *o, const EventObject &e) 

  {

    const itk::ParticlePositionSetEvent &event

      = dynamic_cast <const itk::ParticlePositionSetEvent &>(e);

  

    const itk::ParticleSystem<VDimension> *ps

      = dynamic_cast<const itk::ParticleSystem<VDimension> *>(o);

    const int d = event.GetDomainIndex();

    const unsigned int idx = event.GetPositionIndex();

    const typename itk::ParticleSystem<VDimension>::PointType pos = ps->GetTransformedPosition(idx, d);

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

  

  virtual void PositionRemoveEventCallback(Object *, const EventObject &) 

  {

    // NEED TO IMPLEMENT THIS

  }

  

  /** Set/Get the number of domains per shape.  This can only be safely done

      before shapes are initialized with points! */

  void SetDomainsPerShape(int i)

  { this->m_DomainsPerShape = i; }

  int GetDomainsPerShape() const

  { return this->m_DomainsPerShape; }



  void SetExplanatory(std::vector<double> v)

  {

    //    std::cout << "Setting expl " << std::endl;

    ResizeExplanatory(v.size());

    for (unsigned int i = 0; i < v.size(); i++)

      {

      //      std::cout << v[i] << std::endl;

      m_Expl[i] = v[i];

      }

  }

  void SetExplanatory(unsigned int i, double q)

  { m_Expl[i] = q; }

  const double &GetExplanatory(unsigned int i) const

  { return m_Expl[i]; }

  double &GetExplanatory(unsigned int i)

  { return m_Expl[i]; }



  const vnl_vector<double> &GetSlope() const

  { return m_Slope; }

  const vnl_vector<double> &GetIntercept() const

  { return m_Intercept; }

  

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



  void EstimateParameters()

  {

    //    std::cout << "Estimating params" << std::endl;

    //    std::cout << "Explanatory: " << m_Expl << std::endl;



    vnl_matrix<double> X = *this + m_MeanMatrix;



    // Number of samples

    double n = static_cast<double>(X.cols());

    

    vnl_vector<double> sumtx = m_Expl[0] * X.get_column(0);

    vnl_vector<double> sumx = X.get_column(0);

    double sumt = m_Expl[0];

    double sumt2 = m_Expl[0] * m_Expl[0];    

    for (unsigned int k = 1; k < X.cols(); k++)      // k is the sample number

      {

      sumtx += m_Expl[k] * X.get_column(k);

      sumx  += X.get_column(k);

      sumt  += m_Expl[k];

      sumt2 += m_Expl[k] * m_Expl[k];      

      }



    m_Slope = (n * sumtx - (sumx * sumt)) / (n * sumt2 - (sumt*sumt));





    vnl_vector<double> sumbt = m_Slope * m_Expl[0];

    for (unsigned int k = 1; k < X.cols(); k++)

      {

      sumbt += m_Slope * m_Expl[k];

      }

    

    

    m_Intercept = (sumx - sumbt) / n;

    

  }

  

  // 

  void Initialize()

  {

    m_Intercept.fill(0.0);

    m_Slope.fill(0.0);

    m_MeanMatrix.fill(0.0);

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

  {    m_RegressionInterval = i;  }

  int GetRegressionInterval() const

  { return m_RegressionInterval; }

  

protected:

  ParticleShapeLinearRegressionMatrixAttribute() 

  {

    this->m_DefinedCallbacks.DomainAddEvent = true;

    this->m_DefinedCallbacks.PositionAddEvent = true;

    this->m_DefinedCallbacks.PositionSetEvent = true;

    this->m_DefinedCallbacks.PositionRemoveEvent = true;

    m_UpdateCounter = 0;

    m_RegressionInterval = 1;

  }

  virtual ~ParticleShapeLinearRegressionMatrixAttribute() {};



  void PrintSelf(std::ostream& os, Indent indent) const

  { Superclass::PrintSelf(os,indent);  }



private:

  ParticleShapeLinearRegressionMatrixAttribute(const Self&); //purposely not implemented

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

};



} // end namespace



#endif

