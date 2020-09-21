#include "Testing.h"

//---------------------------------------------------------------------------
TEST(pythonTests, info)
{
  pythonEnvSetup();
  ASSERT_FALSE(system("python info.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, compare)
{
  pythonEnvSetup();
  ASSERT_FALSE(system("python compare.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, compareFail)
{
  pythonEnvSetup();
  ASSERT_TRUE(system("python compareFail.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, antialias)
{
  pythonEnvSetup();
  ASSERT_FALSE(system("python antialias.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, antialiasFail)
{
  pythonEnvSetup();
  ASSERT_TRUE(system("python antialiasFail.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, resample)
{
  pythonEnvSetup();
  ASSERT_FALSE(system("python resample.py"));
}
