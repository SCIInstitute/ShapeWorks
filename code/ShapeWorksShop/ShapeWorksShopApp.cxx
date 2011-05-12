/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ShapeWorksShopApp.cxx,v $
  Date:      $Date: 2011/03/24 01:17:38 $
  Version:   $Revision: 1.3 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "ShapeWorksShopApp.h"
#include "itkImageFileReader.h"
#include "itkMultiThreader.h"
#include "param.h"
#include "itkMacro.h"
#include "filenameFactory.h"
#include <vector>
#include "object_reader.h"
#include "object_writer.h"
#include "itkZeroCrossingImageFilter.h"
#include "itkImageRegionIteratorWithIndex.h"

void ShapeWorksShopApp::AddSinglePoint()
{
  typedef itk::ParticleSystem<3> ParticleSystemType;
  typedef ParticleSystemType::PointType PointType;
  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++)
    {
    itk::ZeroCrossingImageFilter<ImageType, ImageType>::Pointer zc =
      itk::ZeroCrossingImageFilter<ImageType, ImageType>::New();
    zc->SetInput( dynamic_cast<itk::ParticleImageDomain<float, 3> *>(
                       m_Sampler->GetParticleSystem()->GetDomain(i))->GetImage());
    zc->Update();
    itk::ImageRegionConstIteratorWithIndex<ImageType> it(zc->GetOutput(),
                                           zc->GetOutput()->GetRequestedRegion());
    bool done = false;
    //    for (; !it.IsAtEnd() && done == false; ++it)
    for (it.GoToReverseBegin(); !it.IsAtReverseEnd() && done == false; --it)
      {
      if (it.Get() == 1.0)
        {
        PointType pos;
        dynamic_cast<itk::ParticleImageDomain<float, 3> *>(
                     m_Sampler->GetParticleSystem()->GetDomain(i))
                    ->GetImage()->TransformIndexToPhysicalPoint(it.GetIndex(), pos);
        done = true;
        try
          {
          m_Sampler->GetParticleSystem()->AddPosition(pos, i);
          }
        catch(itk::ExceptionObject &)
          {
          done = false;
          }
        }
      }
    }
}

void ShapeWorksShopApp::WriteParameters()
{
  std::string slopename     = std::string( this->pointfile_prefix_input->value() ) + std::string(".slope");
  std::string interceptname = std::string( this->pointfile_prefix_input->value() ) + std::string(".intercept");

  std::cout << "writing " << slopename << std::endl;
  std::cout << "writing " << interceptname << std::endl;

  std::vector< double > slope;
  vnl_vector<double> slopevec = dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
      (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())
    ->GetSlope();


 for (unsigned int i = 0; i < slopevec.size(); i++)
    {    slope.push_back(slopevec[i]);    }
   
  object_writer< double > writer;
  writer.SetFileName(slopename);
  writer.SetInput(slope);
  writer.Update();

  
  std::vector< double > intercept;
  vnl_vector<double> interceptvec = dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
    (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())
    ->GetIntercept();
  
  for (unsigned int i = 0; i < slopevec.size(); i++)
    {    intercept.push_back(interceptvec[i]);    }
   
  object_writer< double > writer2;
  writer2.SetFileName(interceptname);
  writer2.SetInput(intercept);
  writer2.Update();

}


void ShapeWorksShopApp::WriteTransformFile() const
{
  std::vector< itk::ParticleSystem<3>::TransformType > tlist;

  for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
       i++)
   {
    tlist.push_back(m_Sampler->GetParticleSystem()->GetTransform(i));
    }

  object_writer< itk::ParticleSystem<3>::TransformType > writer;
  writer.SetFileName(this->transform_filename_input->value());
  writer.SetInput(tlist);
  writer.Update();
}

void ShapeWorksShopApp::ReadTransformFile()
{
  m_Sampler->ReadTransforms();

//   object_reader< itk::ParticleSystem<3>::TransformType > reader;
//   reader.SetFileName(this->transform_filename_input->value());
//   reader.Update();

//   for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
//        i++)
//     {
//     std::cout << "Transform " << i << std::endl << reader.GetOutput()[i] << std::endl;
//     m_Sampler->GetParticleSystem()->SetTransform(i, reader.GetOutput()[i]);
//     }
// }
// void ShapeWorksShopApp::ReadPrefixTransformFile(const std::string &fn)
// {
//   object_reader< itk::ParticleSystem<3>::TransformType > reader;
//   reader.SetFileName(fn.c_str());
//   reader.Update();

//   for (unsigned int i = 0; i < m_Sampler->GetParticleSystem()->GetNumberOfDomains();
//        i++)
//     {
//     std::cout << "Prefix Transform " << i << std::endl << reader.GetOutput()[i] << std::endl;
//     m_Sampler->GetParticleSystem()->SetPrefixTransform(i, reader.GetOutput()[i]);
//     }
// }
}

