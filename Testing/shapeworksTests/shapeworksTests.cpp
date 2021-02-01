#include "Testing.h"

//---------------------------------------------------------------------------
// only need to run shapeworksEnvSetup once or it continuously appends to paths
// FIXME: This is a problem if you only need to execute a single test (same problem in PythonTests)
TEST(shapeworksTests, setup)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(true);
}

TEST(shapeworksTests, fieldnameTest)
{
  ASSERT_FALSE(system("bash meshfieldnames.sh"));
}

TEST(shapeworksTests, getfieldvalueTest)
{
  ASSERT_FALSE(system("bash getfieldvalue.sh"));
}

TEST(shapeworksTests, fieldrangeTest)
{
  ASSERT_FALSE(system("bash fieldrange.sh"));
}

TEST(shapeworksTests, fieldmeanTest)
{
  ASSERT_FALSE(system("bash fieldmean.sh"));
}

TEST(shapeworksTests, fieldsdvTest)
{
  ASSERT_FALSE(system("bash fieldsdv.sh"));
}

TEST(shapeworksTests, tomeshTest)
{
  ASSERT_FALSE(system("bash tomesh.sh"));
}

TEST(shapeworksTests, clipmeshTest)
{
  ASSERT_FALSE(system("bash clipmesh.sh"));
}

TEST(shapeworksTests, meshinfoTest)
{
  ASSERT_FALSE(system("bash meshinfo.sh"));
}

TEST(shapeworksTests, coverageTest)
{
  ASSERT_FALSE(system("bash coverage.sh"));
}

TEST(shapeworksTests, probeTest)
{
  ASSERT_FALSE(system("bash probe.sh"));
}

TEST(shapeworksTests, compareTest)
{
  ASSERT_FALSE(system("bash compare.sh"));
}

TEST(shapeworksTests, compareFailTest)
{
  ASSERT_TRUE(system("bash comparefail.sh"));
}

TEST(shapeworksTests, antialiasTest)
{
  ASSERT_FALSE(system("bash antialias.sh"));
}

TEST(shapeworksTests, antialiasFailTest)
{
  ASSERT_TRUE(system("bash antialiasfail.sh"));
}

TEST(shapeworksTests, resampleTest)
{
  ASSERT_FALSE(system("bash resample.sh"));
}

TEST(shapeworksTests, resizeTest)
{
  ASSERT_FALSE(system("bash resize.sh"));
}

TEST(shapeworksTests, recenterTest)
{
  ASSERT_FALSE(system("bash recenter.sh"));
}

TEST(shapeworksTests, padTest)
{
  ASSERT_FALSE(system("bash pad.sh"));
}

TEST(shapeworksTests, translateimageTest)
{
  ASSERT_FALSE(system("bash translateimage.sh"));
}

TEST(shapeworksTests, centerofmassTest)
{
  ASSERT_FALSE(system("bash centerofmass.sh"));
}

TEST(shapeworksTests, scaleimageTest)
{
  ASSERT_FALSE(system("bash scaleimage.sh"));
}

TEST(shapeworksTests, scaleimageFailTest)
{
  ASSERT_TRUE(system("bash scaleimagefail.sh"));
}

TEST(shapeworksTests, rotateTest)
{
  ASSERT_FALSE(system("bash rotate.sh"));
}

TEST(shapeworksTests, rotateFailTest)
{
  ASSERT_TRUE(system("bash rotatefail.sh"));
}

TEST(shapeworksTests, extractabelTest)
{
  ASSERT_FALSE(system("bash extractlabel.sh"));
}

TEST(shapeworksTests, closeholesTest)
{
  ASSERT_FALSE(system("bash closeholes.sh"));
}

TEST(shapeworksTests, binarizeTest)
{
  ASSERT_FALSE(system("bash binarize.sh"));
}

TEST(shapeworksTests, computedtTest)
{
  ASSERT_FALSE(system("bash computedt.sh"));
}

TEST(shapeworksTests, curvatureTest)
{
  ASSERT_FALSE(system("bash curvature.sh"));
}

TEST(shapeworksTests, curvatureFailTest)
{
  ASSERT_TRUE(system("bash curvaturefail.sh"));
}

TEST(shapeworksTests, gradientTest)
{
  ASSERT_FALSE(system("bash gradient.sh"));
}

TEST(shapeworksTests, sigmoidTest)
{
  ASSERT_FALSE(system("bash sigmoid.sh"));
}

