#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <stdio.h>
#include "Eigen/Core"
#include "Eigen/Dense"
#include "itkImage.h"
#include "itkImageFileReader.h"

// VTK imports
#include <vtkPolyData.h>
#include <vtkPLYReader.h>
#include <vtkSmartPointer.h>
#include <vtkDataArray.h>
#include <vtkPoints.h>
#include <vtkPLYWriter.h>
#include <vtkIdList.h>
#include <vtkIdTypeArray.h>
#include "tinyxml.h"

// IGL dependencies
#include <igl/biharmonic_coordinates.h>
#include <igl/cat.h>
#include <igl/cotmatrix.h>
#include <igl/matrix_to_list.h>
#include <igl/point_mesh_squared_distance.h>
#include <igl/readPLY.h>
#include <igl/writePLY.h>
#include <igl/remove_unreferenced.h>
#include <igl/slice.h>

Eigen::MatrixXd W_precomputation(Eigen::MatrixXd Vcontrol_static, Eigen::MatrixXd TV, Eigen::MatrixXi TT, Eigen::MatrixXi TF){ 

    Eigen::MatrixXd W;
    Eigen::VectorXi b;
	{
		Eigen::VectorXi J = Eigen::VectorXi::LinSpaced(TV.rows(),0,TV.rows()-1);
		Eigen::VectorXd sqrD;
		Eigen::MatrixXd _2;
		std::cout<<"Finding closest points..."<<std::endl;
		igl::point_mesh_squared_distance(Vcontrol_static,TV,J,sqrD,b,_2);
		std::cout << "sqrd " <<sqrD.minCoeff() <<std::endl;
		// assert(sqrD.minCoeff() < 1e-7 && "low.V should exist in high.V");
	}
    // force perfect positioning, rather have popping in low-res than high-res.
    // The correct/elaborate thing to do is express original low.V in terms of
    // linear interpolation (or extrapolation) via elements in (high.V,high.F)
    igl::slice(TV,b,1,Vcontrol_static);
    // list of points --> list of singleton lists
    std::vector<std::vector<int> > S;
    igl::matrix_to_list(b,S);
    std::cout<<"Computing weights for "<<b.size()<<
      " handles at "<<TV.rows()<<" vertices..."<<std::endl;
    // Technically k should equal 3 for smooth interpolation in 3d, but 2 is
    // faster and looks OK
    const int k = 2;
    igl::biharmonic_coordinates(TV,TT,S,k,W);
    std::cout<<"Reindexing..."<< std::endl;
    std::cout << W.rows() << " " << W.cols() << std::endl;
    // Throw away interior tet-vertices, keep weights and indices of boundary
    Eigen::VectorXi I,J;
    igl::remove_unreferenced(TV.rows(),TF,I,J);
    std::for_each(TF.data(),TF.data()+TF.size(),[&I](int & a){a=I(a);});
    std::for_each(b.data(),b.data()+b.size(),[&I](int & a){a=I(a);});
    igl::slice(Eigen::MatrixXd(TV),J,1,TV);
    igl::slice(Eigen::MatrixXd(W),J,1,W);
    std::cout << "It's done!!" << std::endl;
    return W;
}