void ShapeWorksShopApp::WriteVTKMesh()
{
  this->m_IsosurfacePipelines[(int)(viewer1_domain_spinner->value())]
    ->write_vtk_mesh(pointfile_prefix_input->value());
}

void ShapeWorksShopApp::AutoInitialize()
{
  // Won't work with one domain.
  //  if (m_Sampler->GetParticleSystem()->GetNumberOfDomains() == 1) return;
  
  this->auto_initialize_button->deactivate();
  this->optimize_start_button->deactivate();
  this->optimize_stop();
  
  this->SetNumericalParameter();
  m_Threader->SetNumberOfThreads(2);
  m_ProcessingThread =  m_Threader->SpawnThread(this->auto_init_callback, this);
  
  this->auto_initialize_button->activate();
  this->optimize_start_button->activate();
}

ITK_THREAD_RETURN_TYPE 
ShapeWorksShopApp
::auto_init_callback( void *arg )
{
  ShapeWorksShopApp *me = static_cast<ShapeWorksShopApp *>(((
                                     itk::MultiThreader::ThreadInfoStruct *)(arg))->UserData);
    
  // Split and optimize until the target number of particles is reached.
  unsigned int N = 0;
  while ( me->GetSampler()->GetParticleSystem()->GetNumberOfParticles(0)
          < (unsigned int)( me->init_count_spinner->value()) ) 
    {
    // Cancel the process?
    if (me->cancel_auto_initialize_button->value() == 1)
      {
      me->cancel_auto_initialize_button->value(0);
      return ITK_THREAD_RETURN_VALUE;
      }

    me->GetSampler()->GetOptimizer()->SetMaximumNumberOfIterations(
                                          (unsigned int)(me->split_interval_spinner->value()));
    me->GetSampler()->GetOptimizer()->SetNumberOfIterations(0);
    me->GetSampler()->GetParticleSystem()->SplitAllParticles(me->split_radius_spinner->value());
    me->SetParticleCounter();

    me->GetSampler()->Modified();
    me->GetSampler()->Update(); 
    }

  return ITK_THREAD_RETURN_VALUE;
}

void ShapeWorksShopApp::WritePointFiles()
{
  typedef  itk::MaximumEntropyCorrespondenceSampler<ImageType>::PointType PointType;
  const int n = m_Sampler->GetParticleSystem()->GetNumberOfDomains();

  // Write points in both local and global coordinate system
  filenameFactory fn;
  fn.number_of_files(n);
  fn.prefix(pointfile_prefix_input->value());
  fn.file_format("lpts");

  filenameFactory fnw;
  fnw.number_of_files(n);
  fnw.prefix(pointfile_prefix_input->value());
  fnw.file_format("wpts");
  int counter;
  for (int i = 0; i < n; i++)
    {
    counter = 0;
    std::ofstream out( fn.filename(i).c_str() );
    std::ofstream outw( fnw.filename(i).c_str() );

    std::cout << "Writing " << fnw.filename(i);
    
    if ( !out || !outw )
      { 
      throw param::Exception("EnsembleSystem()::Error opening output file");
      }
    
    for (int j = 0; j < m_Sampler->GetParticleSystem()->GetNumberOfParticles(i); j++ )
      {
      PointType pos = m_Sampler->GetParticleSystem()->GetPosition(j, i);
      PointType wpos = m_Sampler->GetParticleSystem()->GetTransformedPosition(j, i);
      
      for (unsigned int k = 0; k < 3; k++)
        {        out << pos[k] << " ";        }
      out << std::endl;

      for (unsigned int k = 0; k < 3; k++)
        {        outw << wpos[k] << " ";        }
      outw << std::endl;
      counter ++;
      } // end for points
    
    out.close();
    outw.close();
    } // end for files

  std::cout << " with " << counter << "points" << std::endl;
  }

