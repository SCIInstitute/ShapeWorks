/*=========================================================================



  Program:   Insight Segmentation & Registration Toolkit

  Module:    $RCSfile: fltkVTKImageViewerBase.h,v $

  Language:  C++

  Date:      $Date: 2011/03/24 01:17:42 $

  Version:   $Revision: 1.2 $



  Copyright (c) 2002 Insight Consortium. All rights reserved.

  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.



=========================================================================*/

#ifndef __fltkVTKImageViewerBase_h

#define __fltkVTKImageViewerBase_h



#include "itkObject.h"

#include "itkObjectFactory.h"



namespace fltk {





class VTKImageViewerBase : public itk::Object

{

public:



   /**

   * Standard "Self" typedef.

   */

  typedef VTKImageViewerBase         Self;



  /** 

   * Smart pointer typedef support.

   */

  typedef itk::SmartPointer<Self>        Pointer;

  typedef itk::SmartPointer<const Self>  ConstPointer;





  /** 

   * Run-time type information (and related methods).

   */

  itkTypeMacro(VTKImageViewerBase, Object);



  /**

   * Method for creation through the object factory.

   */

  itkNewMacro(Self);  





protected:



  /** Constructor and Destructor */

  VTKImageViewerBase();

  virtual ~VTKImageViewerBase();





private:

  



};





} // end namespace fltk









#endif

