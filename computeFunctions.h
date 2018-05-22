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


#endif