void ShapeWorksShopApp::ViewerSelectDomain()
{
  m_Renderer1->RemoveActor( m_IsosurfacePipelines[m_Viewer1Domain]->output() );
  m_Renderer1->RemoveActor( m_GlyphPipelines[m_Viewer1Domain]->output());

  if (m_ShowSpheres)
    {
    for (unsigned int i = 0; i < m_SpheresPerDomain; i++)
      {
      m_Renderer1->RemoveActor( m_SphereWidgetPipelines[m_Viewer1Domain][i]->output() );
      }
    }
  
  if (m_ShowCuttingPlanes)
    { m_Renderer1->RemoveActor( m_PlaneWidgetPipelines[m_Viewer1Domain]->output() ); }
  
  m_Renderer1->AddActor( m_IsosurfacePipelines[(int)(this->viewer1_domain_spinner->value())]->output() );
  m_Renderer1->AddActor( m_GlyphPipelines[(int)(this->viewer1_domain_spinner->value())]->output());

  if (m_ShowSpheres)
    {
    for (unsigned int i = 0; i < m_SpheresPerDomain; i++)
      {
      m_Renderer1->AddActor( m_SphereWidgetPipelines[(int)(this->viewer1_domain_spinner->value())][i]->output() );
      }
    }
  
  if (m_ShowCuttingPlanes)
    {
    m_Renderer1->AddActor( m_PlaneWidgetPipelines[(int)(this->viewer1_domain_spinner->value())]->output() );
    }
  
  m_Viewer1Domain = (int)(this->viewer1_domain_spinner->value());
  //  m_RenderWindow1->Render();
  //  m_Viewer1->Render();
  
  //  m_Renderer2->RemoveActor( m_IsosurfacePipelines[m_Viewer2Domain]->output() );
  //  m_Renderer2->RemoveActor( m_GlyphPipelines[m_Viewer2Domain]->output());
  //  m_Renderer2->AddActor( m_IsosurfacePipelines[(int)(this->viewer2_domain_spinner->value())]->output() );
  //  m_Renderer2->AddActor( m_GlyphPipelines[(int)(this->viewer2_domain_spinner->value())]->output());
  
  //  m_Viewer2Domain = (int)(this->viewer2_domain_spinner->value());
  //m_RenderWindow2->Render();
  //  m_Viewer2->Render();

  if (m_Sampler->GetParticleSystem()->GetDomainFlag((int)(this->viewer1_domain_spinner->value())) == true)
    {
    this->toggle_domain_lock_button->value(1);
    }
  else 
    {
    this->toggle_domain_lock_button->value(0);
    }
}

