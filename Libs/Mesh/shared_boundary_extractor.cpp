#include <igl/readPLY.h>
#include <igl/writePLY.h>
#include <igl/AABB.h>
#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <iostream>
#include <igl/remove_unreferenced.h>
#include <igl/boundary_loop.h>

/*
 * shared_boundary_extractor
 * shared_boundary_extractor <input_l.ply> <input_r.ply> <out_l.ply> <out_r.ply> <out_m.ply> <tol>
 *
 * Extract the shared boundary triangles between input_l and input_r. The meshes with the boundary removed are
 * saved in out_l and out_r. The boundary triangles are stored in out_m.ply. tol is a data specific value that
 * defines the threshold for two surfaces to be "close"
 */

std::tuple<Eigen::MatrixXd,Eigen::MatrixXi> rem_into_eigen_mesh(const std::vector<int>& faces,
                         const Eigen::MatrixXd& src_V,
                         const Eigen::MatrixXi& src_F) 
{
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  const std::unordered_set<int> faces_set(faces.begin(), faces.end());
  Eigen::MatrixXi F2;
  F2.resize(src_F.rows() - faces_set.size(), 3);
  int next_idx = 0;
  for(int i=0; i<src_F.rows(); i++) {
    if(faces_set.find(i) == faces_set.end()) {
      F2.row(next_idx++) = src_F.row(i);
    }
  }

  Eigen::VectorXi mapping;
  igl::remove_unreferenced(src_V, F2, V, F, mapping);

  return std::make_tuple(V,F);
}



std::tuple<Eigen::MatrixXd,Eigen::MatrixXi> shared_into_eigen_mesh(const std::vector<int>& faces,
                            const Eigen::MatrixXd& src_V,
                            const Eigen::MatrixXi& src_F) 
{
  Eigen::MatrixXd V;
  Eigen::MatrixXi F;
  const std::unordered_set<int> faces_set(faces.begin(), faces.end());
  Eigen::MatrixXi F2;
  F2.resize(faces_set.size(), 3);
  int next_idx = 0;
  for(int i=0; i<src_F.rows(); i++) {
    if(faces_set.find(i) != faces_set.end()) {
      F2.row(next_idx++) = src_F.row(i);
    }
  }

  Eigen::VectorXi mapping;
  igl::remove_unreferenced(src_V, F2, V, F, mapping);
  return std::make_tuple(V,F);
}



bool is_empty(const Eigen::MatrixXd& V,
              const Eigen::MatrixXi& F) {
  return V.size() == 0 || F.size() == 0;
}


void move_to_boundary(const Eigen::MatrixXd& src_V,
                      const Eigen::MatrixXi& src_F,
                      const Eigen::MatrixXd& shared_V,
                      const Eigen::MatrixXi& shared_F,
                      Eigen::MatrixXd& out_V,
                      Eigen::MatrixXi& out_F) 
{
  // Eigen::MatrixXd out_V;
  // Eigen::MatrixXi out_F;

  std::vector<std::vector<int>> src_loops, shared_loops;
  igl::boundary_loop(src_F, src_loops);
  igl::boundary_loop(shared_F, shared_loops);

  assert(src_loops.size() == 1);
  assert(shared_loops.size() == 1);

  const auto& src_loop = src_loops[0];
  const auto& shared_loop = shared_loops[0];

  Eigen::MatrixXi shared_F_boundary;
  shared_F_boundary.resize(shared_loop.size(), 3);
  for(int i=0; i<shared_loop.size(); i++) {
    const int v0 = shared_loop[i];
    const int v1 = shared_loop[(i+1)%shared_loop.size()];
    shared_F_boundary.row(i) = Eigen::Vector3i{v0, v1, v1};
  }


  out_V = src_V;
  out_F = src_F;

  igl::AABB<Eigen::MatrixXd, 3> tree;
  tree.init(shared_V, shared_F_boundary);

  for(int i=0; i<src_loop.size(); i++) {
    Eigen::VectorXd sqrD;
    Eigen::VectorXi I;
    Eigen::MatrixXd C;
    Eigen::MatrixXd P(1, 3);
    P.row(0) = src_V.row(src_loop[i]);
    tree.squared_distance(shared_V, shared_F_boundary, P, sqrD, I, C);

    out_V.row(src_loop[i]) = C.row(0);
  }

  // std::make_tuple(out_V,out_F);
}





