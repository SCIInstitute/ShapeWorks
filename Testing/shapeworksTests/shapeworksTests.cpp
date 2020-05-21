#include <gtest/gtest.h>
#include <cstdlib>

#include "TestConfiguration.h"

//---------------------------------------------------------------------------
void shapeworksEnvSetup() // fixme: use googletest's setup/teardown: https://github.com/google/googletest/blob/master/googletest/docs/advanced.md
{
#ifdef _WIN32
  std::string bin_dir = std::string(BUILD_DIR) + "/Release";
  std::replace(bin_dir.begin(), bin_dir.end(), '/', '\\');
  std::string path = getenv("PATH");
  path = path + ";" + std::string(INSTALL_DIR) + "\\bin";
  std::cerr << "Setting PATH to " << path << "\n";
  _putenv_s("PATH", path.c_str());
#else
  // set PATH to shapeworks executable called by shell scripts
  const char *env_p = std::getenv("PATH");
  std::string path(env_p);
  // fixme: /bin/Release or /bin/Debug for some build systems (Xcode, VS, etc)
  // fixme: need windows PATH separator (;).  there's some handy function for this
  path = std::string(BUILD_DIR) + "/bin/" + ":" + path; // might be /bin/Debug or /bin/Release (ex: if using Xcode)
  setenv("PATH", path.c_str(), true);                   // fixme: setenv may not exist

  // set path to test data for shell scripts to use
  std::string data(TEST_DATA_DIR);
  data += "/shapeworks";
  setenv("DATA", data.c_str(), true);

  // change to the shapeworksTest directory
  std::string shapeworksTestsDir(TEST_DATA_DIR);
  shapeworksTestsDir += "/../shapeworksTests";
  chdir(shapeworksTestsDir.c_str());
#endif
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, compare) {
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./compare.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, comparefail) {
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./comparefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, antialias)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./antialias.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, antialiasfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./antialiasfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, resample)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./resample.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, recenter)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./recenter.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, pad)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./pad.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, translate)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./translate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, translatefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./translatefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, centerofmass)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./centerofmass.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scale)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./scale.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, scalefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./scalefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotate)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./rotate.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, rotatefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./rotatefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, extractlabel)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./extractlabel.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, closeholes)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./closeholes.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, threshold)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./threshold.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, thresholdfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./thresholdfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, computedt)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./computedt.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, curvature)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./curvature.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, curvaturefail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./curvaturefail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, gradient)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./gradient.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, sigmoid)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./sigmoid.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, tplevelset)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./tplevelset.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, tplevelsetfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./tplevelsetfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, topo)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./topo.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, topofail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./topofail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, blur)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./blur.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, icp)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./icp.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, icpfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./icpfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, clip)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./clip.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, clipfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./clipfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, reflect)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./reflect.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, reflectfail)
{
  shapeworksEnvSetup();
  ASSERT_TRUE(system("bash ./reflectfail.sh"));
}

//---------------------------------------------------------------------------
TEST(shapeworksTests, setorigin)
{
  shapeworksEnvSetup();
  ASSERT_FALSE(system("bash ./setorigin.sh"));
}