ShapeWorksShopApp::ShapeWorksShopApp(const char *fn)
{
  m_ShowCuttingPlanes = false;
  m_ShowSpheres = false;
  m_SpheresPerDomain = 0;
  m_CheckpointCounter = 0;
  // Initialize multithreading.
  m_Threader = itk::MultiThreader::New();
  m_ProcessingThread = -1;
  // Initialize sampler/particle system
  m_Sampler  = itk::MaximumEntropyCorrespondenceSampler<ImageType>::New();
  
  m_Viewer1Domain = 0;
  //m_Viewer2Domain = 0;
  viewer1_domain_spinner->value(0);
  viewer2_domain_spinner->value(0);
  rho_spinner->value(0.0);
  
  // Read each filename and add its image domain
  param::parameterFile pf(fn);
  bool ok = true;
  std::string shape_file;
  std::string point_file;
#ifdef SW_USE_MESH
  std::string mesh_file;
#endif
  std::string attribute_file;

  this->pointfile_prefix_input->value("checkpoint");
  this->transform_filename_input->value("checkpoint_trans");

  // The domains per shape MUST be initialized FIRST!
  int tmpint;
  PARAMSET(pf, tmpint, "domains_per_shape", 0, ok, 1);
  std::cout << "Domains per shape = " << tmpint << std::endl;
  m_Sampler->SetDomainsPerShape(tmpint);

  // Set up the procrustes registration object.
  m_Procrustes = itk::ParticleProcrustesRegistration<3>::New();
  m_Procrustes->SetParticleSystem(m_Sampler->GetParticleSystem());
  m_Procrustes->SetDomainsPerShape(tmpint);

  
  // Read fixed scales if present
  std::vector<double> fs;
  double stemp;
  ok = true;
  for (unsigned int i = 0; ok == true; i++)
    {
    PARAMSET(pf, stemp, "fixed_scales", i, ok, 1.0);
    if (ok == true) { fs.push_back(stemp);}
    }
  if (fs.size() > 0)
    {
    m_Procrustes->SetFixedScales(fs);
    }
  // SCALE ON OR OFF -- this is also a toggle button in the GUI
  //m_Procrustes->ScalingOff();
  m_Procrustes->ScalingOn();
  this->use_procrustes_scaling_button->value(1);

  // Read explanatory variables if present
  std::vector<double> evars;
  double etmp;
  ok = true;
  for (unsigned int i = 0; ok == true; i++)
    {
    PARAMSET(pf, etmp, "explanatory_variable", i, ok, 1.0);
    if (ok == true) { evars.push_back(etmp);}
    }

  dynamic_cast<itk::ParticleShapeLinearRegressionMatrixAttribute<double,3> *>
    (m_Sampler->GetEnsembleRegressionEntropyFunction()->GetShapeMatrix())
    ->SetExplanatory(evars);
  
 
  
  ok = true;
  int i=0;
  while (ok == true)
    {
    // First load the surface image data.
    PARAMSET(pf, shape_file, "inputs", i, ok, "");
    if (i==0 && ok != true)
      {
      std::cerr << "No input files have been specified" << std::endl;
      throw 1;
      }
    if (ok == true)
      {
      std::cout << "Read file " << shape_file << std::endl;
      // Read surface image data.
      itk::ImageFileReader<ImageType>::Pointer reader = itk::ImageFileReader<ImageType>::New();
      reader->SetFileName(shape_file.c_str());
      reader->UpdateLargestPossibleRegion();
      m_Sampler->SetInput(i, reader->GetOutput()); // set the ith input
      
      // Use the first loaded image to set some numerical constants
      unsigned int size[3];
      int index[3];
      double origin[3];
      unsigned int maxsz = 0;
      
      double spacing = reader->GetOutput()->GetSpacing()[0];
        // assume isotropic
      for (unsigned int D = 0; D < 3; D++)
        {
        size[D] = reader->GetOutput()->GetRequestedRegion().GetSize()[D];
        index[D] = reader->GetOutput()->GetRequestedRegion().GetIndex()[D];
        origin[D] = reader->GetOutput()->GetOrigin()[D];
        if (size[D] > maxsz) maxsz = size[D];
        }
      double min = 5.0 * spacing;
      double max = (static_cast<double>(maxsz) - 1.0) * spacing;
      
      this->glyph_scale_spinner->value(spacing * 0.5);
      //      std::cout << "SetMinimum = " << min << std::endl;
      
      
      if (i == 0)
        {
        std::string s = "Spacing: ";
        s += toStr<double>(spacing);
        s += "\nSize: ";
        s += toStr<int>(size[0]) + "x" + toStr<int>(size[1]) + "x" + toStr<int>(size[2]);
        
        s += "\nExtent: (" + toStr<double>(origin[0]) + ", "
          + toStr<int>(origin[1]) + ", "
          
          + toStr<int>(origin[2]) + ") - ("
          + toStr<int>(origin[0] + (double)size[0]) + ", "
          + toStr<int>(origin[1] + (double)size[1]) + ", "
          + toStr<int>(origin[2] + (double)size[2]) + ")";
        
        s += "\nIndex: (" + toStr<int>(index[0]) + ", " + toStr<int>(index[1])
          + ", " + toStr<int>(index[2]) + ")";
        
        s += "\nDomains per shape = " + toStr<int>(tmpint);
        
        this->image_information->buffer( new Fl_Text_Buffer(2048) );
        this->image_information->insert(s.c_str());
        }
      
      // Read sphere data if present
      //      bool sph = true;
      bool cp = true;
      double cpa;
      m_SpheresPerDomain = 0;
      PARAMSET(pf, m_SpheresPerDomain, "spheres_per_domain", 0, cp, 0);
      for (unsigned int j = 0; j < m_SpheresPerDomain; j++)
        {
        m_ShowSpheres = true;
        vnl_vector_fixed<double,3> vec;

        double rad;
        int idx = i*m_SpheresPerDomain*3;
        PARAMSET(pf, rad, "sphere_radii", i*m_SpheresPerDomain + j, cp, 0.0);
        std::cout << "domain " << i << " sphere " << j
                  << " radius = " << rad << " center = (";

        PARAMSET(pf, cpa, "sphere_centers", idx + j*3 , cp, 0.0);
        std::cout << cpa << ", ";
        vec[0] = cpa;

        PARAMSET(pf, cpa, "sphere_centers", idx + j*3 + 1, cp, 0.0);
        std::cout << cpa << ", ";
        vec[1] = cpa;
        
        PARAMSET(pf, cpa, "sphere_centers", idx + j*3 + 2, cp, 0.0);
        std::cout << cpa << ") " << std::endl;
        vec[2] = cpa;        
        
        if (cp == false)
          {
          std::cerr << "WARNING: ERROR READING THE SPHERE INFORMATION" << std::endl;
          }
        m_Sampler->AddSphere(i, vec,rad);

        // Set up a sphere widget for this domain
        if (m_SphereWidgetPipelines.size() <= i)
          {
          std::cout << "Adding new sphere viz list for " << i << std::endl;
          m_SphereWidgetPipelines.push_back( std::vector< sphere_widget_pipeline * >());
          }

        std::cout << "Adding new sphere pipeline " << j << std::endl;
        m_SphereWidgetPipelines[i].push_back(new sphere_widget_pipeline);
        m_SphereWidgetPipelines[i][j]->color(0.0, 0.4, 0.2);
        m_SphereWidgetPipelines[i][j]->opacity(0.5);
        m_SphereWidgetPipelines[i][j]->MyDomain(i);
        m_SphereWidgetPipelines[i][j]->SetRadius(rad);
        m_SphereWidgetPipelines[i][j]->SetCenter(vec);
        //        m_SphereWidgetPipelines[i][j]->SetResolution(10,10);
                
        itk::MemberCommand< sphere_widget_pipeline >::Pointer tmpcmd84
          = itk::MemberCommand< sphere_widget_pipeline >::New();
        itk::MemberCommand< sphere_widget_pipeline >::Pointer tmpcmd814
          = itk::MemberCommand< sphere_widget_pipeline >::New();
        tmpcmd84->SetCallbackFunction( m_SphereWidgetPipelines[i][j], &sphere_widget_pipeline::SetTransformCallback);
        tmpcmd814->SetCallbackFunction( m_SphereWidgetPipelines[i][j], &sphere_widget_pipeline::SetPrefixTransformCallback);
        m_Sampler->GetParticleSystem()->AddObserver(itk::ParticleTransformSetEvent(), tmpcmd84);
        m_Sampler->GetParticleSystem()->AddObserver(itk::ParticlePrefixTransformSetEvent(), tmpcmd814);        
        }

      // Read cutting plane data if present
      cp = true;
      PARAMSET(pf, cpa, "cutting_planes", i * 9, cp, 0.0);
      std::cout << "cp = " << cp << std::endl;
      if (cp == true)
        {
        vnl_vector_fixed<double,3> a,b,c;
        m_ShowCuttingPlanes = true;

        a[0] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 1, cp, 0.0);
        if (cp == true) a[1] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 2, cp, 0.0);
        if (cp == true) a[2] = cpa;
        
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 3, cp, 0.0);
        if (cp == true) b[0] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 4, cp, 0.0);
        if (cp == true) b[1] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 5, cp, 1.0);
        if (cp == true) b[2] = cpa;
        
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 6, cp, 1.0);
        if (cp == true) c[0] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 7, cp, 0.0);
        if (cp == true) c[1] = cpa;
        PARAMSET(pf, cpa, "cutting_planes", (i * 9) + 8, cp, 0.0);
        if (cp == true) c[2] = cpa;

        if (cp == false)
         {
         std::cerr << "ERROR: Incomplete cutting plane data!" << std::endl;
          }

        std::cout << "ShapeWorksShopApp-> Setting Cutting Plane "
                  << i << " (" << a << ") (" << b << ") (" << c << ")"<< std::endl;

        m_Sampler->SetCuttingPlane(i,a,b,c);
   
        // Set up a plane widget for this domain
        m_PlaneWidgetPipelines.push_back(new plane_widget_pipeline);

        double extent = spacing * size[0];
        //     m_PlaneWidgetPipelines[i]->SetPoints(a, b*extent, c*extent);
        m_PlaneWidgetPipelines[i]->SetPoints(a, b, c, extent,extent);
        //        m_PlaneWidgetPipelines[i]->SetCenter();
        m_PlaneWidgetPipelines[i]->color(0.0, 0.4, 0.2);
        m_PlaneWidgetPipelines[i]->opacity(0.5);
        m_PlaneWidgetPipelines[i]->MyDomain(i);
        //        m_PlaneWidgetPipelines[i]->SetResolution(10,10);


        itk::MemberCommand< plane_widget_pipeline >::Pointer tmpcmd84
          = itk::MemberCommand< plane_widget_pipeline >::New();
        itk::MemberCommand< plane_widget_pipeline >::Pointer tmpcmd814
          = itk::MemberCommand< plane_widget_pipeline >::New();
        tmpcmd84->SetCallbackFunction( m_PlaneWidgetPipelines[i], &plane_widget_pipeline::SetTransformCallback);
        tmpcmd814->SetCallbackFunction( m_PlaneWidgetPipelines[i], &plane_widget_pipeline::SetPrefixTransformCallback);
        m_Sampler->GetParticleSystem()->AddObserver(itk::ParticleTransformSetEvent(), tmpcmd84);
        m_Sampler->GetParticleSystem()->AddObserver(itk::ParticlePrefixTransformSetEvent(), tmpcmd814);      



        }


      
      // Set up an isosurface pipeline for this surface data
      m_IsosurfacePipelines.push_back(new isosurface_pipeline());
      m_IsosurfacePipelines[i]->filename(shape_file.c_str());
      m_IsosurfacePipelines[i]->color(0.9, 0.7, 0.2);
      m_IsosurfacePipelines[i]->opacity(1.0);

      m_IsosurfacePipelines[i]->MyDomain(i);
      itk::MemberCommand< isosurface_pipeline >::Pointer tmpcmd4
        = itk::MemberCommand< isosurface_pipeline >::New();
      itk::MemberCommand< isosurface_pipeline >::Pointer tmpcmd14
        = itk::MemberCommand< isosurface_pipeline >::New();
      tmpcmd4->SetCallbackFunction( m_IsosurfacePipelines[i], &isosurface_pipeline::SetTransformCallback);
      tmpcmd14->SetCallbackFunction( m_IsosurfacePipelines[i], &isosurface_pipeline::SetPrefixTransformCallback);
      m_Sampler->GetParticleSystem()->AddObserver(itk::ParticleTransformSetEvent(), tmpcmd4);
      m_Sampler->GetParticleSystem()->AddObserver(itk::ParticlePrefixTransformSetEvent(), tmpcmd14);      
      
      // Set up a glyph pipeline and register it as an observer of the
      // particle system
      m_GlyphPipelines.push_back(new glyph_pipeline());
      m_GlyphPipelines[i]->MyDomain(i);
      itk::MemberCommand< glyph_pipeline >::Pointer tmpcmd
        = itk::MemberCommand< glyph_pipeline >::New();
      tmpcmd->SetCallbackFunction(m_GlyphPipelines[i], &glyph_pipeline::AddPointCallback);
      itk::MemberCommand< glyph_pipeline >::Pointer tmpcmd2
        = itk::MemberCommand< glyph_pipeline >::New();
      tmpcmd2->SetCallbackFunction( m_GlyphPipelines[i], &glyph_pipeline::SetPointCallback);
      itk::MemberCommand< glyph_pipeline >::Pointer tmpcmd3
        = itk::MemberCommand< glyph_pipeline >::New();
      itk::MemberCommand< glyph_pipeline >::Pointer tmpcmd13
        = itk::MemberCommand< glyph_pipeline >::New();
      tmpcmd3->SetCallbackFunction( m_GlyphPipelines[i], &glyph_pipeline::SetTransformCallback);
      tmpcmd13->SetCallbackFunction( m_GlyphPipelines[i], &glyph_pipeline::SetPrefixTransformCallback);
      m_Sampler->GetParticleSystem()->AddObserver(itk::ParticlePositionAddEvent(), tmpcmd);
      m_Sampler->GetParticleSystem()->AddObserver(itk::ParticlePositionSetEvent(), tmpcmd2);
      m_Sampler->GetParticleSystem()->AddObserver(itk::ParticleTransformSetEvent(), tmpcmd3);
      m_Sampler->GetParticleSystem()->AddObserver(itk::ParticlePrefixTransformSetEvent(), tmpcmd13);
      m_GlyphPipelines[i]->scale(reader->GetOutput()->GetSpacing()[0] * 0.5);

      // Generate a random color map for the first glyph set, and set all
      // subsequent glyph sets to use the same color map.
      if (i == 0)
        {
        m_GlyphPipelines[0]->generate_color_list(1024);
        }
      else
        {
        m_GlyphPipelines[i]->set_color_list(m_GlyphPipelines[0]->get_color_list());
        }

      m_GlyphPipelines[i]->scale(glyph_scale_spinner->value());
      // Tell the sampler about the corresponding list of points.
      bool ok1 = true;
      PARAMSET(pf, point_file, "point_files", i, ok1, "");
      if (ok1 == true)
        {
        std::cout << "Point file = " << point_file << std::endl;
        m_Sampler->SetPointsFile(i, point_file);
        }
      //      else
      //        {
      //        std::cerr << "Missing a point file for shape file " << shape_file << std::endl;
      //        }
      
