#include "Testing.h"

//---------------------------------------------------------------------------
// only need to run pythonEnvSetup once or it continuously appends to paths
TEST(setupPythonTests, info)
{
  pythonEnvSetup();
  ASSERT_TRUE(true);
}
  
//---------------------------------------------------------------------------
TEST(pythonTests, info)
{
  ASSERT_FALSE(system("python info.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, compare)
{
  ASSERT_FALSE(system("python compare.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, compareFail)
{
  ASSERT_TRUE(system("python compareFail.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, antialias)
{
  ASSERT_FALSE(system("python antialias.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, antialiasFail)
{
  ASSERT_TRUE(system("python antialiasFail.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, resample)
{
  ASSERT_FALSE(system("python resample.py"));
}
