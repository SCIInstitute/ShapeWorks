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
  virtual size_t numVerts() const = 0;
  virtual Vec3d point(int i) const = 0;
};

class MeshFIMDistance : public GeodesicMethod {
private:
  TriMesh *mesh;
  meshFIM fim;
public:
  MeshFIMDistance(const std::string& plyFilePath, const std::string& meshFIMPath) {
    mesh = trimesh::TriMesh::read(plyFilePath);
    fim.SetMesh(mesh);
    fim.SetStopDistance(3.1415f * 1.0f);
    fim.computeFIM(mesh, meshFIMPath.c_str());
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

int main(int argc, char *argv[]) {
  const std::string plyFilePath = argv[1];
  const std::string meshFIMPath = argv[2];

  GeodesicMethod* geodesicFunc = new MeshFIMDistance(plyFilePath, meshFIMPath);

  // For random point generation
  std::uniform_int_distribution<int> points_dist(0, geodesicFunc->numVerts());
  std::default_random_engine re;

  // Benchmark loop
  // gonna be biased, these rands
  for(int i=0; i<1000; i++) {
    const int idx0 = points_dist(re);
    const int idx1 = points_dist(re);

    const Vec3d p0 = geodesicFunc->point(idx0);
    const Vec3d p1 = geodesicFunc->point(idx1);

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
    const double soln = geodesicFunc->distance(idx0, idx1);
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