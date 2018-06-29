/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticlePositionMetaWriter.txx,v $
  Date:      $Date: 2009/05/06 21:49:15 $
  Version:   $Revision: 1.1.1.1 $
  Author:    $Author: cates $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __itkParticlePositionMetaWriter_txx
#define __itkParticlePositionMetaWriter_txx

#include <fstream>

namespace itk
{

template <unsigned int VDimension>
void ParticlePositionMetaWriter<VDimension>::Update()
{
  // Open the output file.
  std::ofstream out( m_FileName.c_str() );
 
  if ( !out )
    {
    itkExceptionMacro("Could not open point file for output: " << m_FileName.c_str());
    }


  // Write the meta header (why is ndims in here twice?)
  out << "ObjectType = Scene" << std::endl;
  out << "NDims = " << VDimension << std::endl;
  out << "NObjects = 1" << std::endl;
  out << "ObjectType = Mesh" << std::endl;
  out << "NDims = " << VDimension << std::endl;
  out << "BinaryData = False" << std::endl;
  out << "TransformMatrix = 1 0 0 0 1 0 0 0 1" << std::endl;
  out << "Offset = 0 0 0" << std::endl;
  out << "CenterOfRotation = 0 0 0" << std::endl;
  out << "ElementSpacing = 1 1 1" << std::endl;
  out << "PointType = MET_FLOAT" << std::endl;
  out << "PointDataType = MET_FLOAT" << std::endl;
  out << "CellDataType = MET_FLOAT" << std::endl;
  out << "NCellTypes = 1" << std::endl;
  out << "PointDim = ID x y ..." << std::endl;
  out << "NPoints = " << m_Input.size() << std::endl;
  out << "Points = "<< std::endl;
  
  // Write points.
  unsigned int k = 0;
  for (typename std::vector<PointType>::const_iterator it= m_Input.begin();
       it != m_Input.end(); it++, k++)
    {
    out << k << " ";
    for (unsigned int i = 0; i < VDimension; i++)
      {
      out << (*it)[i] << " ";
      }
    out << std::endl;
    }

  // Write bogus triangle edges.
  int numtri = (m_Input.size() -2) * 2;
  out << "CellType = TRI" << std::endl;
  out << "NCells = " <<  numtri << std::endl;
  out << "Cells = " << std::endl;

  for (k = 0; k < numtri; k++)
    {
    out << k << " 0 1 2" << std::endl;
    }
   
  out.close();
}

}

#endif
