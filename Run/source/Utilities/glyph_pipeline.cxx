/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: glyph_pipeline.cxx,v $
  Date:      $Date: 2011/03/24 01:17:41 $
  Version:   $Revision: 1.2 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "glyph_pipeline.h"
#include "vtkFloatArray.h"
#include "string_io.h"
#include "vtkTransform.h"
#include "vtkPointData.h"
#include <cstdlib>
#include <vector>

asdfasdf
asdf
asdf

void glyph_pipeline::SetPrefixTransformCallback(itk::Object *o, const itk::EventObject &e)
{
  // NOTE: Ignoring scale
  const itk::ParticlePrefixTransformSetEvent &event = dynamic_cast<const itk::ParticlePrefixTransformSetEvent &>(e);
  const itk::ParticleSystem<3> *ps= dynamic_cast<const itk::ParticleSystem<3> *>(o);

  unsigned int d = event.GetDomainIndex();
  if (d != m_MyDomain)
    {
    return;
    }

  vtkMatrix4x4 *m1;
  vtkTransform *t;
  //  vtkMatrix4x4 *m;
  //  vtkMatrix4x4 *m0;
  
  //  m  = vtkMatrix4x4::New();
  //  m0 = vtkMatrix4x4::New();
  m1 = vtkMatrix4x4::New();
  t  = vtkTransform::New();

  itk::ParticleSystem<3>::TransformType T = ps->GetTransform(d) * ps->GetPrefixTransform(d);
  
  m1->SetElement(0, 0, T(0,0)); // * transforms[i].scale);
  m1->SetElement(1, 0, T(1,0)); // * transforms[i].scale);
  m1->SetElement(2, 0, T(2,0)); // * transforms[i].scale);
  m1->SetElement(3, 0, 0.0);

  m1->SetElement(0, 1, T(0,1)); // * transforms[i].scale);
  m1->SetElement(1, 1, T(1,1)); // * transforms[i].scale);
  m1->SetElement(2, 1, T(2,1)); // * transforms[i].scale);
  m1->SetElement(3, 1, 0.0);
    
  m1->SetElement(0, 2, T(0,2)); // * transforms[i].scale);
  m1->SetElement(1, 2, T(1,2)); // * transforms[i].scale);
  m1->SetElement(2, 2, T(2,2)); // * transforms[i].scale);
  m1->SetElement(3, 2, 0.0);

  m1->SetElement(0, 3, T(0,3)); //transforms.translation(0));
  m1->SetElement(1, 3, T(1,3)); //transforms.translation(1));
  m1->SetElement(2, 3, T(2,3)); //transforms.translation(2));
  m1->SetElement(3, 3, 1.0);

  //  m0 =reinterpret_cast<vtkTransform *>(m_transformer->GetTransform())->GetMatrix();
      
  //  vtkMatrix4x4::Multiply4x4(m1, m0, m);

  t->SetMatrix(m1);
    
  //print final matrix
  //  std::cout << "Domain " << d << std::endl;
//    for (unsigned int r = 0; r < 4; r++)
//      {
//      for (unsigned int c = 0; c < 4; c++)
//        {
//        std::cout << m->GetElement(r,c) << " ";
//        }
//      std::cout << std::endl;
//      }
  
  m_transformer->SetTransform(t);
  //  this->output()->SetUserTransform(t);
}as

