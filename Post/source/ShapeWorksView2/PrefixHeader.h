// standard includes
#include <iostream>
#include <sstream>
#include <list>

// vnl
#include "vnl/vnl_vector.h"
#include "vnl/algo/vnl_symmetric_eigensystem.h"
#include "vnl/vnl_matrix.h"

// qt
#include <QtGui>
#include <QSettings>
#include <QThread>
#include <QMutex>
#include <QWaitCondition>
#include <QTimer>

// vtk
#include <vtkSmartPointer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkColorTransferFunction.h>
#include <vtkContourFilter.h>
#include <vtkDecimatePro.h>
#include <vtkFloatArray.h>
#include <vtkGlyph3D.h>
#include <vtkIdList.h>
#include <vtkImageConstantPad.h>
#include <vtkImageData.h>
#include <vtkImageGaussianSmooth.h>
#include <vtkImageGradient.h>
#include <vtkImageWriter.h>
#include <vtkLookupTable.h>
#include <vtkPLYReader.h>
#include <vtkPointData.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkPolyDataNormals.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkReverseSense.h>
#include <vtkSmoothPolyDataFilter.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkUnsignedLongArray.h>

#include "tinyxml.h"