#ifdef SW_USE_MESH
      bool ok11 = true;
      PARAMSET(pf, mesh_file, "mesh_files", i, ok11, "");
      if (ok11 == true)
        {
        std::cout << "Mesh file = " << mesh_file << std::endl;
        m_Sampler->SetMeshFile(i, mesh_file);
        }
#endif      

      
      // Tell the sampler about the corresponding list of attributes
      bool ok2 = true;
      int apd;
      PARAMSET(pf, apd, "attributes_per_domain", 0, ok2, 0);
      if (ok2 == true)
        {
        general_entropy_choice->activate();

        double sc;
        std::vector<double> attr_scales;
        for (unsigned int kk = 0; kk < apd; kk++)
          {
          PARAMSET(pf, sc, "attribute_scales", 0, ok2, 1.0);
         attr_scales.push_back(sc);
          }
        m_Sampler->SetAttributeScales(attr_scales);
        
        for (unsigned int kk = 0; kk < apd; kk++)
          {
          PARAMSET(pf, attribute_file, "attribute_files", i*apd + kk, ok2, "");
          if (ok1 == true)
            {
            std::cout << "Reading attribute file: " << attribute_file << std::endl;
            
            // Read attribute file
            itk::ImageFileReader<ImageType>::Pointer reader2
              = itk::ImageFileReader<ImageType>::New();

           reader2->SetFileName(attribute_file.c_str());
            reader2->Update();
            m_Sampler->AddAttributeImage(i, reader2->GetOutput());
            }
          else
            {
            std::cerr << "Missing an attribute file for shape file " << shape_file << std::endl;
            }
          }
        } // done read attribute block      
      
      } // if ok == true
    i++;
    } // while ok == true

  // Set the extent of the viewer selectors
  viewer1_domain_spinner->maximum(i-2);
  viewer2_domain_spinner->maximum(i-2);

  // Set extent of number of mode selector
  number_of_modes_spinner->maximum(i-2);

  m_Iteratecmd  = itk::MemberCommand<ShapeWorksShopApp>::New();
  m_Iteratecmd->SetCallbackFunction(this, &ShapeWorksShopApp::IterateCallback);
  m_Sampler->GetOptimizer()->AddObserver(itk::IterationEvent(), m_Iteratecmd);

  m_AddPointcmd  = itk::MemberCommand<ShapeWorksShopApp>::New();
  m_AddPointcmd->SetCallbackFunction(this, &ShapeWorksShopApp::AddPointCallback);
  m_Sampler->GetParticleSystem()->AddObserver(itk::ParticlePositionAddEvent(), m_AddPointcmd);

  // Set up renderer and interactor.
  m_Renderer1= vtkRenderer::New();
  //  m_Renderer2= vtkRenderer::New();
  m_RenderWindow1 = vtkRenderWindow::New();
  //  m_RenderWindow2 = vtkRenderWindow::New();
  m_RenderWindow1->AddRenderer(m_Renderer1);
  //  m_RenderWindow2->AddRenderer(m_Renderer2);

  m_Renderer1->AddActor( m_IsosurfacePipelines[0]->output() );
  //  m_Renderer2->AddActor( m_IsosurfacePipelines[0]->output() );

  m_Renderer1->AddActor( m_GlyphPipelines[0]->output());
  //  m_Renderer2->AddActor( m_GlyphPipelines[0]->output());

  if (m_ShowCuttingPlanes == true) m_Renderer1->AddActor( m_PlaneWidgetPipelines[0]->output());
  if (m_ShowSpheres == true)
    {
    for (unsigned int qq = 0; qq < m_SpheresPerDomain; qq++)
      {
      m_Renderer1->AddActor( m_SphereWidgetPipelines[0][qq]->output());
      }
    }
  
  m_Renderer1->SetBackground(0.6, 0.6, 0.6);
  //  m_Renderer2->SetBackground(0.5, 0.5, 0.5);