void glyph_pipeline::SetTransformCallback(itk::Object *o, const itk::EventObject &e)
{
  // NOTE: Ignoring scale
  const itk::ParticleTransformSetEvent &event = dynamic_cast<const itk::ParticleTransformSetEvent &>(e);
  const itk::ParticleSystem<3> *ps= dynamic_cast<const itk::ParticleSystem<3> *>(o);

  unsigned int d = event.GetDomainIndex();
  if (d != m_MyDomain)
    {
    return;
    }

  vtkMatrix4x4 *m1;
  vtkTransform *t;
  //  vtkMatrix4x4 *m;
  //  vtkMatrix4x4 *m0;
  
  //  m  = vtkMatrix4x4::New();
  //  m0 = vtkMatrix4x4::New();
  m1 = vtkMatrix4x4::New();
  t  = vtkTransform::New();

  itk::ParticleSystem<3>::TransformType T = ps->GetTransform(d) * ps->GetPrefixTransform(d);
  
  m1->SetElement(0, 0, T(0,0)); // * transforms[i].scale);
  m1->SetElement(1, 0, T(1,0)); // * transforms[i].scale);
  m1->SetElement(2, 0, T(2,0)); // * transforms[i].scale);
  m1->SetElement(3, 0, 0.0);

  m1->SetElement(0, 1, T(0,1)); // * transforms[i].scale);
  m1->SetElement(1, 1, T(1,1)); // * transforms[i].scale);
  m1->SetElement(2, 1, T(2,1)); // * transforms[i].scale);
  m1->SetElement(3, 1, 0.0);
    
  m1->SetElement(0, 2, T(0,2)); // * transforms[i].scale);
  m1->SetElement(1, 2, T(1,2)); // * transforms[i].scale);
  m1->SetElement(2, 2, T(2,2)); // * transforms[i].scale);
  m1->SetElement(3, 2, 0.0);

  m1->SetElement(0, 3, T(0,3)); //transforms.translation(0));
  m1->SetElement(1, 3, T(1,3)); //transforms.translation(1));
  m1->SetElement(2, 3, T(2,3)); //transforms.translation(2));
  m1->SetElement(3, 3, 1.0);

  //  m0 =reinterpret_cast<vtkTransform *>(m_transformer->GetTransform())->GetMatrix();
      
  //  vtkMatrix4x4::Multiply4x4(m1, m0, m);

  t->SetMatrix(m1);
    
  //print final matrix
  //  std::cout << "Domain " << d << std::endl;
//    for (unsigned int r = 0; r < 4; r++)
//      {
//      for (unsigned int c = 0; c < 4; c++)
//        {
//        std::cout << m->GetElement(r,c) << " ";
//        }
//      std::cout << std::endl;
//      }
  
  m_transformer->SetTransform(t);
  //  this->output()->SetUserTransform(t);
}

void glyph_pipeline::AddPointCallback(itk::Object *o, const itk::EventObject &e)
{
  const itk::ParticlePositionAddEvent &event = dynamic_cast<const itk::ParticlePositionAddEvent &>(e);
  const itk::ParticleSystem<3> *ps= dynamic_cast<const itk::ParticleSystem<3> *>(o);

  unsigned int d = event.GetDomainIndex();
  if (d != m_MyDomain)
    {
    return;
    }

  PointType pos = ps->GetPosition(event.GetPositionIndex(), event.GetDomainIndex());  
  m_points->InsertPoint(event.GetPositionIndex(), pos[0], pos[1], pos[2]);

  ((vtkUnsignedLongArray *)(m_pointset->GetPointData()->GetScalars()))->InsertValue(event.GetPositionIndex(),
                                                                                    event.GetPositionIndex());

  m_glyphs->SetRange(0.0, (double) m_number_of_points + 1);

  m_lut->SetNumberOfTableValues(m_number_of_points + 1);
  m_lut->SetTableRange(0.0, (double)m_number_of_points + 1.0);

  int colidx = m_number_of_points % m_color_list.size();
  m_lut->SetTableValue(m_number_of_points,
                       m_color_list[colidx].r,
                       m_color_list[colidx].g,
                       m_color_list[colidx].b,
                       1.0);
  
  m_mapper->SetScalarRange(0.0, (double) m_number_of_points + 1.0);
  m_mapper->SetLookupTable(m_lut);

  m_number_of_points++;
  this->m_pointset->Modified();
}

