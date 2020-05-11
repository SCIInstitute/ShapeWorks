#include <gtest/gtest.h>
#include <cstdlib>

#include "TestConfiguration.h"

//---------------------------------------------------------------------------
void shapeworksEnvSetup() // fixme: use googletest's setup/teardown: https://github.com/google/googletest/blob/master/googletest/docs/advanced.md
{
  // set PATH to shapeworks executable called by shell scripts
  const char* env_p = std::getenv("PATH");
  std::string path(env_p);
  // fixme: /bin/Release or /bin/Debug for some build systems (Xcode, VS, etc)
  // fixme: need windows PATH separator (;).  there's some handy function for this
  path = std::string(BUILD_DIR) + "/bin/" + ":" + path;     // might be /bin/Debug or /bin/Release (ex: if using Xcode)
  setenv("PATH", path.c_str(), true);           // fixme: setenv may not exist

  // set path to test data for shell scripts to use
  std::string data(TEST_DATA_DIR);
  data += "/shapeworks";
  setenv("DATA", data.c_str(), true);

  // change to the shapeworksTest directory
  std::string shapeworksTestsDir(TEST_DATA_DIR);
  shapeworksTestsDir += "/../shapeworksTests";
  chdir(shapeworksTestsDir.c_str());
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, compare) {
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./compare.sh"));
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
  ASSERT_TRUE(system("./antialias.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, recenter)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./recenter.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, isoresample)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./isoresample.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, centerofmass)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./centerofmass.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, translate)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./translate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scale)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./scale.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotate)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./rotate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, extractlabel)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./extractlabel.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, closeholes)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./closeholes.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, threshold)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./threshold.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, computedt)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./computedt.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, curvature)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./curvature.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, gradient)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./gradient.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, sigmoid)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./sigmoid.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, tplevelset)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./tplevelset.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, blur)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./blur.sh"));
}
