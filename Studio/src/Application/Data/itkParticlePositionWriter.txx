/*=========================================================================
   Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
   Module:    $RCSfile: itkParticlePositionWriter.txx,v $
   Date:      $Date: 2011/03/24 01:17:33 $
   Version:   $Revision: 1.2 $
   Author:    $Author: wmartin $

   Copyright (c) 2009 Scientific Computing and Imaging Institute.
   See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.
   =========================================================================*/
#ifndef __itkParticlePositionWriter_txx
#define __itkParticlePositionWriter_txx

#include <fstream>

namespace itk
{

template <unsigned int VDimension>
void ParticlePositionWriter<VDimension>::Update()
{
  // Open the output file.
  std::ofstream out( m_FileName.c_str() );

  if ( !out )
  {
    itkExceptionMacro( "Could not open point file for output: " << m_FileName.c_str() );
  }

  //  out << num_points;

  // Write points.
  for ( typename std::vector<PointType>::const_iterator it = m_Input.begin();
        it != m_Input.end(); it++ )
  {
    for ( unsigned int i = 0; i < VDimension; i++ )
    {
      out << ( *it )[i] << " ";
    }
    out << std::endl;
  }

  out.close();
}
}

#endif /* ifndef __itkParticlePositionWriter_txx */
