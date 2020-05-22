#include <gtest/gtest.h>
#include <cstdlib>

#include "TestConfiguration.h"

//---------------------------------------------------------------------------
void shapeworksEnvSetup() // fixme: use googletest's setup/teardown: https://github.com/google/googletest/blob/master/googletest/docs/advanced.md
{
  // set PATH to shapeworks executable called by shell scripts
#ifdef _WIN32
  auto path(std::string(BUILD_DIR) + "\\Release\\bin" + ";" + std::getenv("PATH"));
  _putenv_s("PATH", path.c_str());
#else
  auto path(std::string(BUILD_DIR) + "/bin/" + ":" + std::getenv("PATH")); // might be /bin/Debug or /bin/Release for systems such as Xcode
  setenv("PATH", path.c_str(), true);
#endif

  // set location of shapeworks DATA used by shell scripts
  std::string data(TEST_DATA_DIR);
  data += "/shapeworks";
#ifdef _WIN32
  _putenv_s("DATA", data.c_str(), true);
#else
  setenv("DATA", data.c_str(), true);
#endif

  // change to the shapeworksTest directory
  auto shapeworksTestsDir(std::string(TEST_DATA_DIR) + "/../shapeworksTests");
  chdir(shapeworksTestsDir.c_str());
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, test1)
{
  ASSERT_FALSE(system("bash"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, test2)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, compare) {
  shapeworksEnvSetup();
  ASSERT_FALSE(system("C:\Program Files\Git\bin\bash .\compare.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, comparefail) {
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./comparefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, antialias)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./antialias.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, antialiasfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system(" ./antialiasfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, resample)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./resample.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, recenter)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./recenter.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, pad)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./pad.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, translate)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./translate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, translatefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./translatefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, centerofmass)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./centerofmass.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scale)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./scale.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scalefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./scalefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotate)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./rotate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotatefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./rotatefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, extractlabel)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./extractlabel.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, closeholes)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./closeholes.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, threshold)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./threshold.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, thresholdfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./thresholdfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, computedt)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./computedt.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, curvature)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./curvature.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, curvaturefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./curvaturefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, gradient)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./gradient.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, sigmoid)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./sigmoid.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, tplevelset)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./tplevelset.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, tplevelsetfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./tplevelsetfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, topo)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./topo.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, topofail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./topofail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, blur)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./blur.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, icp)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./icp.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, icpfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./icpfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, clip)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./clip.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, clipfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./clipfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, reflect)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./reflect.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, reflectfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./reflectfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, setorigin)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./setorigin.sh"));
}
