#include "Testing.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
void run_test(const std::string& name) {
  setupenv(std::string(TEST_DATA_DIR) + "/../shapeworksTests");

  std::string command = "bash " + name;
  ASSERT_FALSE(system(command.c_str()));
}
//---------------------------------------------------------------------------

void run_sandboxed_test(const std::string& name) {
  TestUtils::Instance().prep_temp(std::string(TEST_DATA_DIR) + "/shapeworksTests/" + name, name);
  std::string command = "bash " + name + ".sh";
  ASSERT_FALSE(system(command.c_str()));
}

//---------------------------------------------------------------------------

TEST(shapeworksTests, meshwriteTest) { run_test("meshwrite.sh"); }

TEST(shapeworksTests, meancurvatureTest) { run_test("meshcurvature.sh"); }

TEST(shapeworksTests, meannormalsTest) { run_test("meannormals.sh"); }

TEST(shapeworksTests, closestpointTest) { run_test("closestpoint.sh"); }

TEST(shapeworksTests, geodesicTest) { run_test("geodesic.sh"); }

TEST(shapeworksTests, meshtoimageTest) { run_test("meshtoimage.sh"); }

TEST(shapeworksTests, meshtodtTest) { run_test("meshtodt.sh"); }

TEST(shapeworksTests, boundingboximageTest) { run_test("boundingboximage.sh"); }

TEST(shapeworksTests, boundingboxmeshTest) { run_test("boundingboxmesh.sh"); }

TEST(shapeworksTests, fieldnameTest) { run_test("meshfieldnames.sh"); }

TEST(shapeworksTests, distanceTest) { run_test("distance.sh"); }

TEST(shapeworksTests, getfieldvalueTest) { run_test("getfieldvalue.sh"); }

TEST(shapeworksTests, fieldrangeTest) { run_test("fieldrange.sh"); }

TEST(shapeworksTests, fieldmeanTest) { run_test("fieldmean.sh"); }

TEST(shapeworksTests, fieldstdTest) { run_test("fieldstd.sh"); }

TEST(shapeworksTests, tomeshTest) { run_test("tomesh.sh"); }

TEST(shapeworksTests, clipmeshTest) { run_test("clipmesh.sh"); }

TEST(shapeworksTests, meshinfoTest) { run_test("meshinfo.sh"); }

TEST(shapeworksTests, coverageTest) { run_test("coverage.sh"); }

TEST(shapeworksTests, probeTest) { run_test("probe.sh"); }

TEST(shapeworksTests, compareTest) { run_test("compare.sh"); }

TEST(shapeworksTests, antialiasTest) { run_test("antialias.sh"); }

TEST(shapeworksTests, antialiasMeshTest) { run_test("antialias_mesh.sh"); }

TEST(shapeworksTests, resampleTest) { run_test("resample.sh"); }

TEST(shapeworksTests, resizeTest) { run_test("resize.sh"); }

TEST(shapeworksTests, recenterTest) { run_test("recenter.sh"); }

TEST(shapeworksTests, padTest) { run_test("pad.sh"); }

TEST(shapeworksTests, translateimageTest) { run_test("translateimage.sh"); }

TEST(shapeworksTests, centerofmassTest) { run_test("centerofmass.sh"); }

TEST(shapeworksTests, scaleimageTest) { run_test("scaleimage.sh"); }

TEST(shapeworksTests, rotateTest) { run_test("rotate.sh"); }

TEST(shapeworksTests, extractabelTest) { run_test("extractlabel.sh"); }

TEST(shapeworksTests, closeholesTest) { run_test("closeholes.sh"); }

TEST(shapeworksTests, binarizeTest) { run_test("binarize.sh"); }

TEST(shapeworksTests, computedtTest) { run_test("computedt.sh"); }

TEST(shapeworksTests, curvatureTest) { run_test("curvature.sh"); }

TEST(shapeworksTests, gradientTest) { run_test("gradient.sh"); }

TEST(shapeworksTests, sigmoidTest) { run_test("sigmoid.sh"); }

TEST(shapeworksTests, intensityTest) { run_test("intensity.sh"); }

TEST(shapeworksTests, tplevelsetTest) { run_test("tplevelset.sh"); }

TEST(shapeworksTests, topoTest) { run_test("topo.sh"); }

TEST(shapeworksTests, blurTest) { run_test("blur.sh"); }

TEST(shapeworksTests, icpimageTest) { run_test("icpimage.sh"); }

TEST(shapeworksTests, cropTest) { run_test("crop.sh"); }

TEST(shapeworksTests, clipimageTest) { run_test("clipimage.sh"); }

TEST(shapeworksTests, reflectimageTest) { run_test("reflectimage.sh"); }

TEST(shapeworksTests, warpTest) { run_test("warp.sh"); }

TEST(shapeworksTests, setoriginTest) { run_test("setorigin.sh"); }

TEST(shapeworksTests, setspacingTest) { run_test("setspacing.sh"); }

TEST(shapeworksTests, negateTest) { run_test("negate.sh"); }

TEST(shapeworksTests, additionTest) { run_test("add.sh"); }

TEST(shapeworksTests, subtractionTest) { run_test("sub.sh"); }

TEST(shapeworksTests, multiplicationTest) { run_test("mul.sh"); }

TEST(shapeworksTests, divisionTest) { run_test("div.sh"); }

TEST(shapeworksTests, smoothTest) { run_test("smooth.sh"); }

TEST(shapeworksTests, remeshTest) { run_test("remesh.sh"); }

TEST(shapeworksTests, reflectmeshTest) { run_test("reflectmesh.sh"); }

TEST(shapeworksTests, fillholesTest) { run_test("fillholes.sh"); }

TEST(shapeworksTests, translatemeshTest) { run_test("translatemesh.sh"); }

TEST(shapeworksTests, scalemeshTest) { run_test("scalemesh.sh"); }

TEST(shapeworksTests, fixelementTest) { ASSERT_FALSE(system("bash fixelement.sh")); }

TEST(shapeworksTests, clipclosedsurfaceTest) { run_test("clipclosedsurface.sh"); }

TEST(shapeworksTests, meshtransformTest) { run_test("meshtransform.sh"); }

TEST(shapeworksTests, computenormalsTest) { run_test("computenormals.sh"); }

TEST(shapeworksTests, shapeevaluationTest) { run_test("shapeevaluation.sh"); }

TEST(shapeworksTests, isolateTest) { run_test("isolate.sh"); }

TEST(shapeworksTests, analyzeTest) { run_sandboxed_test("analyze"); }

TEST(shapeworksTests, thicknessTest) { run_test("thickness.sh"); }

TEST(shapeworksTests, meshlargestcomponentTest) { run_test("meshlargestcomponent.sh"); }
