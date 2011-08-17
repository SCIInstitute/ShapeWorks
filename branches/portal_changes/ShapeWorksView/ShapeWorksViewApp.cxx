/*=========================================================================
  Program:   ShapeWorks: Particle-based Shape Correspondence & Visualization
  Module:    $RCSfile: ShapeWorksViewApp.cxx,v $
  Date:      $Date: 2011/03/24 01:17:39 $
  Version:   $Revision: 1.12 $
  Author:    $Author: wmartin $

  Copyright (c) 2009 Scientific Computing and Imaging Institute.
  See ShapeWorksLicense.txt for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.
=========================================================================*/
#include "ShapeWorksViewApp.h"


void ShapeWorksViewApp::ResetPointScalars()
{
  vtkUnsignedLongArray *mags = vtkUnsignedLongArray::New();
  mags->SetNumberOfComponents(1);
  mags->SetNumberOfTuples(m_glyphPoints->GetNumberOfPoints());
  
  for (unsigned int i = 0; i < m_glyphPoints->GetNumberOfPoints(); i++)
    {   
    mags->InsertTuple1(i, i);
    }
  m_glyphPointset->GetPointData()->SetScalars(mags);  
  m_glyphPoints->Modified();
  mags->Delete();
}

void ShapeWorksViewApp::PointFileDiff()
{
  // Get a file name
  Fl_File_Chooser chooser(m_CurrentDirectory.c_str(), "",
                          Fl_File_Chooser::CREATE, "Load Point File");
  chooser.show();
  
  while(chooser.shown()) { Fl::wait(); }
  
  // User hit cancel?
  if ( chooser.value() == NULL )
    { return; }

  m_CurrentDirectory = std::string(chooser.directory());  

  itk::ParticlePositionReader<3>::Pointer reader
    = itk::ParticlePositionReader<3>::New();
  reader->SetFileName(chooser.value());
  reader->Update();

  unsigned int q = reader->GetOutput().size();


  if (q  !=  m_glyphPoints->GetNumberOfPoints())
    {
    std::cerr << "PointFileDiff::Point file does not have the same number of points" << std::endl;
    return;
    }

  std::vector< itk::ParticlePositionReader<3>::PointType > vecs;  
  double pt[3];
  for (unsigned int i = 0; i < q; i++)
    {
    itk::ParticlePositionReader<3>::PointType tmp;
    m_glyphPoints->GetPoint(i, pt);

    tmp[0] = reader->GetOutput()[i][0] - pt[0];
    tmp[1] = reader->GetOutput()[i][1] - pt[1];
    tmp[2] = reader->GetOutput()[i][2] - pt[2];
    vecs.push_back(tmp);
    }
  
  this->DisplayVectorField(vecs);
}


void ShapeWorksViewApp::LoadVectorField()
{
  // Read the vectors.  Assume 3D and stored just like points.
  Fl_File_Chooser chooser(m_CurrentDirectory.c_str(), "",
                          Fl_File_Chooser::CREATE, "Load Vector File");
  chooser.show();
  
  while(chooser.shown()) { Fl::wait(); }
  
  // User hit cancel?
  if ( chooser.value() == NULL )
    { return; }

  m_CurrentDirectory = std::string(chooser.directory());  
  
  itk::ParticlePositionReader<3>::Pointer reader
    = itk::ParticlePositionReader<3>::New();
  reader->SetFileName(chooser.value());
  reader->Update();
  
  this->DisplayVectorField(reader->GetOutput());   
}

void ShapeWorksViewApp::ShowSpheres()
{
  m_renderer->RemoveActor(m_arrowGlyphActor);

  this->ResetPointScalars();  
  m_glyphMapper->SetLookupTable(m_lut);
  m_glyphs->SetSourceConnection(m_sphereSource->GetOutputPort());
  m_glyphs->SetScaleModeToDataScalingOff();

  m_showingArrowGlyphs = false;
  this->SetGlyphScale();
}

void ShapeWorksViewApp::LoadPointFile()
{
  // Get a file name
  Fl_File_Chooser chooser(m_CurrentDirectory.c_str(), "",
                          Fl_File_Chooser::CREATE, "Load Point File");
  chooser.show();
 
  while(chooser.shown()) { Fl::wait(); }
  
  // User hit cancel?
  if ( chooser.value() == NULL )
    { return; }

  m_CurrentDirectory = std::string(chooser.directory());  

  itk::ParticlePositionReader<3>::Pointer reader
    = itk::ParticlePositionReader<3>::New();
  reader->SetFileName(chooser.value());
  reader->Update();

  unsigned int q = reader->GetOutput().size();
  vnl_vector<double> pos;
  pos.set_size(q * 3);
  unsigned int k = 0;
  for (unsigned int j = 0; j < q; j++)
    {
    pos[k] = reader->GetOutput()[j][0];
    k++;
    pos[k] = reader->GetOutput()[j][1];
    k++;
    pos[k] = reader->GetOutput()[j][2];
    k++;    
    }

  this->DisplayShape(pos);
}

