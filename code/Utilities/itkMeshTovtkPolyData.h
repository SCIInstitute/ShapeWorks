/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkMeshTovtkPolyData.h,v $

  Date:      $Date: 2011/03/24 01:17:41 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __itkMeshTovtkPolyData_h__

#define __itkMeshTovtkPolyData_h__



#include "vtkPoints.h"

#include "vtkCellArray.h"

#include "vtkPolyData.h"

#include "itkDefaultDynamicMeshTraits.h"

#include "itkMesh.h"

#include "itkTriangleCell.h"

#include "itkPoint.h"





/** 

  \class itkMeshTovtkPolyData

  \brief 

    \warning

  \sa 

  */



class itkMeshTovtkPolyData

{



 public:



  itkMeshTovtkPolyData( void );

  virtual ~itkMeshTovtkPolyData( void );



  typedef itk::DefaultDynamicMeshTraits<double, 3, 3,double,double> TriangleMeshTraits;

  typedef itk::Mesh<double,3, TriangleMeshTraits> TriangleMeshType;

  typedef TriangleMeshType::PointType                       PointType;

  typedef TriangleMeshType::PointsContainer                 InputPointsContainer;

  typedef InputPointsContainer::Pointer            InputPointsContainerPointer;

  typedef InputPointsContainer::Iterator           InputPointsContainerIterator;

  typedef TriangleMeshType::CellType                        CellType; 

  

  typedef TriangleMeshType::CellsContainerPointer           CellsContainerPointer;

  typedef TriangleMeshType::CellsContainerIterator          CellsContainerIterator;

  /**

  The SetInput method provides pointer to the vtkPolyData

  */

  void SetInput(TriangleMeshType::Pointer mesh);

  vtkPolyData * GetOutput();

  void ConvertitkTovtk();



  TriangleMeshType::Pointer m_itkTriangleMesh;



  vtkPoints  * m_Points;

  vtkPolyData * m_PolyData;

  vtkCellArray * m_Polys;

  

};



#endif

