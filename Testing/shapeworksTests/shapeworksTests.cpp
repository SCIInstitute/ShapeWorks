#include "Testing.h"

//---------------------------------------------------------------------------
// only need to run shapeworksEnvSetup once or it continuously appends to paths
TEST(shapeworksTests, setup)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(true);
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, info)
{
  ASSERT_FALSE(system("bash info.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, compare) {
  ASSERT_FALSE(system("bash compare.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, comparefail) {
  ASSERT_TRUE(system("bash comparefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, antialias)
{
  ASSERT_FALSE(system("bash antialias.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, antialiasfail)
{
  ASSERT_TRUE(system("bash antialiasfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, resample)
{
  ASSERT_FALSE(system("bash resample.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, resize)
{
  ASSERT_FALSE(system("bash resize.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, recenter)
{
  ASSERT_FALSE(system("bash recenter.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, pad)
{
  ASSERT_FALSE(system("bash pad.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, translate)
{
  ASSERT_FALSE(system("bash translate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, centerofmass)
{
  ASSERT_FALSE(system("bash centerofmass.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scale)
{
  ASSERT_FALSE(system("bash scale.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scalefail)
{
  ASSERT_TRUE(system("bash scalefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotate)
{
  ASSERT_FALSE(system("bash rotate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotatefail)
{
  ASSERT_TRUE(system("bash rotatefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, extractlabel)
{
  ASSERT_FALSE(system("bash extractlabel.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, closeholes)
{
  ASSERT_FALSE(system("bash closeholes.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, binarize)
{
  ASSERT_FALSE(system("bash binarize.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, computedt)
{
  ASSERT_FALSE(system("bash computedt.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, curvature)
{
  ASSERT_FALSE(system("bash curvature.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, curvaturefail)
{
  ASSERT_TRUE(system("bash curvaturefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, gradient)
{
  ASSERT_FALSE(system("bash gradient.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, sigmoid)
{
  ASSERT_FALSE(system("bash sigmoid.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, tplevelset)
{
  ASSERT_FALSE(system("bash tplevelset.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, tplevelsetfail)
{
  ASSERT_TRUE(system("bash tplevelsetfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, topo)
{
  ASSERT_FALSE(system("bash topo.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, blur)
{
  ASSERT_FALSE(system("bash blur.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, boundingbox)
{
  ASSERT_FALSE(system("bash boundingbox.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, boundingboxfail)
{
  ASSERT_TRUE(system("bash boundingboxfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, icp)
{
  ASSERT_FALSE(system("bash icp.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, icpfail)
{
  ASSERT_TRUE(system("bash icpfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, clip)
{
  ASSERT_FALSE(system("bash clip.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, clipfail)
{
  ASSERT_TRUE(system("bash clipfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, reflect)
{
  ASSERT_FALSE(system("bash reflect.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, warp)
{
  ASSERT_FALSE(system("bash warp.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, warpfail)
{
  ASSERT_TRUE(system("bash warpfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, reflectfail)
{
  ASSERT_TRUE(system("bash reflectfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, setorigin)
{
  ASSERT_FALSE(system("bash setorigin.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, negate)
{
  ASSERT_FALSE(system("bash negate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, addition)
{
  ASSERT_FALSE(system("bash add.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, subtraction)
{
  ASSERT_FALSE(system("bash sub.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, multiplication)
{
  ASSERT_FALSE(system("bash mul.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, division)
{
  ASSERT_FALSE(system("bash div.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, crop)
{
  ASSERT_FALSE(system("bash crop.sh"));
}

