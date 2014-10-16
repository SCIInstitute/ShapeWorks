/*=========================================================================
   Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
   Module:    $RCSfile: itkParticlePositionReader.txx,v $
   Date:      $Date: 2011/03/24 01:17:33 $
   Version:   $Revision: 1.2 $
   Author:    $Author: wmartin $

   Copyright (c) 2009 Scientific Computing and Imaging Institute.
   See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
   =========================================================================*/
#ifndef __itkParticlePositionReader_txx
#define __itkParticlePositionReader_txx

#include <fstream>

namespace itk
{

template <unsigned int VDimension>
void ParticlePositionReader<VDimension>::Update()
{
  int counter = 0;
  // Open the ascii file.
  std::ifstream in( m_FileName.c_str() );
  if ( !in )
  {
    itkExceptionMacro( "Could not open point file for input: " << m_FileName.c_str() );
  }

  //  in >> num_points;

  // Read all of the points, one point per line.
  while ( in )
  {
    PointType pt;
    for ( unsigned int d = 0; d < VDimension; d++ )
    {
      in >> pt[d];
    }

    m_Output.push_back( pt );
    counter++;
  }
  // this algorithm pushes the last point twice
  m_Output.pop_back();
  //  std::cout << "Read " << counter-1 << " points. " << std::endl;
  in.close();
}
}

#endif /* ifndef __itkParticlePositionReader_txx */
