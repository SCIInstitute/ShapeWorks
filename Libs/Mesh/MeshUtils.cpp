#include "MeshUtils.h"
#include "ParticleSystem.h"

#include <vtkIterativeClosestPointTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkLandmarkTransform.h>
#include <igl/biharmonic_coordinates.h>
#include <igl/cat.h>
#include <igl/cotmatrix.h>
#include <igl/matrix_to_list.h>
#include <igl/point_mesh_squared_distance.h>
#include <igl/remove_unreferenced.h>
#include <igl/slice.h>

// tbb
#include <tbb/mutex.h>


namespace shapeworks {

// locking to handle non-thread-safe code
static tbb::mutex mesh_mutex;

const vtkSmartPointer<vtkMatrix4x4> MeshUtils::createICPTransform(const vtkSmartPointer<vtkPolyData> source,
                                                                  const vtkSmartPointer<vtkPolyData> target,
                                                                  Mesh::AlignmentType align,
                                                                  const unsigned iterations,
                                                                  bool meshTransform)
{
  vtkSmartPointer<vtkIterativeClosestPointTransform> icp = vtkSmartPointer<vtkIterativeClosestPointTransform>::New();
  icp->SetSource(source);
  icp->SetTarget(target);

  if (align == Mesh::Rigid)
    icp->GetLandmarkTransform()->SetModeToRigidBody();
  else if (align == Mesh::Similarity)
    icp->GetLandmarkTransform()->SetModeToSimilarity();
  else
    icp->GetLandmarkTransform()->SetModeToAffine();

  icp->SetMaximumNumberOfIterations(iterations);
  if (meshTransform)
    icp->StartByMatchingCentroidsOn();
  icp->Modified();
  icp->Update();

  vtkSmartPointer<vtkTransformPolyDataFilter> icpTransformFilter = vtkSmartPointer<vtkTransformPolyDataFilter>::New();
  icpTransformFilter->SetInputData(source);
  icpTransformFilter->SetTransform(icp);
  icpTransformFilter->Update();

  vtkSmartPointer<vtkMatrix4x4> m = vtkMatrix4x4::New();
  if (meshTransform)
    m = icp->GetMatrix();
  else
    vtkMatrix4x4::Invert(icp->GetMatrix(), m);

  return m;
}

Eigen::MatrixXd MeshUtils::distilVertexInfo(Mesh mesh) {
  vtkSmartPointer<vtkPolyData> poly_data = mesh.getVTKMesh();
  vtkSmartPointer<vtkPoints> points = poly_data->GetPoints();
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

Eigen::MatrixXi MeshUtils::distilFaceInfo(Mesh mesh){
  vtkSmartPointer<vtkPolyData> poly_data = mesh.getVTKMesh();
  int numFaces = poly_data->GetNumberOfCells();
  Eigen::MatrixXi Fref_new(numFaces, 3);

  vtkSmartPointer<vtkIdList> cellIdList =
    vtkSmartPointer<vtkIdList>::New();

  for(int j = 0; j < numFaces; j++){
    poly_data->GetCellPoints(j, cellIdList);
    Fref_new(j, 0) = cellIdList->GetId(0);
    Fref_new(j, 1) = cellIdList->GetId(1);
    Fref_new(j, 2) = cellIdList->GetId(2);
  }
  return Fref_new;
}

bool MeshUtils::generateWarpMatrix(Eigen::MatrixXd TV, Eigen::MatrixXi TF,
                                              const Eigen::MatrixXd& Vref, Eigen::MatrixXd& W)
{
  Eigen::VectorXi b;
	{
		Eigen::VectorXi J = Eigen::VectorXi::LinSpaced(TV.rows(),0,TV.rows()-1);
		Eigen::VectorXd sqrD;
		Eigen::MatrixXd _2;
    // using J which is N by 1 instead of a matrix that represents faces of N by 3
    // so that we will find the closest vertices instead of closest point on the face
    // so far the two meshes are not separated. So what we are really doing here
    // is computing handles from low resolution and use that for the high resolution one
		igl::point_mesh_squared_distance(Vref,TV,J,sqrD,b,_2);
    //assert(sqrD.maxCoeff() < 1e-7 && "Particles must exist on vertices");
	}

  // list of points --> list of singleton lists
  std::vector<std::vector<int> > S;
  igl::matrix_to_list(b,S);

  // Technically k should equal 3 for smooth interpolation in 3d, but 2 is
  // faster and looks OK
  const int k = 2;
  if (!igl::biharmonic_coordinates(TV,TF,S,k,W))
  {
    std::cerr << "igl:biharmonic_coordinates failed\n";
    return false;
  }
  // Throw away interior tet-vertices, keep weights and indices of boundary
  Eigen::VectorXi I,J;
  igl::remove_unreferenced(TV.rows(),TF,I,J);
  std::for_each(TF.data(),TF.data()+TF.size(),[&I](int & a){a=I(a);});
  std::for_each(b.data(),b.data()+b.size(),[&I](int & a){a=I(a);});
  igl::slice(Eigen::MatrixXd(TV),J,1,TV);
  igl::slice(Eigen::MatrixXd(W),J,1,W);
  return true;
}

Mesh MeshUtils::warpMesh(const Eigen::MatrixXd& movPts,
                         const Eigen::MatrixXd& W, const Eigen::MatrixXi& Fref)
{
  
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

bool MeshUtils::warpMeshes(std::vector<std::string> movingPointpaths,
                           std::vector<std::string> outputMeshPaths, const Eigen::MatrixXd& W,
                           const Eigen::MatrixXi& Fref, const int numP)
{
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

Mesh MeshUtils::threadSafeReadMesh(std::string filename)
{
  tbb::mutex::scoped_lock lock(mesh_mutex);
  Mesh mesh(filename);
  return mesh;
}

void MeshUtils::threadSafeWriteMesh(std::string filename, Mesh mesh)
{
  tbb::mutex::scoped_lock lock(mesh_mutex);
  mesh.write(filename);
}

Region MeshUtils::boundingBox(std::vector<std::string> &filenames, bool center)
{
  if (filenames.empty())
    throw std::invalid_argument("No filenames provided to compute a bounding box");
  
  Mesh mesh(filenames[0]);
  Region bbox(mesh.boundingBox());

  for (auto filename : filenames)
  {
    Mesh mesh(filename);
    bbox.grow(mesh.boundingBox());
  }

  return bbox;
}

Region MeshUtils::boundingBox(std::vector<Mesh> &meshes, bool center)
{
  if (meshes.empty())
    throw std::invalid_argument("No meshes provided to compute a bounding box");

  Region bbox(meshes[0].boundingBox());

  for (auto mesh : meshes)
    bbox.grow(mesh.boundingBox());

  return bbox;
}


} // shapeworks
