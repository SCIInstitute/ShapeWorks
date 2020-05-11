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
  ASSERT_FALSE(system("./antialias.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, antialiasfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("./antialiasfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, recenter)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./recenter.sh"));
}

//---------------------------------------------------------------------------
// TEST(shapeworksTests, isoresample)
// {
//   shapeworksEnvSetup();
//   ASSERT_FALSE(system("./isoresample.sh"));
// }

//---------------------------------------------------------------------------
TEST(shapeworksTests, centerofmass)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./centerofmass.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, translate)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./translate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scale)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./scale.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotate)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./rotate.sh"));
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
TEST(shapeworksTests, blur)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("./blur.sh"));
}
