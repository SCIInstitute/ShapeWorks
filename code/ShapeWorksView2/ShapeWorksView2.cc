/*
 * Shapeworks license
 */

// standard includes
#include <iostream>
#include <sstream>

// qt
#include <QFileDialog>

#include <QTextStream>
#include <QMessageBox>

// vnl
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_matrix.h"

// vtk
#include <vtkActor.h>
#include <vtkAnnotatedCubeActor.h>
#include <vtkAppendPolyData.h>
#include <vtkArrowSource.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkIdList.h>
#include <vtkImageData.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageGradient.h>
#include <vtkImageWriter.h>
#include <vtkLookupTable.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPLYReader.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkUnsignedLongArray.h>
#include <vtkPointLocator.h>
#include <vtkPolyDataWriter.h>
#include <vtkSTLWriter.h>
#include <vtkScalarBarActor.h>
#include <vtkTextProperty.h>

#include <ShapeWorksView2.h>
#include <ui_ShapeWorksView2.h>
#include "tinyxml.h"
#include "CustomSurfaceReconstructionFilter.h"
#include <Preferences.h>

//---------------------------------------------------------------------------
ShapeWorksView2::ShapeWorksView2( int argc, char** argv )
{
  this->ui = new Ui_ShapeWorksView2;
  this->ui->setupUi( this );

  this->arrowsVisible = false;
  this->pcaAnimateDirection = true;
  this->regressionAnimateDirection = true;
  this->groupAnimateDirection = true;

#ifdef _WIN32
  // only want to do this on windows.  On apple, the default is better
  this->ui->tabWidget->setStyleSheet( QString( "QTabWidget::pane { border: 2px solid rgb( 80, 80, 80 ); }" ) );
#endif

  QSize size = Preferences::Instance().getSettings().value( "mainwindow/size", QSize( 1280, 720 ) ).toSize();
  this->resize( size );

  QObject::connect(
    &Preferences::Instance(), SIGNAL( colorSchemeChanged( int ) ),
    this, SLOT( colorSchemeChanged() ) );

  QObject::connect(
    &Preferences::Instance(), SIGNAL( glyphPropertiesChanged() ),
    this, SLOT( glyphPropertiesChanged() ) );

  QObject::connect(
    &Preferences::Instance(), SIGNAL( slidersChangedSignal() ),
    this, SLOT( handleSliderPreferencesChanged() ) );

  QObject::connect(
    &this->pcaAnimateTimer, SIGNAL( timeout() ),
    this, SLOT( handlePcaTimer() ) );

  QObject::connect(
    &this->groupAnimateTimer, SIGNAL( timeout() ),
    this, SLOT( handleGroupTimer() ) );

  QObject::connect(
    &this->regressionAnimateTimer, SIGNAL( timeout() ),
    this, SLOT( handleRegressionTimer() ) );

  this->setPregenSteps();
  this->updateSliders();

  if ( !this->readParameterFile( argv[1] ) )
  {
    exit( -1 );
  }

  // set to mean
  this->ui->tabWidget->setCurrentIndex( 0 );

  // Compute the linear regression
  this->regression = itk::ParticleShapeLinearRegressionMatrixAttribute<double, 3>::New();
  this->regression->SetMatrix( this->stats.ShapeMatrix() );

  // Load the explanatory variables
  this->readExplanatoryVariables( argv[1] );
  if ( this->regressionAvailable )
  {
    this->regression->ResizeParameters( stats.ShapeMatrix().rows() );
    this->regression->ResizeMeanMatrix( stats.ShapeMatrix().rows(), stats.ShapeMatrix().cols() );
    this->regression->Initialize();
    this->regression->EstimateParameters();
  }

  this->updateColorScheme();
  this->updateGlyphProperties();
  this->updateAnalysisMode();

  this->orientationMarkerWidget = this->createOrientationMarker();
  this->orientationMarkerWidget->SetInteractor( this->ui->view->GetRenderWindow()->GetInteractor() );
  this->orientationMarkerWidget->EnabledOn();
  this->orientationMarkerWidget->InteractiveOff();

  this->scalar_bar_actor_ = vtkSmartPointer<vtkScalarBarActor>::New();
  this->scalar_bar_actor_->SetTitle( "" );
  this->scalar_bar_actor_->SetLookupTable( this->differenceLUT );
  this->scalar_bar_actor_->SetOrientationToHorizontal();
  this->scalar_bar_actor_->SetMaximumNumberOfColors( 1000 );
  this->scalar_bar_actor_->GetPositionCoordinate()->SetCoordinateSystemToNormalizedViewport();
  this->scalar_bar_actor_->GetPositionCoordinate()->SetValue( .2, .05 );
  this->scalar_bar_actor_->SetWidth( 0.8 );
  this->scalar_bar_actor_->SetHeight( 0.1 );
  this->scalar_bar_actor_->SetPosition( 0.1, 0.01 );
  this->scalar_bar_actor_->SetLabelFormat( "%.0f" );
  this->scalar_bar_actor_->GetTitleTextProperty()->SetFontFamilyToArial();
  this->scalar_bar_actor_->GetTitleTextProperty()->SetFontSize( 12 );
  this->scalar_bar_actor_->GetLabelTextProperty()->SetFontFamilyToArial();
  this->scalar_bar_actor_->GetLabelTextProperty()->SetFontSize( 10 );
  this->scalar_bar_actor_->GetLabelTextProperty()->SetJustificationToCentered();
  //this->scalar_bar_actor_->GetLabelTextProperty()->SetColor( 0, 0, 0 );
  this->renderer->AddActor( this->scalar_bar_actor_ );
}

//---------------------------------------------------------------------------
ShapeWorksView2::~ShapeWorksView2()
{}



//---------------------------------------------------------------------------
void ShapeWorksView2::write_stats_file(QString filename)
{
  this->stats.WriteCSVFile2( filename.toStdString() );
}

/********************************************************************/
/* Qt event handling                                                */
/********************************************************************/