void ShapeWorksViewApp::LoadPCAShape()
{
 // Get a file name
  Fl_File_Chooser chooser(m_CurrentDirectory.c_str(), "",
                          Fl_File_Chooser::CREATE, "Load PCA Shape");
  chooser.show();
  
  while(chooser.shown()) { Fl::wait(); }
  
  // User hit cancel?
  if ( chooser.value() == NULL )
    { return; }

  m_CurrentDirectory = std::string(chooser.directory());
  
  itk::ParticlePositionReader<1>::Pointer reader1 = itk::ParticlePositionReader<1>::New();
  reader1->SetFileName( chooser.value() );
  reader1->Update();

  // Each position in reader correspondends to a PCA loading.  Missing PCA
  // components are filled in with 0s.

  unsigned int numModes = reader1->GetOutput().size();
  vnl_vector<double> wext(m_NumberOfSamples);
  //  std::cout << "PCAShape is ";
  for (unsigned int i = 0; i < m_NumberOfSamples; i++)
    {    
    if (i >= numModes)
      {
      wext[i] = 0.0;
      }
    else
      {
      wext[i] = reader1->GetOutput()[i][0];
      }
    //    std::cout << wext[i] << " ";
    }
  //  std::cout <<  std::endl;

   // Rotate the LD back into the full dimensional space
   // Rearrange the eigenvectors:     
  vnl_matrix<double> tmpeigs = m_Stats.Eigenvectors();
  tmpeigs.fliplr();  
  vnl_vector<double> bigLD = wext.post_multiply(tmpeigs.transpose());

  // Load the points
  this->DisplayShape(bigLD + m_Stats.Mean());  
}


void ShapeWorksViewApp::generate_color_list(int n)
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

void ShapeWorksViewApp::TrilinearInterpolate(vtkImageData *grad,
                                                    double x,
                                                    double y,
                                                    double z,
                                                    vnl_vector_fixed<double,3> &ans) const
{
  // NOTE: This method is not working correctly.  I don't seem to understand
  // how to properly access the vtkImageData.  For now, just use the nearest
  // node point.  jc 5/15/09
  
  
 // Access gradient image information.
  const double *gradData = (const double *)(grad->GetScalarPointer());
  const double *spacing = grad->GetSpacing();
  
  // See, e.g. http://en.wikipedia.org/wiki/Trilinear_interpolation for description
  // Identify the surrounding 8 points (corners).  c is the closest grid point.
  vtkIdType idx = grad->FindPoint(x,y,z);
  const double *c = grad->GetPoint(idx);

  //  std::cout << "idx = " << idx << std::endl;
  //  std::cout << "c = " << c[0] << " " << c[1] << " " << c[2] << std::endl;
  
  if (m_InterpolateNormals != true) // return value at closest node point
    {
    ans[0] = gradData[idx*3];
    ans[1] = gradData[idx*3 +1];
    ans[2] = gradData[idx*3 +2];
    return;
    }

  double x0,y0,z0,x1,y1,z1;
  if (x < c[0])  { x0 = c[0] - spacing[0]; x1 = c[0]; }
  else           { x0 = c[0]; x1 = c[0] + spacing[0]; }
  
  if (y < c[1])  { y0 = c[1] - spacing[1]; y1 = c[1]; }
  else           { y0 = c[1]; y1 = c[1] + spacing[1]; }
  
  if (z < c[2])  { z0 = c[2] - spacing[2]; z1 = c[2]; }
  else           { z0 = c[2]; z1 = c[2] + spacing[2]; }

  //  std::cout << "x = " << x << "\tc[0] = " << c[0] << 
  
  // Find corner indices
  vtkIdType idxLLL = grad->FindPoint(x0,y0,z0);
  vtkIdType idxLLU = grad->FindPoint(x0,y0,z1);
  vtkIdType idxLUU = grad->FindPoint(x0,y1,z1);
  vtkIdType idxLUL = grad->FindPoint(x0,y1,z0);
  vtkIdType idxULL = grad->FindPoint(x1,y0,z0);
  vtkIdType idxULU = grad->FindPoint(x1,y0,z1);
  vtkIdType idxUUL = grad->FindPoint(x1,y1,z0);
  vtkIdType idxUUU = grad->FindPoint(x1,y1,z1);

  const double *lll = grad->GetPoint(idxLLL);
  double xd = x - lll[0];
  double yd = y - lll[1];
  double zd = z - lll[2];
  
  // Interpolate along Z
  vnl_vector_fixed<double,3> i1,i2,j1,j2,w1,w2;

  i1(0) = gradData[idxLLL*3]   * (1.0 - zd) + gradData[idxLLU*3]   * zd;
  i1(1) = gradData[idxLLL*3+1] * (1.0 - zd) + gradData[idxLLU*3+1] * zd;
  i1(2) = gradData[idxLLL*3+2] * (1.0 - zd) + gradData[idxLLU*3+2] * zd;

  i2(0) = gradData[idxLUL*3]   * (1.0 - zd) + gradData[idxLUU*3]   * zd;
  i2(1) = gradData[idxLUL*3+1] * (1.0 - zd) + gradData[idxLUU*3+1] * zd;
  i2(2) = gradData[idxLUL*3+2] * (1.0 - zd) + gradData[idxLUU*3+2] * zd;

  j1(0) = gradData[idxULL*3]   * (1.0 - zd) + gradData[idxULU*3]   * zd;
  j1(1) = gradData[idxULL*3+1] * (1.0 - zd) + gradData[idxULU*3+1] * zd;
  j1(2) = gradData[idxULL*3+2] * (1.0 - zd) + gradData[idxULU*3+2] * zd;

  j2(0) = gradData[idxUUL*3]   * (1.0 - zd) + gradData[idxUUU*3]   * zd;
  j2(1) = gradData[idxUUL*3+1] * (1.0 - zd) + gradData[idxUUU*3+1] * zd;
  j2(2) = gradData[idxUUL*3+2] * (1.0 - zd) + gradData[idxUUU*3+2] * zd;

  // Interpolate along Y
  w1 = i1 * (1.0 - yd) + i2 * yd;
  w2 = j1 * (1.0 - yd) + j2 * yd;

  // Interpolate along X
  ans = w1 * (1.0 - xd) + w2 * xd;
}

