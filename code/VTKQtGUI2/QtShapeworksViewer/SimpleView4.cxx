/*
 * Copyright 2004 Sandia Corporation.
 * Under the terms of Contract DE-AC04-94AL85000, there is a non-exclusive
 * license for use of this work by or on behalf of the
 * U.S. Government. Redistribution and use in source and binary forms, with
 * or without modification, are permitted provided that this Notice and any
 * statement of authorship are reproduced on all copies.
 */

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

#include <qapplication.h>
#include <qfiledialog.h>
#include "qmenubar.h"

#include "SimpleView4.h"
#include <vtkActor.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include "vtkCylinderSource.h"
#include <vtkPolyDataMapper.h>

#include "SamplesDialog.h"
#include "PCADialog.h"
#include "ColorDialog.h"
#include <QStatusBar>

// Constructor
SimpleView::SimpleView(QWidget* p) 
 : QMainWindow(p)
{
  setupUi(this);

 
	dispActorHandle=NULL;
	dispGlyphsActorHandle=NULL;
  
	connect(actionLoad_Parameter_File, SIGNAL(triggered()), this, SLOT(fileOpen()));
	connect(actionExit, SIGNAL(triggered()), this, SLOT(fileExit()));
  
	connect(actionView_Mean, SIGNAL(triggered()), this, SLOT(displayMeanShape()));
	connect(actionView_Samples, SIGNAL(triggered()), this, SLOT(displaySamples()));
	connect(actionView_Median, SIGNAL(triggered()), this, SLOT(displayMedian()));
	connect(actionView_PCA_Modes, SIGNAL(triggered()), this, SLOT(displayPCAModes()));
 	connect(actionColor_Scheme, SIGNAL(triggered()), this, SLOT(displayColorSchemeDialog()));
	connect(actionShow_Glyphs, SIGNAL(triggered()), this, SLOT(toggleGlyphsDisplay()));
  
  
	
	 actionView_Samples->setDisabled(1);
	 actionView_Mean->setDisabled(1);
	 actionView_Median->setDisabled(1);
	 actionView_PCA_Modes->setDisabled(1);

  // QT/VTK interact
  ren = vtkRenderer::New();
  vtkWidget->GetRenderWindow()->AddRenderer(ren);

  source = NULL;
  mapper = NULL;
  actor = NULL;

  //initial pca mode and stdDev
  mode=0;
  stdDev=0;

	

}

SimpleView::~SimpleView()
{
  ren->Delete();
  if( source ) 
    {
    source->Delete();
    }
  if( mapper ) 
    {
    mapper->Delete();
    }
  if( actor ) 
    {
    actor->Delete();
    }
}
   
// Action to be taken upon file open 
void SimpleView::fileOpen()
{
	
 

  QFileDialog dialog(this);
    dialog.setDirectory(QDir::homePath());
    dialog.setFileMode(QFileDialog::ExistingFile);
	dialog.setDirectory("C:\\Work\\vs2008\\myprojects\\VTKQtGUI2\\solution3\\SimpleView");
	dialog.setDirectory(QDir::current());
    dialog.setNameFilter(trUtf8("Parameter Files (*.xml)"));
    QStringList fileNames;
    if (dialog.exec())
		fileNames = dialog.selectedFiles();
	else
		return;
	std::string str,str2;
    //std::cout << "Got filename: " << fileNames.toStdString() << std::endl;
   for (int i = 0; i < fileNames.size(); ++i){
          //std::cout << fileNames.at(i).toStdString();//.toLocal8Bit().constData() << endl;
		str=fileNames.at(i).toStdString();
		std::cout<<str<<endl;
		str2=str.substr(0,str.find_last_of('/')+1);
		str=str.substr(str.find_last_of('/')+1);
		
		
   }
	
	const char * c = str.c_str();
	if(str.length()>0){
		 m_Stats.ReadPointFiles(c);
		 m_Stats.ComputeModes();
		 m_Stats.PrincipalComponentProjections();

		 //this->DisplayShape(m_Stats.ShapeMatrix().get_column(1));
		actionView_Samples->setDisabled(0);
		actionView_Mean->setDisabled(0);
		actionView_Median->setDisabled(0);
		actionView_PCA_Modes->setDisabled(0);
		actionColor_Scheme->setDisabled(0);
		actionShow_Glyphs->setDisabled(0);

		//mySpinner->setMaximum( m_Stats.ShapeMatrix().cols()-1);
		
	}
	
	
	setWindowTitle(c);
	displaySamples();
	
	//okToContinue();
}

