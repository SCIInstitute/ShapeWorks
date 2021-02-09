#include "Testing.h"

// only need to run pythonEnvSetup once or it continuously appends to paths
// FIXME: This is a problem if you only need to execute a single test (same problem in ShapeworksTests)
TEST(pythonTests, setup)
{
  pythonEnvSetup();
  ASSERT_TRUE(true);
}

TEST(pythonTests, infoTest)
{
  ASSERT_FALSE(system("python imageinfo.py"));
}
  
TEST(pythonTests, copyTest)
{
  ASSERT_FALSE(system("python copy.py"));
}

TEST(pythonTests, compareTest)
{
  ASSERT_FALSE(system("python compare.py"));
}

TEST(pythonTests, compareFailTest)
{
  ASSERT_TRUE(system("python comparefail.py"));
}

TEST(pythonTests, antialiasTest)
{
  ASSERT_FALSE(system("python antialias.py"));
}

TEST(pythonTests, antialiasFailTest)
{
  ASSERT_TRUE(system("python antialiasfail.py"));
}

TEST(pythonTests, resampleTest)
{
  ASSERT_FALSE(system("python resample.py"));
}

TEST(pythonTests, resizeTest)
{
  ASSERT_FALSE(system("python resize.py"));
}

TEST(pythonTests, resizeFailTest)
{
  ASSERT_TRUE(system("python resizefail.py"));
}

TEST(pythonTests, recenterTest)
{
  ASSERT_FALSE(system("python recenter.py"));
}

TEST(pythonTests, padTest)
{
  ASSERT_FALSE(system("python pad.py"));
}

TEST(pythonTests, padFailTest)
{
  ASSERT_TRUE(system("python padfail.py"));
}

TEST(pythonTests, translateTest)
{
  ASSERT_FALSE(system("python translate.py"));
}

TEST(pythonTests, translatefailTest)
{
  ASSERT_TRUE(system("python translatefail.py"));
}

TEST(pythonTests, centerofmassTest)
{
  ASSERT_FALSE(system("python centerofmass.py"));
}

TEST(pythonTests, scaleTest)
{
  ASSERT_FALSE(system("python scale.py"));
}

TEST(pythonTests, scaleFailTest)
{
  ASSERT_TRUE(system("python scalefail.py"));
}

TEST(pythonTests, rotateTest)
{
  ASSERT_FALSE(system("python rotate.py"));
}

TEST(pythonTests, rotateFailTest)
{
  ASSERT_TRUE(system("python rotatefail.py"));
}

TEST(pythonTests, extractLabelTest)
{
  ASSERT_FALSE(system("python extractlabel.py"));
}

TEST(pythonTests, closeHolesTest)
{
  ASSERT_FALSE(system("python closeholes.py"));
}

TEST(pythonTests, binarizeTest)
{
  ASSERT_FALSE(system("python binarize.py"));
}

TEST(pythonTests, computeDTTest)
{
  ASSERT_FALSE(system("python computedt.py"));
}

TEST(pythonTests, curvatureTest)
{
  ASSERT_FALSE(system("python curvature.py"));
}

TEST(pythonTests, curvatureFailTest)
{
  ASSERT_TRUE(system("python curvaturefail.py"));
}

TEST(pythonTests, gradientTest)
{
  ASSERT_FALSE(system("python gradient.py"));
}

TEST(pythonTests, sigmoidTest)
{
  ASSERT_FALSE(system("python sigmoid.py"));
}

TEST(pythonTests, tplevelsetTest)
{
  ASSERT_FALSE(system("python tplevelset.py"));
}

TEST(pythonTests, tplevelsetFailTest)
{
  ASSERT_TRUE(system("python tplevelsetfail.py"));
}

TEST(pythonTests, intensityTest)
{
  ASSERT_FALSE(system("python intensity.py"));
}

TEST(pythonTests, topoTest)
{
  ASSERT_FALSE(system("python topo.py"));
}

TEST(pythonTests, topoFailTest)
{
  ASSERT_TRUE(system("python topofail.py"));
}

