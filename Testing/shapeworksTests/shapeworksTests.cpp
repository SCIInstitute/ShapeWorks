#include "Testing.h"

//---------------------------------------------------------------------------
// only need to run shapeworksEnvSetup once or it continuously appends to paths
// FIXME: This is a problem if you only need to execute a single test (same problem in PythonTests)
TEST(shapeworksTests, setup)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(true);
}

TEST(shapeworksTests, info)
{
  ASSERT_FALSE(system("bash info.sh"));
}

TEST(shapeworksTests, compare)
{
  ASSERT_FALSE(system("bash compare.sh"));
}

TEST(shapeworksTests, comparefail)
{
  ASSERT_TRUE(system("bash comparefail.sh"));
}

TEST(shapeworksTests, antialias)
{
  ASSERT_FALSE(system("bash antialias.sh"));
}

TEST(shapeworksTests, antialiasfail)
{
  ASSERT_TRUE(system("bash antialiasfail.sh"));
}

TEST(shapeworksTests, resample)
{
  ASSERT_FALSE(system("bash resample.sh"));
}

TEST(shapeworksTests, resize)
{
  ASSERT_FALSE(system("bash resize.sh"));
}

TEST(shapeworksTests, recenter)
{
  ASSERT_FALSE(system("bash recenter.sh"));
}

TEST(shapeworksTests, pad)
{
  ASSERT_FALSE(system("bash pad.sh"));
}

TEST(shapeworksTests, translateimage)
{
  ASSERT_FALSE(system("bash translateimage.sh"));
}

TEST(shapeworksTests, centerofmass)
{
  ASSERT_FALSE(system("bash centerofmass.sh"));
}

TEST(shapeworksTests, scaleimage)
{
  ASSERT_FALSE(system("bash scaleimage.sh"));
}

TEST(shapeworksTests, scaleimagefail)
{
  ASSERT_TRUE(system("bash scaleimagefail.sh"));
}

TEST(shapeworksTests, rotate)
{
  ASSERT_FALSE(system("bash rotate.sh"));
}

TEST(shapeworksTests, rotatefail)
{
  ASSERT_TRUE(system("bash rotatefail.sh"));
}

TEST(shapeworksTests, extractlabel)
{
  ASSERT_FALSE(system("bash extractlabel.sh"));
}

TEST(shapeworksTests, closeholes)
{
  ASSERT_FALSE(system("bash closeholes.sh"));
}

TEST(shapeworksTests, binarize)
{
  ASSERT_FALSE(system("bash binarize.sh"));
}

TEST(shapeworksTests, computedt)
{
  ASSERT_FALSE(system("bash computedt.sh"));
}

TEST(shapeworksTests, curvature)
{
  ASSERT_FALSE(system("bash curvature.sh"));
}

TEST(shapeworksTests, curvaturefail)
{
  ASSERT_TRUE(system("bash curvaturefail.sh"));
}

TEST(shapeworksTests, gradient)
{
  ASSERT_FALSE(system("bash gradient.sh"));
}

TEST(shapeworksTests, sigmoid)
{
  ASSERT_FALSE(system("bash sigmoid.sh"));
}

TEST(shapeworksTests, intensity)
{
  ASSERT_FALSE(system("bash intensity.sh"));
}

TEST(shapeworksTests, tplevelset)
{
  ASSERT_FALSE(system("bash tplevelset.sh"));
}

TEST(shapeworksTests, tplevelsetfail)
{
  ASSERT_TRUE(system("bash tplevelsetfail.sh"));
}

TEST(shapeworksTests, topo)
{
  ASSERT_FALSE(system("bash topo.sh"));
}

TEST(shapeworksTests, blur)
{
  ASSERT_FALSE(system("bash blur.sh"));
}

TEST(shapeworksTests, icpimage)
{
  ASSERT_FALSE(system("bash icpimage.sh"));
}

TEST(shapeworksTests, icpimagefail)
{
  ASSERT_TRUE(system("bash icpimagefail.sh"));
}

TEST(shapeworksTests, boundingboximage)
{
  ASSERT_FALSE(system("bash boundingboximage.sh"));
}

TEST(shapeworksTests, boundingboximagefail)
{
  ASSERT_TRUE(system("bash boundingboximagefail.sh"));
}

TEST(shapeworksTests, crop)
{
  ASSERT_FALSE(system("bash crop.sh"));
}

TEST(shapeworksTests, clipimage)
{
  ASSERT_FALSE(system("bash clipimage.sh"));
}

TEST(shapeworksTests, clipimagefail)
{
  ASSERT_TRUE(system("bash clipimagefail.sh"));
}

TEST(shapeworksTests, reflectimage)
{
  ASSERT_FALSE(system("bash reflectimage.sh"));
}

TEST(shapeworksTests, reflectimagefail)
{
  ASSERT_TRUE(system("bash reflectimagefail.sh"));
}

TEST(shapeworksTests, warp)
{
  ASSERT_FALSE(system("bash warp.sh"));
}

TEST(shapeworksTests, warpfail)
{
  ASSERT_TRUE(system("bash warpfail.sh"));
}

TEST(shapeworksTests, setorigin)
{
  ASSERT_FALSE(system("bash setorigin.sh"));
}

TEST(shapeworksTests, negate)
{
  ASSERT_FALSE(system("bash negate.sh"));
}

TEST(shapeworksTests, addition)
{
  ASSERT_FALSE(system("bash add.sh"));
}

TEST(shapeworksTests, subtraction)
{
  ASSERT_FALSE(system("bash sub.sh"));
}

TEST(shapeworksTests, multiplication)
{
  ASSERT_FALSE(system("bash mul.sh"));
}

TEST(shapeworksTests, division)
{
  ASSERT_FALSE(system("bash div.sh"));
}

TEST(shapeworksTests, smooth)
{
  ASSERT_FALSE(system("bash smooth.sh"));
}

TEST(shapeworksTests, decimate)
{
  ASSERT_FALSE(system("bash decimate.sh"));
}

TEST(shapeworksTests, reflectmesh)
{
  ASSERT_FALSE(system("bash reflectmesh.sh"));
}

TEST(shapeworksTests, fillholes)
{
  ASSERT_FALSE(system("bash fillholes.sh"));
}

TEST(shapeworksTests, clipmesh)
{
  ASSERT_FALSE(system("bash fillholes.sh"));
}

TEST(shapeworksTests, translatemesh)
{
  ASSERT_FALSE(system("bash translatemesh.sh"));
}

TEST(shapeworksTests, scalemesh)
{
  ASSERT_FALSE(system("bash scalemesh.sh"));
}