//   m_RenderWindow1->SetSize(600, 700);
//   m_RenderWindow2->SetSize(600, 700);

  // Change to trackball style interaction.
  vtkInteractorStyleSwitch *iass1 =
    dynamic_cast<vtkInteractorStyleSwitch*>(this->m_Viewer1->GetInteractorStyle());
  iass1->SetCurrentStyleToTrackballCamera();

  //  vtkInteractorStyleSwitch *iass2 =
  //    dynamic_cast<vtkInteractorStyleSwitch*>(this->m_Viewer2->GetInteractorStyle());
  //  iass2->SetCurrentStyleToTrackballCamera();
    
  this->m_Viewer1->SetRenderWindow(m_RenderWindow1);
  //  this->m_Viewer2->SetRenderWindow(m_RenderWindow2);
  m_RenderWindow1->SetInteractor(this->m_Viewer1);
  //  m_RenderWindow2->SetInteractor(this->m_Viewer2);


  // Now read the transform file if present.
  ok = true;
  std::string transform_file;
  PARAMSET(pf, transform_file, "transform_file", 0, ok, "");
  if ( ok == true)
    {
    m_Sampler->SetTransformFile(transform_file.c_str());
    this->transform_filename_input->value(transform_file.c_str());
    }

  // Now read the fixed transform file if present.
  ok = true;
  std::string prefix_transform_file;
  PARAMSET(pf, prefix_transform_file, "prefix_transform_file", 0, ok, "");
  if ( ok == true)
    { 
    m_Sampler->SetPrefixTransformFile(prefix_transform_file.c_str());
    }

  
  // Intitialize the solver without actually doing any iterations.
  this->SetNumericalParameter();
  this->SetAdaptivityMode();
  this->SetCorrespondenceMode();
  this->SetOptimizationMode();

  std::cout << "Initializing SOLVER" << std::endl;
  
  m_Sampler->Initialize();
  std::cout << "DONE Initializing SOLVER" << std::endl;
  m_Sampler->SetCorrespondenceOn();



  // SET UP ANY FIXED LANDMARK POSITIONS
  ok = true;
  for (unsigned int i = 0; ok == true; i++)
    {
    unsigned int f;
    PARAMSET(pf, f, "fixed_landmarks", i, ok, 0);
    if (ok == true) m_Sampler->GetParticleSystem()->SetFixedParticleFlag(f);
    }

  

  this->toggle_correspondence_button->value(1);
  this->toggle_sampling_button->value(1);
  this->SetParticleCounter();  
  this->m_Viewer1->Initialize();

  RecolorGlyphs();
  //  this->m_Viewer2->Initialize();
  
  //  m_Renderer2->SetActiveCamera(m_Renderer1->GetActiveCamera());
}

