#include "Testing.h"
#include <boost/filesystem/operations.hpp>

using namespace shapeworks;

//---------------------------------------------------------------------------
void run_isolated_test(const std::string& name) {
  // store the initial path
  auto initial_path = boost::filesystem::current_path();

  TestUtils::Instance().prep_temp(std::string(TEST_DATA_DIR) + "/python/" + name, "python_" + name);

  // -u generates unbuffered output, shown even if crashes
  std::string command = "python -u " + name + ".py";
  ASSERT_FALSE(system(command.c_str()));

  // change dir back to initial path
  boost::filesystem::current_path(initial_path);
}

//---------------------------------------------------------------------------
void run_test(const std::string& name) {
  std::string python_test_dir = std::string(TEST_DATA_DIR) + "/../PythonTests";
  setupenv(python_test_dir);

  // store the initial path
  auto initial_path = boost::filesystem::current_path();

  // change to temp dir
  auto temp_dir = shapeworks::TestUtils::Instance().get_output_dir(name);
  boost::filesystem::current_path(temp_dir);

  std::string
      command = "python -u " + python_test_dir + "/" + name; // -u generates unbuffered output, shown even if crashes
  ASSERT_FALSE(system(command.c_str()));

  // change dir back to initial path
  boost::filesystem::current_path(initial_path);
}

TEST(pythonTests, shared_boundaryTest) {
  run_test("shared_boundary.py");
}

TEST(pythonTests, reconstructsurfaceTest) {
  run_test("reconstructsurface.py");
}

TEST(pythonTests, reconstructpcaTest) {
  run_test("reconstructpca.py");
}

TEST(pythonTests, transformTest) {
  run_test("transform.py");
}

TEST(pythonTests, meshwriteTest) {
  run_test("meshwrite.py");
}

TEST(pythonTests, subdivisionTest) {
  run_test("subdivision.py");
}

TEST(pythonTests, sw2vtkmeshTest) {
  run_test("sw2vtkmesh.py");
}

TEST(pythonTests, meshcurvatureTest) {
  run_test("meshcurvature.py");
}

TEST(pythonTests, initImageTest) {
  run_test("initImage.py");
}

TEST(pythonTests, toArrayTest) {
  run_test("toArray.py");
}

TEST(pythonTests, imageInfoTest) {
  run_test("imageinfo.py");
}

TEST(pythonTests, meshInfoTest) {
  run_test("meshinfo.py");
}

TEST(pythonTests, copyTest) {
  run_test("copyImgMesh.py");
}

TEST(pythonTests, compareTest) {
  run_test("compare.py");
}

TEST(pythonTests, antialiasTest) {
  run_test("antialias.py");
}

TEST(pythonTests, resampleTest) {
  run_test("resample.py");
}

TEST(pythonTests, resizeTest) {
  run_test("resize.py");
}

TEST(pythonTests, padTest) {
  run_test("pad.py");
}

TEST(pythonTests, recenterTest) {
  run_test("recenter.py");
}

TEST(pythonTests, translateTest) {
  run_test("translate.py");
}

TEST(pythonTests, centerofmassTest) {
  run_test("centerofmass.py");
}

TEST(pythonTests, scaleTest) {
  run_test("scale.py");
}

TEST(pythonTests, rotateTest) {
  run_test("rotate.py");
}

TEST(pythonTests, extractLabelTest) {
  run_test("extractlabel.py");
}

TEST(pythonTests, closeholesTest) {
  run_test("closeholes.py");
}

TEST(pythonTests, binarizeTest) {
  run_test("binarize.py");
}

TEST(pythonTests, computedtTest) {
  run_test("computedt.py");
}

TEST(pythonTests, curvatureTest) {
  run_test("curvature.py");
}

TEST(pythonTests, gradientTest) {
  run_test("gradient.py");
}

TEST(pythonTests, sigmoidTest) {
  run_test("sigmoid.py");
}

TEST(pythonTests, tplevelsetTest) {
  run_test("tplevelset.py");
}

