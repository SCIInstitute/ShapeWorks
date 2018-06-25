#ifndef _computeFunctions
#define _computeFunctions
#include <iostream>
#include <math.h>

#include <igl/biharmonic_coordinates.h>
#include <igl/cat.h>
#include <igl/cotmatrix.h>
#include <igl/matrix_to_list.h>
#include <igl/parula.h>
#include <igl/copyleft/tetgen/tetrahedralize.h>
#include <igl/point_mesh_squared_distance.h>
#include <igl/readOBJ.h>
#include <igl/remove_unreferenced.h>
#include <igl/slice.h>
#include <igl/readMESH.h>
#include <igl/readOBJ.h>
#include <igl/viewer/Viewer.h>
#include <Eigen/Sparse>
#include <fstream>

using namespace std;
using namespace Eigen;



/*////////////////////////////////////////////////////////////////////////////////////////
W MATRIX PRECOMPUTATION FUNCTION
*/////////////////////////////////////////////////////////////////////////////////////////

vector<Eigen::MatrixXd> W_precomputation(Eigen::MatrixXd Vcontrol_static, Eigen::MatrixXd TV, Eigen::MatrixXi TT, Eigen::MatrixXi TF){ 

    vector<Eigen::MatrixXd> v;
    Eigen::MatrixXd W;
    Eigen::VectorXi b;
    {
      Eigen::VectorXi J = Eigen::VectorXi::LinSpaced(TV.rows(),0,TV.rows()-1);
      Eigen::VectorXd sqrD;
      Eigen::MatrixXd _2;
      std::cout<<"Finding closest points..."<<std::endl;
      igl::point_mesh_squared_distance(Vcontrol_static,TV,J,sqrD,b,_2);
      assert(sqrD.minCoeff() < 1e-7 && "low.V should exist in high.V");
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
    for_each(TF.data(),TF.data()+TF.size(),[&I](int & a){a=I(a);});
    for_each(b.data(),b.data()+b.size(),[&I](int & a){a=I(a);});
    igl::slice(Eigen::MatrixXd(TV),J,1,TV);
    igl::slice(Eigen::MatrixXd(W),J,1,W);
    std::cout << "It's done!!" << std::endl;
    v.push_back(W);
    v.push_back(Vcontrol_static);
    return v;
}




/*////////////////////////////////////////////////////////////////////////////////////////
PCA MODE VARIATION FUNCTION
*/////////////////////////////////////////////////////////////////////////////////////////

Eigen::MatrixXd mode_variation(Eigen::MatrixXd pca_mode, Eigen::MatrixXd mean_space, 
  float eigval, float sigma){

  // computes the new shape which varies from the mean shape along the PCA 
  // mode of variation, with deviation proportional to the sigma*eigval

  Eigen::MatrixXd Pout(pca_mode.rows(), pca_mode.cols());
  for(int i = 0; i < mean_space.rows(); i++){
    Pout(i,0) = mean_space(i,0) + sigma * std::sqrt(eigval) * pca_mode(i,0);
    Pout(i,1) = mean_space(i,1) + sigma * std::sqrt(eigval) * pca_mode(i,1);
    Pout(i,2) = mean_space(i,2) + sigma * std::sqrt(eigval) * pca_mode(i,2);
  }

  return Pout;
}

/*////////////////////////////////////////////////////////////////////////////////////////
POINT FILE READ FUNCTION
*/////////////////////////////////////////////////////////////////////////////////////////

Eigen::MatrixXd pointReadFormat(std::string refPointPath, int numP){
  Eigen::MatrixXd Vout(numP, 3);
  std::ifstream inFile;
  inFile.open(refPointPath.c_str());
  int count = 0;
  while(!inFile.eof()){
    inFile >> Vout(count, 0) >> Vout(count, 1) >> Vout(count, 2);
    count++;
  }
  inFile.close();
  return Vout;
}

/*////////////////////////////////////////////////////////////////////////////////////////
Get Mean Function
*/////////////////////////////////////////////////////////////////////////////////////////

Eigen::MatrixXd findMean(std::vector< std::string > pointPaths, int numP){
  Eigen::MatrixXd meanOut = Eigen::MatrixXd::Zero(numP, 3);
  std::string inFilename;
  for(int i = 0; i < pointPaths.size(); i++){
    inFilename = pointPaths[i];
    meanOut += pointReadFormat(inFilename, numP);
  }
  meanOut = meanOut/pointPaths.size();
  return meanOut;
}

/*////////////////////////////////////////////////////////////////////////////////////////
Debug Functions
*/////////////////////////////////////////////////////////////////////////////////////////
void printeigenvalues(Eigen::VectorXd vec){
  std::cout << "PRINTING EIGENVALUES " << std::endl;
  for(int i = 0; i < vec.size(); i++){ std::cout << "Number " << i << ": " << vec(i) << std::endl;}
}

void printpcamode(Eigen::MatrixXd pcaMoMat, int N, int numP){
  std::cout << "PRINTING PCAMODE " << N << std::endl;
  Eigen::MatrixXd outM = Eigen::Map<Eigen::MatrixXd>(pcaMoMat.col(N).data(), numP, 3);
  std::cout << outM << std::endl;
  // for(int i = 0; i < pcaMoMat.col(N).size(); i++){ std::cout <<  << vec(i) << std::endl;}
}

/*////////////////////////////////////////////////////////////////////////////////////////
Compute PCA Mode Function
*/////////////////////////////////////////////////////////////////////////////////////////

struct eigenOut {
  int numP;
  int ppSize;
  Eigen::MatrixXd pcaModes;// = Eigen::MatrixXd::Zero(numP*3, ppSize);
  Eigen::VectorXd eigenvalues;// = Eigen::VectorXd::Zero(ppSize);
};

eigenOut findPCAModes(std::vector< std::string > pointPaths, int numP){
    
  eigenOut newEigenOut;
  newEigenOut.numP = numP;
  newEigenOut.ppSize = pointPaths.size();
  // Create the Data Matrix and Mean Normalize it
  Eigen::MatrixXd dataMean = findMean(pointPaths, numP);
  Eigen::MatrixXd dataMat = Eigen::MatrixXd::Zero(numP*3, pointPaths.size());
  Eigen::MatrixXd Vtemp;
  std::string inFilename;
  for(int i = 0; i < pointPaths.size(); i++){
    inFilename = pointPaths[i];
    Vtemp = pointReadFormat(inFilename, numP);
    dataMat.col(i) = Eigen::Map<Eigen::VectorXd>(Vtemp.data(), Vtemp.size()) - Eigen::Map<Eigen::VectorXd>(dataMean.data(), dataMean.size());
  }

  // Find the Trick Covariance and all that jazz
  Eigen::MatrixXd trickCov = (dataMat * dataMat.transpose())/ (pointPaths.size() - 1);
  Eigen::SelfAdjointEigenSolver<MatrixXd> eig(trickCov);
  if (numP*3 > pointPaths.size()){
  newEigenOut.pcaModes = eig.eigenvectors().block(0, numP*3 - pointPaths.size(), numP*3 , pointPaths.size());
  newEigenOut.eigenvalues = eig.eigenvalues().segment(numP*3 - pointPaths.size(), pointPaths.size());
  }
  else{
  newEigenOut.pcaModes = eig.eigenvectors();
  newEigenOut.eigenvalues = eig.eigenvalues();
}
  
  return newEigenOut;
}

#endif