ITK_THREAD_RETURN_TYPE 
ShapeWorksShopApp
::optimize_callback( void *arg )
{
  try {
  static_cast<ShapeWorksShopApp *>(((itk::MultiThreader::ThreadInfoStruct *)(arg))->UserData)->GetSampler()->Modified();
  static_cast<ShapeWorksShopApp *>(((itk::MultiThreader::ThreadInfoStruct *)(arg))->UserData)->GetSampler()->Update();
  }
  catch (itk::ExceptionObject &e)
    {
    std::cerr << "Caught itk Exception: " <<  e << std::endl;
    std::cout << "Optimize failed. " << std::endl;
    }
 catch ( ... )
    {
    std::cerr << "Caught unknown exception. " << std::endl;
    std::cout << "Optimize Failed." << std::endl;
    }
     
  return ITK_THREAD_RETURN_VALUE;
}

void
ShapeWorksShopApp::single_iteration()
{
  this->optimize_stop();
	this->SetNumericalParameter();
  // Make sure the solver will only do a single iteration.
  m_Sampler->GetOptimizer()->SetMaximumNumberOfIterations(m_Sampler->GetOptimizer()->GetNumberOfIterations() + 1);
  m_Sampler->Update();  // run a single iteration

  //m_Viewer1->Render();  // Rerender the scene.
  //  m_Viewer2->Render();  // Rerender the scene.
}

