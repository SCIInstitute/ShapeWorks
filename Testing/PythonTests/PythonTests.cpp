#include "Testing.h"

//---------------------------------------------------------------------------
void run_use_case(const std::string& name)
{
  setupenv(std::string(TEST_DATA_DIR) + "/../PythonTests");

  std::string command = "python " + name;
  ASSERT_FALSE(system(command.c_str()));
}

TEST(pythonTests, initImageTest)
{
  run_use_case("initImage.py");
}

TEST(pythonTests, toArrayTest)
{
  run_use_case("toArray.py");
}

TEST(pythonTests, imageInfoTest)
{
  run_use_case("imageinfo.py");
}

TEST(pythonTests, meshInfoTest)
{
  run_use_case("meshinfo.py");
}

TEST(pythonTests, copyTest)
{
  run_use_case("copyImgMesh.py");
}

TEST(pythonTests, compareTest)
{
  run_use_case("compare.py");
}

TEST(pythonTests, antialiasTest)
{
  run_use_case("antialias.py");
}

TEST(pythonTests, resampleTest)
{
  run_use_case("resample.py");
}

TEST(pythonTests, resizeTest)
{
  run_use_case("resize.py");
}

TEST(pythonTests, padTest)
{
  run_use_case("pad.py");
}

TEST(pythonTests, recenterTest)
{
  run_use_case("recenter.py");
}

TEST(pythonTests, translateTest)
{
  run_use_case("translate.py");
}

TEST(pythonTests, centerofmassTest)
{
  run_use_case("centerofmass.py");
}

TEST(pythonTests, scaleTest)
{
  run_use_case("scale.py");
}

TEST(pythonTests, rotateTest)
{
  run_use_case("rotate.py");
}

TEST(pythonTests, extractLabelTest)
{
  run_use_case("extractlabel.py");
}

TEST(pythonTests, closeholesTest)
{
  run_use_case("closeholes.py");
}

TEST(pythonTests, binarizeTest)
{
  run_use_case("binarize.py");
}

TEST(pythonTests, computedtTest)
{
  run_use_case("computedt.py");
}

TEST(pythonTests, curvatureTest)
{
  run_use_case("curvature.py");
}

TEST(pythonTests, gradientTest)
{
  run_use_case("gradient.py");
}

TEST(pythonTests, sigmoidTest)
{
  run_use_case("sigmoid.py");
}

TEST(pythonTests, tplevelsetTest)
{
  run_use_case("tplevelset.py");
}

TEST(pythonTests, intensityTest)
{
  run_use_case("intensity.py");
}

TEST(pythonTests, topoTest)
{
  run_use_case("topo.py");
}

TEST(pythonTests, blurTest)
{
  run_use_case("blur.py");
}

TEST(pythonTests, boundingboxTest)
{
  run_use_case("boundingbox.py");
}

TEST(pythonTests, cropTest)
{
  run_use_case("crop.py");
}

TEST(pythonTests, icpTest)
{
  run_use_case("icp.py");
}

TEST(pythonTests, clipTest)
{
  run_use_case("clip.py");
}

TEST(pythonTests, reflectTest)
{
  run_use_case("reflect.py");
}

TEST(pythonTests, warpTest)
{
  run_use_case("warp.py");
}

TEST(pythonTests, setoriginTest)
{
  run_use_case("setorigin.py");
}

TEST(pythonTests, setspacingTest)
{
  run_use_case("setspacing.py");
}

TEST(pythonTests, negateTest)
{
  run_use_case("negate.py");
}

TEST(pythonTests, addTest)
{
  run_use_case("add.py");
}

TEST(pythonTests, subTest)
{
  run_use_case("sub.py");
}

TEST(pythonTests, mulTest)
{
  run_use_case("mul.py");
}

TEST(pythonTests, divTest)
{
  run_use_case("div.py");
}

TEST(pythonTests, smoothTest)
{
  run_use_case("smooth.py");
}

TEST(pythonTests, decimateTest)
{
  run_use_case("decimate.py");
}

TEST(pythonTests, invertnormalsTest)
{
  run_use_case("invertNormals.py");
}

TEST(pythonTests, fillholesTest)
{
  run_use_case("fillHoles.py");
}

TEST(pythonTests, probevolumeTest)
{
  run_use_case("probeVolume.py");
}

TEST(pythonTests, coverageTest)
{
  run_use_case("coverage.py");
}

TEST(pythonTests, distanceTest)
{
  run_use_case("distance.py");
}

TEST(pythonTests, closestpointTest)
{
  run_use_case("closestpoint.py");
}

TEST(pythonTests, closestpointidTest)
{
  run_use_case("closestpointid.py");
}

TEST(pythonTests, meannormalsTest)
{
  run_use_case("meannormals.py");
}

TEST(pythonTests, getpointTest)
{
  run_use_case("getpoint.py");
}

TEST(pythonTests, getfaceTest)
{
  run_use_case("getface.py");
}

TEST(pythonTests, facesTest)
{
  run_use_case("faces.py");
}

TEST(pythonTests, pointsTest)
{
  run_use_case("points.py");
}

TEST(pythonTests, geodesicTest)
{
  run_use_case("geodesic.py");
}

TEST(pythonTests, fixElementTest)
{
  ASSERT_FALSE(system("python fixelement.py"));
}

TEST(pythonTests, computenormalsTest)
{
  run_use_case("computenormals.py");
}

TEST(pythonTests, toimageTest)
{
  run_use_case("toImage.py");
}

TEST(pythonTests, todistancetransformTest)
{
  run_use_case("toDistanceTransform.py");
}

TEST(pythonTests, gradientinterpolatorTest)
{
  run_use_case("gradientinterp.py");
}

TEST(pythonTests, fieldTest)
{
  run_use_case("field.py");
}

// note: we just use numpy arrays for coordinates/indices, points, dimensions, vectors, and matrices
TEST(pythonTests, coordTest)
{
  run_use_case("coord.py");
}

TEST(pythonTests, statsTest)
{
  run_use_case("stats.py");
}

TEST(pythonTests, optimizeTest)
{
  run_use_case("optimize.py");
}

TEST(pythonTests, pcaTest)
{
  run_use_case("pca.py");
}

TEST(pythonTests, findreferencemeshTest)
{
  run_use_case("findReferenceMesh.py");
}
