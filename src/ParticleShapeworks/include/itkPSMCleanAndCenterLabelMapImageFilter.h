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
#ifndef __itkPSMCleanAndCenterLabelMapImageFilter_h
#define __itkPSMCleanAndCenterLabelMapImageFilter_h

#include "itkImageToImageFilter.h"

namespace itk
{
/** \class PSMCleanAndCenterLabelMapImageFilter 
 *
 * \brief Process an input segmentation to produce an isotropically spaced
 * version with holes filled and center-of-mass at the origin.
 *
 * This filter may be used to turn a label map (segmentation) image
 * into an isotropic version with its center-of-mass at the origin. This
 * filter is intended to be used as part of a preprocessing pipeline
 * to prepare segmentations for use with any of the Particle Shape
 * Modeling filters (e.g. PSMEntropyModelFilter).
 *
 * The filter processes the input image as follows:
 * 
 * 1) The largest connected component for the specified foreground
 * component is identified and isolated.  All other pixels in the
 * image are set to the background value (default zero).
 *
 * 2) Holes are filled in the segmentation resulting from Step 1.
 *
 * 3) The image is resampled to isotropic voxels, if necessary.
 *
 * 4) The center of mass of the foreground object is computed and the
 * center of the image is transformed to that location.
 * 
 * NOTE: This filter assumes that the upper-left-hand corner of the image is
 * NOT part of the foreground.  
 *
 * WHAT ARE THE PARAMETERS?
 *
 * This filter is templated over the input image type. It produces an
 * output image of the same type.
 *
 * \ingroup PSM
 * \ingroup PSMPreprocess
 *
 * \author Josh Cates
 */
template< class TImage>
class ITK_EXPORT PSMCleanAndCenterLabelMapImageFilter:
  public ImageToImageFilter<TImage, TImage>
{
public:
  /** Standard class typedefs. */
  typedef PSMCleanAndCenterLabelMapImageFilter      Self;
  typedef ImageToImageFilter< TImage, TImage > Superclass;
  typedef SmartPointer< Self >                 Pointer;
  typedef SmartPointer< const Self >           ConstPointer;

  /** Image-type-related typedefs */
  typedef TImage ImageType;
  typedef typename ImageType::PixelType  PixelType;
  typedef typename ImageType::PointType PointType;
  typedef typename ImageType::RegionType RegionType;
  typedef typename RegionType::SizeType  SizeType;
  typedef typename RegionType::IndexType IndexType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(PSMCleanAndCenterLabelMapImageFilter, ImageToImageFilter);

  /** Dimensionality of the domain of the particle system. */
  itkStaticConstMacro(Dimension, unsigned int, TImage::ImageDimension);
  
   /** Do the work of batch-processing the input images. */
  void GenerateData();

  /** Set/Get the foreground value. This is the label of interest in
      the input images. All other values in the input images will be
      considered background and relabeled to the background value in
      the output.  You MUST set this value. Default value is 1. */
  itkSetMacro(ForegroundValue,PixelType);
  itkGetMacro(ForegroundValue,PixelType);

  /** Get/Set the background value.  This is the value to which all
      non-foreground values will be changed in the output.  The
      default value is zero. */
  itkSetMacro(BackgroundValue,PixelType);
  itkGetMacro(BackgroundValue,PixelType);

protected:
  PSMCleanAndCenterLabelMapImageFilter();
  ~PSMCleanAndCenterLabelMapImageFilter() {}
  void PrintSelf(std::ostream & os, Indent indent) const;
  
   /** Isolates the largest connected component in an image.  Pixels in this
   *  component are set to the foreground value and pixels in other components
   *  are set to the background value. */
  void IsolateLargestComponent(ImageType *) const;

  /** Fills holes in the foreground segmentation. */
  void FillHoles(ImageType *) const;

  /** Resamples the image to isotropic voxel spacing if needed. */
  void ResampleToIsotropic(ImageType *) const
  {
    return;
  }
  

  /** Translates the center-of-mass to the center of the image. Also
      modifies the image information.*/
  void Center(ImageType *);


   /** This filter must provide an implementation for
   * GenerateOutputInformation() in order to inform the pipeline
   * execution model.  The original documentation of this method is
   * below.  \sa ProcessObject::GenerateOutputInformaton()  */
  virtual void GenerateOutputInformation();

  /** This filter must provide an implementation for
   * GenerateInputRequestedRegion() in order to inform the pipeline
   * execution model.  
   * \sa ProcessObject::GenerateInputRequestedRegion()  */
  virtual void GenerateInputRequestedRegion();


private:
  PSMCleanAndCenterLabelMapImageFilter(const Self &); //purposely not implemented
  void operator=(const Self &);             //purposely not implemented

  PixelType  m_ForegroundValue;
  PixelType  m_BackgroundValue;
};
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkPSMCleanAndCenterLabelMapImageFilter.hxx"
#endif

#endif
