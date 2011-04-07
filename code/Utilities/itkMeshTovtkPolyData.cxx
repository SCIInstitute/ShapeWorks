/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: itkMeshTovtkPolyData.cxx,v $

  Date:      $Date: 2011/03/24 01:17:41 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>

#include "itkMeshTovtkPolyData.h"



#ifndef vtkDoubleType

#define vtkDoubleType double

#endif



#ifndef vtkFloatingPointType

# define vtkFloatingPointType vtkFloatingPointType

typedef float vtkFloatingPointType;

#endif



itkMeshTovtkPolyData

::itkMeshTovtkPolyData()

{



  m_itkTriangleMesh = TriangleMeshType::New();

  m_Points = vtkPoints::New();

  m_PolyData = vtkPolyData::New();

  m_Polys = vtkCellArray::New();

}





itkMeshTovtkPolyData

::~itkMeshTovtkPolyData()

{

  

}



void

itkMeshTovtkPolyData

::SetInput(TriangleMeshType::Pointer mesh)

{

  m_itkTriangleMesh = mesh;

  this->ConvertitkTovtk();

}



vtkPolyData *

itkMeshTovtkPolyData

::GetOutput()

{

  return m_PolyData;

}



void

itkMeshTovtkPolyData

::ConvertitkTovtk()

{

  int numPoints =  m_itkTriangleMesh->GetNumberOfPoints();



  InputPointsContainerPointer      myPoints = m_itkTriangleMesh->GetPoints();

  InputPointsContainerIterator     points = myPoints->Begin();

  PointType point;

  

  if (numPoints == 0)

    {

      printf( "Aborting: No Points in GRID\n");

      return; 

    }



  m_Points->SetNumberOfPoints(numPoints);

  

  int idx=0;

  double vpoint[3];

  while( points != myPoints->End() ) 

    {   

    point = points.Value();

    vpoint[0]= point[0];

    vpoint[1]= point[1];

    vpoint[2]= point[2];

    m_Points->SetPoint(idx++,vpoint);

    points++;

    }



  m_PolyData->SetPoints(m_Points);



  m_Points->Delete();



  CellsContainerPointer cells = m_itkTriangleMesh->GetCells();

  CellsContainerIterator cellIt = cells->Begin();

  vtkIdType pts[3];

  while ( cellIt != cells->End() )

    {

  CellType *nextCell = cellIt->Value();

    CellType::PointIdIterator pointIt = nextCell->PointIdsBegin() ;

    PointType  p;

    int i;

    

    switch (nextCell->GetType())

      {

      case CellType::VERTEX_CELL:

      case CellType::LINE_CELL:

      case CellType::POLYGON_CELL:

        break;        

      case CellType::TRIANGLE_CELL:

        i=0;

        while (pointIt != nextCell->PointIdsEnd() ) 

        {

        pts[i++] = *pointIt++;  

        }

        m_Polys->InsertNextCell(3,pts);

        break;

      default:

        printf("something \n");

      }

    cellIt++;

    

    }

  

  m_PolyData->SetPolys(m_Polys);

  m_Polys->Delete();



}