void glyph_pipeline::SetPointCallback(itk::Object *o, const itk::EventObject &e)
{
  const itk::ParticlePositionSetEvent &event = dynamic_cast<const itk::ParticlePositionSetEvent &>(e);
  const itk::ParticleSystem<3> *ps= dynamic_cast<const itk::ParticleSystem<3> *>(o);

  unsigned int d = event.GetDomainIndex();
  if (d != m_MyDomain) return;

  PointType pos = ps->GetPosition(event.GetPositionIndex(), event.GetDomainIndex());
  m_points->SetPoint(event.GetPositionIndex(), pos[0], pos[1], pos[2]);

  this->m_pointset->Modified();
}


glyph_pipeline::glyph_pipeline()
{
  m_MyDomain = 0;
  m_number_of_points = 0;
  m_glyphs = vtkGlyph3D::New();
  m_mapper = vtkPolyDataMapper::New();
  m_actor = vtkActor::New();  
  m_sphere = vtkSphereSource::New();
  m_disk = vtkDiskSource::New();
  m_pointset = vtkPolyData::New();
  m_transformer= vtkTransformPolyDataFilter::New();
  m_lut = vtkLookupTable::New();

  m_sphere->SetThetaResolution(5);
  m_sphere->SetPhiResolution(5);    
  
  vtkTransform *tmp = vtkTransform::New();
  tmp->Identity();

  m_transformer->SetTransform(tmp);
  m_transformer->SetInput(m_pointset);
  
  m_glyphs->SetSource(m_sphere->GetOutput());
  //m_glyphs->SetSource(m_disk->GetOutput());
  m_glyphs->SetInput(m_transformer->GetOutput());
  m_glyphs->ScalingOn();
  m_glyphs->ClampingOff();
  m_glyphs->SetColorModeToColorByScalar();
  m_glyphs->SetScaleModeToDataScalingOff();
  m_glyphs->SetScaleFactor(1.0);

  m_mapper->SetInputConnection(m_glyphs->GetOutputPort());
  m_mapper->ScalarVisibilityOn();

  this->generate_color_list(1024);

  //  m_mapper->InterpolateScalarsBeforeMappingOn();

  //  m_lut->SetHueRange(0.0, 1.0);
  //  m_lut->SetValueRange(1.0, 1.0);
  //  m_lut->SetSaturationRange(1.0,1.0);
  
  m_mapper->SetLookupTable(m_lut);
  
  //  m_mapper->SetScalarRange(0.0, 2000.0);
  //  m_mapper->UseLookupTableScalarRangeOn();
  //  m_mapper->ImmediateModeRenderingOff();
  //  m_mapper->SetColorModeToMapScalars();
  //  m_mapper->CreateDefaultLookupTable();

  m_actor->GetProperty()->SetSpecularColor(0.0, 1.0, 0.0);
  m_actor->GetProperty()->SetDiffuse(0.8);
  m_actor->GetProperty()->SetSpecular(0.3);
  m_actor->GetProperty()->SetSpecularPower(10.0);
  
  m_actor->SetMapper(m_mapper);
  
  // Set up the points
  // copy points into the pcoords
  m_points = vtkPoints::New();
  m_points->SetDataTypeToDouble();
  m_pointset->SetPoints(m_points);

  // Set up the scalars labeling the points.  These will be used to color the glyphs.
  m_scalars = vtkUnsignedLongArray::New();

  m_pointset->GetPointData()->SetScalars(m_scalars);


  
  tmp->Delete();
}

void glyph_pipeline::recolor()
{
  unsigned int k =0;
  for (unsigned int i = 0; i < m_lut->GetNumberOfTableValues(); i++)
    {
    m_lut->SetTableValue(i,
                         m_color_list[k].r,
                         m_color_list[k].g,
                         m_color_list[k].b,
                         1.0);
    k++;
    if (k >= m_color_list.size()) k = 0;
    }
}