TEST(shapeworksTests, intensityTest)
{
  ASSERT_FALSE(system("bash intensity.sh"));
}

TEST(shapeworksTests, tplevelsetTest)
{
  ASSERT_FALSE(system("bash tplevelset.sh"));
}

TEST(shapeworksTests, tplevelsetFailTest)
{
  ASSERT_TRUE(system("bash tplevelsetfail.sh"));
}

TEST(shapeworksTests, topoTest)
{
  ASSERT_FALSE(system("bash topo.sh"));
}

TEST(shapeworksTests, blurTest)
{
  ASSERT_FALSE(system("bash blur.sh"));
}

TEST(shapeworksTests, icpimageTest)
{
  ASSERT_FALSE(system("bash icpimage.sh"));
}

TEST(shapeworksTests, icpimageFailTest)
{
  ASSERT_TRUE(system("bash icpimagefail.sh"));
}

TEST(shapeworksTests, boundingboximageTest)
{
  ASSERT_FALSE(system("bash boundingboximage.sh"));
}

TEST(shapeworksTests, boundingboximageFailTest)
{
  ASSERT_TRUE(system("bash boundingboximagefail.sh"));
}

TEST(shapeworksTests, cropTest)
{
  ASSERT_FALSE(system("bash crop.sh"));
}

TEST(shapeworksTests, clipimageTest)
{
  ASSERT_FALSE(system("bash clipimage.sh"));
}

TEST(shapeworksTests, clipimageFailTest)
{
  ASSERT_TRUE(system("bash clipimagefail.sh"));
}

TEST(shapeworksTests, reflectimageTest)
{
  ASSERT_FALSE(system("bash reflectimage.sh"));
}

TEST(shapeworksTests, reflectimageFailTest)
{
  ASSERT_TRUE(system("bash reflectimagefail.sh"));
}

TEST(shapeworksTests, warpTest)
{
  ASSERT_FALSE(system("bash warp.sh"));
}

TEST(shapeworksTests, warpFailTest)
{
  ASSERT_TRUE(system("bash warpfail.sh"));
}

TEST(shapeworksTests, setoriginTest)
{
  ASSERT_FALSE(system("bash setorigin.sh"));
}

TEST(shapeworksTests, setspacingTest)
{
  ASSERT_FALSE(system("bash setspacing.sh"));
}

TEST(shapeworksTests, setspacingFailTest)
{
  ASSERT_TRUE(system("bash setspacingfail.sh"));
}

TEST(shapeworksTests, negateTest)
{
  ASSERT_FALSE(system("bash negate.sh"));
}

TEST(shapeworksTests, additionTest)
{
  ASSERT_FALSE(system("bash add.sh"));
}

TEST(shapeworksTests, subtractionTest)
{
  ASSERT_FALSE(system("bash sub.sh"));
}

TEST(shapeworksTests, multiplicationTest)
{
  ASSERT_FALSE(system("bash mul.sh"));
}

TEST(shapeworksTests, divisionTest)
{
  ASSERT_FALSE(system("bash div.sh"));
}

TEST(shapeworksTests, smoothTest)
{
  ASSERT_FALSE(system("bash smooth.sh"));
}

TEST(shapeworksTests, decimateTest)
{
  ASSERT_FALSE(system("bash decimate.sh"));
}

TEST(shapeworksTests, reflectmeshTest)
{
  ASSERT_FALSE(system("bash reflectmesh.sh"));
}

TEST(shapeworksTests, fillholesTest)
{
  ASSERT_FALSE(system("bash fillholes.sh"));
}

TEST(shapeworksTests, translatemeshTest)
{
  ASSERT_FALSE(system("bash translatemesh.sh"));
}

TEST(shapeworksTests, scalemeshTest)
{
  ASSERT_FALSE(system("bash scalemesh.sh"));
}

// todo: fix is broken (lol)
// TEST(shapeworksTests, meshfixTest)
// {
//   ASSERT_FALSE(system("bash meshfix.sh"));
// }

TEST(shapeworksTests, meshtoimageTest)
{
  ASSERT_FALSE(system("bash meshtoimage.sh"));
}

TEST(shapeworksTests, meshtodtTest)
{
  ASSERT_FALSE(system("bash meshtodt.sh"));
}

TEST(shapeworksTests, meshtransformTest)
{
  ASSERT_FALSE(system("bash meshtransform.sh"));
}

TEST(shapeworksTests, shapeevaluationTest)
{
  ASSERT_FALSE(system("bash shapeevaluation.sh"));
}
