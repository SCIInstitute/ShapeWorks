#include "Testing.h"
#include "Mesh.h"
#include "MeshUtils.h"
#include "MeshWarper.h"
#include "Image.h"
#include "ParticleSystem.h"

#include <igl/point_mesh_squared_distance.h>

using namespace shapeworks;

TEST(MeshTests, meshLocators)
{
  Mesh mesh1(std::string(TEST_DATA_DIR) + "/butterfly.vtk");
  Mesh mesh2(std::string(TEST_DATA_DIR) + "/smoothsinc.vtk");
  Point center_1 = mesh1.center();
  Point center_2 = mesh2.center();
  int center_id_1 = mesh1.closestPointId(center_1);
  int center_id_2 = mesh2.closestPointId(center_2);
  Point center_point_1 = mesh1.getPoint(center_id_1);
  Point center_point_2 = mesh2.getPoint(center_id_2);
  ASSERT_TRUE(center_id_1 != center_id_2 &&  // ensure next test doesn't succeed by coincidene
              center_point_1 != center_point_2);

  mesh2 = mesh1; // mesh2 should no longer use cached locators from mesh1
  ASSERT_TRUE(mesh2.center() != center_2); // function doesn't update any locators, so uses cached
  ASSERT_TRUE(mesh2.getPoint(center_id_1) == center_point_1); // doesn't update locators
  ASSERT_TRUE(mesh2.closestPointId(center_1) == center_id_1); // updates point locater, so noop
}

TEST(MeshTests, writeTest1)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/ellipsoid_01.vtk");
  ellipsoid.write(std::string(TEST_DATA_DIR) + "/ellipsoidBinary.vtk", true);

  Mesh ellipsoidBinary(std::string(TEST_DATA_DIR) + "/ellipsoidBinary.vtk");

  ASSERT_TRUE(ellipsoid == ellipsoidBinary);
}

TEST(MeshTests, writeTest2)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  ellipsoid.write(std::string(TEST_DATA_DIR) + "/ellipsoidBinary.ply", true);

  Mesh ellipsoidBinary(std::string(TEST_DATA_DIR) + "/ellipsoidBinary.ply");

  ASSERT_TRUE(ellipsoid == ellipsoidBinary);
}

TEST(MeshTests, subdivisionTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/m03.vtk");
  femur.applySubdivisionFilter();;

  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/butterfly.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, subdivisionTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/m03.vtk");
  femur.applySubdivisionFilter(Mesh::SubdivisionType::Loop);

  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/loop.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, geodesicTest1)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  const int p1 = 10;
  const int p2 = 200;
  double geodesic_dist = ellipsoid.geodesicDistance(p1, p2);

  auto pt1 = ellipsoid.getPoint(p1);
  auto pt2 = ellipsoid.getPoint(p2);
  auto euclidean_dist = pt2.EuclideanDistanceTo(pt1);

  ASSERT_TRUE(std::abs(euclidean_dist - 6.70625) < 1e-4);
  ASSERT_TRUE(std::abs(geodesic_dist - 6.577) < 1e-4);
}

TEST(MeshTests, geodesicTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/m03.vtk");
  auto distField = femur.geodesicDistance(femur.getPoint(200));
  femur.setField("GeodesicDistanceToLandmark", distField, Mesh::Point);

  Mesh ground_truth(std::string(TEST_DATA_DIR) + std::string("/geodesic1.vtk"));

  ASSERT_TRUE(femur.compareField(ground_truth, "GeodesicDistanceToLandmark"));
}

TEST(MeshTests, geodesicTest3)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/ellipsoid_01.vtk");

  std::vector<Point3> curve;
  curve.push_back(ellipsoid.getPoint(100));
  curve.push_back(ellipsoid.getPoint(200));
  curve.push_back(ellipsoid.getPoint(300));

  auto distField = ellipsoid.geodesicDistance(curve);
  ellipsoid.setField("GeodesicDistanceToCurve", distField, Mesh::Point);

  Mesh ground_truth(std::string(TEST_DATA_DIR) + std::string("/geodesic_curve.vtk"));

  ASSERT_TRUE(ellipsoid.compareField(ground_truth, "GeodesicDistanceToCurve"));
}

