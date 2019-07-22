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
#ifndef __itkPSMImplicitSurfaceImageFilter_h
#define __itkPSMImplicitSurfaceImageFilter_h

#include "itkImageToImageFilter.h"

namespace itk
{
/** \class PSMImplicitSurfaceImageFilter 
 *
 * \brief This filter generates an antialiased and smoothed signed
 * distance transform from the input label map image.
 *
 * This filter may be used to turn a label map (segmentation)
 * image into a smoothed, signed distance transform image.  The distance
 * transform image can be used as an implicit surface representation,
 * where the zero level-set in the image represents the surface.
 * Outputs from this filter are suitable for direct input into any of
 * the PSM shape modeling filters, such as PSMEntropyShapeModelingFilter.
 *
 * There are two parameters for this filter. The first parameter,
 * IsosurfaceValue, is the value of the level-set in the input volume
 * that you wish to be converted to the zero level-set in the output
 * and, thus, the implicit surface representation.  By default, this
 * value is 0.5.  The second parameter, SmoothingSigma, is the
 * standard deviation of the Gaussian blurring kernel that is applied
 * to the signed distance transform.  By default, this parameter is
 * zero, which indicates no blurring should occur.  In general, some
 * smoothing may be desireable to remove artifacts in the distance
 * transform.  Size of the blurring kernel is specified in physical
 * units of the input volume.
 *
 * This filter is templated over the input and output image types.
 * Typical input and output image types are unsigned char for the
 * input (label map) and a float image for the output (signed distance
 * transform).
 *
 * \ingroup PSM
 * \ingroup PSMPreprocess
 *
 * \author Josh Cates
 */
  template< class TInputImage, class TOutputImage>
class ITK_EXPORT PSMImplicitSurfaceImageFilter:
  public ImageToImageFilter<TInputImage, TOutputImage>
{
public:
  /** Standard class typedefs. */
  typedef PSMImplicitSurfaceImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TOutputImage > Superclass;
  typedef SmartPointer< Self >             Pointer;
  typedef SmartPointer< const Self >       ConstPointer;

  /** Image-type-related typedefs */
  typedef TInputImage  InputImageType;
  typedef TOutputImage OutputImageType;
  typedef typename OutputImageType::PixelType PixelType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMImplicitSurfaceImageFilter, ImageToImageFilter);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, TInputImage::ImageDimension);
  
   /** Do the work of batch-processing the input images. */
  void GenerateData();

  /** Set/Get the Gaussian smoothing standard deviation parameter. The
      default value is zero, which means no smoothing will take place
      by default.  The value is in given in the units of the output
      image volume. */
  itkSetMacro(SmoothingSigma,double);
  itkGetMacro(SmoothingSigma,double);

  /** Get/Set the value for the isosurface in the input image volume.
      This is the value in the input image that will be converted to
      the zero level-set in the output volume.  The default value is
      zero. */
  itkSetMacro(IsosurfaceValue,PixelType);
  itkGetMacro(IsosurfaceValue,PixelType);

protected:
  PSMImplicitSurfaceImageFilter() 
    {
      m_SmoothingSigma  = 0.0f;
      m_IsosurfaceValue = 0.5f;
    }
  ~PSMImplicitSurfaceImageFilter() {}
  void PrintSelf(std::ostream & os, Indent indent) const;

private:
  PSMImplicitSurfaceImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);             //purposely not implemented

  double    m_SmoothingSigma;
  PixelType m_IsosurfaceValue; 
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMImplicitSurfaceImageFilter.hxx"
#endif

#endif