void ShapeWorksViewApp::DisplayMeanDifference()
{
  std::vector< itk::ParticlePositionReader<3>::PointType > vecs;

  for (unsigned int i = 0; i < m_glyphPoints->GetNumberOfPoints(); i++)
    {
    itk::ParticlePositionReader<3>::PointType tmp;
    tmp[0] =  m_Stats.Group2Mean()[i*3]   - m_Stats.Group1Mean()[i*3];
    tmp[1] =  m_Stats.Group2Mean()[i*3+1] - m_Stats.Group1Mean()[i*3+1];
    tmp[2] =  m_Stats.Group2Mean()[i*3+2] - m_Stats.Group1Mean()[i*3+2];
    vecs.push_back(tmp);
    }
  this->DisplayVectorField(vecs);
}

void ShapeWorksViewApp
::DisplayVectorField(const std::vector<itk::ParticlePositionReader<3>::PointType > &vecs)
{
  if (vecs.size() <  m_glyphPoints->GetNumberOfPoints() )
    {
    std::cerr << "Error: not enough vectors" << std::endl;
    return;
    }
   
  double minmag = 1.0e20;
  double maxmag = 0.0;
  this->ComputeSurface(); // need the surface information for the normals

  vtkFloatArray *vectors = vtkFloatArray::New() ;
  vtkFloatArray *vectors2= vtkFloatArray::New() ;
  
  vectors->SetNumberOfComponents(3);
  vectors2->SetNumberOfComponents(3);

  vtkImageGaussianSmooth *smoother = vtkImageGaussianSmooth::New();
  // smoother->SetStandardDeviations(1.0,1.0,1.0);
   smoother->SetStandardDeviations(m_surf->GetOutput()->GetSpacing()[0]
                                    * this->gradsmooth->value(),
                                    m_surf->GetOutput()->GetSpacing()[1]
                                    * this->gradsmooth->value(),
                                    m_surf->GetOutput()->GetSpacing()[2]
                                    * this->gradsmooth->value()
                                );
  smoother->SetInputConnection(m_surf->GetOutputPort());
  
  // Compute normals from the isosurface volume
  vtkImageGradient *grad = vtkImageGradient::New();
  grad->SetDimensionality(3);

  grad->SetInputConnection(smoother->GetOutputPort());
  grad->Update();

  //  std::cout << "Grad spacing: " << grad->GetOutput()->GetSpacing()[0]
  //            << " " << grad->GetOutput()->GetSpacing()[1]
  //            << grad->GetOutput()->GetSpacing()[2] << std::endl;


  //  smoother->SetInputPort
                                  
  
  //  for (int i = 0 ; i < m_glyphPoints->GetNumberOfPoints() ; i++ )
  //    {
  //    vectors->InsertNextTuple3(vecs[i][0],vecs[i][1],vecs[i][2]);
  //    }  
  
  
  // Dot product difference vectors with the surface normals.
  vtkFloatArray *mags = vtkFloatArray::New();
  mags->SetNumberOfComponents(1);
  mags->SetNumberOfTuples(m_glyphPoints->GetNumberOfPoints());

  vnl_vector_fixed<double,3> n;

  // Compute difference vector dot product with normal.  Length of vector is
  // stored in the "scalars" so that the vtk color mapping and glyph scaling
  // happens properly.
  
  for (unsigned int i = 0; i < m_glyphPoints->GetNumberOfPoints(); i++)
    {
    double x = m_glyphPoints->GetPoint(i)[0];
    double y = m_glyphPoints->GetPoint(i)[1];
    double z = m_glyphPoints->GetPoint(i)[2];
    
    //float xd = vectors->GetTuple3(i)[0];
    //float yd = vectors->GetTuple3(i)[1];
    //float zd = vectors->GetTuple3(i)[2];

    float xd = vecs[i][0];
    float yd = vecs[i][1];
    float zd = vecs[i][2];

    this->TrilinearInterpolate(grad->GetOutput(),x,y,z,n);
    
    float mag =  xd*n(0) + yd*n(1) + zd*n(2);

    if (mag < minmag) minmag = mag;
    if (mag > maxmag) maxmag = mag;
    
    vectors2->InsertNextTuple3(n(0) * mag, n(1) * mag, n(2) * mag);
    mags->InsertTuple1(i, mag) ;
    }
  

  m_glyphMapper->SetLookupTable(m_differenceLUT);
  m_arrowGlyphMapper->SetLookupTable(m_differenceLUT);

  
  m_glyphs->SetSourceConnection(m_arrowSource->GetOutputPort());
  m_glyphPointset->GetPointData()->SetVectors(vectors2);
  m_glyphPointset->GetPointData()->SetScalars(mags);

  m_glyphs->SetScaleModeToScaleByVector();
  //  m_glyphs->SetVectorModeToUseVector();

  this->UpdateDifferenceLUT(minmag, maxmag);
  
  m_renderer->AddActor(m_arrowGlyphActor);
  vectors->Delete();
  mags->Delete();

  m_showingArrowGlyphs = true;
  
  if (m_Initialized) this->m_render_window->Render();
}