void SimpleView::toggleGlyphsDisplay(){
	if(actionShow_Glyphs->isChecked()){
		ren->AddActor(dispGlyphsActorHandle);
		ren->GetRenderWindow()->Render();
	}else{
		ren->RemoveActor(dispGlyphsActorHandle);
		ren->GetRenderWindow()->Render();
	}
	ren->ResetCamera();
}
void SimpleView::fileExit() {
  qApp->exit();
}

//void SimpleView::loadPointFile(){
//
//	okToContinue();
//
//	SimpleView::reader=itk::ParticlePositionReader<3>::New();
//	reader->SetFileName("C:/Users/Mukund/Desktop/Research/Old/HippoEg/torus.1.wpts");
//	reader->Update();
//
//	unsigned int q = reader->GetOutput().size();
//	vnl_vector<double> pos;
//	pos.set_size(q * 3);
//	unsigned int k = 0;
//	for (unsigned int j = 0; j < q; j++)
//		{
//		pos[k] = reader->GetOutput()[j][0];
//		k++;
//		pos[k] = reader->GetOutput()[j][1];
//		k++;
//		pos[k] = reader->GetOutput()[j][2];
//		k++;    
//
//		
//    }
//	//this->DisplayShape(pos);
//	this->DisplayShape(m_Stats.Mean());
//
//}
void SimpleView::spunObjectLoader(int i){

		std::cout<<i<<endl;
		clear();
		this->DisplayShape(m_Stats.ShapeMatrix().get_column(i));

}

void SimpleView::pcaSpinnerObjectLoader(int i){
	
		clear();
		double s = stdDev;
		mode=i;

		unsigned int m = m_Stats.Eigenvectors().columns() - (mode+1)/*(this->mode->value()+1)*/;
		vnl_vector<double> e = m_Stats.Eigenvectors().get_column(m);
		double lambda = sqrt(m_Stats.Eigenvalues()[m]);
		this->DisplayShape(m_Stats.Mean() + (e * (s * lambda)));
}

void SimpleView::pcaSliderObjectLoader(int i){

		std::cout<<i<<endl;
		clear();
		double s = i;
		stdDev=i;

		unsigned int m = m_Stats.Eigenvectors().columns() - (mode+1)/*(this->mode->value()+1)*/;
		vnl_vector<double> e = m_Stats.Eigenvectors().get_column(m);
		double lambda = sqrt(m_Stats.Eigenvalues()[m]);
		this->DisplayShape(m_Stats.Mean() + (e * (s * lambda)));

		

}


void SimpleView::displayPCAModes(){

	PCADialog *pcaDialog= new PCADialog;
	
	pcaDialog->show();

	int max=m_Stats.ShapeMatrix().cols()-1;
	int min=0;
	pcaDialog->spinBox->setMinimum(min);
	pcaDialog->spinBox->setMaximum(max);

	connect(pcaDialog->hSlider, SIGNAL(valueChanged(int)), this, SLOT(pcaSliderObjectLoader(int)));
	connect(pcaDialog->spinBox, SIGNAL(valueChanged(int)), this, SLOT(pcaSpinnerObjectLoader(int)));
	
	
	statusBar()->showMessage(tr("Viewing PCA Modes"));

}

void SimpleView::displayColorSchemeDialog(){

	ColorDialog *colorDialog= new ColorDialog;
	
	colorDialog->show();

	connect(colorDialog->spinBox, SIGNAL(valueChanged(int)), this, SLOT(ChangeColorScheme(int)));
	
	

}

void SimpleView::displayMedian(){
	
	clear();
	this->DisplayShape(m_Stats.ShapeMatrix().get_column(m_Stats.ComputeMedianShape(-32)));
	statusBar()->showMessage(tr("Viewing Median"));
}

void SimpleView::displaySamples(){

	clear();
	
	
	

	this->DisplayShape(m_Stats.ShapeMatrix().get_column(0));
	

	SamplesDialog *sdialog= new SamplesDialog;
	sdialog->spinBox->setMaximum( m_Stats.ShapeMatrix().cols()-1);
	sdialog->show();

	connect(sdialog->spinBox, SIGNAL(valueChanged(int)), this, SLOT(spunObjectLoader(int)));
	
	statusBar()->showMessage(tr("Viewing Samples"));
}

