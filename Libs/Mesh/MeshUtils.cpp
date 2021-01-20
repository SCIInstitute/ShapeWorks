#include "MeshUtils.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <vtkTransform.h>
#include <vtkPLYWriter.h>
#include "Eigen/Core"
#include "Eigen/Dense"
#include "ParticleSystem.h"

// IGL dependencies
#include <igl/biharmonic_coordinates.h>
#include <igl/cat.h>
#include <igl/cotmatrix.h>
#include <igl/matrix_to_list.h>
#include <igl/point_mesh_squared_distance.h>
#include <igl/remove_unreferenced.h>
#include <igl/slice.h>

namespace shapeworks {

const vtkSmartPointer<vtkMatrix4x4> MeshUtils::createIcpTransform(const vtkSmartPointer<vtkPolyData> source,
                                                                  const vtkSmartPointer<vtkPolyData> target,
                                                                  const unsigned iterations)
{
  vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
  icp->SetSource(source);
  icp->SetTarget(target);
  icp->GetLandmarkTransform()->SetModeToRigidBody();
  icp->SetMaximumNumberOfIterations(iterations);
  icp->Modified();
  icp->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  icpTransformFilter->SetInputData(source);
  icpTransformFilter->SetTransform(icp);
  icpTransformFilter->Update();

  vtkSmartPointer<vtkMatrix4x4> m = vtkMatrix4x4::New();
  vtkMatrix4x4::Invert(icp->GetMatrix(), m);
  return m;
}

Eigen::MatrixXd MeshUtils::pointReadFormat(std::string refPointPath, int numP){
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

Eigen::MatrixXd MeshUtils::distilVertexInfo(vtkSmartPointer<vtkPolyData> mesh){
  vtkSmartPointer<vtkPoints> points = mesh->GetPoints();
	vtkSmartPointer<vtkDataArray> dataArray = points->GetData();
	
  int numVertices = points->GetNumberOfPoints();
	
	Eigen::MatrixXd Vref_new(numVertices, 3);
	
	for(int i=0; i<numVertices;i++){
		Vref_new(i, 0) = dataArray->GetComponent(i, 0);
		Vref_new(i, 1) = dataArray->GetComponent(i, 1);
		Vref_new(i, 2) = dataArray->GetComponent(i, 2);
	}
  return Vref_new;
}

Eigen::MatrixXi MeshUtils::distilFaceInfo(vtkSmartPointer<vtkPolyData> mesh){
  
  int numFaces = mesh->GetNumberOfCells();
  Eigen::MatrixXi Fref_new(numFaces, 3);
	
	vtkIdType cellId = 0;

	vtkSmartPointer<vtkIdList> cellIdList =
      vtkSmartPointer<vtkIdList>::New();
	
	for(int j = 0; j < numFaces; j++){
		mesh->GetCellPoints(j, cellIdList);
		Fref_new(j, 0) = cellIdList->GetId(0);
		Fref_new(j, 1) = cellIdList->GetId(1);
		Fref_new(j, 2) = cellIdList->GetId(2);
	}
  return Fref_new;
}


Eigen::MatrixXd MeshUtils::generateWarpMatrix(Eigen::MatrixXd TV , Eigen::MatrixXi TF, Eigen::MatrixXd Vref){
  
  Eigen::MatrixXd W;
  Eigen::VectorXi b;
	{
		Eigen::VectorXi J = Eigen::VectorXi::LinSpaced(TV.rows(),0,TV.rows()-1);
		Eigen::VectorXd sqrD;
		Eigen::MatrixXd _2;
		std::cout<<"Finding closest points..."<<std::endl;
		igl::point_mesh_squared_distance(Vref,TV,J,sqrD,b,_2);
		std::cout << "sqrd " <<sqrD.minCoeff() <<std::endl;
	}
  // force perfect positioning, rather have popping in low-res than high-res.
  // The correct/elaborate thing to do is express original low.V in terms of
  // linear interpolation (or extrapolation) via elements in (high.V,high.F)
  igl::slice(TV,b,1,Vref);
  // list of points --> list of singleton lists
  std::vector<std::vector<int> > S;
  igl::matrix_to_list(b,S);
  std::cout<<"Computing weights for "<<b.size()<<
    " handles at "<<TV.rows()<<" vertices..."<<std::endl;
  // Technically k should equal 3 for smooth interpolation in 3d, but 2 is
  // faster and looks OK
  const int k = 2;
  igl::biharmonic_coordinates(TV,TF,S,k,W);
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

Mesh MeshUtils::warpMesh(Eigen::MatrixXd movPts, Eigen::MatrixXd W, Eigen::MatrixXi Fref){
  
  int numVertices = W.rows();
  int numFaces = Fref.rows();
  // std::cout<< (Vcontrol_moving.rowwise() + Eigen::RowVector3d(0,0,0)) <<std::endl;
	Eigen::MatrixXd Voutput = W * (movPts.rowwise() + Eigen::RowVector3d(0,0,0));
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
	
  return Mesh(outmesh);
}

bool MeshUtils::warpMeshes(std::vector< std::string> movingPointpaths, std::vector< std::string> outputMeshPaths, Eigen::MatrixXd W, Eigen::MatrixXi Fref, const int numP){
  
  // assert for pointsPath.size() == outMeshPaths.size()
  // now tranform the meshes
  ParticleSystem particlesystem(movingPointpaths);
  Eigen::MatrixXd pts = particlesystem.Particles();
  Eigen::MatrixXd temp;
  for(int i = 0; i < movingPointpaths.size(); i++){	
    temp = pts.col(i);
    temp.resize(3, numP);
		Mesh warped = warpMesh(temp.transpose(), W, Fref);
    // write the mesh
    warped.write(outputMeshPaths[i]);
	}
  return true;
}



} // shapeworks