ShapeWorksViewApp::ShapeWorksViewApp(const char *fn)
{
  this->generate_color_list(1024);
  m_Initialized = false;
  m_CurrentDirectory = ".";
  m_InterpolateNormals = false;
  this->ZeroAllVTK();

  // Read parameter file
  TiXmlDocument doc(fn);
  bool loadOkay = doc.LoadFile();
  if (!loadOkay) std::cerr << "invalid parameter file..." << std::endl;

  TiXmlHandle docHandle( &doc );
  TiXmlElement *elem;
  std::istringstream inputsBuffer;

  // Hide group menus if no groups are present 
  elem = docHandle.FirstChild( "group_ids" ).Element();
  if (!elem)
  {
    this->menuGroup1Mean->hide();
    this->menuGroup2Mean->hide();
    this->menuGroup1Median->hide();
    this->menuGroup2Median->hide();
    this->groupdiff_position->hide();
    this->group2LabelBox->hide();
    this->group1LabelBox->hide();
    this->group_display->hide();
  }

  // Run statistics
  m_Stats.ReadPointFiles(fn);
  m_Stats.ComputeModes();
  m_Stats.PrincipalComponentProjections();

  unsigned int num_points = m_Stats.Mean().size();

  m_NumberOfSamples = m_Stats.ShapeMatrix().cols();
                                          
  // Set some gui variables
  this->sample_selector->minimum(0);
  this->sample_selector->maximum(m_NumberOfSamples-1);
  this->sample_selector->value(0);
  this->mode->minimum(0);
  this->mode->maximum(m_NumberOfSamples-1);
  this->mode->step(1);
  this->mode->value(0);
  
  // Compute the linear regression
  m_Regression = itk::ParticleShapeLinearRegressionMatrixAttribute<double,3>::New();
  m_Regression->SetMatrix( m_Stats.ShapeMatrix() );

  // Look for user-supplied simple linear regression parameters
  this->ReadSimpleRegressionParameters(fn);
  
  // Load the explanatory variables
  this->ReadExplanatoryVariables(fn);
  m_Regression->ResizeParameters( m_Stats.ShapeMatrix().rows());
  m_Regression->ResizeMeanMatrix( m_Stats.ShapeMatrix().rows(), m_Stats.ShapeMatrix().cols());
  m_Regression->Initialize();
  m_Regression->EstimateParameters();
 
  this->color_switcher->minimum(0);
  this->color_switcher->maximum(m_ColorSchemes.size() -1);
  this->color_switcher->step(1);
  this->color_switcher->value(0);
  
  this->InitializeRenderer();
  this->InitializeGlyphs();
   
  // Create num_points glyphs
  for (unsigned int i = 0; i < num_points/3; i++)
    {
    m_glyphs->SetRange(0.0, (double) i + 1);
    m_glyphMapper->SetScalarRange(0.0, (double) i + 1.0);
    m_lut->SetNumberOfTableValues(i + 1);
    m_lut->SetTableRange(0.0, (double)i + 1.0);
    
    ((vtkUnsignedLongArray *)(m_glyphPointset->GetPointData()->GetScalars()))
      ->InsertValue(i, i);
    
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    
    m_glyphPoints->InsertPoint(i, x,y,z);
    }

  m_glyphPoints->Modified();

  // Set the color modes
  m_arrowGlyphs->SetColorModeToColorByScalar();
  m_glyphs->SetColorModeToColorByScalar();  
  m_glyphMapper->SetColorModeToMapScalars();
  m_arrowGlyphMapper->SetColorModeToMapScalars();
  m_glyphMapper->ScalarVisibilityOn();
  m_arrowGlyphMapper->ScalarVisibilityOn();
  
  this->RecolorGlyphs(0,0,0);
  this->DisplayMean(0);
  this->InitializeSurface();

  m_renderer->AddActor(m_glyphActor);
  this->ChangeColorScheme(); // initialize the colors
  this->ComputeSurface();
  this->HideGroups();
  m_Initialized = true;
}





void ShapeWorksViewApp::ChangeColorScheme()
{
  int q = this->color_switcher->value();
   
  m_surfActor->GetProperty()->SetDiffuseColor(m_ColorSchemes[q].foreground.r,
                                              m_ColorSchemes[q].foreground.g,
                                              m_ColorSchemes[q].foreground.b);

  this->RecolorGlyphs(m_ColorSchemes[q].alt.r,
                      m_ColorSchemes[q].alt.g,
                      m_ColorSchemes[q].alt.b);
  
  m_renderer->SetBackground(m_ColorSchemes[q].background.r,
                            m_ColorSchemes[q].background.g,
                            m_ColorSchemes[q].background.b);


  
  if (m_Initialized) this->m_render_window->Render();
  
}
void ShapeWorksViewApp::ComputeSurface()
{
  m_surf->SetNeighborhoodSize(this->neighborhoodsize->value());
  m_surf->SetSampleSpacing(this->samplespacing->value());
  m_renderer->AddActor(m_surfActor);

  if (m_Initialized) this->m_render_window->Render();
}