void
ShapeWorksShopApp::optimize_start()
{
  this->SetNumericalParameter();
  //  m_Threader->SetNumberOfThreads(2);
  m_ProcessingThread =  m_Threader->SpawnThread(this->optimize_callback, this);
}

void
ShapeWorksShopApp::optimize_stop()
{
  m_Sampler->GetOptimizer()->StopOptimization();
  m_Threader->TerminateThread(m_ProcessingThread);
  //  m_Viewer1->Render(); // rerender the scene
  //  m_Viewer2->Render(); // rerender the scene
}


ShapeWorksShopApp::~ShapeWorksShopApp()
{
  m_Renderer1->Delete();
  m_RenderWindow1->Delete();
  //  m_Renderer2->Delete();
  //  m_RenderWindow2->Delete();

  for (unsigned int i = 0; i < m_GlyphPipelines.size(); i++)
    { delete m_GlyphPipelines[i]; }
  for (unsigned int i = 0; i < m_IsosurfacePipelines.size(); i++)
    { delete m_IsosurfacePipelines[i]; }
  for (unsigned int i = 0; i < m_PlaneWidgetPipelines.size(); i++)
    { delete m_PlaneWidgetPipelines[i]; }
  for (unsigned int i = 0; i < m_SphereWidgetPipelines.size(); i++)
    {
    for (unsigned int j = 0; j < m_SpheresPerDomain; j++)
      {    delete m_SphereWidgetPipelines[i][j]; }
    }
}
