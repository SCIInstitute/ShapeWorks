#include <gtest/gtest.h>
#include <cstdlib>

#include "TestConfiguration.h"

//---------------------------------------------------------------------------
void shapeworksEnvSetup() // fixme: use googletest's setup/teardown: https://github.com/google/googletest/blob/master/googletest/docs/advanced.md
{
  // set PATH to shapeworks executable called by shell scripts
#ifdef _WIN32
  auto path(std::string(BUILD_DIR) + "\\bin\\Release" + ";" + std::getenv("PATH"));
  _putenv_s("PATH", path.c_str());
#else
  auto path(std::string(BUILD_DIR) + "/bin" + ":" + std::getenv("PATH")); // might be /bin/Debug or /bin/Release for systems such as Xcode
  setenv("PATH", path.c_str(), true);
#endif

  // set location of shapeworks DATA used by shell scripts
  std::string data(TEST_DATA_DIR);
  data += "/shapeworks";
#ifdef _WIN32
  _putenv_s("DATA", data.c_str());
#else
  setenv("DATA", data.c_str(), true);
#endif

  // change to the shapeworksTest directory
  auto shapeworksTestsDir(std::string(TEST_DATA_DIR) + "/../shapeworksTests");
  chdir(shapeworksTestsDir.c_str());
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, info)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash info.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, compare) {
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash compare.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, comparefail) {
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash comparefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, antialias)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash antialias.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, antialiasfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash antialiasfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, resample)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash resample.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, recenter)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash recenter.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, pad)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash pad.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, translate)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash translate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, centerofmass)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash centerofmass.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scale)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash scale.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scalefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash scalefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotate)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash rotate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotatefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash rotatefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, extractlabel)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash extractlabel.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, closeholes)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash closeholes.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, threshold)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash threshold.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, thresholdfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash thresholdfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, computedt)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash computedt.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, curvature)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash curvature.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, curvaturefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash curvaturefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, gradient)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash gradient.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, sigmoid)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash sigmoid.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, tplevelset)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash tplevelset.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, tplevelsetfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash tplevelsetfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, topo)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash topo.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, blur)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash blur.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, boundingbox)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash boundingbox.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, boundingboxfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash boundingboxfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, icp)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash icp.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, icpfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash icpfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, clip)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash clip.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, clipfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash clipfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, reflect)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash reflect.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, reflectfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash reflectfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, setorigin)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash setorigin.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, negate)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash negate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, addition)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash add.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, subtraction)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash sub.sh"));
}