TEST(pythonTests, blurTest)
{
  ASSERT_FALSE(system("python blur.py"));
}

TEST(pythonTests, boundingBox)
{
  ASSERT_FALSE(system("python boundingbox.py"));
}

TEST(pythonTests, boundingBoxFailTest)
{
  ASSERT_TRUE(system("python boundingboxfail.py"));
}

TEST(pythonTests, cropTest)
{
  ASSERT_FALSE(system("python crop.py"));
}

TEST(pythonTests, icpTest)
{
  ASSERT_FALSE(system("python icp.py"));
}

TEST(pythonTests, clip)
{
  ASSERT_FALSE(system("python clip.py"));
}

TEST(pythonTests, clipfail)
{
  ASSERT_TRUE(system("python clipfail.py"));
}

TEST(pythonTests, reflect)
{
  ASSERT_FALSE(system("python reflect.py"));
}

TEST(pythonTests, reflectfail)
{
  ASSERT_TRUE(system("python reflectfail.py"));
}

TEST(pythonTests, warp)
{
  ASSERT_FALSE(system("python warp.py"));
}

TEST(pythonTests, warpfail)
{
  ASSERT_TRUE(system("python warpfail.py"));
}

TEST(pythonTests, setorigin)
{
  ASSERT_FALSE(system("python setorigin.py"));
}

TEST(pythonTests, setspacing)
{
  ASSERT_FALSE(system("python setspacing.py"));
}

TEST(pythonTests, setspacingfail)
{
  ASSERT_TRUE(system("python setspacingfail.py"));
}

TEST(pythonTests, negate)
{
  ASSERT_FALSE(system("python negate.py"));
}

TEST(pythonTests, add)
{
  ASSERT_FALSE(system("python add.py"));
}

TEST(pythonTests, addfail)
{
  ASSERT_TRUE(system("python addfail.py"));
}

TEST(pythonTests, sub)
{
  ASSERT_FALSE(system("python sub.py"));
}

TEST(pythonTests, subfail)
{
  ASSERT_TRUE(system("python subfail.py"));
}

TEST(pythonTests, mul)
{
  ASSERT_FALSE(system("python mul.py"));
}

TEST(pythonTests, div)
{
  ASSERT_FALSE(system("python div.py"));
}

TEST(pythonTests, smoothTest)
{
  ASSERT_FALSE(system("python smooth.py"));
}

TEST(pythonTests, decimateTest)
{
  ASSERT_FALSE(system("python decimate.py"));
}

TEST(pythonTests, invertNormalsTest)
{
  ASSERT_FALSE(system("python invertNormals.py"));
}

TEST(pythonTests, fillHolesTest)
{
  ASSERT_FALSE(system("python fillHoles.py"));
}

TEST(pythonTests, probeVolumeTest)
{
  ASSERT_FALSE(system("python probeVolume.py"));
}

TEST(pythonTests, toImageTest)
{
  ASSERT_FALSE(system("python toImage.py"));
}

TEST(pythonTests, toDistanceTransformTest)
{
  ASSERT_FALSE(system("python toDistanceTransform.py"));
}

TEST(pythonTests, coverageTest)
{
  ASSERT_FALSE(system("python coverage.py"));
}

TEST(pythonTests, coord)
{
  ASSERT_FALSE(system("python coord.py"));
}

TEST(pythonTests, dims)
{
  ASSERT_FALSE(system("python dims.py"));
}

TEST(pythonTests, point)
{
  ASSERT_FALSE(system("python point.py"));
}

TEST(pythonTests, vector)
{
  ASSERT_FALSE(system("python vector.py"));
}

TEST(pythonTests, ipoint3)
{
  ASSERT_FALSE(system("python ipoint3.py"));
}

TEST(pythonTests, fpoint3)
{
  ASSERT_FALSE(system("python fpoint3.py"));
}

TEST(pythonTests, optimize)
{
  ASSERT_FALSE(system("python optimize.py"));
}
