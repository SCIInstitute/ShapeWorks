/*=========================================================================

  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization

  Module:    $RCSfile: glyph_pipeline.h,v $

  Date:      $Date: 2011/03/24 01:17:41 $

  Version:   $Revision: 1.2 $

  Author:    $Author: wmartin $



  Copyright (c) 2009 Scientific Computing and Imaging Institute.

  See ShapeWorksLicense.txt for details.



     This software is distributed WITHOUT ANY WARRANTY; without even 

     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 

     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#ifndef __glyph_pipeline_h

#define __glyph_pipeline_h



#ifdef _WIN32

#pragma warning(disable : 4996)

#endif



#include "vtkSphereSource.h"

#include "vtkDiskSource.h"

#include "vtkGlyph3D.h"

#include "vtkPolyDataMapper.h"

#include "vtkActor.h"

#include "vtkProperty.h"

#include "vtkPolyData.h"

#include "vtkTransformPolyDataFilter.h"

#include "itkCommand.h"

#include "itkParticleEvents.h"

#include "itkParticleSystem.h"

#include <string>

#include "itkCommand.h"

#include "vtkUnsignedLongArray.h"

#include "vtkLookupTable.h"

//#include "vtkSurfaceReconstructionFilter.h"



  

class glyph_pipeline

{

public:



  typedef itk::ParticleSystem<3>::PointType PointType; 



  

  glyph_pipeline();

  ~glyph_pipeline();



  vtkActor *output()

  { return m_actor; }



  void load_points(const char *fn);

  void write_points(const char *fn);



  void AddPointCallback(itk::Object *, const itk::EventObject &);

  void SetPointCallback(itk::Object *, const itk::EventObject &);

  void SetTransformCallback(itk::Object *, const itk::EventObject &);

  void SetPrefixTransformCallback(itk::Object *, const itk::EventObject &);

  

  void scale(double s)

  {

    m_glyphs->SetScaleFactor(s);

    m_glyphs->Update();

  }



  void transform(vtkAbstractTransform *t)

  { m_transformer->SetTransform(t); }

  

  void color(double r, double g, double b)

  { m_actor->GetProperty()->SetDiffuseColor(r,g,b); }



  void opacity(double o)

  { m_actor->GetProperty()->SetOpacity(o); }



  vtkPoints *points()

  { return m_transformer->GetOutput()->GetPoints(); }



  vtkTransformPolyDataFilter *transformer()

  { return m_transformer; }



  void lut(vtkLookupTable* l)

  {

    m_lut = l;

    m_mapper->SetLookupTable(m_lut);

  }

  

  vtkLookupTable *lut()

  { return m_lut;}



  void color_neighborhood(unsigned long int, double, itk::ParticleSystem<3> *);



  /** Set the domain whose events this glyph pipeline will respond to. */

  int MyDomain() const

  { return m_MyDomain; }

  void MyDomain(int d)

  { m_MyDomain = d;}



  /** Generate a random list of colors.  New glyphs will take on the next color

      in the list. */

  struct rgb

  {

    float r;

    float g;

    float b;

  };

  void generate_color_list(int);

  const std::vector<rgb> &get_color_list() const

  { return m_color_list; }

  void set_color_list(const std::vector<rgb> &c)

  { m_color_list = c; }



  void recolor();

protected:

  glyph_pipeline(const glyph_pipeline &o);  

  const glyph_pipeline& operator=(const glyph_pipeline &o);



  std::vector<rgb> m_color_list;

  

  vtkGlyph3D *m_glyphs;

  vtkPolyDataMapper *m_mapper;

  vtkActor *m_actor;

  vtkPolyData *m_pointset;

  vtkUnsignedLongArray *m_scalars;

  vtkSphereSource *m_sphere;

  vtkDiskSource *m_disk;

  vtkTransformPolyDataFilter *m_transformer;

  vtkPoints *m_points;

  vtkLookupTable *m_lut;

  unsigned int m_number_of_points;

  unsigned int m_MyDomain;

  

  //  vtkFloatArray *m_pcoords;

};





#endif

