#include "Testing.h"

//---------------------------------------------------------------------------
void run_use_case(const std::string& name)
{
  setupenv(std::string(TEST_DATA_DIR) + "/../shapeworksTests");

  std::string command = "bash " + name;
  ASSERT_FALSE(system(command.c_str()));
}

TEST(shapeworksTests, meannormalsTest)
{
  run_use_case("meannormals.sh");
}

TEST(shapeworksTests, closestpointTest)
{
  run_use_case("closestpoint.sh");
}

TEST(shapeworksTests, geodesicTest)
{
  run_use_case("geodesic.sh");
}

TEST(shapeworksTests, meshtoimageTest)
{
  run_use_case("meshtoimage.sh");
}

TEST(shapeworksTests, meshtodtTest)
{
  run_use_case("meshtodt.sh");
}

TEST(shapeworksTests, boundingboximageTest)
{
  run_use_case("boundingboximage.sh");
}

TEST(shapeworksTests, boundingboxmeshTest)
{
  run_use_case("boundingboxmesh.sh");
}

TEST(shapeworksTests, fieldnameTest)
{
  run_use_case("meshfieldnames.sh");
}

TEST(shapeworksTests, distanceTest)
{
  run_use_case("distance.sh");
}

TEST(shapeworksTests, getfieldvalueTest)
{
  run_use_case("getfieldvalue.sh");
}

TEST(shapeworksTests, fieldrangeTest)
{
  run_use_case("fieldrange.sh");
}

TEST(shapeworksTests, fieldmeanTest)
{
  run_use_case("fieldmean.sh");
}

TEST(shapeworksTests, fieldstdTest)
{
  run_use_case("fieldstd.sh");
}

TEST(shapeworksTests, tomeshTest)
{
  run_use_case("tomesh.sh");
}

TEST(shapeworksTests, clipmeshTest)
{
  run_use_case("clipmesh.sh");
}

TEST(shapeworksTests, meshinfoTest)
{
  run_use_case("meshinfo.sh");
}

TEST(shapeworksTests, coverageTest)
{
  run_use_case("coverage.sh");
}

TEST(shapeworksTests, probeTest)
{
  run_use_case("probe.sh");
}

TEST(shapeworksTests, compareTest)
{
  run_use_case("compare.sh");
}

TEST(shapeworksTests, antialiasTest)
{
  run_use_case("antialias.sh");
}

TEST(shapeworksTests, antialiasMeshTest)
{
  run_use_case("antialias_mesh.sh");
}

TEST(shapeworksTests, resampleTest)
{
  run_use_case("resample.sh");
}

TEST(shapeworksTests, resizeTest)
{
  run_use_case("resize.sh");
}

TEST(shapeworksTests, recenterTest)
{
  run_use_case("recenter.sh");
}

TEST(shapeworksTests, padTest)
{
  run_use_case("pad.sh");
}

TEST(shapeworksTests, translateimageTest)
{
  run_use_case("translateimage.sh");
}

TEST(shapeworksTests, centerofmassTest)
{
  run_use_case("centerofmass.sh");
}

TEST(shapeworksTests, scaleimageTest)
{
  run_use_case("scaleimage.sh");
}

TEST(shapeworksTests, rotateTest)
{
  run_use_case("rotate.sh");
}

TEST(shapeworksTests, extractabelTest)
{
  run_use_case("extractlabel.sh");
}

TEST(shapeworksTests, closeholesTest)
{
  run_use_case("closeholes.sh");
}

TEST(shapeworksTests, binarizeTest)
{
  run_use_case("binarize.sh");
}

TEST(shapeworksTests, computedtTest)
{
  run_use_case("computedt.sh");
}

TEST(shapeworksTests, curvatureTest)
{
  run_use_case("curvature.sh");
}

TEST(shapeworksTests, gradientTest)
{
  run_use_case("gradient.sh");
}

TEST(shapeworksTests, sigmoidTest)
{
  run_use_case("sigmoid.sh");
}

TEST(shapeworksTests, intensityTest)
{
  run_use_case("intensity.sh");
}

TEST(shapeworksTests, tplevelsetTest)
{
  run_use_case("tplevelset.sh");
}

TEST(shapeworksTests, topoTest)
{
  run_use_case("topo.sh");
}

TEST(shapeworksTests, blurTest)
{
  run_use_case("blur.sh");
}

TEST(shapeworksTests, icpimageTest)
{
  run_use_case("icpimage.sh");
}

TEST(shapeworksTests, cropTest)
{
  run_use_case("crop.sh");
}

TEST(shapeworksTests, clipimageTest)
{
  run_use_case("clipimage.sh");
}

TEST(shapeworksTests, reflectimageTest)
{
  run_use_case("reflectimage.sh");
}

TEST(shapeworksTests, warpTest)
{
  run_use_case("warp.sh");
}

TEST(shapeworksTests, setoriginTest)
{
  run_use_case("setorigin.sh");
}

TEST(shapeworksTests, setspacingTest)
{
  run_use_case("setspacing.sh");
}

TEST(shapeworksTests, negateTest)
{
  run_use_case("negate.sh");
}

TEST(shapeworksTests, additionTest)
{
  run_use_case("add.sh");
}

TEST(shapeworksTests, subtractionTest)
{
  run_use_case("sub.sh");
}

TEST(shapeworksTests, multiplicationTest)
{
  run_use_case("mul.sh");
}

TEST(shapeworksTests, divisionTest)
{
  run_use_case("div.sh");
}

TEST(shapeworksTests, smoothTest)
{
  run_use_case("smooth.sh");
}

TEST(shapeworksTests, decimateTest)
{
  run_use_case("decimate.sh");
}

TEST(shapeworksTests, reflectmeshTest)
{
  run_use_case("reflectmesh.sh");
}

TEST(shapeworksTests, fillholesTest)
{
  run_use_case("fillholes.sh");
}

TEST(shapeworksTests, translatemeshTest)
{
  run_use_case("translatemesh.sh");
}

TEST(shapeworksTests, scalemeshTest)
{
  run_use_case("scalemesh.sh");
}

// https://github.com/SCIInstitute/ShapeWorks/issues/938
// TEST(shapeworksTests, meshfixTest)
// {
//   run_use_case("meshfix.sh");
// }

TEST(shapeworksTests, clipclosedsurfaceTest)
{
  run_use_case("clipclosedsurface.sh");
}

TEST(shapeworksTests, meshtransformTest)
{
  run_use_case("meshtransform.sh");
}

TEST(shapeworksTests, generatenormalsTest)
{
  run_use_case("generatenormals.sh");
}

TEST(shapeworksTests, shapeevaluationTest)
{
  run_use_case("shapeevaluation.sh");
}