TEST(pythonTests, intensityTest) {
  run_test("intensity.py");
}

TEST(pythonTests, topoTest) {
  run_test("topo.py");
}

TEST(pythonTests, blurTest) {
  run_test("blur.py");
}

TEST(pythonTests, boundingboxTest) {
  run_test("boundingbox.py");
}

TEST(pythonTests, cropTest) {
  run_test("crop.py");
}

TEST(pythonTests, icpTest) {
  run_test("icp.py");
}

TEST(pythonTests, clipTest) {
  run_test("clip.py");
}

TEST(pythonTests, reflectTest) {
  run_test("reflect.py");
}

TEST(pythonTests, warpTest) {
  run_test("warp.py");
}

TEST(pythonTests, coordsysTest) {
  run_test("coordsys.py");
}

TEST(pythonTests, setoriginTest) {
  run_test("setorigin.py");
}

TEST(pythonTests, setspacingTest) {
  run_test("setspacing.py");
}

TEST(pythonTests, negateTest) {
  run_test("negate.py");
}

TEST(pythonTests, addTest) {
  run_test("add.py");
}

TEST(pythonTests, subTest) {
  run_test("sub.py");
}

TEST(pythonTests, mulTest) {
  run_test("mul.py");
}

TEST(pythonTests, divTest) {
  run_test("div.py");
}

TEST(pythonTests, smoothTest) {
  run_test("smooth.py");
}

TEST(pythonTests, remeshTest) {
  run_test("remesh.py");
}

TEST(pythonTests, invertnormalsTest) {
  run_test("invertNormals.py");
}

TEST(pythonTests, fillholesTest) {
  run_test("fillHoles.py");
}

TEST(pythonTests, probevolumeTest) {
  run_test("probeVolume.py");
}

TEST(pythonTests, coverageTest) {
  run_test("coverage.py");
}

TEST(pythonTests, distanceTest) {
  run_test("distance.py");
}

TEST(pythonTests, fixElementTest) {
  run_test("fixelement.py");
}

TEST(pythonTests, closestpointTest) {
  run_test("closestpoint.py");
}

TEST(pythonTests, closestpointidTest) {
  run_test("closestpointid.py");
}

TEST(pythonTests, meannormalsTest) {
  run_test("meannormals.py");
}

TEST(pythonTests, getpointTest) {
  run_test("getpoint.py");
}

TEST(pythonTests, getfaceTest) {
  run_test("getface.py");
}

TEST(pythonTests, facesTest) {
  run_test("faces.py");
}

TEST(pythonTests, pointsTest) {
  run_test("points.py");
}

TEST(pythonTests, geodesicTest) {
  run_test("geodesic.py");
}

TEST(pythonTests, computenormalsTest) {
  run_test("computenormals.py");
}

TEST(pythonTests, toimageTest) {
  run_test("toImage.py");
}

TEST(pythonTests, todistancetransformTest) {
  run_test("toDistanceTransform.py");
}

TEST(pythonTests, gradientinterpolatorTest) {
  run_test("gradientinterp.py");
}

TEST(pythonTests, fieldTest) {
  run_test("field.py");
}

// note: we just use numpy arrays for coordinates/indices, points, dimensions, vectors, and matrices
TEST(pythonTests, coordTest) {
  run_test("coord.py");
}

TEST(pythonTests, statsTest) {
  run_test("stats.py");
}

TEST(pythonTests, optimizeTest) {
  run_test("optimize.py");
}

TEST(pythonTests, pcaTest) {
  run_test("pca.py");
}

TEST(pythonTests, pcaEmbedderTest) {
  run_test("pcaembedder.py")
}

TEST(pythonTests, findreferencemeshTest) {
  run_test("findReferenceMesh.py");
}

TEST(pythonTests, isolateTest) {
  run_test("isolate.py");
}

TEST(pythonTests, projectTest) {
  run_isolated_test("project");
}

TEST(pythonTests, thicknessTest) {
  run_test("thickness.py");
}