std::tuple<Eigen::MatrixXd,Eigen::MatrixXi,Eigen::MatrixXd,Eigen::MatrixXi> find_shared_surface(const Eigen::MatrixXd& src_V, const Eigen::MatrixXi& src_F,
                         const Eigen::MatrixXd& other_V, const Eigen::MatrixXi& other_F,
                         double tol=1e-3) 
{
  Eigen::MatrixXd out_V; 
  Eigen::MatrixXi out_F;
  Eigen::MatrixXd rem_V; 
  Eigen::MatrixXi rem_F;
  igl::AABB<Eigen::MatrixXd, 3> tree;
  tree.init(other_V, other_F);

  std::vector<Eigen::Vector3d> new_pts;

  std::vector<int> shared_faces;

  // find stuff
  for(int f=0; f<src_F.rows(); f++) {
    bool keep = true;
    for(int i=0; i<3; i++) {
      const auto& vi = src_V.row(src_F(f, i));
      Eigen::VectorXd sqrD;
      Eigen::VectorXi I;
      Eigen::MatrixXd C;
      Eigen::MatrixXd P(1, 3);
      P.row(0) = vi;
      tree.squared_distance(other_V, other_F, P, sqrD, I, C);


      if(sqrD(0) > tol) {
        keep = false;
        break;
      }
    }

    if(keep) {
      shared_faces.push_back(f);
    }
  }

  std::tie(out_V,out_F) = shared_into_eigen_mesh(shared_faces, src_V, src_F);
  std::tie(rem_V,rem_F) = rem_into_eigen_mesh(shared_faces, src_V, src_F);
  return std::make_tuple(out_V,out_F,rem_V,rem_F);
}



int main(int argc, char *argv[])
{
  if(argc < 6) {
    std::cerr << "Usage: " << argv[0] << " <input_l.ply> <input_r.ply> <out_l.ply> <out_r.ply> <out_m.ply> <tol>\n";
    exit(1);
  }

  const std::string fname_l = argv[1];
  const std::string fname_r = argv[2];
  const std::string fname_l_new = argv[3];
  const std::string fname_r_new = argv[4];
  const std::string fname_shared= argv[5];
  double tol = 1e-3;
  if(argc == 7) {
    tol = std::stod(argv[6]);
  }


  Eigen::MatrixXd V_l, V_r, V_boundary;
  Eigen::MatrixXi F_l, F_r, F_boundary;
  igl::readPLY(fname_l, V_l, F_l);
  igl::readPLY(fname_r, V_r, F_r);


  Eigen::MatrixXd shared_V_l, shared_V_r, rem_V_l, rem_V_r;
  Eigen::MatrixXi shared_F_l, shared_F_r, rem_F_l, rem_F_r;
  std::tie(shared_V_l, shared_F_l, rem_V_l, rem_F_l) = find_shared_surface(V_l, F_l, V_r, F_r, tol);
  std::tie(shared_V_r, shared_F_r, rem_V_r, rem_F_r) = find_shared_surface(V_r, F_r, V_l, F_l, tol);

  if (is_empty(shared_V_l, shared_F_l) || is_empty(shared_V_r, shared_F_r)
      || is_empty(rem_V_l, rem_F_l) || is_empty(rem_V_r, rem_F_r)) {
    //todo this should return a status code to the caller so that it can be displayed or handled based on the
    // downstream task
    throw std::runtime_error("No shared surface detected. Please check the input meshes and/or increase the tolerance");
  }

  Eigen::MatrixXd bridge_V;
  Eigen::MatrixXi bridge_F;
  // std::tie(bridge_V, bridge_F) = move_to_boundary(rem_V_l, rem_F_l, shared_V_r, shared_F_r);
  move_to_boundary(rem_V_l, rem_F_l, shared_V_r, shared_F_r,bridge_V,bridge_F);

  igl::writePLY(fname_l_new, bridge_V, bridge_F);
  igl::writePLY(fname_r_new, rem_V_r, rem_F_r);
  igl::writePLY(fname_shared, shared_V_r, shared_F_r);

  // igl::writePLY("out_shared_l.ply", shared_V_l, shared_F_l);
  // igl::writePLY("out_shared_r.ply", shared_V_r, shared_F_r);
  // igl::writePLY("out_rem_l.ply", rem_V_l, rem_F_l);
  // igl::writePLY("out_rem_r.ply", rem_V_r, rem_F_r);
  // igl::writePLY("out_bridge.vtp", bridge_V, bridge_F);

  /*
  igl::opengl::glfw::Viewer viewer;
  viewer.data().set_mesh(bridge_V, bridge_F);
  viewer.data().set_face_based(true);
  viewer.launch();
   */
}