TEST(MeshTests, curvatureTest1)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + std::string("/ellipsoid_0.ply"));
  auto curv = mesh.curvature(Mesh::CurvatureType::Mean);
  mesh.setField("MeanCurvature", curv, Mesh::Point);

  Mesh ground_truth(std::string(TEST_DATA_DIR) + std::string("/meanCurvatureEllipsoid.vtk"));

  ASSERT_TRUE(mesh.compareField(ground_truth, "MeanCurvature", "MeanCurvature", 1e-14));
}

TEST(MeshTests, curvatureTest2)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + std::string("/ellipsoid_0.ply"));
  auto curv = mesh.curvature();
  mesh.setField("PrincipalCurvature", curv, Mesh::Point);

  Mesh ground_truth(std::string(TEST_DATA_DIR) + std::string("/principalCurvatureEllipsoid.vtk"));

  ASSERT_TRUE(mesh.compareField(ground_truth, "PrincipalCurvature", "PrincipalCurvature", 1e-14));
}

TEST(MeshTests, curvatureTest3)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + std::string("/ellipsoid_0.ply"));
  auto curv = mesh.curvature(Mesh::CurvatureType::Gaussian);
  mesh.setField("GaussianCurvature", curv, Mesh::Point);

  Mesh ground_truth(std::string(TEST_DATA_DIR) + std::string("/gaussianCurvatureEllipsoid.vtk"));

  ASSERT_TRUE(mesh.compareField(ground_truth, "GaussianCurvature", "GaussianCurvature", 1e-14));
}

TEST(MeshTests, curvatureTest4)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + std::string("/m03.vtk"));
  auto curv = mesh.curvature(Mesh::CurvatureType::Mean);
  mesh.setField("MeanCurvature", curv, Mesh::Point);

  Mesh ground_truth(std::string(TEST_DATA_DIR) + std::string("/meanCurvatureFemur.vtk"));

  ASSERT_TRUE(mesh.compareField(ground_truth, "MeanCurvature", "MeanCurvature", 1e-14));
}

TEST(MeshTests, curvatureTest5)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + std::string("/m03.vtk"));
  auto curv = mesh.curvature();
  mesh.setField("PrincipalCurvature", curv, Mesh::Point);

  Mesh ground_truth(std::string(TEST_DATA_DIR) + std::string("/principalCurvatureFemur.vtk"));

  ASSERT_TRUE(mesh.compareField(ground_truth, "PrincipalCurvature", "PrincipalCurvature", 1e-14));
}

TEST(MeshTests, curvatureTest6)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + std::string("/m03.vtk"));
  auto curv = mesh.curvature(Mesh::CurvatureType::Gaussian);
  mesh.setField("GaussianCurvature", curv, Mesh::Point);

  Mesh ground_truth(std::string(TEST_DATA_DIR) + std::string("/gaussianCurvatureFemur.vtk"));

  ASSERT_TRUE(mesh.compareField(ground_truth, "GaussianCurvature", "GaussianCurvature", 1e-14));
}

TEST(MeshTests, computemeannormalsTest1)
{
  std::vector<std::reference_wrapper<const Mesh>> meshes;
  Mesh mesh1(std::string(TEST_DATA_DIR) + std::string("/m03_normals.vtk"));
  meshes.push_back(mesh1);
  Mesh mesh2(std::string(TEST_DATA_DIR) + std::string("/m04_normals.vtk"));
  meshes.push_back(mesh2);

  auto meanNormalsArray = MeshUtils::computeMeanNormals(meshes);
  Mesh mesh(std::string(TEST_DATA_DIR) + std::string("/m03.vtk"));
  mesh.setField("MeanNormals", meanNormalsArray, Mesh::Point);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/meannormals.vtk");

  ASSERT_TRUE(mesh.compareField(ground_truth, "MeanNormals"));
}