void ShapeWorksViewApp::RemoveSurface()
{
  m_renderer->RemoveActor(m_surfActor);
  m_render_window->Render();
}


void ShapeWorksViewApp::ComputeRegressionShape()
{
  vnl_vector<double> pos = m_Regression->ComputeMean(this->position->value());
  this->DisplayShape(pos);
}

void ShapeWorksViewApp::ComputeSimpleRegressionParameters()
{
  //  int m = this->mode->value()-1;
  unsigned int m = this->mode->value();

  // Are there user-specified regression parameters?
  if (m_SimpleRegressionSlopes.size() > m && m_SimpleRegressionIntercepts.size() > m)
    {    
    m_SimpleRegressionA =  m_SimpleRegressionIntercepts[m];
    m_SimpleRegressionB =  m_SimpleRegressionSlopes[m];
    std::cout << "Found user-supplied paramters for mode = " << m << std::endl;
    std::cout << "a = " << m_SimpleRegressionA << std::endl;
    std::cout << "b = " << m_SimpleRegressionB << std::endl;
    return;
    }
  else
    {  
    std::vector<double> x,y;
    for (unsigned int i = 0; i < m_Stats.PCALoadings().rows(); i++)
      {
      y.push_back( (m_Stats.PCALoadings())(i,m) );
      x.push_back(m_Regression->GetExplanatory(i));
      }
    
    m_Stats.SimpleLinearRegression(y, x, m_SimpleRegressionA, m_SimpleRegressionB);
    std::cout << "mode = " << m << std::endl;
    std::cout << "a = " << m_SimpleRegressionA << std::endl;
    std::cout << "b = " << m_SimpleRegressionB << std::endl;
    return;
    }
}

void ShapeWorksViewApp::ComputeSimpleRegressionShape()
{
  unsigned int m = m_Stats.Eigenvectors().columns() - (this->mode->value()+1);
  double lambda = sqrt(m_Stats.Eigenvalues()[m]);
  double y = m_SimpleRegressionA + m_SimpleRegressionB * this->simple_regression->value();  
  this->mode_position->value(y / lambda);
  this->ComputeModeShape();
}

void ShapeWorksViewApp::ComputeModeShape()
{
  double s = this->mode_position->value();
  unsigned int m = m_Stats.Eigenvectors().columns() - (this->mode->value()+1);
  vnl_vector<double> e = m_Stats.Eigenvectors().get_column(m);
  double lambda = sqrt(m_Stats.Eigenvalues()[m]);
  this->lambda_display->value(m_Stats.Eigenvalues()[m]);
  this->loading_display->value(s*lambda);
  
  this->DisplayShape(m_Stats.Mean() + (e * (s * lambda)));

}

void ShapeWorksViewApp::ComputeGroupMeanDifferenceShape()
{
  double s = this->groupdiff_position->value();
  this->DisplayShape(m_Stats.Group1Mean() + (m_Stats.GroupDifference() *s));
}

void ShapeWorksViewApp::DisplayShape(const vnl_vector<double> &pos)
{
  unsigned int k = 0;
  for (unsigned int i = 0; i < this->m_Stats.ShapeMatrix().rows() / 3; i++)
    {
    double x = pos[k];
    k++;
    double y = pos[k];
    k++;
    double z = pos[k];
    k++;
    m_glyphPoints->SetPoint(i, x,y,z);
    }
  m_glyphPoints->Modified();
  
  if (m_Initialized)
    {
    // The following two lines overcome some wierd pipeline updating bug I
    // can't figure out.  The origin information propagating from the surface
    // reconstruction somehow does not get updated correctly until run twice.
    if (m_surfActor) this->m_surfActor->Render(m_renderer, m_surfMap);
    if (m_surf) m_surf->Modified();
    
    this->m_render_window->Render();
    }  
}

void ShapeWorksViewApp::InitializeRenderer()
{
  // Set up renderer and interactor.
  m_renderer= vtkRenderer::New();
  m_render_window = vtkRenderWindow::New();
  m_render_window->AddRenderer(m_renderer);

  vnl_vector<double> col(3);
  col[0] = 255;
  col[1] = 239;
  col[2] = 213;
  col.normalize();
  m_renderer->SetBackground(col[0], col[1], col[2]);
  m_render_window->SetSize(500, 500);

  // Change to trackball style interaction.
  vtkInteractorStyleSwitch *iass =
    dynamic_cast<vtkInteractorStyleSwitch*>(this->imageView->GetInteractorStyle());
  iass->SetCurrentStyleToTrackballCamera();
  
  this->imageView->SetRenderWindow(m_render_window);
  m_render_window->SetInteractor(this->imageView);
}