void SimpleView::displayMeanShape(){
	
	clear();
	this->DisplayShape(m_Stats.Mean());
	statusBar()->showMessage(tr("Viewing Mean Shape"));

}

void SimpleView::DisplayShape(const vnl_vector<double> &pos)
{

	MyVTKPointPlotter pointPlotter ;

	m_glyphPoints = vtkPoints::New();
	m_glyphPoints->SetDataTypeToDouble();
	m_glyphPointset = vtkPolyData::New();
	
	
  unsigned int k = 0;
  for (unsigned int i = 0; i <256; i++)
    {
		double x = pos[k];
		k++;
		double y = pos[k];
		k++;
		double z = pos[k];
		k++;
		pointPlotter.PlotPoint(x,y,z,255,112,10);

		m_glyphPoints->InsertNextPoint(x,y,z);

		
		
    }
  m_glyphPoints->Modified();
  
  m_glyphPointset->SetPoints(m_glyphPoints);    
  

  //getSurfaceActor(pointPlotter.CreatePolyData());//just a test line
  

  if(dispActorHandle!=NULL){
	  dispActorHandle->Delete();
  }
  if(dispGlyphsActorHandle!=NULL){
	  dispGlyphsActorHandle->Delete();
  }

  dispActorHandle=this->getSurfaceActor(m_glyphPointset);
  dispGlyphsActorHandle=pointPlotter.CreateActor();
  
  ren->AddActor(dispActorHandle);
  ren->AddActor(dispGlyphsActorHandle);
  


  if (m_surfActor) this->m_surfActor->Render(ren, m_surfMap);
  if (m_surf) m_surf->Modified();

  ren->GetRenderWindow()->Render();
  ren->GetRenderWindow()->Render();

     // Reset camera
  ren->ResetCamera();

  //m_glyphPoints->Modified();
  //
  //if (m_Initialized)
  //  {
  //  // The following two lines overcome some wierd pipeline updating bug I
  //  // can't figure out.  The origin information propagating from the surface
  //  // reconstruction somehow does not get updated correctly until run twice.
  //  if (m_surfActor) this->m_surfActor->Render(m_renderer, m_surfMap);
  //  if (m_surf) m_surf->Modified();
  //  
  //  this->m_render_window->Render();
  //  }  

}


bool SimpleView::okToContinue(){

	

	/*int r= QMessageBox::warning(this,tr("Mean"),tr("test1?"),
		QMessageBox::Yes | QMessageBox::Default, QMessageBox::No,QMessageBox::Cancel);
		*/
		return true;
}


void SimpleView::clear(){
	std::cout<<"clear";
	ren->RemoveActor(dispActorHandle);
	ren->RemoveActor(dispGlyphsActorHandle);

	ren->GetRenderWindow()->Render();
}

vtkActor* SimpleView::getSurfaceActor(vtkPolyData * m_glyphPointset)
{
	  
	  m_surf = vtkSurfaceReconstructionFilter::New();
	  m_surf->SetInput(m_glyphPointset);
	  m_surf->SetNeighborhoodSize(/*this->neighborhoodsize->value()*/5);
	  m_surf->SetSampleSpacing(/*this->samplespacing->value()*/1);
	  
	  m_surfContour = vtkContourFilter::New();
	  m_surfContour->SetInputConnection(m_surf->GetOutputPort());
	  m_surfContour->SetValue(0,0.0);
	  m_surfContour->ComputeNormalsOn();
	  
	  m_surfReverse = vtkReverseSense::New();
	  m_surfReverse->SetInputConnection(m_surfContour->GetOutputPort());
	  m_surfReverse->ReverseCellsOn();
	  m_surfReverse->ReverseNormalsOn();

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

	
	return m_surfActor;
}

void SimpleView::ChangeColorScheme(int q)
{
  //int q = 1;//this->color_switcher->value();
   
  m_surfActor->GetProperty()->SetDiffuseColor(m_ColorSchemes[q].foreground.r,
                                              m_ColorSchemes[q].foreground.g,
                                              m_ColorSchemes[q].foreground.b);

  
  ren->SetBackground(m_ColorSchemes[q].background.r,
                            m_ColorSchemes[q].background.g,
                            m_ColorSchemes[q].background.b);


  ren->GetRenderWindow()->Render();
  //if (m_Initialized) ren->m_render_window->Render();
  
}