Eigen::MatrixXd pointReadFormat(std::string refPointPath, int numP){
  Eigen::MatrixXd Vout(numP, 3);
  std::ifstream inFile;
  inFile.open(refPointPath.c_str());
  int count = 0;
  float a, b, c;
  while(!inFile.eof()){
	inFile >> a >> b >> c;
	if(count < numP){
		Vout(count, 0) = a;
		Vout(count, 1) = b;
		Vout(count, 2) = c;
	}
    count++;
  }
  inFile.close();
  return Vout;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        std::cout << "Usage: " << argv[0] << " plyMeshFilePath inPointsPath  outXMLFile numParticles" << std::endl;
        return EXIT_FAILURE;
    }
    std::string inMeshname  = std::string(argv[1]);
    std::string inPointname   = std::string(argv[2]);
	int numP = atoi(argv[4]);

	// read the points
	Eigen::MatrixXd Vcontrol_static;
	Eigen::MatrixXd Vcontrol_moving;

	Vcontrol_static = pointReadFormat(inPointname, numP);

	TiXmlDocument doc(argv[3]);
  	bool loadOkay = doc.LoadFile();
  	std::istringstream inputsBuffer;
  	std::string tmpString;

  	// vector of point paths and output mesh
  	std::vector< std::string > pointPaths;
	std::vector< std::string > outMeshPaths;
	if(loadOkay){
		TiXmlHandle docHandle( &doc );
  		TiXmlElement *elem;
		elem = docHandle.FirstChild("point_files").Element();
		if (!elem){
		std::cerr << "ERROR : No Point Files Provided" << std::endl;
		throw 1;
		}
		else{
		inputsBuffer.str(elem->GetText());
		while (inputsBuffer >> tmpString){
			pointPaths.push_back(tmpString);
		}
		inputsBuffer.clear();
		inputsBuffer.str("");
		}

		elem = docHandle.FirstChild("out_mesh").Element();
		if (!elem){
		std::cerr << "ERROR : No Output Mesh Files Provided" << std::endl;
		throw 1;
		}
		else{
		inputsBuffer.str(elem->GetText());
		while (inputsBuffer >> tmpString){
			outMeshPaths.push_back(tmpString);
		}
		inputsBuffer.clear();
		inputsBuffer.str("");
		}
	}
	
	vtkSmartPointer<vtkPLYReader> reader =
    vtkSmartPointer<vtkPLYReader>::New();
  	reader->SetFileName ( inMeshname.c_str() );
	reader->Update();

	// convert the VTK IO into Eigen Matrices
	vtkSmartPointer<vtkPolyData> mesh = reader->GetOutput();

	vtkSmartPointer<vtkPoints> points = mesh->GetPoints();
	vtkSmartPointer<vtkDataArray> dataArray = points->GetData();
	int numVertices = points->GetNumberOfPoints();
	int numFaces = mesh->GetNumberOfCells();
	
	Eigen::MatrixXd Vref(numVertices, 3);
	Eigen::MatrixXi Fref(numFaces, 3);

	std::cout << points->GetNumberOfPoints() << "  " << mesh->GetNumberOfCells() <<std::endl;
	for(int i=0; i<numVertices;i++){
		Vref(i, 0) = dataArray->GetComponent(i, 0);
		Vref(i, 1) = dataArray->GetComponent(i, 1);
		Vref(i, 2) = dataArray->GetComponent(i, 2);
	}
	vtkIdType cellId = 0;

	vtkSmartPointer<vtkIdList> cellIdList =
      vtkSmartPointer<vtkIdList>::New();
	
	for(int j = 0; j < numFaces; j++){
		mesh->GetCellPoints(j, cellIdList);
		Fref(j, 0) = cellIdList->GetId(0);
		Fref(j, 1) = cellIdList->GetId(1);
		Fref(j, 2) = cellIdList->GetId(2);
	}
	
	// Compute the Warp Matrix
	Eigen::MatrixXd TV = Vref;
  	Eigen::MatrixXi TF = Fref;
  	Eigen::MatrixXi TT = TF;  
	Eigen::MatrixXd W = W_precomputation(Vcontrol_static, TV, TT, TF);	 

	for(int i = 0; i < pointPaths.size(); i++){	
		Vcontrol_moving = pointReadFormat(pointPaths[i], numP);
		Eigen::MatrixXd Voutput = W * (Vcontrol_moving.rowwise() + Eigen::RowVector3d(1,0,0));
		// Save Output Mesh
		vtkSmartPointer<vtkPolyData> outmesh = vtkSmartPointer<vtkPolyData>::New();
		vtkSmartPointer<vtkPoints> outpoints = vtkSmartPointer<vtkPoints>::New();
		outpoints->SetNumberOfPoints(numVertices);

		for (vtkIdType i = 0; i < numVertices; i++)
		{
			outpoints->SetPoint(i, Voutput(i, 0), Voutput(i, 1), Voutput(i, 2));
		}

		vtkSmartPointer<vtkCellArray> polys = vtkSmartPointer<vtkCellArray>::New();
		for (vtkIdType i = 0; i < numFaces; i++)
		{
			vtkIdType a, b, c;
			polys->InsertNextCell(3);
			polys->InsertCellPoint(Fref(i, 0));
			polys->InsertCellPoint(Fref(i, 1));
			polys->InsertCellPoint(Fref(i, 2));
		}
		outmesh->SetPoints(outpoints);
		outmesh->SetPolys(polys);
		vtkSmartPointer<vtkPLYWriter> writer = vtkSmartPointer<vtkPLYWriter>::New();
		writer->SetInputData( outmesh );
		writer->SetFileName( outMeshPaths[i].c_str() );
		writer->Update();
	}
	return 0;
}
