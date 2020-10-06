#include "Testing.h"

//---------------------------------------------------------------------------
// only need to run pythonEnvSetup once or it continuously appends to paths
TEST(pythonTests, setup)
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

//---------------------------------------------------------------------------
TEST(pythonTests, resize)
{
  ASSERT_FALSE(system("python resize.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, recenter)
{
  ASSERT_FALSE(system("python recenter.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, pad)
{
  ASSERT_FALSE(system("python pad.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, translate)
{
  ASSERT_FALSE(system("python translate.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, scale)
{
  ASSERT_FALSE(system("python scale.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, scalefail)
{
  ASSERT_TRUE(system("python scalefail.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, rotate)
{
  ASSERT_FALSE(system("python rotate.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, rotatefail)
{
  ASSERT_TRUE(system("python rotatefail.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, extractlabel)
{
  ASSERT_FALSE(system("python extractlabel.py"));
}

//---------------------------------------------------------------------------
TEST(pythonTests, closeholes)
{
  ASSERT_FALSE(system("python closeholes.py"));
}