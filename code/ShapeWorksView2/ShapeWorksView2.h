#include <QMainWindow>
#include "tinyxml.h"

#include "itkParticleShapeLinearRegressionMatrixAttribute.h"
#include "itkParticlePositionReader.h"
#include "itkParticleShapeStatistics.h"
#include "itkParticlePositionWriter.h"

#include <vtkSmartPointer.h>

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
class vtkContourFilter;
class vtkReverseSense;
class vtkSmoothPolyDataFilter;
class vtkPolyDataNormals;
class vtkDecimatePro;
class vtkImageConstantPad;
class vtkPowerCrustSurfaceReconstruction;

// Forward Qt class declarations
class Ui_ShapeWorksView2;

class ViewerLayout;

// comparison class for vnl_vectors (for cache)
class vnl_vector_compare
{
public:
  bool operator()( const vnl_vector<double> &x, const vnl_vector<double> &y ) const
  {
    if ( x.size() < y.size() )
    {
      return true;
    }

    for ( unsigned i = 0; i < x.size(); i++ )
    {
      if ( x[i] < y[i] )
      {
        return true;
      }
      else if ( y[i] < x[i] )
      {
        return false;
      }
    }

    return false;
  }
};

// mesh cache type
typedef std::map< const vnl_vector<double>, vtkSmartPointer<vtkPolyData>, vnl_vector_compare > CacheMap;

class ShapeWorksView2 : public QMainWindow
{
  Q_OBJECT
public:

  ShapeWorksView2( int argc, char** argv );
  ~ShapeWorksView2();

public Q_SLOTS:

  void on_actionQuit_triggered();

  // display modes
  void on_meanButton_clicked();
  void on_sampleButton_clicked();
  void on_pcaButton_clicked();

  // mean mode
  void on_meanOverallButton_clicked();
  void on_meanGroup1Button_clicked();
  void on_meanGroup2Button_clicked();

  // sample mode
  void on_sampleSpinBox_valueChanged();
  void on_medianButton_clicked();
  void on_medianGroup1Button_clicked();
  void on_medianGroup2Button_clicked();

  // PCA mode
  void on_pcaSlider_valueChanged();
  void on_pcaModeSpinBox_valueChanged();
  void on_pcaGroupSlider_valueChanged();

  // visualization
  void on_showGlyphs_stateChanged();
  void on_showSurface_stateChanged();
  void on_usePowerCrustCheckBox_stateChanged();
  void on_neighborhoodSpinBox_valueChanged();
  void on_spacingSpinBox_valueChanged();

private:

  void initializeRenderer();
  void initializeGlyphs();
  void initializeSurface();

  void updateShapeMode();
  void updateSurfaceSettings();
  void updateActors();

  void redraw();

  bool readParameterFile( char* filename );
  void displayShape( const vnl_vector<double> &pos );
  void computeModeShape();

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
  vtkSmartPointer<vtkPolyDataMapper>       surfaceMapper;
  vtkSmartPointer<vtkActor>                surfaceActor;
  vtkSmartPointer<vtkContourFilter>        surfaceContourFilter;
  vtkSmartPointer<vtkReverseSense>         surfaceReverseSense;
  vtkSmartPointer<vtkSmoothPolyDataFilter> surfaceSmoothFilter;
  vtkSmartPointer<vtkPolyDataNormals>      polydataNormals;

  vtkSmartPointer<CustomSurfaceReconstructionFilter>  surface;
  vtkSmartPointer<vtkPowerCrustSurfaceReconstruction> powercrust;

  //vtkSmartPointer<vtkColorTransferFunction>   differenceLUT;
  //vtkSmartPointer<vtkColorTransferFunction>   pValueTFunc;
  //vtkSmartPointer<vtkTransformPolyDataFilter> arrowFlipFilter;
  //vtkSmartPointer<vtkGlyph3D>                 arrowGlyphs;
  //vtkSmartPointer<vtkPolyDataNormals>         m_surfNormals;
  //vtkSmartPointer<vtkDecimatePro>             m_surfDecimate;
  //vtkSmartPointer<vtkTransform>               transform180;
  //vtkSmartPointer<vtkArrowSource>             arrowSource;

  ParticleShapeStatistics<3> stats;

  int numSamples;
  bool groupsAvailable;

  // a copy of the current shape mesh
  vnl_vector<double> currentShape;

  // mesh cache
  CacheMap meshCache;
};
