#include "MyVTKPointPlotter.h"


#include "vtkPoints.h" 
#include "vtkUnsignedCharArray.h" 
#include "vtkPolyData.h" 
#include "vtkActor.h"
#include "vtkDataArray.h"
#include "vtkGlyph3D.h"
#include "vtkDiskSource.h"
#include "vtkPolyDataMapper.h"
#include "vtkPointData.h"
#include "vtkSphereSource.h"

MyVTKPointPlotter::MyVTKPointPlotter()
{
	pts = NULL ;
	scalars = NULL ;

	SetPointRadius();
	SetPointResolution();
	
}

MyVTKPointPlotter::~MyVTKPointPlotter()
{
	if (pts)
		pts->Delete();
	if (scalars)
		scalars->Delete();
}

void MyVTKPointPlotter::PlotPoint(double x, double y, double z, 
									unsigned char r, unsigned char g, unsigned char b)
{
	if (pts==NULL)
		pts = vtkPoints::New();

	if (scalars==NULL)		
	{
		scalars = vtkUnsignedCharArray::New();
		scalars->SetNumberOfComponents(3);
	}

	pts->InsertNextPoint(x,y,z);
	scalars->InsertNextTuple3(r,g,b);
		
}

vtkPolyData* MyVTKPointPlotter::CreatePolyData()
{
	//vtkDiskSource* src = vtkDiskSource::New();
	vtkSphereSource* src = vtkSphereSource::New();
	//vtkSphereSource* src = vtkSphereSource::New();
	
	/*src->SetRadialResolution(1);
	src->SetCircumferentialResolution(pt_res);

	src->SetInnerRadius(0.0);
	src->SetOuterRadius(pt_radius);*/

	src->SetPhiResolution(2);
    src->SetThetaResolution(2);
    src->SetReleaseDataFlag(1);

	vtkPolyData* polyData = vtkPolyData::New();
	polyData->SetPoints(pts);
	polyData->GetPointData()->SetScalars(scalars);

	vtkGlyph3D* glyph = vtkGlyph3D::New();
	glyph->SetSourceConnection(src->GetOutputPort());
	glyph->SetInput(polyData);

	glyph->SetColorModeToColorByScalar();
	glyph->SetScaleModeToDataScalingOff() ;


	vtkPolyData* output = glyph->GetOutput();
	return output ;
}

vtkActor* MyVTKPointPlotter::CreateActor()
{
	vtkPolyData* polyData = CreatePolyData();

	vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
	mapper->SetInput(polyData);

	vtkActor* actor = vtkActor::New();
	actor->SetMapper(mapper);

	return actor ;
}
