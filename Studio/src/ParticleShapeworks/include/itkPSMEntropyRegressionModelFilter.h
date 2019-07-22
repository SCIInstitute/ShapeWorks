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
#ifndef __itkPSMEntropyRegressionModelFilter_h
#define __itkPSMEntropyRegressionModelFilter_h

#include "itkPSMEntropyModelFilter.h"
#include "itkPSMRegressionShapeMatrixAttribute.h"

namespace itk
{
  
/** \class PSMEntropyRegressionModelFilter
 *
 * \brief
 *
 * This class decorates the base PSMEntropyModelFilter class with some
 * additional methods for setting explanatory variables for the
 * regression computation and retrieving regression model paramters.
 * 
 * \ingroup PSM 
 * \ingroup PSMModelingFilters
 * \author Josh Cates
 */
template <class TImage, class TShapeMatrix = PSMRegressionShapeMatrixAttribute<double, TImage::ImageDimension> >
class ITK_EXPORT PSMEntropyRegressionModelFilter
  : public PSMEntropyModelFilter<TImage,TShapeMatrix>
{
 public:
 /** Standard class typedefs. */
 typedef PSMEntropyRegressionModelFilter  Self;
 typedef PSMEntropyModelFilter<TImage, TShapeMatrix> Superclass;
 typedef SmartPointer<Self>   Pointer;
 typedef SmartPointer<const Self>  ConstPointer;
 
 /** Dimensionality of the domain of the particle system. */
 itkStaticConstMacro(Dimension, unsigned int, TImage::ImageDimension);
 
 /** Type of the particle system */
 typedef typename Superclass::ParticleSystemType ParticleSystemType;
 
 /** The type of the Shape Matrix, which defines optimization behavior. */
 typedef TShapeMatrix ShapeMatrixType;

 /** Method for creation through the object factory. */
 itkNewMacro(Self);
 
 /** Run-time type information (and related methods). */
 itkTypeMacro(PSMEntropyRegressionModelFilter, PSMEntropyModelFilter);
 
 /** Type of the input/output image. */
 typedef typename Superclass::ImageType ImageType;

  /** Expose the point type */
 typedef typename Superclass::PointType PointType;

  /** Type of the optimizer */
 typedef typename Superclass::OptimizerType OptimizerType;

 /** Set/Get the explanatory variables */
 void SetVariables(const std::vector<double> &v)
 {
   this->GetShapeMatrix()->SetVariables(v);
 }
 const std::vector<double> &GetVariables() const
 {
   return this->GetShapeMatrix()->GetVariables();
 }

protected:
 PSMEntropyRegressionModelFilter() {}
 virtual ~PSMEntropyRegressionModelFilter() {}
 
 void PrintSelf(std::ostream& os, Indent indent) const
 {
   Superclass::PrintSelf(os, indent);
  }

private:
  PSMEntropyRegressionModelFilter(const Self&); //purposely not implemented
  void operator=(const Self&);        //purposely not implemented
};

} // end namespace itk

#endif
