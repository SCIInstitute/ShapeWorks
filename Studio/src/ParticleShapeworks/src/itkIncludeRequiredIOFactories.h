/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    itkIncludeRequiredIOFactories.h
  Language:  C++
  Date:      $Date$
  Version:   $Revision$

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkTestDriverIncludeRequiredIOFactories_h
#define __itkTestDriverIncludeRequiredIOFactories_h
#include "itkGDCMImageIOFactory.h"
#include "itkMetaImageIOFactory.h"
#include "itkObjectFactoryBase.h"
#include "itkNrrdImageIOFactory.h"

/** Fixed ITK runtime error where image format is not recognized. */
void RegisterRequiredFactories(){
  itk::ObjectFactoryBase::RegisterFactory( itk::MetaImageIOFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::GDCMImageIOFactory::New() );
  itk::ObjectFactoryBase::RegisterFactory( itk::NrrdImageIOFactory::New() );
}

#endif