/*
void sharedBoundaryExtractor(Mesh mesh_l, Mesh mesh_r, std::string filename_l,std::string filename_r,std::string filename_shared,double tol)
{
  


  Eigen::MatrixXd V_l, V_r;
  Eigen::MatrixXi F_l, F_r;
  V_l = mesh_l.points();
  F_l = mesh_l.faces();
  V_r = mesh_r.points();
  F_r = mesh_r.faces();



  Eigen::MatrixXd shared_V_l, shared_V_r, rem_V_l, rem_V_r;
  Eigen::MatrixXi shared_F_l, shared_F_r, rem_F_l, rem_F_r;
  std::tie(shared_V_l, shared_F_l, rem_V_l, rem_F_l) = find_shared_surface(V_l, F_l, V_r, F_r, tol);
  std::tie(shared_V_r, shared_F_r, rem_V_r, rem_F_r) = find_shared_surface(V_r, F_r, V_l, F_l, tol);

  if (is_empty(shared_V_l, shared_F_l) || is_empty(shared_V_r, shared_F_r)
      || is_empty(rem_V_l, rem_F_l) || is_empty(rem_V_r, rem_F_r)) {
    //todo this should return a status code to the caller so that it can be displayed or handled based on the
    // downstream task
    throw std::runtime_error("No shared surface detected. Please check the input meshes and/or increase the tolerance");
  }

  Eigen::MatrixXd bridge_V;
  Eigen::MatrixXi bridge_F;
  // std::tie(bridge_V, bridge_F) = move_to_boundary(rem_V_l, rem_F_l, shared_V_r, shared_F_r);
  move_to_boundary(rem_V_l, rem_F_l, shared_V_r, shared_F_r,bridge_V,bridge_F);

  igl::writePLY(filename_l, bridge_V, bridge_F);
  igl::writePLY(filename_r, rem_V_r, rem_F_r);
  igl::writePLY(filename_shared, shared_V_r, shared_F_r);

}



*/





/*
void move_to_boundary(const Eigen::MatrixXd& src_V,
                      const Eigen::MatrixXi& src_F,
                      const Eigen::MatrixXd& shared_V,
                      const Eigen::MatrixXi& shared_F,
                      Eigen::MatrixXd& out_V,
                      Eigen::MatrixXi& out_F) 
{
  // Eigen::MatrixXd out_V;
  // Eigen::MatrixXi out_F;

  std::vector<std::vector<int>> src_loops, shared_loops;
  igl::boundary_loop(src_F, src_loops);
  igl::boundary_loop(shared_F, shared_loops);

  assert(src_loops.size() == 1);
  assert(shared_loops.size() == 1);

  const auto& src_loop = src_loops[0];
  const auto& shared_loop = shared_loops[0];

  Eigen::MatrixXi shared_F_boundary;
  shared_F_boundary.resize(shared_loop.size(), 3);
  for(int i=0; i<shared_loop.size(); i++) {
    const int v0 = shared_loop[i];
    const int v1 = shared_loop[(i+1)%shared_loop.size()];
    shared_F_boundary.row(i) = Eigen::Vector3i{v0, v1, v1};
  }


  out_V = src_V;
  out_F = src_F;

  igl::AABB<Eigen::MatrixXd, 3> tree;
  tree.init(shared_V, shared_F_boundary);

  for(int i=0; i<src_loop.size(); i++) {
    Eigen::VectorXd sqrD;
    Eigen::VectorXi I;
    Eigen::MatrixXd C;
    Eigen::MatrixXd P(1, 3);
    P.row(0) = src_V.row(src_loop[i]);
    tree.squared_distance(shared_V, shared_F_boundary, P, sqrD, I, C);

    out_V.row(src_loop[i]) = C.row(0);
  }

  // std::make_tuple(out_V,out_F);
}

*/