void ShapeWorksViewApp::InitializeSurface()
{
  m_surf = vtkSurfaceReconstructionFilter::New();
  m_surf->SetInput(m_glyphPointset);
  m_surf->SetNeighborhoodSize(this->neighborhoodsize->value());
  m_surf->SetSampleSpacing(this->samplespacing->value());
  
  m_surfContour = vtkContourFilter::New();
  m_surfContour->SetInputConnection(m_surf->GetOutputPort());
  m_surfContour->SetValue(0,0.0);
  m_surfContour->ComputeNormalsOn();
  
  m_surfReverse = vtkReverseSense::New();
  m_surfReverse->SetInputConnection(m_surfContour->GetOutputPort());
  m_surfReverse->ReverseCellsOn();
  m_surfReverse->ReverseNormalsOn();

  //  m_surfDecimate = vtkDecimatePro::New();  
  //  m_surfDecimate->SetInputConnection(m_surfReverse->GetOutputPort());
  //  m_surfDecimate->SetTargetReduction(0.0);
  //  m_surfDecimate->PreserveTopologyOff();

  //  m_surfNormals = vtkPolyDataNormals::New();
  //  m_surfNormals->SetInputConnection(m_surfDecimate->GetOutputPort());
  
  m_surfSmoother = vtkSmoothPolyDataFilter::New();
  m_surfSmoother->SetInputConnection(m_surfReverse->GetOutputPort());
  m_surfSmoother->SetNumberOfIterations(0);
  
  m_surfMap = vtkPolyDataMapper::New();
  m_surfMap->SetInputConnection(m_surfSmoother->GetOutputPort());
  m_surfMap->ScalarVisibilityOff();

  m_surfActor = vtkActor::New();
  m_surfActor->SetMapper(m_surfMap);
  m_surfActor->GetProperty()->SetSpecular(.4);
  m_surfActor->GetProperty()->SetSpecularPower(50);
}

void ShapeWorksViewApp::SetGlyphScale()
{
  m_glyphs->SetScaleFactor(this->glyph_scale->value());
  m_arrowGlyphs->SetScaleFactor(this->glyph_scale->value());

  m_sphereSource->SetThetaResolution(this->glyph_quality->value());
  m_sphereSource->SetPhiResolution(this->glyph_quality->value());

  m_arrowSource->SetTipResolution(this->glyph_quality->value());
  m_arrowSource->SetShaftResolution(this->glyph_quality->value());
  
  m_glyphs->Update();
  m_arrowGlyphs->Update();
  
  if (m_Initialized) m_render_window->Render();
}