//---------------------------------------------------------------------------
void ShapeWorksView2::closeEvent( QCloseEvent* event )
{
  // close the preferences window in case it is open
  Preferences::Instance().closeWindow();

  // save the size of the window to preferences
  Preferences::Instance().getSettings().setValue( "mainwindow/size", this->size() );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_actionExportPcaLoadings_triggered()
{
  QString filename = QFileDialog::getSaveFileName( this, "Export PCA Loadings As... ",
                                                   QString(), "CSV files (*.csv)" );
  if ( filename.isEmpty() ) {return; }


  this->write_stats_file(filename);

  // write out eigenvectors
  /*

     std::cerr << this->stats.Eigenvectors().columns() << "\n";
     std::cerr << this->stats.Eigenvectors().rows() << "\n";

     // Write eigen csv file
     std::ofstream outfile;
     outfile.open( "eigen.csv" );

     for ( int c = 0; c < this->stats.Eigenvectors().columns(); c++ )
     {
     for ( int r = 0; r < this->stats.Eigenvectors().rows(); r++ )
     {
     outfile << this->stats.Eigenvectors()(r,c) << ",";
     }
     outfile << "\n";
     }

     outfile.close();
   */
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_actionExportPoints_triggered()
{
  QString filename = QFileDialog::getSaveFileName( this, "Export Points As... ",
                                                   QString(), QString() );
  if ( filename.isEmpty() ) {return; }

  itk::ParticlePositionReader<3>::PointType p;
  std::vector<itk::ParticlePositionReader<3>::PointType> plist;
  for ( unsigned int i = 0; i < this->glyphPoints->GetNumberOfPoints(); i++ )
  {
    p[0] = this->glyphPoints->GetPoint( i )[0];
    p[1] = this->glyphPoints->GetPoint( i )[1];
    p[2] = this->glyphPoints->GetPoint( i )[2];
    plist.push_back( p );
  }

  itk::ParticlePositionWriter<3>::Pointer writer = itk::ParticlePositionWriter<3>::New();
  writer->SetFileName( filename.toStdString() );
  writer->SetInput( plist );
  writer->Write();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_actionExportSurfaceMesh_triggered()
{
  bool seperateFilesForDomains = false;
  if ( this->numDomains > 1 )
  {
    if ( QMessageBox::Yes == QMessageBox::question( this, "Write domains in separate files?",
                                                    "Multiple domains are present, would you like them written in separate files?",
                                                    QMessageBox::Yes | QMessageBox::No ) )
    {
      seperateFilesForDomains = true;
    }
  }

  QString filename = QFileDialog::getSaveFileName( this, "Export Surface Mesh As... ",
                                                   QString(), "VTK files (*.vtk);;STL files (*.stl)" );
  if ( filename.isEmpty() ) {return; }

  if ( seperateFilesForDomains )
  {
    for ( int i = 0; i < this->numDomains; i++ )
    {
      QFileInfo fi( filename );
      QString base = fi.baseName();

      if ( fi.suffix() == "stl" )
      {
        QString domainFilename = fi.dir().path() + QDir::separator() + base + QString::number( i + 1 ) + ".stl";
        vtkSmartPointer<vtkSTLWriter> stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
        stlWriter->SetInputData( this->surfaceMappers[i]->GetInput() );
        stlWriter->SetFileName( domainFilename.toStdString().c_str() );
        stlWriter->Write();
      }
      else
      {
        QString domainFilename = fi.dir().path() + QDir::separator() + base + QString::number( i + 1 ) + ".vtk";

        vtkSmartPointer<vtkPolyDataWriter> surfaceWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
        surfaceWriter->SetInputData( this->surfaceMappers[i]->GetInput() );
        surfaceWriter->SetFileName( domainFilename.toStdString().c_str() );
        surfaceWriter->Write();
      }
    }
  }
  else
  {
    // combine polydata from each domain
    vtkSmartPointer<vtkAppendPolyData> appendPolyData = vtkSmartPointer<vtkAppendPolyData>::New();
    for ( int i = 0; i < this->numDomains; i++ )
    {
      appendPolyData->AddInputData( this->surfaceMappers[i]->GetInput() );
    }

    QFileInfo fi( filename );
    if ( fi.suffix() == "stl" )
    {
      vtkSmartPointer<vtkSTLWriter> stlWriter = vtkSmartPointer<vtkSTLWriter>::New();
      stlWriter->SetInputData( appendPolyData->GetOutput() );
      stlWriter->SetFileName( filename.toStdString().c_str() );
      stlWriter->Write();
    }
    else
    {
      vtkSmartPointer<vtkPolyDataWriter> surfaceWriter = vtkSmartPointer<vtkPolyDataWriter>::New();
      surfaceWriter->SetInputData( appendPolyData->GetOutput() );
      surfaceWriter->SetFileName( filename.toStdString().c_str() );
      surfaceWriter->Write();
    }
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_actionExportVarianceReport_triggered()
{
  QString filename = QFileDialog::getSaveFileName( this, "Export Variance Report As... ",
                                                   QString(), "CSV files (*.csv)" );
  if ( filename.isEmpty() ) {return; }

  // Write variance report as a csv file
  std::ofstream outfile;
  outfile.open( filename.toStdString().c_str() );
  outfile << "pca mode, variance, percent variance, sum percent\n";

  double totalVariance = 0;
  for ( int c = 0; c < this->stats.Eigenvectors().columns(); c++ )
  {
    totalVariance += this->stats.Eigenvalues()[this->stats.Eigenvectors().columns() - ( c + 1 )];
  }

  double sum_variance = 0;
  for ( int c = 0; c < this->stats.Eigenvectors().columns(); c++ )
  {
    double variance = this->stats.Eigenvalues()[this->stats.Eigenvectors().columns() - ( c + 1 )];
    sum_variance += variance;
    outfile << c << ",";
    outfile << variance << ",";
    outfile << variance / totalVariance * 100.0 << "%,";
    outfile << sum_variance / totalVariance * 100.0 << "%";
    outfile << "\n";
  }

  outfile.close();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_actionExportEigenvectors_triggered()
{
  QString filename = QFileDialog::getSaveFileName( this, "Export Eigenvectors As... ",
                                                   QString(), "CSV files (*.csv)" );
  if ( filename.isEmpty() ) {return; }

  std::cerr << this->stats.Eigenvectors().columns() << "\n";
  std::cerr << this->stats.Eigenvectors().rows() << "\n";

  // Write out eigenvectors
  std::ofstream outfile;
  outfile.open( filename.toStdString().c_str() );

  for ( int c = 0; c < this->stats.Eigenvectors().columns(); c++ )
  {
    for ( int r = 0; r < this->stats.Eigenvectors().rows(); r++ )
    {
      outfile << this->stats.Eigenvectors() ( r, c ) << ",";
    }
    outfile << "\n";
  }

  outfile.close();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_actionImportScalarValues_triggered()
{
  QString filename = QFileDialog::getOpenFileName( this, "Import Scalar Values from... ",
                                                   QString(), "CSV files (*.csv)" );

  if ( filename.isEmpty() ) { return; }

  std::ifstream infile;
  infile.open( filename.toStdString().c_str() );

  scalarValues.clear();
  for ( int i = 0; i < this->numPoints; i++ )
  {
    float value;
    infile >> value;
//    std::cerr << value << "\n";
    scalarValues.push_back( value );
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Step 1. Assign values at each correspondence point based on the image gradient
  /////////////////////////////////////////////////////////////////////////////////

  // Dot product difference vectors with the surface normals.
  vtkSmartPointer<vtkFloatArray> magnitudes = vtkSmartPointer<vtkFloatArray>::New();
  magnitudes->SetNumberOfComponents( 1 );

  vtkSmartPointer<vtkFloatArray> vectors = vtkSmartPointer<vtkFloatArray>::New();
  vectors->SetNumberOfComponents( 3 );

  float maxValue = 0;
  for ( unsigned int i = 0; i < this->glyphPoints->GetNumberOfPoints(); i++ )
  {

    float v = this->scalarValues[i];
    vectors->InsertNextTuple3( v, v, v );
    magnitudes->InsertNextTuple1( v );
    if ( v > maxValue )
    {
      maxValue = v;
    }
  }

  this->updateDifferenceLUT( 0, maxValue );

  /////////////////////////////////////////////////////////////////////////////////
  // Step 2. Assign values at each mesh point based on the closest correspondence points
  this->computeSurfaceDifferences( magnitudes, vectors );
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  // Step 3. Assign the vectors and magnitudes to the glyphs and surface
  /////////////////////////////////////////////////////////////////////////////////

  // assign glyph vectors and magnitudes
  this->glyphPointSet->GetPointData()->SetVectors( vectors );
  this->glyphPointSet->GetPointData()->SetScalars( magnitudes );

  this->glyphs->SetSourceConnection( this->arrowSource->GetOutputPort() );
  this->glyphs->SetScaleModeToScaleByVector();

  // update glyph rendering
  this->glyphMapper->SetLookupTable( this->differenceLUT );
  this->arrowGlyphMapper->SetLookupTable( this->differenceLUT );
  this->arrowsVisible = true;

  if ( this->ui->showGlyphs->isChecked() )
  {
    this->renderer->AddActor( this->arrowGlyphActor );
  }

  // update surface rendering
  for ( int i = 0; i < this->numDomains; i++ )
  {
    this->surfaceMappers[i]->SetLookupTable( this->differenceLUT );
    this->surfaceMappers[i]->InterpolateScalarsBeforeMappingOn();
    this->surfaceMappers[i]->SetColorModeToMapScalars();
    this->surfaceMappers[i]->ScalarVisibilityOn();
  }

  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_actionLoadScalars_triggered()
{
  QString filename = QFileDialog::getOpenFileName( this, "Load Scalars... ",
                                                   QString(), "CSV files (*.csv)" );
  if ( filename.isEmpty() ) {return; }

  QFile* file = new QFile( filename );

  if ( !file->open( QIODevice::ReadOnly ) )
  {
    std::cerr << "Error opening file for reading\n";
  }

  QTextStream ts( file );

  this->scalars.clear();

  while ( !ts.atEnd() )
  {
    QString str;
    ts >> str;
    if ( str != "" )
    {
      this->scalars.append( str.toDouble() );
    }
  }
  file->close();

  std::cerr << "found " << this->scalars.size() << " scalars\n";

  if ( this->scalars.size() != this->numPoints )
  {
    QString message = "Error: number of scalars does not match number of points ("
                      + QString::number( this->scalars.size() ) + " != " + QString::number( ( this->numPoints ) ) + ")";
    QMessageBox::critical( this, "Number of points mismatch", message );
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_actionQuit_triggered()
{
  this->close();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_actionPreferences_triggered()
{
  Preferences::Instance().showWindow();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_tabWidget_currentChanged()
{
  this->updateAnalysisMode();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_meanOverallButton_clicked()
{
  this->updateAnalysisMode();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_meanGroup1Button_clicked()
{
  this->updateAnalysisMode();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_meanGroup2Button_clicked()
{
  this->updateAnalysisMode();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_meanDifferenceButton_clicked()
{
  this->updateAnalysisMode();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_sampleSpinBox_valueChanged()
{
  // this will make the UI appear more responsive
  QCoreApplication::processEvents();

  this->updateAnalysisMode();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_medianButton_clicked()
{
  int sampleNumber = this->stats.ComputeMedianShape( -32 );       // -32 means use both groups
  this->ui->sampleSpinBox->setValue( sampleNumber );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_medianGroup1Button_clicked()
{
  int sampleNumber = this->stats.ComputeMedianShape( 1 );
  this->ui->sampleSpinBox->setValue( sampleNumber );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_medianGroup2Button_clicked()
{
  int sampleNumber = this->stats.ComputeMedianShape( 2 );
  this->ui->sampleSpinBox->setValue( sampleNumber );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_pcaSlider_valueChanged()
{
  // this will make the slider handle redraw making the UI appear more responsive
  QCoreApplication::processEvents();

  this->computeModeShape();
  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_pcaModeSpinBox_valueChanged()
{
  this->computeModeShape();
  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_pcaGroupSlider_valueChanged()
{
  // this will make the UI appear more responsive
  QCoreApplication::processEvents();

  this->computeModeShape();
  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_pcaAnimateCheckBox_stateChanged()
{
  if ( this->ui->pcaAnimateCheckBox->isChecked() )
  {
    this->setPregenSteps();
    this->pcaAnimateTimer.setInterval( 10 );
    this->pcaAnimateTimer.start();
  }
  else
  {
    this->pcaAnimateTimer.stop();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_pcaGroupAnimateCheckBox_stateChanged()
{
  if ( this->ui->pcaGroupAnimateCheckBox->isChecked() )
  {
    this->setPregenSteps();
    this->groupAnimateTimer.setInterval( 10 );
    this->groupAnimateTimer.start();
  }
  else
  {
    this->groupAnimateTimer.stop();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::handlePcaTimer()
{
  int value = this->ui->pcaSlider->value();
  if ( this->pcaAnimateDirection )
  {
    value += this->ui->pcaSlider->singleStep();
  }
  else
  {
    value -= this->ui->pcaSlider->singleStep();
  }

  if ( value >= this->ui->pcaSlider->maximum() || value <= this->ui->pcaSlider->minimum() )
  {
    this->pcaAnimateDirection = !this->pcaAnimateDirection;
    this->setPregenSteps();
  }

  this->ui->pcaSlider->setValue( value );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::handleGroupTimer()
{
  int value = this->ui->pcaGroupSlider->value();
  if ( this->groupAnimateDirection )
  {
    value += this->ui->pcaGroupSlider->singleStep();
  }
  else
  {
    value -= this->ui->pcaGroupSlider->singleStep();
  }

  if ( value >= this->ui->pcaGroupSlider->maximum() || value <= this->ui->pcaGroupSlider->minimum() )
  {
    this->groupAnimateDirection = !this->groupAnimateDirection;
    this->setPregenSteps();
  }

  this->ui->pcaGroupSlider->setValue( value );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_regressionSlider_valueChanged()
{
  this->ui->regressionLabel->setText( QString::number( this->getRegressionValue( this->ui->regressionSlider->value() ) ) );

  if ( this->ui->tabWidget->currentWidget() != this->ui->regressionTab )
  {
    return;
  }

  // this will make the UI appear more responsive
  QCoreApplication::processEvents();

  this->computeRegressionShape();
  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_regressionAnimateCheckBox_stateChanged()
{
  if ( this->ui->regressionAnimateCheckBox->isChecked() )
  {
    this->setPregenSteps();
    this->regressionAnimateTimer.setInterval( 10 );
    this->regressionAnimateTimer.start();
  }
  else
  {
    this->regressionAnimateTimer.stop();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::handleRegressionTimer()
{
  int value = this->ui->regressionSlider->value();
  if ( this->regressionAnimateDirection )
  {
    value += this->ui->regressionSlider->singleStep();
  }
  else
  {
    value -= this->ui->regressionSlider->singleStep();
  }

  if ( value >= this->ui->regressionSlider->maximum() || value <= this->ui->regressionSlider->minimum() )
  {
    this->regressionAnimateDirection = !this->regressionAnimateDirection;
    this->setPregenSteps();
  }

  this->ui->regressionSlider->setValue( value );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_showGlyphs_stateChanged()
{
  this->updateActors();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_showSurface_stateChanged()
{
  this->updateActors();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_usePowerCrustCheckBox_stateChanged()
{
  this->updateSurfaceSettings();
  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_neighborhoodSpinBox_valueChanged()
{
  this->updateSurfaceSettings();
  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_spacingSpinBox_valueChanged()
{
  this->updateSurfaceSettings();
  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::on_smoothingSlider_valueChanged()
{
  this->updateSurfaceSettings();
  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::colorSchemeChanged()
{
  this->updateColorScheme();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::glyphPropertiesChanged()
{
  this->updateGlyphProperties();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::handleSliderPreferencesChanged()
{
  this->updateSliders();
}

/********************************************************************/
/* private methods                                                  */
/********************************************************************/

//---------------------------------------------------------------------------
void ShapeWorksView2::initializeRenderer()
{
  // Set up renderer and interactor.
  this->renderer = vtkSmartPointer<vtkRenderer>::New();

  this->ui->view->GetRenderWindow()->AddRenderer( this->renderer );

  vnl_vector<double> col( 3 );
  //col[0] = 255;
  //col[1] = 239;
  //col[2] = 213;
  col[0] = 255;
  col[1] = 239;
  col[2] = 213;
  col.normalize();
  this->renderer->SetBackground( col[0], col[1], col[2] );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::initializeGlyphs()
{
  this->lut = vtkSmartPointer<vtkLookupTable>::New();
  this->sphereSource = vtkSmartPointer<vtkSphereSource>::New();

  this->glyphPoints = vtkSmartPointer<vtkPoints>::New();
  this->glyphPoints->SetDataTypeToDouble();
  this->glyphPointSet = vtkSmartPointer<vtkPolyData>::New();
  this->glyphPointSet->SetPoints( this->glyphPoints );
  this->glyphPointSet->GetPointData()->SetScalars( vtkSmartPointer<vtkUnsignedLongArray>::New() );

  this->glyphs = vtkSmartPointer<vtkGlyph3D>::New();
  this->glyphs->SetInputData( this->glyphPointSet );
  this->glyphs->ScalingOn();
  this->glyphs->ClampingOff();
  this->glyphs->SetScaleModeToDataScalingOff();
  this->glyphs->SetSourceConnection( this->sphereSource->GetOutputPort() );

  this->glyphMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
  this->glyphMapper->SetInputConnection( this->glyphs->GetOutputPort() );
  this->glyphMapper->SetLookupTable( this->lut );

  this->glyphActor = vtkSmartPointer<vtkActor>::New();
  this->glyphActor->GetProperty()->SetSpecularColor( 1.0, 1.0, 1.0 );
  this->glyphActor->GetProperty()->SetDiffuse( 0.8 );
  this->glyphActor->GetProperty()->SetSpecular( 0.3 );
  this->glyphActor->GetProperty()->SetSpecularPower( 10.0 );
  this->glyphActor->SetMapper( this->glyphMapper );

  this->arrowFlipFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  this->arrowGlyphs = vtkGlyph3D::New();
  this->arrowGlyphMapper = vtkPolyDataMapper::New();
  this->arrowGlyphActor = vtkActor::New();

  /*this->pValueTFunc = vtkSmartPointer<vtkColorTransferFunction>::New();
     this->pValueTFunc->SetColorSpaceToHSV();
     this->pValueTFunc->AddHSVPoint( 0, 0, 1, 1 );
     this->pValueTFunc->AddHSVPoint( 0.05, 1, .40, 1 );
     this->pValueTFunc->AddHSVPoint( 0.05 + 0.00001, 1, 0.0, 1 );
     this->pValueTFunc->AddHSVPoint( 1, 1, 0.0, 1 );*/

  this->differenceLUT = vtkSmartPointer<vtkColorTransferFunction>::New();
  //this->differenceLUT->SetColorSpaceToHSV();

  // Arrow glyphs
  this->arrowSource = vtkSmartPointer<vtkArrowSource>::New();
  this->arrowSource->SetTipResolution( 6 );
  this->arrowSource->SetShaftResolution( 6 );

  vtkSmartPointer<vtkTransform> t1 = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> t2 = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> t3 = vtkSmartPointer<vtkTransform>::New();
  vtkSmartPointer<vtkTransform> t4 = vtkSmartPointer<vtkTransform>::New();
  t1->Translate( -0.5, 0.0, 0.0 );
  t2->RotateY( 180 );
  t3->Translate( 0.5, 0.0, 0.0 );
  t4->RotateY( 180 );
  t3->Concatenate( t4 );
  t2->Concatenate( t3 );
  t1->Concatenate( t2 );
  this->transform180 = vtkSmartPointer<vtkTransform>::New();
  this->transform180->Concatenate( t1 );

  this->arrowFlipFilter->SetTransform( this->transform180 );
  this->arrowFlipFilter->SetInputConnection( this->arrowSource->GetOutputPort() );

  this->arrowGlyphs->SetSourceConnection( this->arrowFlipFilter->GetOutputPort() );
  this->arrowGlyphs->SetInputData( this->glyphPointSet );
  this->arrowGlyphs->ScalingOn();
  this->arrowGlyphs->ClampingOff();

  this->arrowGlyphs->SetVectorModeToUseVector();
  this->arrowGlyphs->SetScaleModeToScaleByVector();

  this->arrowGlyphMapper->SetInputConnection( this->arrowGlyphs->GetOutputPort() );

  this->arrowGlyphActor->GetProperty()->SetSpecularColor( 1.0, 1.0, 1.0 );
  this->arrowGlyphActor->GetProperty()->SetDiffuse( 0.8 );
  this->arrowGlyphActor->GetProperty()->SetSpecular( 0.3 );
  this->arrowGlyphActor->GetProperty()->SetSpecularPower( 10.0 );
  this->arrowGlyphActor->SetMapper( this->arrowGlyphMapper );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::initializeSurfaces()
{
  this->surfaceMappers.resize( this->numDomains );
  this->surfaceActors.resize( this->numDomains );

  for ( int i = 0; i < this->numDomains; i++ )
  {
    this->surfaceMappers[i] = vtkSmartPointer<vtkPolyDataMapper>::New();
    this->surfaceMappers[i]->ScalarVisibilityOff();

    this->surfaceActors[i] = vtkSmartPointer<vtkActor>::New();
    this->surfaceActors[i]->SetMapper( this->surfaceMappers[i] );
    //this->surfaceActors[i]->GetProperty()->SetSpecular( .4 );
    //this->surfaceActors[i]->GetProperty()->SetSpecularPower( 25 );
    this->surfaceActors[i]->GetProperty()->SetSpecular( .2 );
    this->surfaceActors[i]->GetProperty()->SetSpecularPower( 15 );
    //this->surfaceActors[i]->GetProperty()->SetRepresentationToWireframe();
    //this->surfaceActors[i]->GetProperty()->SetOpacity( 0.5 + ( ( 1 - i ) * 0.5 ) );
  }
  this->updateSurfaceSettings();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::updateAnalysisMode()
{
  // update UI
  this->ui->meanGroup1Button->setVisible( this->groupsAvailable );
  this->ui->meanGroup2Button->setVisible( this->groupsAvailable );
  this->ui->medianGroup1Button->setVisible( this->groupsAvailable );
  this->ui->medianGroup2Button->setVisible( this->groupsAvailable );
  this->ui->pcaGroup1Label->setVisible( this->groupsAvailable );
  this->ui->pcaGroup2Label->setVisible( this->groupsAvailable );
  this->ui->pcaGroupSlider->setVisible( this->groupsAvailable );
  this->ui->pcaGroupAnimateCheckBox->setVisible( this->groupsAvailable );

  this->ui->tabWidget->setTabEnabled( 3, this->regressionAvailable );

  this->ui->pcaAnimateCheckBox->setChecked( false );
  this->ui->pcaGroupAnimateCheckBox->setChecked( false );
  this->ui->regressionAnimateCheckBox->setChecked( false );

  // update tabs for hiding/showing
  this->ui->pcaGridLayout->invalidate();

  // this will make the UI appear more responsive
  QCoreApplication::processEvents();

#ifndef SW_USE_POWERCRUST
  this->ui->powerCrustLabel->hide();
  this->ui->usePowerCrustCheckBox->hide();
#endif

  // switch back to spheres
  this->displaySpheres();

  if ( this->ui->tabWidget->currentWidget() == this->ui->meanTab )
  {
    if ( this->scalars.size() == this->numPoints )
    {
      this->displayShape( this->stats.Mean() );
      this->displayScalars();
    }
    else if ( this->ui->meanGroup1Button->isChecked() )
    {
      this->displayShape( this->stats.Group1Mean() );
    }
    else if ( this->ui->meanGroup2Button->isChecked() )
    {
      this->displayShape( this->stats.Group2Mean() );
    }
    else if ( this->ui->meanDifferenceButton->isChecked() )
    {
      this->displayShape( this->stats.Mean() );
      this->displayMeanDifference();
    }
    else
    {
      this->displayShape( this->stats.Mean() );
    }
  }
  else if ( this->ui->tabWidget->currentWidget() == this->ui->samplesTab )
  {
    int sampleNumber = this->ui->sampleSpinBox->value();

    // pre-generate
    for ( int step = 0; step < this->pregenSteps.size(); step++ )
    {
      int pregenSample = sampleNumber + this->pregenSteps[step];
      if ( pregenSample >= this->ui->sampleSpinBox->minimum() && pregenSample <= this->ui->sampleSpinBox->maximum() )
      {
        vnl_vector<double> shape = this->stats.ShapeMatrix().get_column( pregenSample );
        for ( int i = 0; i < this->numDomains; i++ )
        {
          this->meshManager.generateMesh( this->getDomainShape( shape, i ) );
        }
      }
    }

    this->displayShape( this->stats.ShapeMatrix().get_column( sampleNumber ) );
  }
  else if ( this->ui->tabWidget->currentWidget() == this->ui->pcaTab )
  {
    this->computeModeShape();
  }
  else if ( this->ui->tabWidget->currentWidget() == this->ui->regressionTab )
  {
    this->computeRegressionShape();
  }

  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::updateSurfaceSettings()
{
  this->meshManager.setNeighborhoodSize( this->ui->neighborhoodSpinBox->value() );
  this->meshManager.setSampleSpacing( this->ui->spacingSpinBox->value() );
  float smoothingAmount = (float)this->ui->smoothingSlider->value() / (float)this->ui->smoothingSlider->maximum() * 100.0f;
  this->meshManager.setSmoothingAmount( smoothingAmount );
  this->meshManager.setUsePowerCrust( this->ui->usePowerCrustCheckBox->isChecked() );

  bool powercrust = this->ui->usePowerCrustCheckBox->isChecked();

  // update UI
  //this->ui->neighborhoodSpinBox->setEnabled( !powercrust );
  //this->ui->spacingSpinBox->setEnabled( !powercrust );
  //this->ui->smoothingSlider->setEnabled( !powercrust );

  this->displayShape( this->currentShape );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::updateActors()
{
  this->renderer->RemoveActor( this->glyphActor );
  this->renderer->RemoveActor( this->arrowGlyphActor );

  for ( int i = 0; i < this->numDomains; i++ )
  {
    this->renderer->RemoveActor( this->surfaceActors[i] );
  }

  if ( this->ui->showGlyphs->isChecked() )
  {
    this->renderer->AddActor( this->glyphActor );
    if ( this->arrowsVisible )
    {
      this->renderer->AddActor( this->arrowGlyphActor );
    }
  }

  if ( this->ui->showSurface->isChecked() )
  {
    for ( int i = 0; i < this->numDomains; i++ )
    {
      this->renderer->AddActor( this->surfaceActors[i] );
    }
  }

  this->displayShape( this->currentShape );

  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::updateColorScheme()
{
  int scheme = Preferences::Instance().getColorScheme();

  for ( int i = 0; i < this->numDomains; i++ )
  {
    int domainScheme = ( scheme + i ) % m_ColorSchemes.size();
    this->surfaceActors[i]->GetProperty()->SetDiffuseColor( m_ColorSchemes[domainScheme].foreground.r,
                                                            m_ColorSchemes[domainScheme].foreground.g,
                                                            m_ColorSchemes[domainScheme].foreground.b );
  }

/*
   this->RecolorGlyphs(m_ColorSchemes[scheme].alt.r,
    m_ColorSchemes[scheme].alt.g,
    m_ColorSchemes[scheme].alt.b);
 */
  this->renderer->SetBackground( m_ColorSchemes[scheme].background.r,
                                 m_ColorSchemes[scheme].background.g,
                                 m_ColorSchemes[scheme].background.b );

  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::updateGlyphProperties()
{
  float size = Preferences::Instance().getGlyphSize();
  float quality = Preferences::Instance().getGlyphQuality();

  this->glyphs->SetScaleFactor( size );
  this->arrowGlyphs->SetScaleFactor( size );

  this->sphereSource->SetThetaResolution( quality );
  this->sphereSource->SetPhiResolution( quality );

  this->arrowSource->SetTipResolution( quality );
  this->arrowSource->SetShaftResolution( quality );

  this->glyphs->Update();
  this->arrowGlyphs->Update();

  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::updateSliders()
{
  int numPcaSteps = Preferences::Instance().getNumPcaSteps();

  if ( numPcaSteps % 2 == 0 )
  {
    numPcaSteps++;
  }
  int halfRange = ( numPcaSteps - 1 ) / 2;

  this->ui->pcaSlider->setMinimum( -halfRange );
  this->ui->pcaSlider->setMaximum( halfRange );
  this->ui->pcaSlider->setTickInterval( halfRange / 2 );
  this->ui->pcaSlider->setSingleStep( 1 );
  this->ui->pcaSlider->setPageStep( 1 );

  // regression slider
  int numRegressionSteps = Preferences::Instance().getNumRegressionSteps();
  this->ui->regressionSlider->setMinimum( 0 );
  this->ui->regressionSlider->setMaximum( numRegressionSteps );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::redraw()
{
  this->renderer->GetRenderWindow()->Render();
}

//---------------------------------------------------------------------------
bool ShapeWorksView2::readParameterFile( char* filename )
{
  // Read parameter file
  TiXmlDocument doc( filename );
  bool loadOkay = doc.LoadFile();
  if ( !loadOkay )
  {
    std::cerr << "Error: Invalid parameter file" << std::endl;
    return false;
  }

  TiXmlHandle docHandle( &doc );
  std::istringstream inputsBuffer;

  this->groupsAvailable = ( docHandle.FirstChild( "group_ids" ).Element() != NULL );

  // number of domains (objects) per patient/shape
  this->numDomains = 1;
  TiXmlElement* elem = docHandle.FirstChild( "domains_per_shape" ).Element();
  if ( elem ) {this->numDomains = atoi( elem->GetText() ); }

  // Run statistics
  this->stats.ReadPointFiles( filename );
  this->stats.ComputeModes();
  this->stats.PrincipalComponentProjections();

  this->numPoints = this->stats.Mean().size() / 3;

  this->pointsPerDomain = numPoints / this->numDomains;

  this->numSamples = this->stats.ShapeMatrix().cols();

  this->initializeRenderer();
  this->initializeGlyphs();
  this->initializeSurfaces();

  // Create numPoints glyphs
  for ( unsigned int i = 0; i < this->numPoints; i++ )
  {
    this->glyphs->SetRange( 0.0, (double) i + 1 );
    this->glyphMapper->SetScalarRange( 0.0, (double) i + 1.0 );
    this->lut->SetNumberOfTableValues( i + 1 );
    this->lut->SetTableRange( 0.0, (double)i + 1.0 );

    ( (vtkUnsignedLongArray*)( this->glyphPointSet->GetPointData()->GetScalars() ) )->InsertValue( i, i );

    double x = 0.0;
    double y = 0.0;
    double z = 0.0;

    this->glyphPoints->InsertPoint( i, x, y, z );
  }

  this->glyphPoints->Modified();

  // Set the color modes
  this->arrowGlyphs->SetColorModeToColorByScalar();
  this->glyphs->SetColorModeToColorByScalar();
  this->glyphMapper->SetColorModeToMapScalars();
  this->arrowGlyphMapper->SetColorModeToMapScalars();
  this->glyphMapper->ScalarVisibilityOn();
  this->arrowGlyphMapper->ScalarVisibilityOn();

  this->displayShape( this->stats.Mean() );

  this->ui->sampleSpinBox->setMinimum( 0 );
  this->ui->sampleSpinBox->setMaximum( this->numSamples - 1 );

  this->updateActors();

  return true;
}

//---------------------------------------------------------------------------
bool ShapeWorksView2::readExplanatoryVariables( char* filename )
{
  TiXmlDocument doc( filename );
  bool loadOkay = doc.LoadFile();

  TiXmlHandle docHandle( &doc );
  TiXmlElement* elem;

  std::istringstream inputBuffer;
  std::vector<double> evars;
  double startT, endT;
  startT = 1.0e16;
  endT = -1.0e16;
  double etmp = 0.0;

  elem = docHandle.FirstChild( "explanatory_variable" ).Element();
  if ( elem )
  {
    inputBuffer.str( elem->GetText() );
    while ( inputBuffer >> etmp )
    {
      if ( etmp > endT ) {endT = ceil( etmp ); }
      if ( etmp < startT ) {startT = floor( etmp ); }

      evars.push_back( etmp );
    }
    inputBuffer.clear();
    inputBuffer.str( "" );
    this->regressionAvailable = true;
  }
  else
  {
    this->regressionAvailable = false;
    return false;
  }

  // Hide simple regression functionality unless parameters have been supplied
  // by the user and there enough explanatory vars to specify a range (needs
  // 2).  This assumes ReadSimpleRegressionParameters has been already been
  // called.
  if ( evars.size() < 2 || ( evars.size() < this->numSamples ) )
  {
    this->regressionAvailable = false;
    return false;
  }

  this->regression->SetExplanatory( evars );

  startT = 2 * startT;
  endT = 2 * endT;

  std::cerr << "startT = " << startT << "\n";
  std::cerr << "endT = " << endT << "\n";

  // Initialize range of explanatory variable.
  this->regressionMin = startT;
  this->regressionMax = endT;
  this->regressionRange = endT - startT;
  double middle = startT + ( ( endT - startT ) / 2.0 );
  this->ui->regressionSlider->setValue( ( middle - this->regressionMin ) / this->regressionRange * this->ui->regressionSlider->maximum() );

/*
   this->simple_regression->minimum(startT);
   this->simple_regression->maximum(endT);
   this->simple_regression->step((endT - startT) / 100.0);
   //  this->simple_regression->value(startT);
   this->simple_regression->value(startT + ((endT - startT) / 2.0));
 */
//this->mode->value(0);
//this->ComputeSimpleRegressionParameters();
  return true;
}

//---------------------------------------------------------------------------
void ShapeWorksView2::displayShape( const vnl_vector<double> &shape )
{
  if ( shape.size() == 0 )
  {
    return;
  }

  // make a copy of the shape
  this->currentShape = shape;

  // assign shape points to the glyph point set
  unsigned int k = 0;
  for ( unsigned int i = 0; i < this->stats.ShapeMatrix().rows() / 3; i++ )
  {
    double x = shape[k++];
    double y = shape[k++];
    double z = shape[k++];
    this->glyphPoints->SetPoint( i, x, y, z );
  }
  this->glyphPoints->Modified();

  if ( this->surfaceActors[0] && this->ui->showSurface->isChecked() )
  {
    for ( int i = 0; i < this->numDomains; i++ )
    {
      vtkSmartPointer<vtkPolyData> polyData = this->meshManager.getMesh( this->getDomainShape( shape, i ) );

      // retrieve the mesh and set it for display
      this->surfaceMappers[i]->SetInputData( polyData );
    }
  }

  if ( this->arrowsVisible )
  {
    this->displayMeanDifference();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::displayVectorField(
  const std::vector<itk::ParticlePositionReader<3>::PointType > &vecs )
{
  if ( vecs.size() < this->glyphPoints->GetNumberOfPoints() )
  {
    std::cerr << "Error: not enough vectors" << std::endl;
    return;
  }

  /////////////////////////////////////////////////////////////////////////////////
  // Step 1. Assign values at each correspondence point based on the image gradient
  /////////////////////////////////////////////////////////////////////////////////

  // Dot product difference vectors with the surface normals.
  vtkSmartPointer<vtkFloatArray> magnitudes = vtkSmartPointer<vtkFloatArray>::New();
  magnitudes->SetNumberOfComponents( 1 );

  vtkSmartPointer<vtkFloatArray> vectors = vtkSmartPointer<vtkFloatArray>::New();
  vectors->SetNumberOfComponents( 3 );

  this->computePointDifferences( vecs, magnitudes, vectors );

  /////////////////////////////////////////////////////////////////////////////////
  // Step 2. Assign values at each mesh point based on the closest correspondence points
  this->computeSurfaceDifferences( magnitudes, vectors );
  /////////////////////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////////////////////
  // Step 3. Assign the vectors and magnitudes to the glyphs and surface
  /////////////////////////////////////////////////////////////////////////////////

  // assign glyph vectors and magnitudes
  this->glyphPointSet->GetPointData()->SetVectors( vectors );
  this->glyphPointSet->GetPointData()->SetScalars( magnitudes );

  this->glyphs->SetSourceConnection( this->arrowSource->GetOutputPort() );
  this->glyphs->SetScaleModeToScaleByVector();

  // update glyph rendering
  this->glyphMapper->SetLookupTable( this->differenceLUT );
  this->arrowGlyphMapper->SetLookupTable( this->differenceLUT );
  this->arrowsVisible = true;

  if ( this->ui->showGlyphs->isChecked() )
  {
    this->renderer->AddActor( this->arrowGlyphActor );
  }

  // update surface rendering
  for ( int i = 0; i < this->numDomains; i++ )
  {
    this->surfaceMappers[i]->SetLookupTable( this->differenceLUT );
    this->surfaceMappers[i]->InterpolateScalarsBeforeMappingOn();
    this->surfaceMappers[i]->SetColorModeToMapScalars();
    this->surfaceMappers[i]->ScalarVisibilityOn();
  }

  this->redraw();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::computePointDifferences(
  const std::vector<itk::ParticlePositionReader<3>::PointType > &vecs,
  vtkSmartPointer<vtkFloatArray> magnitudes, vtkSmartPointer<vtkFloatArray> vectors )
{
  double minmag = 1.0e20;
  double maxmag = 0.0;

  for ( int domain = 0; domain < this->numDomains; domain++ )
  {
    vtkSmartPointer<vtkPolyData> pointSet = this->getDomainPoints( domain );

    vtkSmartPointer<CustomSurfaceReconstructionFilter> surfaceReconstruction =
      vtkSmartPointer<CustomSurfaceReconstructionFilter>::New();
    surfaceReconstruction->SetInputData( pointSet );
    surfaceReconstruction->SetNeighborhoodSize( this->meshManager.getNeighborhoodSize() );
    surfaceReconstruction->SetSampleSpacing( this->meshManager.getSampleSpacing() );
    surfaceReconstruction->Update();

    vtkImageGaussianSmooth* smoother = vtkImageGaussianSmooth::New();
    smoother->SetStandardDeviations( surfaceReconstruction->GetOutput()->GetSpacing()[0],
                                     surfaceReconstruction->GetOutput()->GetSpacing()[1],
                                     surfaceReconstruction->GetOutput()->GetSpacing()[2] );

    smoother->SetInputConnection( surfaceReconstruction->GetOutputPort() );

    // Compute normals from the isosurface volume
    vtkSmartPointer<vtkImageGradient> grad = vtkSmartPointer<vtkImageGradient>::New();
    grad->SetDimensionality( 3 );
    grad->SetInputConnection( smoother->GetOutputPort() );
    grad->Update();

    vnl_vector_fixed<double, 3> normal;

    // Compute difference vector dot product with normal.  Length of vector is
    // stored in the "scalars" so that the vtk color mapping and glyph scaling
    // happens properly.
    for ( unsigned int i = 0; i < pointSet->GetNumberOfPoints(); i++ )
    {
      double x = pointSet->GetPoint( i )[0];
      double y = pointSet->GetPoint( i )[1];
      double z = pointSet->GetPoint( i )[2];

      float xd = vecs[i][0];
      float yd = vecs[i][1];
      float zd = vecs[i][2];

      this->trilinearInterpolate( grad->GetOutput(), x, y, z, normal );

      float mag = xd * normal( 0 ) + yd * normal( 1 ) + zd * normal( 2 );

      if ( mag < minmag ) {minmag = mag; }
      if ( mag > maxmag ) {maxmag = mag; }

      vectors->InsertNextTuple3( normal( 0 ) * mag, normal( 1 ) * mag, normal( 2 ) * mag );
      magnitudes->InsertNextTuple1( mag );
    }
  }
  this->updateDifferenceLUT( minmag, maxmag );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::computeSurfaceDifferences( vtkSmartPointer<vtkFloatArray> magnitudes,
                                                 vtkSmartPointer<vtkFloatArray> vectors )
{
  vtkSmartPointer<vtkPolyData> pointData = vtkSmartPointer<vtkPolyData>::New();
  pointData->SetPoints( this->glyphPoints );

  vtkPointLocator* pointLocator = vtkPointLocator::New();
  pointLocator->SetDataSet( pointData );
  pointLocator->SetDivisions( 100, 100, 100 );
  pointLocator->BuildLocator();

  for ( int domain = 0; domain < this->numDomains; domain++ )
  {
    vtkPolyData* polyData = this->surfaceMappers[domain]->GetInput();

    vtkFloatArray* surfaceMagnitudes = vtkFloatArray::New();
    surfaceMagnitudes->SetNumberOfComponents( 1 );
    surfaceMagnitudes->SetNumberOfTuples( polyData->GetPoints()->GetNumberOfPoints() );

    vtkFloatArray* surfaceVectors = vtkFloatArray::New();
    surfaceVectors->SetNumberOfComponents( 3 );
    surfaceVectors->SetNumberOfTuples( polyData->GetPoints()->GetNumberOfPoints() );

    for ( unsigned int i = 0; i < surfaceMagnitudes->GetNumberOfTuples(); i++ )
    {
      // find the 8 closest correspondence points the to current point
      vtkSmartPointer<vtkIdList> closestPoints = vtkSmartPointer<vtkIdList>::New();
      pointLocator->FindClosestNPoints( 8, polyData->GetPoint( i ), closestPoints );

      // assign scalar value based on a weighted scheme
      float weightedScalar = 0.0f;
      float distanceSum = 0.0f;
      float distance[8];
      for ( unsigned int p = 0; p < closestPoints->GetNumberOfIds(); p++ )
      {
        // get a particle position
        vtkIdType id = closestPoints->GetId( p );

        // compute distance to current particle
        double x = polyData->GetPoint( i )[0] - pointData->GetPoint( id )[0];
        double y = polyData->GetPoint( i )[1] - pointData->GetPoint( id )[1];
        double z = polyData->GetPoint( i )[2] - pointData->GetPoint( id )[2];
        distance[p] = 1.0f / ( x * x + y * y + z * z );

        // multiply scalar value by weight and add to running sum
        distanceSum += distance[p];
      }

      float vecX = 0.0f;
      float vecY = 0.0f;
      float vecZ = 0.0f;

      for ( unsigned int p = 0; p < closestPoints->GetNumberOfIds(); p++ )
      {
        vtkIdType currID = closestPoints->GetId( p );
        weightedScalar += distance[p] / distanceSum * magnitudes->GetValue( currID );
        vecX += distance[p] / distanceSum * vectors->GetComponent( currID, 0 );
        vecY += distance[p] / distanceSum * vectors->GetComponent( currID, 1 );
        vecZ += distance[p] / distanceSum * vectors->GetComponent( currID, 2 );
      }

      surfaceMagnitudes->SetValue( i, weightedScalar );

      surfaceVectors->SetComponent( i, 0, vecX );
      surfaceVectors->SetComponent( i, 1, vecY );
      surfaceVectors->SetComponent( i, 2, vecZ );
    }

    // surface coloring
    polyData->GetPointData()->SetScalars( surfaceMagnitudes );
    polyData->GetPointData()->SetVectors( surfaceVectors );
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::displayMeanDifference()
{
  std::vector< itk::ParticlePositionReader<3>::PointType > vecs;

  for ( unsigned int i = 0; i < this->glyphPoints->GetNumberOfPoints(); i++ )
  {
    itk::ParticlePositionReader<3>::PointType tmp;
    tmp[0] = this->stats.Group2Mean()[i * 3] - this->stats.Group1Mean()[i * 3];
    tmp[1] = this->stats.Group2Mean()[i * 3 + 1] - this->stats.Group1Mean()[i * 3 + 1];
    tmp[2] = this->stats.Group2Mean()[i * 3 + 2] - this->stats.Group1Mean()[i * 3 + 2];
    vecs.push_back( tmp );
  }
  this->displayVectorField( vecs );
}

//---------------------------------------------------------------------------
void ShapeWorksView2::displayScalars()
{
  std::cerr << "displayScalars\n";

  vtkSmartPointer<vtkFloatArray> magnitudes = vtkSmartPointer<vtkFloatArray>::New();
  magnitudes->SetNumberOfComponents( 1 );

  double max_value = 0;

  for ( int i = 0; i < this->scalars.size(); i++ )
  {
    if ( this->scalars[i] > max_value )
    {
      max_value = this->scalars[i];
    }
    magnitudes->InsertNextTuple1( this->scalars[i] );
  }

  vtkSmartPointer<vtkPolyData> pointData = vtkSmartPointer<vtkPolyData>::New();
  pointData->SetPoints( this->glyphPoints );

  vtkPointLocator* pointLocator = vtkPointLocator::New();
  pointLocator->SetDataSet( pointData );
  pointLocator->SetDivisions( 100, 100, 100 );
  pointLocator->BuildLocator();

  for ( int domain = 0; domain < this->numDomains; domain++ )
  {
    vtkPolyData* polyData = this->surfaceMappers[domain]->GetInput();

    vtkFloatArray* surfaceMagnitudes = vtkFloatArray::New();
    surfaceMagnitudes->SetNumberOfComponents( 1 );
    surfaceMagnitudes->SetNumberOfTuples( polyData->GetPoints()->GetNumberOfPoints() );

    for ( unsigned int i = 0; i < surfaceMagnitudes->GetNumberOfTuples(); i++ )
    {
      // find the 8 closest correspondence points the to current point
      vtkSmartPointer<vtkIdList> closestPoints = vtkSmartPointer<vtkIdList>::New();
      pointLocator->FindClosestNPoints( 8, polyData->GetPoint( i ), closestPoints );

      // assign scalar value based on a weighted scheme
      float weightedScalar = 0.0f;
      float distanceSum = 0.0f;
      float distance[8];
      for ( unsigned int p = 0; p < closestPoints->GetNumberOfIds(); p++ )
      {
        // get a particle position
        vtkIdType id = closestPoints->GetId( p );

        // compute distance to current particle
        double x = polyData->GetPoint( i )[0] - pointData->GetPoint( id )[0];
        double y = polyData->GetPoint( i )[1] - pointData->GetPoint( id )[1];
        double z = polyData->GetPoint( i )[2] - pointData->GetPoint( id )[2];
        distance[p] = 1.0f / ( x * x + y * y + z * z );

        // multiply scalar value by weight and add to running sum
        distanceSum += distance[p];
      }

      float vecX = 0.0f;
      float vecY = 0.0f;
      float vecZ = 0.0f;

      for ( unsigned int p = 0; p < closestPoints->GetNumberOfIds(); p++ )
      {
        vtkIdType currID = closestPoints->GetId( p );
        weightedScalar += distance[p] / distanceSum * magnitudes->GetValue( currID );
      }

      surfaceMagnitudes->SetValue( i, weightedScalar );
    }

    // surface coloring
    polyData->GetPointData()->SetScalars( surfaceMagnitudes );
  }

  this->glyphPointSet->GetPointData()->SetScalars( magnitudes );

  this->updateDifferenceLUT( 0, max_value );
  this->glyphMapper->SetLookupTable( this->differenceLUT );

  // update surface rendering
  for ( int i = 0; i < this->numDomains; i++ )
  {
    this->surfaceMappers[i]->SetLookupTable( this->differenceLUT );
    this->surfaceMappers[i]->InterpolateScalarsBeforeMappingOn();
    this->surfaceMappers[i]->SetColorModeToMapScalars();
    this->surfaceMappers[i]->ScalarVisibilityOn();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::displaySpheres()
{
  this->arrowsVisible = false;

  this->renderer->RemoveActor( this->arrowGlyphActor );

  this->resetPointScalars();
  this->glyphMapper->SetLookupTable( this->lut );
  this->glyphs->SetSourceConnection( this->sphereSource->GetOutputPort() );
  this->glyphs->SetScaleModeToDataScalingOff();

  this->updateGlyphProperties();
}

//---------------------------------------------------------------------------
void ShapeWorksView2::resetPointScalars()
{
  vtkSmartPointer<vtkUnsignedLongArray> scalars = vtkSmartPointer<vtkUnsignedLongArray>::New();
  scalars->SetNumberOfComponents( 1 );
  scalars->SetNumberOfTuples( this->glyphPoints->GetNumberOfPoints() );

  for ( unsigned int i = 0; i < this->glyphPoints->GetNumberOfPoints(); i++ )
  {
    scalars->InsertTuple1( i, ( i * this->numDomains ) % ( this->glyphPoints->GetNumberOfPoints() ) );
  }
  this->glyphPointSet->GetPointData()->SetScalars( scalars );
  this->glyphPoints->Modified();

  for ( int i = 0; i < this->numDomains; i++ )
  {
    this->surfaceMappers[i]->SetColorModeToDefault();
    this->surfaceMappers[i]->ScalarVisibilityOff();
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::computeModeShape()
{
  double pcaSliderValue = this->getPcaValue( this->ui->pcaSlider->value() );

  unsigned int m = this->stats.Eigenvectors().columns() - ( this->ui->pcaModeSpinBox->value() + 1 );

  vnl_vector<double> e = this->stats.Eigenvectors().get_column( m );

  double lambda = sqrt( this->stats.Eigenvalues()[m] );

  this->ui->pcaValueLabel->setText( QString::number( pcaSliderValue, 'g', 2 ) );
  this->ui->pcaEigenValueLabel->setText( QString::number( this->stats.Eigenvalues()[m] ) );
  this->ui->pcaLambdaLabel->setText( QString::number( pcaSliderValue * lambda ) );

  double groupSliderValue = this->ui->pcaGroupSlider->value();
  double groupRatio = groupSliderValue / static_cast<double>( this->ui->pcaGroupSlider->maximum() );

  // pre-generate
  for ( int step = 0; step < this->pregenSteps.size(); step++ )
  {
    int pregenValue = this->ui->pcaSlider->value() + this->pregenSteps[step];
    if ( pregenValue >= this->ui->pcaSlider->minimum() && pregenValue <= this->ui->pcaSlider->maximum() )
    {
      double pcaValue = this->getPcaValue( pregenValue );
      vnl_vector<double> shape;
      if ( this->groupsAvailable )
      {
        shape = this->stats.Group1Mean() + ( this->stats.GroupDifference() * groupRatio ) + ( e * ( pcaValue * lambda ) );
      }
      else
      {
        shape = this->stats.Mean() + ( e * ( pcaValue * lambda ) );
      }
      for ( int i = 0; i < this->numDomains; i++ )
      {
        this->meshManager.generateMesh( this->getDomainShape( shape, i ) );
      }
    }
  }

  if ( this->groupsAvailable )
  {
    this->displayShape( this->stats.Group1Mean() + ( this->stats.GroupDifference() * groupRatio ) + ( e * ( pcaSliderValue * lambda ) ) );
  }
  else
  {
    this->displayShape( this->stats.Mean() + ( e * ( pcaSliderValue * lambda ) ) );
  }
}

//---------------------------------------------------------------------------
void ShapeWorksView2::computeRegressionShape()
{
  // pre-generate
  for ( int step = 0; step < this->pregenSteps.size(); step++ )
  {
    int pregenValue = this->ui->regressionSlider->value() + this->pregenSteps[step];
    if ( pregenValue >= this->ui->regressionSlider->minimum() && pregenValue <= this->ui->regressionSlider->maximum() )
    {
      // scale value back to range
      double value = this->getRegressionValue( pregenValue );

      vnl_vector<double> shape = this->regression->ComputeMean( value );
      for ( int i = 0; i < this->numDomains; i++ )
      {
        this->meshManager.generateMesh( this->getDomainShape( shape, i ) );
      }
    }
  }

  vnl_vector<double> shape = this->regression->ComputeMean( this->getRegressionValue( this->ui->regressionSlider->value() ) );
  this->displayShape( shape );
}

//---------------------------------------------------------------------------
double ShapeWorksView2::getRegressionValue( int sliderValue )
{
  // scale value back to range
  return ( (double)sliderValue / (double)this->ui->regressionSlider->maximum() * this->regressionRange ) + this->regressionMin;
}

//---------------------------------------------------------------------------
double ShapeWorksView2::getPcaValue( int sliderValue )
{
  float range = Preferences::Instance().getPcaRange();
  int halfRange = this->ui->pcaSlider->maximum();

  double value = (double)sliderValue / (double)halfRange * range;
  return value;
}

//---------------------------------------------------------------------------
void ShapeWorksView2::trilinearInterpolate( vtkImageData* grad, double x, double y, double z, vnl_vector_fixed<double, 3> &ans ) const
{
  // Access gradient image information.
  const double* gradData = (const double*)( grad->GetScalarPointer() );
  //const double* spacing = grad->GetSpacing();

  // See, e.g. http://en.wikipedia.org/wiki/Trilinear_interpolation for description
  // Identify the surrounding 8 points (corners).  c is the closest grid point.
  vtkIdType idx = grad->FindPoint( x, y, z );
  const double* c = grad->GetPoint( idx );

  //std::cout << "idx = " << idx << std::endl;
  //std::cout << "c = " << c[0] << " " << c[1] << " " << c[2] << std::endl;

  ans[0] = gradData[idx * 3];
  ans[1] = gradData[idx * 3 + 1];
  ans[2] = gradData[idx * 3 + 2];
  return;
}

//---------------------------------------------------------------------------
void ShapeWorksView2::updateDifferenceLUT( float r0, float r1 )
{

  double black[3] = { 0.0, 0.0, 0.0 };
  double white[3] = { 1.0, 1.0, 1.0 };
  double red[3] = { 1.0, 0.3, 0.3 };
  double red_pure[3] = { 1.0, 0.0, 0.0 };
  double green[3] = { 0.3, 1.0, 0.3 };
  double green_pure[3] = { 0.0, 1.0, 0.0 };
  double blue[3] = { 0.3, 0.3, 1.0 };
  double blue_pure[3] = { 0.0, 0.0, 1.0 };
  double yellow[3] = { 1.0, 1.0, 0.3 };
  double yellow_pure[3] = { 1.0, 1.0, 0.0 };
  double magenta[3] = { 1.0, 0.3, 1.0 };
  double cyan[3] = { 0.3, 1.0, 1.0 };
  double orange[3] = { 1.0, 0.5, 0.0 };
  double violet[3] = { 2.0 / 3.0, 0.0, 1.0 };

  this->differenceLUT->RemoveAllPoints();

  //const float yellow = 0.86666;
  //const float blue = 0.66666;
  //const float yellow = 0.33;
  //const float blue = 0.66;
  const unsigned int resolution = 100;
  const float resinv = 1.0 / static_cast<float>( resolution );
  float maxrange;
  if ( fabs( r0 ) > fabs( r1 ) ) {maxrange = fabs( r0 ); }
  else {maxrange = fabs( r1 ); }

  std::cerr << "r0 = " << r0 << "\n";
  std::cerr << "r1 = " << r1 << "\n";
  std::cerr << "maxrange = " << maxrange << "\n";
  //this->differenceLUT->SetScalarRange(-maxrange, maxrange);

  //this->differenceLUT->SetValueRange(r0, r1);

  //this->differenceLUT->se

  this->scalar_bar_actor_->SetNumberOfLabels( 4 );
  //this->scalar_bar_actor_->SetNumberOfLabels(4);

  //this->differenceLUT->SetColorSpaceToHSV();
  //this->differenceLUT->AddHSVPoint(-maxrange, 0.33, 1.0, 1.0);
  //this->differenceLUT->AddHSVPoint(maxrange, 0.66, 1.0, 1.0);

  double rd = r1 - r0;

  this->differenceLUT->SetColorSpaceToHSV();

  // AKM, switched from this...
  this->differenceLUT->AddRGBPoint( r0, blue[0], blue[1], blue[2] );
  this->differenceLUT->AddRGBPoint( r0 + rd * 0.5, green[0], green[1], green[2] );
  this->differenceLUT->AddRGBPoint( r1, red[0], red[1], red[2] );

  //this->differenceLUT->AddRGBPoint( r0, blue[0], blue[1], blue[2] );
  //this->differenceLUT->AddRGBPoint( r1, red[0], red[1], red[2] );

  return;

  const float pip = fabs( maxrange ) * resinv;
  for ( unsigned int i = 0; i < resolution; i++ )
  {
    float fi = static_cast<float>( i );

    //this->differenceLUT->AddHSVPoint( -maxrange + ( fi * pip ), yellow, 1.0 - ( fi * resinv ), 1.0 );
    //this->differenceLUT->AddHSVPoint( maxrange - ( fi * pip ), blue, 1.0 - ( fi * resinv ), 1.0 );
  }

  this->differenceLUT->AddHSVPoint( 0.0, 0.0, 0.0, 1.0 );
}

//---------------------------------------------------------------------------
vnl_vector<double> ShapeWorksView2::getDomainShape( const vnl_vector<double> &shape, int domain )
{
  if ( numDomains == 1 )
  {
    return shape;
  }

  int numCoords = this->pointsPerDomain * 3;

  vnl_vector<double> domainShape( numCoords );

  for ( int j = 0; j < numCoords; j++ )
  {
    domainShape[j] = shape[domain * numCoords + j];
  }

  return domainShape;
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkPolyData> ShapeWorksView2::getDomainPoints( int domain )
{
  if ( numDomains == 1 )
  {
    return this->glyphPointSet;
  }

  vtkSmartPointer<vtkPoints> points = vtkSmartPointer<vtkPoints>::New();
  points->SetDataTypeToDouble();

  for ( int j = 0; j < this->pointsPerDomain; j++ )
  {
    double p[3];
    this->glyphPoints->GetPoint( domain * this->pointsPerDomain + j, p );
    points->InsertNextPoint( p );
  }

  vtkSmartPointer<vtkPolyData> pointSet = vtkSmartPointer<vtkPolyData>::New();
  pointSet->SetPoints( points );
  return pointSet;
}

//---------------------------------------------------------------------------
void ShapeWorksView2::setPregenSteps()
{
  // when animating, we want to pre-generate in a specific direction first
  // otherwise, we want to pre-generate alternating left and right from the current position
  bool animate = false;
  bool direction = false;

  if ( this->ui->pcaAnimateCheckBox->isChecked() )
  {
    animate = true;
    direction = this->pcaAnimateDirection;
  }
  else if ( this->ui->regressionAnimateCheckBox->isChecked() )
  {
    animate = true;
    direction = this->regressionAnimateDirection;
  }
  else if ( this->ui->pcaGroupAnimateCheckBox->isChecked() )
  {
    animate = true;
    direction = this->groupAnimateDirection;
  }

  const int size = 100;
  const int half = 50;

  this->pregenSteps.resize( size );

  if ( animate )
  {
    int idx = 0;
    for ( int i = 0; i < half; i++ )
    {
      if ( direction )
      {
        this->pregenSteps[i] = i;
        this->pregenSteps[i + half] = -i;
      }
      else
      {
        this->pregenSteps[i] = -i;
        this->pregenSteps[i + half] = i;
      }
    }
  }
  else
  {
    int idx = 0;
    for ( int i = 0; i < half; i++ )
    {
      this->pregenSteps[idx++] = i;
      this->pregenSteps[idx++] = -i;
    }
  }
}

//---------------------------------------------------------------------------
vtkSmartPointer<vtkOrientationMarkerWidget> ShapeWorksView2::createOrientationMarker()
{
  // create the orientation cube
  vtkSmartPointer<vtkAnnotatedCubeActor> cubeActor = vtkSmartPointer<vtkAnnotatedCubeActor>::New();
  cubeActor->SetXPlusFaceText( "L" );
  cubeActor->SetXMinusFaceText( "R" );
  cubeActor->SetYPlusFaceText( "P" );
  cubeActor->SetYMinusFaceText( "A" );
  cubeActor->SetZPlusFaceText( "S" );
  cubeActor->SetZMinusFaceText( "I" );
  cubeActor->SetFaceTextScale( 0.67 );
  cubeActor->GetTextEdgesProperty()->SetColor( 0.5, 0.5, 0.5 );
  cubeActor->SetTextEdgesVisibility( 1 );
  cubeActor->SetCubeVisibility( 1 );
  cubeActor->SetFaceTextVisibility( 1 );
  vtkProperty* prop = cubeActor->GetXPlusFaceProperty();
  prop->SetColor( 0, 0, 1 );
  prop = cubeActor->GetXMinusFaceProperty();
  prop->SetColor( 0, 0, 1 );
  prop = cubeActor->GetYPlusFaceProperty();
  prop->SetColor( 0, 1, 0 );
  prop = cubeActor->GetYMinusFaceProperty();
  prop->SetColor( 0, 1, 0 );
  prop = cubeActor->GetZPlusFaceProperty();
  prop->SetColor( 1, 0, 0 );
  prop = cubeActor->GetZMinusFaceProperty();
  prop->SetColor( 1, 0, 0 );
  vtkSmartPointer<vtkOrientationMarkerWidget> orientationMarketWidget =
    vtkSmartPointer<vtkOrientationMarkerWidget>::New();
  orientationMarketWidget->SetOrientationMarker( cubeActor );
  orientationMarketWidget->SetViewport( 0.85, 0.85, 1, 1 );
  return orientationMarketWidget;
}
