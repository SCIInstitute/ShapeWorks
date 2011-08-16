/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: icp.h,v $
  Date:      $Date: 2011/03/23 22:40:11 $
  Version:   $Revision: 1.1 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_icp_h
#define __st_icp_h

#include "itkImage.h"
#include "itkRigid3DTransform.h"
#include "param.h"
#include "tool.h"

namespace shapetools
{
/**
 * \class icp
 *
 *
 */

template <typename ITK_Exporter, typename VTK_Importer>
void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

/**
 * This function will connect the given vtkImageExport filter to
 * the given itk::VTKImageImport filter.
 */
template <typename VTK_Exporter, typename ITK_Importer>
void ConnectPipelines(VTK_Exporter* exporter, ITK_Importer importer)
{
  importer->SetUpdateInformationCallback(exporter->GetUpdateInformationCallback());
  importer->SetPipelineModifiedCallback(exporter->GetPipelineModifiedCallback());
  importer->SetWholeExtentCallback(exporter->GetWholeExtentCallback());
  importer->SetSpacingCallback(exporter->GetSpacingCallback());
  importer->SetOriginCallback(exporter->GetOriginCallback());
  importer->SetScalarTypeCallback(exporter->GetScalarTypeCallback());
  importer->SetNumberOfComponentsCallback(exporter->GetNumberOfComponentsCallback());
  importer->SetPropagateUpdateExtentCallback(exporter->GetPropagateUpdateExtentCallback());
  importer->SetUpdateDataCallback(exporter->GetUpdateDataCallback());
  importer->SetDataExtentCallback(exporter->GetDataExtentCallback());
  importer->SetBufferPointerCallback(exporter->GetBufferPointerCallback());
  importer->SetCallbackUserData(exporter->GetCallbackUserData());
}

template <class T, unsigned int D> 
class icp : public batchtool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  
  icp(param::parameterFile &);
  icp() { m_iterations = 50; }
  virtual ~icp() {}
  
  virtual void operator()();

  /** */
  int iterations() const
  { return m_iterations; }
  int &iterations()
  { return m_iterations; }
  
private: 
  int m_iterations;


  template <class TScalarType>
  class Rigid3DTransformSurrogate : public itk::Rigid3DTransform < TScalarType >
  {
  public:
	  /** Standard class typedefs. */
	  typedef Rigid3DTransformSurrogate        Self;
	  typedef itk::Rigid3DTransform< TScalarType >  Superclass;
	  typedef itk::SmartPointer< Self >        Pointer;
	  typedef itk::SmartPointer< const Self >  ConstPointer;

	  itkNewMacro(Self);

  private:
	  Rigid3DTransformSurrogate() {}
	  ~Rigid3DTransformSurrogate() {}
  };


  //template <typename ITK_Exporter, typename VTK_Importer>
  //void ConnectPipelines(ITK_Exporter exporter, VTK_Importer* importer);

  //template <typename VTK_Exporter, typename ITK_Importer>
  //void ConnectPipelines(VTK_Exporter* exporter, ITK_Importer importer);

  
};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "icp.txx"
#endif
