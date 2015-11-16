/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: pca.h,v $
  Date:      $Date: 2011/03/24 01:17:36 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#ifndef __st_pca_h
#define __st_pca_h

#include "itkImage.h"
#include "tinyxml.h"
#include <sstream>
#include <string>
#include "tool.h"
#include "vnl/vnl_vector.h"
#include "vnl/vnl_matrix.h"
#include "vnl/vnl_diag_matrix.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"

namespace shapetools
{
/**
 * \class pca
 *
 */
template <class T, unsigned int D> 
class pca : public tool<T, D>
{
public:
  typedef T pixel_type;
  typedef itk::Image<T, D> image_type;
  
  pca(const char *fname);

  pca()  { }
  virtual ~pca() {}
  
  virtual void operator()(typename image_type::Pointer);

  /** */
  const pixel_type foreground() const
  { return m_foreground; }
  pixel_type &foreground()
  { return m_foreground; }

  /** */
  const pixel_type background() const
  { return m_background; }
  pixel_type &background()
  { return m_background; }

  /** */
  vnl_vector<double> eigenvector(unsigned int i)
  { return m_eigenvectors.get_column(i); }

  /** */
  double eigenvalue(unsigned int i)
  { return m_eigenvalues(i); }

  /** */
  const  vnl_vector<double> &mean() const
  { return m_mean; }
  
private: 
  pixel_type m_foreground;
  pixel_type m_background;

  vnl_vector<double> m_mean;
  vnl_matrix<double> m_eigenvectors;
  vnl_diag_matrix<double> m_eigenvalues;

};

 
} // end namespace 
#endif

#ifndef ST_MANUAL_INSTANTIATION
#include "pca.txx"
#endif