TEST(MeshTests, computemeannormalsTest2)
{
  std::vector<std::string> filenames;
  filenames.push_back(std::string(TEST_DATA_DIR) + std::string("/m03.vtk"));
  filenames.push_back(std::string(TEST_DATA_DIR) + std::string("/m04.vtk"));

  auto meanNormalsArray = MeshUtils::computeMeanNormals(filenames, true);
  Mesh mesh(std::string(TEST_DATA_DIR) + std::string("/m03.vtk"));
  mesh.setField("MeanNormals", meanNormalsArray, Mesh::Point);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/meannormals.vtk");

  ASSERT_TRUE(mesh.compareField(ground_truth, "MeanNormals"));
}

TEST(MeshTests, generateNormalsTest)
{
  std::vector<std::reference_wrapper<Mesh>> meshes;
  Mesh mesh1(std::string(TEST_DATA_DIR) + std::string("/m03.vtk"));
  meshes.push_back(mesh1);
  Mesh mesh2(std::string(TEST_DATA_DIR) + std::string("/m04.vtk"));
  meshes.push_back(mesh2);
  MeshUtils::generateNormals(meshes);

  Mesh ground_truth1(std::string(TEST_DATA_DIR) + std::string("/m03_normals.vtk"));
  Mesh ground_truth2(std::string(TEST_DATA_DIR) + std::string("/m04_normals.vtk"));

  ASSERT_TRUE(mesh1.compareField(ground_truth1, "Normals") && mesh2.compareField(ground_truth2, "Normals"));
}

TEST(MeshTests, readFailTest)
{
  try {
    Mesh mesh(std::string(TEST_DATA_DIR) + "/foo.vtk");
  } catch(...) {
    ASSERT_TRUE(true);
    return;
  }

  ASSERT_TRUE(false);
}

TEST(MeshTests, fixelementTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.fixElement();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/fixElement.vtk");
  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, remeshTest1)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  ellipsoid.remesh(3000, 1.0);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/remesh1.vtk");
  ASSERT_TRUE(ellipsoid == ground_truth);
}

TEST(MeshTests, remeshTest2)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/ellipsoid_01.vtk");
  ellipsoid.remesh(1000, 2.0);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/remesh2.vtk");
  ASSERT_TRUE(ellipsoid == ground_truth);
}

