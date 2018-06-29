/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: itkParticleGaussianModeWriter.txx,v $
  Date:      $Date: 2011/03/24 01:17:33 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
========================================================================*/
#ifndef __itkParticleGaussianModeWriter_txx
#define __itkParticleGaussianModeWriter_txx

#include "itkParticleImageDomainWithGradients.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "itkParticlePositionWriter.h"
#include <string>

namespace itk
{

template <unsigned int VDimension>
void
ParticleGaussianModeWriter<VDimension>
::Update() const
{
  // ASSUMES ALL SHAPES HAVE SAME NUMBER OF SAMPLES
  const int num_samples = m_ShapeMatrix->cols();
  const int num_dims = m_ShapeMatrix->rows();

  // Compute means
  vnl_vector_type means(num_dims);
  for (int j = 0; j < num_dims;  j++)
   {
    double total = 0.0;
    for (int i = 0; i < num_samples; i++)
      {
      total += m_ShapeMatrix->operator()(j, i);
      }
    means(j) = total/(double)num_samples;
    }

 //Compute modes.
  vnl_matrix_type points_minus_mean(num_dims, num_samples);
  for (int j = 0; j < num_dims; j++)
    {
    for (int i = 0; i < num_samples; i++)
      {
     points_minus_mean(j, i) = m_ShapeMatrix->operator()(j, i) - means(j);
      }
    }
  
  vnl_matrix_type A = points_minus_mean.transpose()
   * points_minus_mean * (1.0/((double)(num_samples-1)));
  
  vnl_symmetric_eigensystem<double> symEigen(A);
  vnl_matrix_type vectors = points_minus_mean * symEigen.V;
  
 // normalize those eigenvectors
  for (int i = 0; i < num_samples; i++)
    {
    double total = 0.0f;
    for (int j = 0; j < num_dims; j++)
      {
      total += vectors(j, i)*vectors(j, i);
      }
    total = sqrt(total);
    
    for (int j = 0; j < num_dims; j++)
     {      vectors(j, i) = vectors(j, i) /  (total + 1.0e-15); }
    }

  // Write each domain to a separate file.  ASSUMES EACH DOMAIN HAS THE SAME
  // NUMBER OF ROWS
  int DomainsPerShape = m_ShapeMatrix->GetDomainsPerShape();
  for (unsigned int d =0; d < (unsigned int)DomainsPerShape; d++)
    {
    char fp[255];
    std::string fn = m_FileName;
    ::sprintf(fp, "domain%d", d);
    fn += fp;
    
    int rowsperdomain = num_dims / DomainsPerShape;
    int rowoffset = d * rowsperdomain;
    
    // Write means to a file
    std::vector<PointType> meanlist;
   
    // Compile list of means
    for (int i = rowoffset; i < rowoffset + rowsperdomain; i+=VDimension)
      {
      PointType p;
      for (unsigned int j = 0; j < VDimension; j++)
        {
        p[j] = means(j+i);
        }
      meanlist.push_back(p);
      }
    
    std::string tmpstr = fn + ".mean";
    typename ParticlePositionWriter<VDimension>::Pointer writer
      = ParticlePositionWriter<VDimension>::New();
    writer->SetFileName(tmpstr.c_str());
    writer->SetInput(meanlist);
    writer->Update();
    
    int modenum = 0;
    for (int mode = num_samples-1; mode > num_samples-(m_NumberOfModes+1); mode--, modenum++)
      {
      double lambda = sqrt(abs(symEigen.D(mode, mode)));
      
      for (int s = -3; s < 4; s++)
        {
        if (s == 0) continue;
        vnl_vector_type x = means + vectors.get_column(mode) * (double)s * lambda;
        
        std::vector<PointType> modelist;
        for (int i = rowoffset; i < rowoffset+ rowsperdomain; i+=VDimension)
          {
          PointType p;
          for (unsigned int j = 0; j < VDimension; j++)
            {
            p[j] = x(j+i);
            }
          modelist.push_back(p);
          }
        
        char fp[255];
        ::sprintf(fp, ".%d.%d.mode", s, modenum);
        tmpstr = fn + fp;
        writer->SetFileName(tmpstr.c_str());
        writer->SetInput(modelist);
        writer->Update();
        }
      }
    }
}

} // end namespace
#endif
