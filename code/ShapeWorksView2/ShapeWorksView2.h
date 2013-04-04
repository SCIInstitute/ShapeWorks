/*
 * Shapeworks license
 */

/**
 * @file ShapeWorksView2.h
 * @brief Main application
 *
 */

#include <QMainWindow>
#include <QTimer>

#include <vtkSmartPointer.h>

#include "tinyxml.h"

#include "itkParticleShapeLinearRegressionMatrixAttribute.h"
#include "itkParticlePositionReader.h"
#include "itkParticleShapeStatistics.h"
#include "itkParticlePositionWriter.h"

#include <ColorSchemes.h>
#include <MeshManager.h>

class vtkRenderer;
class vtkLookupTable;
class vtkColorTransferFunction;
class vtkPoints;
class vtkPolyData;
class vtkGlyph3D;
class vtkPolyDataMapper;
class vtkActor;
class vtkSphereSource;
class vtkUnsignedLongArray;
class vtkArrowSource;
class vtkTransform;

class CustomSurfaceReconstructionFilter;
class vtkImageConstantPad;
class vtkImageData;
class vtkTransformPolyDataFilter;

// Forward Qt class declarations
class Ui_ShapeWorksView2;

class ViewerLayout;

class ShapeWorksView2 : public QMainWindow
{
  Q_OBJECT
public:

  ShapeWorksView2( int argc, char** argv );
  ~ShapeWorksView2();

  virtual void closeEvent( QCloseEvent* event );

public Q_SLOTS:

  void on_actionExportPcaLoadings_triggered();
  void on_actionExportPoints_triggered();
  void on_actionExportSurfaceMesh_triggered();

  void on_actionQuit_triggered();
  void on_actionPreferences_triggered();

  // analysis mode
  void on_tabWidget_currentChanged();

  // mean mode
  void on_meanOverallButton_clicked();
  void on_meanGroup1Button_clicked();
  void on_meanGroup2Button_clicked();
  void on_meanDifferenceButton_clicked();

  // sample mode
  void on_sampleSpinBox_valueChanged();
  void on_medianButton_clicked();
  void on_medianGroup1Button_clicked();
  void on_medianGroup2Button_clicked();

  // PCA mode
  void on_pcaSlider_valueChanged();
  void on_pcaModeSpinBox_valueChanged();
  void on_pcaGroupSlider_valueChanged();
  void on_pcaAnimateCheckBox_stateChanged();
  void handlePcaTimer();

  // Regression mode
  void on_regressionSlider_valueChanged();

  // visualization
  void on_showGlyphs_stateChanged();
  void on_showSurface_stateChanged();
  void on_usePowerCrustCheckBox_stateChanged();
  void on_neighborhoodSpinBox_valueChanged();
  void on_spacingSpinBox_valueChanged();

  void colorSchemeChanged();
  void glyphPropertiesChanged();

private:

  void initializeRenderer();
  void initializeGlyphs();
  void initializeSurfaces();

  void updateAnalysisMode();
  void updateSurfaceSettings();
  void updateActors();
  void updateColorScheme();
  void updateGlyphProperties();
  void updateDifferenceLUT( float r0, float r1 );

  void redraw();

  bool readParameterFile( char* filename );
  bool readExplanatoryVariables( char* filename );

  void displayShape( const vnl_vector<double> &pos );
  void displayVectorField( const std::vector<itk::ParticlePositionReader<3>::PointType > &vecs );
  void displayMeanDifference();
  void displaySpheres();
  void resetPointScalars();

  void computeModeShape();
  void computeRegressionShape();

  double getRegressionValue();

  void trilinearInterpolate( vtkImageData* grad, double x, double y, double z,
                             vnl_vector_fixed<double, 3> &ans ) const;

  vnl_vector<double> getDomain( const vnl_vector<double> &shape, int domain);

  // designer form
  Ui_ShapeWorksView2* ui;

  vtkSmartPointer<vtkRenderer>             renderer;
  vtkSmartPointer<vtkLookupTable>          lut;
  vtkSmartPointer<vtkPoints>               glyphPoints;
  vtkSmartPointer<vtkPolyData>             glyphPointSet;
  vtkSmartPointer<vtkGlyph3D>              glyphs;
  vtkSmartPointer<vtkPolyDataMapper>       glyphMapper;
  vtkSmartPointer<vtkActor>                glyphActor;
  vtkSmartPointer<vtkSphereSource>         sphereSource;
  vtkSmartPointer<vtkUnsignedLongArray>    scalars;

  std::vector< vtkSmartPointer<vtkPolyDataMapper> >      surfaceMappers;
  std::vector< vtkSmartPointer<vtkActor> >               surfaceActors;

  vtkSmartPointer<CustomSurfaceReconstructionFilter>  surface;

  vtkSmartPointer<vtkColorTransferFunction>   differenceLUT;
  vtkSmartPointer<vtkArrowSource>             arrowSource;
  vtkSmartPointer<vtkTransformPolyDataFilter> arrowFlipFilter;
  vtkSmartPointer<vtkGlyph3D>                 arrowGlyphs;
  vtkSmartPointer<vtkPolyDataMapper>          arrowGlyphMapper;
  vtkSmartPointer<vtkActor>                   arrowGlyphActor;

  vtkSmartPointer<vtkTransform>               transform180;
  //vtkSmartPointer<vtkColorTransferFunction>   pValueTFunc;
  //vtkSmartPointer<vtkPolyDataNormals>         m_surfNormals;
  //vtkSmartPointer<vtkDecimatePro>             m_surfDecimate;

  ParticleShapeStatistics<3> stats;
  itk::ParticleShapeLinearRegressionMatrixAttribute<double, 3>::Pointer regression;

  int numSamples;
  int numDomains;

  bool groupsAvailable;
  bool regressionAvailable;

  double regressionMin;
  double regressionMax;
  double regressionRange;

  // a copy of the current shape mesh
  vnl_vector<double> currentShape;

  bool arrowsVisible;

  MeshManager meshManager;

  ColorSchemes m_ColorSchemes;

  bool pcaAnimateDirection;
  QTimer pcaAnimateTimer;
};