TEST(MeshTests, remeshPercentTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.remeshPercent(0.25, 1.0);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/remeshPercent1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, remeshPercentTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.remeshPercent(0.10, 0.5);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/remeshPercent2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, smoothTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.smooth(10, 0.01);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/smooth1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, smoothTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.smooth();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/smooth2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, smoothSincTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/la-bin.vtk");
  femur.smoothSinc(10,0.05);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/smoothsinc.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, invertNormalsTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.invertNormals();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/invertnormals.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, reflectTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.reflect(Axis::X);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/reflect1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, reflectTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.reflect(Axis::Y);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/reflect2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, fillHolesTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.fillHoles();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/fillholes.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, probeVolumeTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.probeVolume(std::string(TEST_DATA_DIR) + "/femurVtkDT.nrrd");
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/probe.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.clip(makePlane(Point3({-91.0, 0.0, 1230.0}),
                       makeVector({0.0, 0.0, 1.0}))); // clip upper half of mesh from center
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clip1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.clip(makePlane(Point3({-91.0, 0.0, 1230.0}),
                       makeVector({0.0, 0.0, -1.0}))); // clip lower half of mesh from center
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clip2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipTest3)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.clip(makePlane(Point3({-60.0, 10.0, 1235.0}),
                       makeVector({-5.0, 3.14159, 1.0}))); // clip arbitrary mesh from an edge
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clip3.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, translateTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.translate(makeVector({1.0,1.0,1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/translate1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, translateTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.translate(makeVector({-10.0,-10.0,-10.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/translate2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, translateTest3)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.translate(makeVector({0,0,1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/translate3.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, scaleTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.scale(makeVector({1.0,1.0,1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/scale1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, scaleTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.scale(makeVector({-1.0, 1.5, 1.0}));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/scale2.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, clipClosedSurfaceTest1)
{
  Point p({10.0, 0.0, 10.0});
  Vector n(makeVector({0, 850, 0}));

  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.clipClosedSurface(makePlane(p, n));
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/clipClosed1.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, centerTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  Point3 center(femur.center());

  ASSERT_TRUE(epsEqual(center, Point3({90.7541, -160.557, -673.572}), 1e-3));
}

TEST(MeshTests, centerofmassTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  Point3 com(femur.centerOfMass());

  ASSERT_TRUE(epsEqual(com, Point3({92.6201, -157.662, -666.611}), 1e-3));
}

TEST(MeshTests, toImageTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  Image image = femur.toImage();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femurImage.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(MeshTests, toImageTest2)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");

  // pass the region (should be identical)
  Image image = femur.toImage(femur.boundingBox());
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femurImage.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(MeshTests, toImageTest3)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");

  // pad the region
  auto bbox = femur.boundingBox();
  Image image = femur.toImage(bbox.pad(1.5));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femurImagePad.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(MeshTests, toImageTest4)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.ply");
  auto bbox = femur.boundingBox();
  Image image = femur.toImage(bbox.pad(1.5), Point({0.5, 1.5, 12.5}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femurImagePadSpace.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(MeshTests, boundingBoxTest1)
{
  std::string meshes_location = std::string(TEST_DATA_DIR) + std::string("/");
  std::vector<std::string> meshes = {
    meshes_location + "m03_L_femur.ply",
    meshes_location + "m04_L_femur.ply",
    meshes_location + "femur.ply",
    meshes_location + "ellipsoid_0.ply",
    meshes_location + "femur.vtk"
  };

  auto region = MeshUtils::boundingBox(meshes);
  Point min({-112.139, -192.471, -1217.76});
  Point max({135.026, 21.495, 1248.45});

  ASSERT_TRUE(epsEqual(region.min, min, 1e-2) && epsEqual(region.max, max, 1e-2));
}

TEST(MeshTests, boundingBoxTest2)
{
  Mesh mesh1(std::string(TEST_DATA_DIR) + std::string("/m03_L_femur.ply"));
  Mesh mesh2(std::string(TEST_DATA_DIR) + std::string("/m04_L_femur.ply"));
  Mesh mesh3(std::string(TEST_DATA_DIR) + std::string("/femur.ply"      ));
  Mesh mesh4(std::string(TEST_DATA_DIR) + std::string("/ellipsoid_0.ply"));
  Mesh mesh5(std::string(TEST_DATA_DIR) + std::string("/femur.vtk"      ));

  std::vector<std::reference_wrapper<const Mesh>> meshes
  {mesh1, mesh2, mesh3, mesh4, mesh5};

  auto region = MeshUtils::boundingBox(meshes);
  Point min({-112.139, -192.471, -1217.76});
  Point max({135.026, 21.495, 1248.45});

  ASSERT_TRUE(epsEqual(region.min, min, 1e-2) && epsEqual(region.max, max, 1e-2));
}

TEST(MeshTests, antialiasTest3)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + "/femur.ply");
  Image aa1(mesh.toImage(mesh.boundingBox().pad(3.0)));
  aa1.antialias(50, 0.0);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/antialiasMesh.nrrd");

  ASSERT_TRUE(aa1 == ground_truth);
}

TEST(MeshTests, toDistanceTransformTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur_remesh.ply");
  Image image = femur.toDistanceTransform(PhysicalRegion(), Point3({5., 5., 5.}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/femur_remesh_dt.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(MeshTests, coverageTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  pelvis.coverage(femur);

  Mesh baseline(std::string(TEST_DATA_DIR) + "/fm_coverage.vtk");
  ASSERT_TRUE(pelvis == baseline);
}

TEST(MeshTests, distanceTest1)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  Mesh pelvis(std::string(TEST_DATA_DIR) + "/pelvis.vtk");
  auto f2p_distances = femur.distance(pelvis, Mesh::DistanceMethod::PointToPoint);
  auto p2f_distances = pelvis.distance(femur, Mesh::DistanceMethod::PointToPoint);
  femur.setField("distance", f2p_distances[0], Mesh::Point);
  pelvis.setField("distance", p2f_distances[0], Mesh::Point);
  femur.setField("closestPoints", f2p_distances[1], Mesh::Point);
  pelvis.setField("closestPoints", p2f_distances[1], Mesh::Point);

  Mesh f2p(std::string(TEST_DATA_DIR) + "/meshdistance_point_fwd.vtk");
  Mesh p2f(std::string(TEST_DATA_DIR) + "/meshdistance_point_rev.vtk");
  ASSERT_TRUE(femur == f2p);
  ASSERT_TRUE(pelvis == p2f);
}

TEST(MeshTests, distanceTest2)
{
  Mesh femur1(std::string(TEST_DATA_DIR) + "/m03_L_femur.ply");
  Mesh femur2(std::string(TEST_DATA_DIR) + "/m04_L_femur.ply");
  auto fwd_distances = femur1.distance(femur2, Mesh::DistanceMethod::PointToCell);
  auto rev_distances = femur2.distance(femur1, Mesh::DistanceMethod::PointToCell);
  femur1.setField("distance", fwd_distances[0], Mesh::Point);
  femur2.setField("distance", rev_distances[0], Mesh::Point);
  femur1.setField("closestCells", fwd_distances[1], Mesh::Point);
  femur2.setField("closestCells", rev_distances[1], Mesh::Point);

  Mesh fwd(std::string(TEST_DATA_DIR) + "/meshdistance_cell_fwd.vtk");
  Mesh rev(std::string(TEST_DATA_DIR) + "/meshdistance_cell_rev.vtk");
  ASSERT_TRUE(femur1 == fwd);
  ASSERT_TRUE(femur2 == rev);
}

TEST(MeshTests, pointsTest)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/simple_ellipsoid.ply");
  auto verts = ellipsoid.points();
  Point3 p0({verts.row(0)[0], verts.row(0)[1], verts.row(0)[2]});
  Point3 g0({1.12801208e+01,  1.84252377e+01,  2.66504917e+01});
  Point3 pn({verts.row(verts.rows()-1)[0], verts.row(verts.rows()-1)[1], verts.row(verts.rows()-1)[2]});
  Point3 gn({3.35370102e+01,  1.25301433e+00,  3.71165695e+01});

  ASSERT_TRUE(verts.rows() == 14 && verts.cols() == 3);
  ASSERT_TRUE(epsEqual(p0, g0, 1e-6));
  ASSERT_TRUE(epsEqual(pn, gn, 1e-6));
}

TEST(MeshTests, facesTest)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/simple_ellipsoid.ply");
  auto faces = ellipsoid.faces();
  IPoint3 f0({faces.row(0)[0], faces.row(0)[1], faces.row(0)[2]});
  IPoint3 g0({7,8,4});
  IPoint3 fn({faces.row(faces.rows()-1)[0], faces.row(faces.rows()-1)[1], faces.row(faces.rows()-1)[2]});
  IPoint3 gn({7,6,8});

  ASSERT_TRUE(faces.rows() == 24 && faces.cols() == 3);
  ASSERT_TRUE(f0 == g0);
  ASSERT_TRUE(fn == gn);
}

TEST(MeshTests, getPointTest)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/simple_ellipsoid.ply");
  auto p = ellipsoid.getPoint(7);
  auto closeToP = Point3({44.7543, 2.43769, 12.953});

  ASSERT_TRUE(epsEqual(p, closeToP, 1e-4));
}

TEST(MeshTests, getFaceTest)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/simple_ellipsoid.ply");
  auto f = ellipsoid.getFace(12);
  auto face = IPoint3({9,12,1});

  ASSERT_TRUE(f == face);
}

TEST(MeshTests, closestpointTest1)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  ellipsoid.computeNormals();
  auto normals = ellipsoid.getField("Normals", Mesh::Point);
  auto n = normals->GetTuple3(42);
  auto v = makeVector({n[0], n[1], n[2]});
  auto p = ellipsoid.getPoint(42);
  auto pNew = p + v;
  bool outside = false;
  double distance;
  vtkIdType face_id = -1;
  auto closeToP = ellipsoid.closestPoint(pNew, outside, distance, face_id);

  ASSERT_TRUE(epsEqual(p, closeToP, 1e-2) && outside == true && epsEqual(distance, 1.0, 1e-5) && face_id == 90);
}

TEST(MeshTests, closestpointTest2)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/sphere_highres.ply");
  ellipsoid.computeNormals();
  auto normals = ellipsoid.getField("Normals", Mesh::Point);
  auto n = normals->GetTuple3(42);
  auto v = makeVector({n[0], n[1], n[2]});
  auto p = ellipsoid.getPoint(42);
  auto pNew = p - v * 1.1;
  bool outside = false;
  double distance;
  vtkIdType face_id = -1;
  auto closeToP = ellipsoid.closestPoint(pNew, outside, distance, face_id);

  ASSERT_TRUE(epsEqual(p, closeToP, 1e-2) && outside == false && epsEqual(distance, 1.1, 1e-5) && face_id == 9);
}

TEST(MeshTests, closestpointIdTest)
{
  Mesh ellipsoid(std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  auto p = ellipsoid.getPoint(50);
  auto id = ellipsoid.closestPointId(p);

  ASSERT_TRUE(id == 50);
}

TEST(MeshTests, fieldTest1)
{
  Mesh dist(std::string(TEST_DATA_DIR) + "/meshdistance2.vtk");
  double a = dist.getFieldValue("distance", 0);
  double b = dist.getFieldValue("distance", 1000);
  double c = dist.getFieldValue("distance", dist.numPoints()-1);

  ASSERT_TRUE(std::abs(a - 0.375761) < 1e-4);
  ASSERT_TRUE(std::abs(b - 2.18114) < 1e-4);
  ASSERT_TRUE(std::abs(c - 6.915) < 1e-4);
}

TEST(MeshTests, fieldTest2)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + "/la-bin.vtk");
  double a = mesh.getFieldValue("scalars", 0);
  double b = mesh.getFieldValue("scalars", 1000);
  double c = mesh.getFieldValue("Normals", 4231);
  double d = mesh.getFieldValue("Normals", 5634);

  ASSERT_TRUE(a==1);
  ASSERT_TRUE(b==1);
  ASSERT_TRUE(std::abs(c - 0.57735) < 1e-4);
  ASSERT_TRUE(d==0);
}

TEST(MeshTests, fieldTest3)
{
  Mesh mesh(std::string(TEST_DATA_DIR) + "/la-bin.vtk");
  std::vector<double> scalarRange = range(mesh.getField("scalars", Mesh::Point));
  std::vector<double> normalsRange = range(mesh.getField("Normals", Mesh::Point));

  ASSERT_TRUE(scalarRange[0]==1);
  ASSERT_TRUE(scalarRange[1]==1);
  ASSERT_TRUE(normalsRange[0]==-1);
  ASSERT_TRUE(normalsRange[1]==1);
}

//TODO: add tests for independent fields and fields on cells once #935 complete

TEST(MeshTests, icpTestRigid)
{
  Mesh source(std::string(TEST_DATA_DIR) + "/m03_L_femur.ply");
  Mesh target(std::string(TEST_DATA_DIR) + "/m04_L_femur.ply");
  shapeworks::MeshTransform transform = source.createTransform(target, Mesh::Rigid);
  source.applyTransform(transform);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/icpRigid.ply");

  ASSERT_TRUE(source == ground_truth);
}

TEST(MeshTests, icpTestSimilarity)
{
  Mesh source(std::string(TEST_DATA_DIR) + "/m03_L_femur.ply");
  Mesh target(std::string(TEST_DATA_DIR) + "/m04_L_femur.ply");
  shapeworks::MeshTransform transform = source.createTransform(target);
  source.applyTransform(transform);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/icpSimilarity.ply");

  ASSERT_TRUE(source == ground_truth);
}

TEST(MeshTests, icpTestAffine)
{
  Mesh source(std::string(TEST_DATA_DIR) + "/m03_L_femur.ply");
  Mesh target(std::string(TEST_DATA_DIR) + "/m04_L_femur.ply");
  shapeworks::MeshTransform transform = source.createTransform(target, Mesh::Affine);
  source.applyTransform(transform);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/icpAffine.ply");

  ASSERT_TRUE(source == ground_truth);
}

TEST(MeshTests, computeNormalsTest)
{
  Mesh femur(std::string(TEST_DATA_DIR) + "/femur.vtk");
  femur.computeNormals();
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/normals.vtk");

  ASSERT_TRUE(femur == ground_truth);
}

TEST(MeshTests, warpTest1)
{
  Mesh ellipsoid( std::string(TEST_DATA_DIR) + "/ellipsoid_0.ply");
  Mesh ellipsoid_warped( std::string(TEST_DATA_DIR) + "/ellipsoid_warped.ply");

  std::string staticPath = std::string(TEST_DATA_DIR) + "/ellipsoid_0.particles";
  std::string movingPath = std::string(TEST_DATA_DIR) + "/ellipsoid_1.particles";
  std::vector<std::string> paths;
  paths.push_back(staticPath);
  paths.push_back(movingPath);
  ParticleSystem particlesystem(paths);
  Eigen::MatrixXd allPts = particlesystem.Particles();
  Eigen::MatrixXd staticPoints = allPts.col(0);
  Eigen::MatrixXd movingPoints = allPts.col(1);

  int numParticles = staticPoints.rows() / 3;
  staticPoints.resize(3, numParticles);
  staticPoints.transposeInPlace();
  movingPoints.resize(3, numParticles);
  movingPoints.transposeInPlace();

  MeshWarper warper;
  warper.set_reference_mesh(ellipsoid.getVTKMesh(), staticPoints);
  ASSERT_TRUE(warper.get_warp_available());

  Mesh output = warper.build_mesh(movingPoints);
  ASSERT_TRUE(output == ellipsoid_warped);
}

TEST(MeshTests, warpTest2)
{
  Mesh ellipsoid( std::string(TEST_DATA_DIR) + "/mesh_warp/mesh_warp2.vtk");
  Mesh ellipsoid_warped( std::string(TEST_DATA_DIR) + "/mesh_warp/mesh_warp2_baseline.vtk");

  std::string staticPath = std::string(TEST_DATA_DIR) + "/mesh_warp/mesh_warp2.particles";
  std::string movingPath = std::string(TEST_DATA_DIR) + "/mesh_warp/mesh_warp2.particles";
  std::vector<std::string> paths;
  paths.push_back(staticPath);
  paths.push_back(movingPath);
  ParticleSystem particlesystem(paths);
  Eigen::MatrixXd allPts = particlesystem.Particles();
  Eigen::MatrixXd staticPoints = allPts.col(0);
  Eigen::MatrixXd movingPoints = allPts.col(1);

  int numParticles = staticPoints.rows() / 3;
  staticPoints.resize(3, numParticles);
  staticPoints.transposeInPlace();
  movingPoints.resize(3, numParticles);
  movingPoints.transposeInPlace();

  MeshWarper warper;
  warper.set_reference_mesh(ellipsoid.getVTKMesh(), staticPoints);
  ASSERT_TRUE(warper.get_warp_available());

  Mesh output = warper.build_mesh(movingPoints);
  ASSERT_TRUE(output == ellipsoid_warped);
}

TEST(MeshTests, warpTest3)
{
  // the output here is terrible because the particles are bad, but this test case is included
  // because it demonstrated a crash in the mesh warping code
  Mesh reference( std::string(TEST_DATA_DIR) + "/mesh_warp/mesh_warp3.ply");
  Mesh baseline( std::string(TEST_DATA_DIR) + "/mesh_warp/mesh_warp3_baseline.vtk");

  std::string staticPath = std::string(TEST_DATA_DIR) + "/mesh_warp/mesh_warp3.particles";
  std::string movingPath = std::string(TEST_DATA_DIR) + "/mesh_warp/mesh_warp3b.particles";
  std::vector<std::string> paths;
  paths.push_back(staticPath);
  paths.push_back(movingPath);
  ParticleSystem particlesystem(paths);
  Eigen::MatrixXd allPts = particlesystem.Particles();
  Eigen::MatrixXd staticPoints = allPts.col(0);
  Eigen::MatrixXd movingPoints = allPts.col(1);

  int numParticles = staticPoints.rows() / 3;
  staticPoints.resize(3, numParticles);
  staticPoints.transposeInPlace();
  movingPoints.resize(3, numParticles);
  movingPoints.transposeInPlace();

  MeshWarper warper;
  warper.set_reference_mesh(reference.getVTKMesh(), staticPoints);
  ASSERT_TRUE(warper.get_warp_available());

  Mesh output = warper.build_mesh(movingPoints);
  ASSERT_TRUE(output == baseline);
}

TEST(MeshTests, findReferenceMeshTest)
{
  std::vector<Mesh> meshes;
  meshes.push_back(Mesh(std::string(TEST_DATA_DIR) + "/m03_L_femur.ply"));
  meshes.push_back(Mesh(std::string(TEST_DATA_DIR) + "/m04_L_femur.ply"));
  meshes.push_back(Mesh(std::string(TEST_DATA_DIR) + "/m03.vtk"));
  int ref = MeshUtils::findReferenceMesh(meshes);
  ASSERT_EQ(ref, 2);
}

TEST(MeshTests, addMesh)
{
  Mesh mesh1(std::string(TEST_DATA_DIR) + "/sphere_00.ply");
  Mesh mesh2(std::string(TEST_DATA_DIR) + "/sphere_00_translated.ply");
  mesh1 += mesh2;
  Mesh baseline(std::string(TEST_DATA_DIR) + "/sphere_add.ply");

  ASSERT_TRUE(mesh1 == baseline);
}

TEST(MeshTests, constructFromMatrixes)
{
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/femur.vtk");
  ground_truth.computeNormals();
  Mesh construct(ground_truth.points(), ground_truth.faces());
  ASSERT_TRUE(construct == ground_truth);
}

TEST(MeshTests, sharedBoundaryExtractor)
{
  Mesh left(std::string(TEST_DATA_DIR) + "/shared_boundary/00_l.vtk");
  Mesh right(std::string(TEST_DATA_DIR) + "/shared_boundary/00_r.vtk");
  double tol = 0.001;
  std::array<Mesh, 3> output = MeshUtils::sharedBoundaryExtractor(left, right, tol);
  Mesh output_l = output[0];
  Mesh output_r = output[1];
  Mesh output_s = output[2];

  Mesh ground_truth_left(std::string(TEST_DATA_DIR) + "/shared_boundary/00_out_l.vtk");
  Mesh ground_truth_right(std::string(TEST_DATA_DIR) + "/shared_boundary/00_out_r.vtk");
  Mesh ground_truth_shared(std::string(TEST_DATA_DIR) + "/shared_boundary/00_out_s.vtk");

  ASSERT_TRUE(ground_truth_left == output_l);
  ASSERT_TRUE(ground_truth_right == output_r);
  ASSERT_TRUE(ground_truth_shared == output_s);

}

TEST(MeshTests, boundaryLoopExtractor)
{
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/shared_boundary/00_out_c.vtp");
  Mesh mesh(std::string(TEST_DATA_DIR) + "/shared_boundary/00_out_s.vtk");
  Mesh loop = MeshUtils::boundaryLoopExtractor(mesh);
  ASSERT_TRUE(loop == ground_truth);
}
