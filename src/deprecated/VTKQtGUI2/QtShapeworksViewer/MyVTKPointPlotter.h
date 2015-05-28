/*
  Fast point plotter, not used yet, need to fix bugs.
  Plan to be added to polar system
  Author : Peter
*/
#ifndef MY_VTK_POINT_PLOTTER_H
#define MY_VTK_POINT_PLOTTER_H


class vtkPoints ;
class vtkUnsignedCharArray ;
class vtkPolyData ;
class vtkActor ;
class vtkDataArray ;


class MyVTKPointPlotter
{
public:
	

	MyVTKPointPlotter();
	~MyVTKPointPlotter();


	void PlotPoint(double x, double y, double z, 
		unsigned char r, unsigned char g, unsigned char b);
	
	void SetPointRadius(double radius = 1.0) { pt_radius = radius ; }
	void SetPointResolution(int res = 15) { pt_res = res ;}



	vtkPolyData* CreatePolyData();  // call it after all points are plotted
	vtkActor* CreateActor(); // call it after all points are plotted
private:


	vtkPoints* pts ;
	vtkDataArray* scalars ;
	int scalar_mode ;

	double pt_radius ;
	int pt_res ;
};

#endif // MY_VTK_POINT_PLOTTER_H