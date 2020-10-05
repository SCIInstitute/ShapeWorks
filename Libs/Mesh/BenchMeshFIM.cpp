#include <iostream>
#include <chrono>
#include <random>
#include "TriMesh.h"
#include "meshFIM.h"

struct Vec3d {
  double x, y, z;
};

class GeodesicMethod {
public:
  virtual double distance(int i, int j) = 0;
  virtual Vec3d point(int i) const = 0;
  virtual size_t numVerts() const = 0;
  virtual size_t numTris() const = 0;
  virtual Vec3d point(int fi, const Vec3d& bary) const = 0;
  virtual double distance(int fi, int fj, const Vec3d& baryi, const Vec3d& baryj) = 0;
};

class MeshFIMDistance : public GeodesicMethod {
private:
  TriMesh *mesh;
  meshFIM fim;
public:
  MeshFIMDistance(const std::string& plyFilePath, const std::string& meshFIMPath, float stopDistance) {
    mesh = trimesh::TriMesh::read(plyFilePath);
    fim.SetMesh(mesh);
    fim.SetStopDistance(stopDistance);
    fim.loadGeodesicFile(mesh, meshFIMPath.c_str());
    // fim.ReadFaceIndexMap(meshFIMPath.c_str());

    // fim.need_edge_lengths();
    // fim.need_oneringfaces();
    // fim.need_abs_curvatures();
    // fim.need_speed();
    // fim.setSpeedType(1);
    // fim.need_kdtree();
  }

  virtual double distance(int i, int j) override {
    return fim.GetGeodesicDistance(i, j);
  }

  virtual size_t numVerts() const override {
    return mesh->vertices.size();
  }
  virtual Vec3d point(int i) const override {
    return {
      mesh->vertices[i][0],
      mesh->vertices[i][1],
      mesh->vertices[i][2]
    };
  }

  size_t numTris() const override {
    return mesh->faces.size();
  }

  Vec3d point(int fi, const Vec3d &bary) const override {
    const auto& f = mesh->faces[fi];
    const auto& p0 = mesh->vertices[f[0]];
    const auto& p1 = mesh->vertices[f[1]];
    const auto& p2 = mesh->vertices[f[2]];

    const auto p = bary.x*p0 + bary.y*p1 + bary.z*p2;
    return {p[0], p[1], p[2]};
  }

  virtual double distance(int fi, int fj, const Vec3d& baryi, const Vec3d& baryj) override {
    const auto tfi = mesh->faces[fi];
    const auto tfj = mesh->faces[fj];
    char *method = "Bary"; // TODO wut

    vnl_vector<float> vbaryi(3);
    vbaryi[0] = baryi.x;
    vbaryi[1] = baryi.y;
    vbaryi[2] = baryi.z;
    vnl_vector<float> vbaryj(3);
    vbaryj[0] = baryj.x;
    vbaryj[1] = baryj.y;
    vbaryj[2] = baryj.z;
    return fim.GetBronsteinGeodesicDistance(tfi, tfj, vbaryi, vbaryj, method);
  }
};

// assumes r=1.0
Vec3d cart_to_geo(const Vec3d& p) {
  return {
          std::atan2(p.y, p.x),
          std::asin(p.z / 1.0),
          1.0
  };
}

double analytic_geo_dist(double lon0, double lat0, double lon1, double lat1) {
  // https://en.wikipedia.org/wiki/Great-circle_distance
  const double del_lon = std::abs(lon0 - lon1);
  const double del_lat = std::abs(lat0 - lat1);

  // for debugging
  const double x0 = std::sin(lat0)*std::sin(lat1) + std::cos(lat0)*std::cos(lat1)*std::cos(del_lon);
  const double central_angle = std::acos(x0);
  const double r = 1.0; // radius

  return r * central_angle;
}

std::uniform_real_distribution<float> dist01(0.0, 1.0);
std::default_random_engine re01;
Vec3d random_bary() {
  float r = dist01(re01);
  float s = dist01(re01);
  if(r + s >= 1.0) {
    r = 1.0 - r;
    s = 1.0 - s;
  }
  return {
    1.0 - r - s,
    r,
    s
  };
}

int main(int argc, char *argv[]) {
  if(argc != 3) {
    std::cerr << "check args \n";
    return 2;
  }
  const std::string plyFilePath = argv[1];
  const std::string meshFIMPath = argv[2];

  GeodesicMethod* geodesicFunc = new MeshFIMDistance(plyFilePath, meshFIMPath, 0.1f);

  // For random point generation
  std::uniform_int_distribution<int> points_dist(0, geodesicFunc->numTris());
  std::default_random_engine re;

  // Benchmark loop
  // gonna be biased, these rands
  for(int i=0; i<1000; i++) {
    const int fidx0 = points_dist(re);
    const int fidx1 = fidx0; // points_dist(re);
    const auto bary0 = random_bary();
    const auto bary1 = random_bary();


    const Vec3d p0 = geodesicFunc->point(fidx0, bary0);
    const Vec3d p1 = geodesicFunc->point(fidx1, bary1);

    Vec3d geo0 = cart_to_geo(p0);
    Vec3d geo1 = cart_to_geo(p1);
    const double lon0 = geo0.x;
    const double lat0 = geo0.y;
    const double lon1 = geo1.x;
    const double lat1 = geo1.y;
    const double analytic_soln = analytic_geo_dist(lon0, lat0, lon1, lat1);

    // Find closest point
    using namespace std::chrono;
    const auto startTime = high_resolution_clock::now();
    const double soln = geodesicFunc->distance(fidx0, fidx1, bary0, bary1);
    const auto endTime = high_resolution_clock::now();

    // Print time taken
    std::cout << duration_cast<nanoseconds>(endTime - startTime).count() << "ns ";

    // Print error
    const auto diff = std::abs(soln - analytic_soln);
    std::cout << diff << " ";

    std::cout << std::endl;
  }

  return 0;
}