void ShapeWorksViewApp::RecolorGlyphs(float r, float g, float b)
{  
  unsigned int k =0;
  if (this->show_correspondence_button->value() == 1)
    {
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
  else
    {
    for (unsigned int i = 0; i < m_lut->GetNumberOfTableValues(); i++)
      {
      m_lut->SetTableValue(i,r,g,b,1.0);
      }
    }

  //  m_glyphMapper->SetLookupTable(m_lut);
}

void ShapeWorksViewApp::InitializeGlyphs()
{
  m_showingArrowGlyphs = false;
  m_lut = vtkLookupTable::New();
  m_PValueTFunc = vtkColorTransferFunction::New();
  m_differenceLUT = vtkColorTransferFunction::New();
  m_glyphPoints = vtkPoints::New();
  m_glyphPointset = vtkPolyData::New();
  m_glyphs = vtkGlyph3D::New();
  m_glyphMapper = vtkPolyDataMapper::New();
  m_glyphActor = vtkActor::New();  
  m_sphereSource = vtkSphereSource::New();
  m_scalars = vtkUnsignedLongArray::New();
  m_arrowSource = vtkArrowSource::New();
  m_transform180 = vtkTransform::New();
  m_arrowFlipFilter = vtkTransformPolyDataFilter::New();
  m_arrowGlyphs = vtkGlyph3D::New();
  m_arrowGlyphMapper = vtkPolyDataMapper::New();
  m_arrowGlyphActor = vtkActor::New();
  
  m_PValueTFunc->SetColorSpaceToHSV();
  m_PValueTFunc->AddHSVPoint(0, 0, 1, 1);
  m_PValueTFunc->AddHSVPoint(0.05, 1, .40, 1);
  m_PValueTFunc->AddHSVPoint(0.05 + 0.00001, 1, 0.0, 1);
  m_PValueTFunc->AddHSVPoint(1, 1, 0.0, 1);

  m_differenceLUT->SetColorSpaceToHSV();

  m_glyphPoints->SetDataTypeToDouble();

  m_glyphPointset->SetPoints(m_glyphPoints);    
  m_glyphPointset->GetPointData()->SetScalars(m_scalars);

  m_glyphs->SetInput(m_glyphPointset);
  m_glyphs->ScalingOn();
  m_glyphs->ClampingOff();
  m_glyphs->SetScaleModeToDataScalingOff();

  m_glyphMapper->SetInputConnection(m_glyphs->GetOutputPort());
  
  m_glyphActor->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  m_glyphActor->GetProperty()->SetDiffuse(0.8);
  m_glyphActor->GetProperty()->SetSpecular(0.3);
  m_glyphActor->GetProperty()->SetSpecularPower(10.0);  
  m_glyphActor->SetMapper(m_glyphMapper);

  // Arrow glyphs
  m_arrowSource->SetTipResolution(6);
  m_arrowSource->SetShaftResolution(6);

  vtkTransform *t1 = vtkTransform::New();
  vtkTransform *t2 = vtkTransform::New();
  vtkTransform *t3 = vtkTransform::New();
  vtkTransform *t4 = vtkTransform::New();
  t1->Translate(-0.5, 0.0, 0.0);
  t2->RotateY(180);
  t3->Translate(0.5, 0.0, 0.0);
  t4->RotateY(180);
  t3->Concatenate(t4);
  t2->Concatenate(t3);
  t1->Concatenate(t2);
  m_transform180->Concatenate(t1);
  t1->Delete();
  t2->Delete();
  t3->Delete();
  t4->Delete();
  
  m_arrowFlipFilter->SetTransform(m_transform180);
  m_arrowFlipFilter->SetInputConnection(m_arrowSource->GetOutputPort());
  
  m_arrowGlyphs->SetSourceConnection(m_arrowFlipFilter->GetOutputPort());
  m_arrowGlyphs->SetInput(m_glyphPointset);
  m_arrowGlyphs->ScalingOn();
  m_arrowGlyphs->ClampingOff();

  m_arrowGlyphs->SetVectorModeToUseVector();
  m_arrowGlyphs->SetScaleModeToScaleByVector();

  m_arrowGlyphMapper->SetInputConnection(m_arrowGlyphs->GetOutputPort());
  
  m_arrowGlyphActor->GetProperty()->SetSpecularColor(1.0, 1.0, 1.0);
  m_arrowGlyphActor->GetProperty()->SetDiffuse(0.8);
  m_arrowGlyphActor->GetProperty()->SetSpecular(0.3);
  m_arrowGlyphActor->GetProperty()->SetSpecularPower(10.0);
  m_arrowGlyphActor->SetMapper(m_arrowGlyphMapper);

  this->ShowSpheres();
}

void ShapeWorksViewApp::ReadSimpleRegressionParameters(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  if (loadOkay)
  {
    TiXmlHandle docHandle( &doc );
    TiXmlElement *elem;

    std::istringstream inputsBuffer;
    std::vector<double> evars;
    double startT, endT;
    startT = 1.0e16;
    endT = -1.0e16;
    double etmp = 0.0;

    elem = docHandle.FirstChild( "simple_regression_slopes" ).Element();
    if (elem)
    {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> etmp)
      {
        if (etmp > endT)   endT = ceil(etmp);
        if (etmp < startT) startT = floor(etmp);
        
        m_SimpleRegressionSlopes.push_back(etmp);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");
    }

    elem = docHandle.FirstChild( "simple_regression_intercepts" ).Element();
    if (elem)
    {
      inputsBuffer.str(elem->GetText());
      while (inputsBuffer >> etmp)
      {
        if (etmp > endT)   endT = ceil(etmp);
        if (etmp < startT) startT = floor(etmp);
        
        m_SimpleRegressionIntercepts.push_back(etmp);
      }
      inputsBuffer.clear();
      inputsBuffer.str("");
    }
  }
}


void ShapeWorksViewApp::ReadExplanatoryVariables(const char *fname)
{
  TiXmlDocument doc(fname);
  bool loadOkay = doc.LoadFile();

  TiXmlHandle docHandle( &doc );
  TiXmlElement *elem;

  std::istringstream inputsBuffer;
  std::vector<double> evars;
  double startT, endT;
  startT = 1.0e16;
  endT = -1.0e16;
  double etmp = 0.0;

  elem = docHandle.FirstChild( "explanatory_variable" ).Element();
  if (elem)
  {
    inputsBuffer.str(elem->GetText());
    while (inputsBuffer >> etmp)
    {
      if (etmp > endT)   endT = ceil(etmp);
      if (etmp < startT) startT = floor(etmp);
      
      evars.push_back(etmp);
    }
    inputsBuffer.clear();
    inputsBuffer.str("");
  }

  // Hide simple regression functionality unless parameters have been supplied
  // by the user and there enough explanatory vars to specify a range (needs
  // 2).  This assumes ReadSimpleRegressionParameters has been already been
  // called.
  if (evars.size() < 2 || (evars.size() < m_NumberOfSamples && m_SimpleRegressionSlopes.size() == 0))
    {
    this->simple_regression->hide();
    }
  
  // This case is where there are not enough explanatory variables.  The
  // regression will be computed with some default variables, but the
  // regression will not be available for viewing.
  double q = 0.0;
  while (evars.size() < m_NumberOfSamples)
    {
    evars.push_back(q);
    q=q+1.0;
    this->ViewLinearRegressionChoice->hide(); // disable viewing linear regression

    }
  
  m_Regression->SetExplanatory(evars);

  // Initialize range of explanatory variable.
  this->position->minimum(startT);
  this->position->maximum(endT);
  this->position->step((endT - startT) / 100.0);
  //  this->position->value(startT);
  this->position->value(startT + ((endT - startT) / 2.0));

  this->simple_regression->minimum(startT);
  this->simple_regression->maximum(endT);
  this->simple_regression->step((endT - startT) / 100.0);
  //  this->simple_regression->value(startT);
  this->simple_regression->value(startT + ((endT - startT) / 2.0));

  this->mode->value(0);
  this->ComputeSimpleRegressionParameters();
}



void ShapeWorksViewApp::WritePCALoadings()
{
  // Get a file name
  Fl_File_Chooser chooser(m_CurrentDirectory.c_str(), ".csv",
                          Fl_File_Chooser::CREATE, "Write PCA Loadings");
  chooser.show();
  
  while(chooser.shown()) { Fl::wait(); }
  
  // User hit cancel?
  if ( chooser.value() == NULL )
    { return; }

  m_CurrentDirectory = std::string(chooser.directory());
  m_Stats.WriteCSVFile2(chooser.value());
  
}
void ShapeWorksViewApp::WritePoints()
{
  typedef itk::ParticlePositionWriter<3>::PointType PointType;
  
  // Get a file name
  Fl_File_Chooser chooser(m_CurrentDirectory.c_str(), "*",
                          Fl_File_Chooser::CREATE, "Write Points");
  chooser.show();
  
  while(chooser.shown()) { Fl::wait(); }
  
  // User hit cancel?
  if ( chooser.value() == NULL )
    { return; }

  m_CurrentDirectory = std::string(chooser.directory());
  
  PointType p;
  std::vector<PointType> plist;
  for (unsigned int i = 0; i < m_glyphPoints->GetNumberOfPoints(); i++)
    {
    p[0] =  m_glyphPoints->GetPoint(i)[0];
    p[1] =  m_glyphPoints->GetPoint(i)[1];
    p[2] =  m_glyphPoints->GetPoint(i)[2];
    plist.push_back(p);
    }
  
  itk::ParticlePositionWriter<3>::Pointer writer = itk::ParticlePositionWriter<3>::New();
  writer->SetFileName(chooser.value());
  writer->SetInput(plist);
  writer->Write();
}


void ShapeWorksViewApp::LoadScalars()
{

 // Get a file name
  Fl_File_Chooser chooser(m_CurrentDirectory.c_str(), "*",
                          Fl_File_Chooser::CREATE, "Load Scalars");
  chooser.show();
  
  while(chooser.shown()) { Fl::wait(); }
  
  // User hit cancel?
  if ( chooser.value() == NULL )
    { return; }

  m_CurrentDirectory = std::string(chooser.directory());

  
  int nPts = m_glyphPoints->GetNumberOfPoints() ;
  double data ;
  vtkFloatArray *scalars = vtkFloatArray::New() ;

  // THE FOLLOWING IS CODE FROM KWMESHVISU
  // read in the file
  std::ifstream attrFile ;
  attrFile.open ( chooser.value() ) ;
  
  // check the header
  int nPtsFile, nDim ;

  // This is a quite sloppy implementation of reading keyword/value pairs!
  bool found;
  char * valuePtr;
  char typeString[1001], line[1001];

  attrFile.seekg(0,std::ios::beg);
  found = false ;
  while ( !found && !attrFile.eof())
  { attrFile.getline ( line, 1000 ) ;
    if (line[0] != '#' && strstr ( line, "NUMBER_OF_POINTS" )) found = true;
  }
  valuePtr=strchr(line, '=');
  if (!valuePtr) return;
  valuePtr++;
  sscanf(valuePtr, " %d ", &nPtsFile);

  attrFile.seekg(0,std::ios::beg);
  found = false ;
  while ( !found && !attrFile.eof())
  { attrFile.getline ( line, 1000 ) ;
    if (line[0] != '#' && strstr ( line, "DIMENSION" )) found = true;
  }
  valuePtr=strchr(line, '=');
  if (!valuePtr) return;
  valuePtr++;
  sscanf(valuePtr, " %d ", &nDim);

  attrFile.seekg(0,std::ios::beg);
  found = false ;
  while ( !found && !attrFile.eof())
  { attrFile.getline ( line, 1000 ) ;
    if (line[0] != '#' && strstr ( line, "TYPE" )) found = true;
  }
  valuePtr=strchr(line, '=');
  if (!valuePtr) return;
  valuePtr++;
  sscanf(valuePtr, " %s ", typeString);

  assert ( nPtsFile == nPts ) ;
  assert ( nDim == 1 ) ;
  assert ( strcmp ( typeString, "Scalar" ) == 0 ) ;

  attrFile.seekg(0,std::ios::beg);
  const int numEntries = 3;
  int counter = 0;
  while ( counter < numEntries && !attrFile.eof())
  { attrFile.getline ( line, 1000 ) ;
    if ((line[0] != '#')) counter++;
  }

  for (int i = 0 ; i < nPts ; i++ )
  {
    attrFile >> data ;
    scalars->InsertTuple1 (i, data) ;
  }
  
  attrFile.close () ;
  // END KWMESHVISU CODE

  m_glyphPointset->GetPointData()->SetScalars(scalars);
  m_glyphMapper->SetLookupTable(m_PValueTFunc);
  //  m_glyphs->SetScaleModeToScaleByValue();

  
  scalars->Delete ();
  
 if (m_Initialized) this->m_render_window->Render();
  
}

void ShapeWorksViewApp::UpdateDifferenceLUT(float r0,float r1)
{
  this->m_differenceLUT->RemoveAllPoints () ;

  //  std::cout << "Min = " << r0 << " Max = " << r1 << std::endl;
  // Color
  const float yellow = 0.16666;
  const float blue   = 0.66666;
  const unsigned int res = 100;
  const float resinv = 1.0 / static_cast<float>(res);
  float maxrange;
  if (fabs(r0) > fabs(r1)) maxrange = fabs(r0);
  else maxrange = fabs(r1);
  
  const float pip = fabs(maxrange) * resinv;
  for (unsigned int i = 0; i < res; i++)
    {
    float fi = static_cast<float>(i);
    
    m_differenceLUT->AddHSVPoint(-maxrange + (fi*pip), yellow, 1.0 - (fi * resinv), 1.0);
    m_differenceLUT->AddHSVPoint(maxrange - (fi*pip), blue, 1.0 - (fi * resinv), 1.0);
    
    }
  
  m_differenceLUT->AddHSVPoint(0.0, 0.0, 0.0, 1.0);
}
