/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: SimpleView4.h,v $
  Language:  C++
  Date:      $Date: 2008-08-28 14:54:58 $
  Version:   $Revision: 1.4 $

  Copyright 2004 Sandia Corporation.
  Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
  license for use of this work by or on behalf of the
  U.S. Government. Redistribution and use in source and binary forms, with
  or without modification, are permitted provided that this Notice and any
  statement of authorship are reproduced on all copies.

=========================================================================*/

/*========================================================================
 For general information about using VTK and Qt, see:
 http://www.trolltech.com/products/3rdparty/vtksupport.html
=========================================================================*/

/*========================================================================
 !!! WARNING for those who want to contribute code to this file.
 !!! If you use a commercial edition of Qt, you can modify this code.
 !!! If you use an open source version of Qt, you are free to modify
 !!! and use this code within the guidelines of the GPL license.
 !!! Unfortunately, you cannot contribute the changes back into this
 !!! file.  Doing so creates a conflict between the GPL and BSD-like VTK
 !!! license.
=========================================================================*/


#ifndef SIMPLEVIEW_H
#define SIMPLEVIEW_H

#include "qmainwindow.h"
#include "ui_uiSimpleView4.h"
#include "ui_SamplesDialog.h"
#include "itkCylinderSpatialObject.h"
#include "itkDataObject.h"
#include "itkParticlePositionReader.h"
#include "itkParticleShapeStatistics.h"
#include "MyVTKPointPlotter.h"

#include "vtkSurfaceReconstructionFilter.h"
#include "vtkSmoothPolyDataFilter.h"
#include "vtkContourFilter.h"
#include "vtkReverseSense.h"
#include "vtkPolyData.h"
#include "vtkPolyDataMapper.h"
#include "vtkActor.h"
#include "colorScheme.h"
#include "vtkProperty.h"


#include "qmessagebox.h"

#include "vtkArrowSource.h"
#include "vtkTransformPolyDataFilter.h"
#include "vtkGlyph3D.h"



// Forward class declarations
class vtkCylinderSource;
class vtkPolyDataMapper;
class vtkExodusReader;
class vtkDataSetMapper;
class vtkActor;
class vtkRenderer;
class vtkPoints;
class vtkSurfaceReconstructionFilter;
class vtkContourFilter;
class vtkReverseSense ;
class vtkSmoothPolyDataFilter;



class SimpleView : public QMainWindow, private Ui_MainWindow
{
    Q_OBJECT

public:
	
	
    // Constructor/Destructor
    SimpleView(QWidget* parent = 0);
    ~SimpleView();
	itk::ParticlePositionReader<3>::Pointer reader;
	ParticleShapeStatistics<3> m_Stats;
	
	
	vtkPolyData       *m_glyphPointset;
	vtkPoints         *m_glyphPoints;  
	
	vtkActor* SimpleView::getSurfaceActor(vtkPolyData * m_glyphPointset);

	vtkActor* dispActorHandle;
	vtkActor* dispGlyphsActorHandle;

	colorSchemes m_ColorSchemes; 
	void ChangeColorScheme();

	int mode;
	int stdDev;

public slots:

     virtual void fileOpen();
     virtual void fileExit();
	 //virtual void loadPointFile();
	 virtual void displayMeanShape();
	 virtual void DisplayShape(const vnl_vector<double> &pos);
	 virtual void displaySamples();
	 virtual void displayMedian();
	 virtual void displayPCAModes();
	 virtual void displayColorSchemeDialog();
	

	 void clear();
	 void spunObjectLoader(int i);
	 void pcaSliderObjectLoader(int i);
	 void pcaSpinnerObjectLoader(int i);
	 void ChangeColorScheme(int i);
	 void toggleGlyphsDisplay();
	 
	 //virtual void LoadPointFile();
	
protected:

protected slots:

private:
     vtkCylinderSource* source;
     vtkPolyDataMapper* mapper;
     vtkActor* actor;
     vtkRenderer* ren;
	
	vtkSurfaceReconstructionFilter *m_surf;
	vtkPolyDataMapper              *m_surfMap;
	vtkActor                       *m_surfActor;
	vtkContourFilter               *m_surfContour;
	vtkReverseSense                *m_surfReverse;
	vtkSmoothPolyDataFilter        *m_surfSmoother;

	

	 bool okToContinue();
   
};

#endif // SIMPLEVIEW_H