void glyph_pipeline::color_neighborhood(unsigned long int selected_point,
                                        double sigma, itk::ParticleSystem<3> *ps)
{
  // Reset lookup table.
  for (unsigned int i = 0; i < m_lut->GetNumberOfTableValues(); i++)
    {
    m_lut->SetTableValue(i, 1.0, 0.0, 0.0, 1.0);
    }
  
  // If this is point 0, color all of its neighbors the same color.
  std::vector<double> weights;
  itk::ParticleSystem<3>::PointVectorType neighborhood
    = ps->FindNeighborhoodPoints(selected_point, weights, sigma, 0);
 
  for (unsigned int i = 0; i < neighborhood.size(); i++)
    {
    if (weights[i] == 1.0)
      {
      m_lut->SetTableValue(neighborhood[i].Index, 1.0, 1.0, 1.0, 1.0);
      }
    else if (weights[i] == 0.0)
      {
      m_lut->SetTableValue(neighborhood[i].Index, 0.0, 0.0, 1.0, 1.0);
      }
    else
      {
      m_lut->SetTableValue(neighborhood[i].Index, 0.0, weights[i], 0.0, 1.0);
      }
    }
  m_lut->SetTableValue(selected_point, 0.0, 0.23, 1.0, 1.0);

  this->m_pointset->Modified();

}

void glyph_pipeline::write_points(const char *fn)
{
  CS6350::string_io io;
  io.filename(fn);

  CS6350::string_list l;
  char buf[1024];

  // First record number of points
  vtkPoints *points = m_transformer->GetOutput()->GetPoints();
  const unsigned int n = points->GetNumberOfPoints();
  sprintf(buf, "%d", n);
  l.push_back(std::string(buf));
  double *p;
  
  // compile the list of points in string format
  for (unsigned int i = 0; i < n; i++)
    {
    p = points->GetPoint(i);
    sprintf(buf, "%f %f %f", p[0], p[1], p[2]);
    l.push_back(std::string(buf));
    }

  // write strings
  io.strings(l);
  io.write();
}


void glyph_pipeline::load_points(const char *fn)
{  
//   m_pcoords = vtkFloatArray::New();
//   m_pcoords->SetNumberOfComponents(3);
  
//   // Read the points
//   CS6350::string_io io;
//   io.filename(fn);
//   io.read();

//   float p[3];
//   CS6350::string_list::const_iterator it = io.strings().begin();
//   m_pcoords->SetNumberOfTuples( atoi((*it).c_str()) );  //Assumes first line is number of points.

//   it++;
//   for (unsigned int i = 0; it != io.strings().end(); it++, i++)
//    {  
//     CS6350::string_list tmp = CS6350::split_string(*it);
//     if (tmp.size() > 2)
//       {
//       p[0] = atof(tmp[0].c_str());
//       p[1] = atof(tmp[1].c_str());
//       p[2] = atof(tmp[2].c_str());
//       m_pcoords->SetTuple(i,p);
//       }
//     }

//   // copy points into the pcoords
//   m_points = vtkPoints::New();
//   m_points->SetData(m_pcoords);
//   m_pointset->SetPoints(m_points);
}

void glyph_pipeline::generate_color_list(int n)
{
  m_color_list.resize(n);
  
  for (int i = 0; i < n; i++)
    {
    float sum = 0.0;
    while (sum < 0.5)
      {
      m_color_list[i].r = (float)(rand()) / (float)(RAND_MAX);
      m_color_list[i].g = (float)(rand()) / (float)(RAND_MAX);
      m_color_list[i].b = (float)(rand()) / (float)(RAND_MAX);

      sum = sqrt(m_color_list[i].r * m_color_list[i].r +
                 m_color_list[i].g * m_color_list[i].g +
                 m_color_list[i].b * m_color_list[i].b);
      }
    }
}

glyph_pipeline::~glyph_pipeline()
{
//   m_pcoords->Delete();
//   m_points->Delete();
//   m_sphere->Delete();
//   m_glyphs->Delete();
//   m_mapper->Delete();
//   m_actor->Delete();
//   m_pointset->Delete();
//   m_transformer->Delete();
}
