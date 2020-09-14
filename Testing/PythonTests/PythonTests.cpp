#include "Testing.h"

//---------------------------------------------------------------------------
TEST(pythonTests, antialias)
{
  pythonEnvSetup();
  ASSERT_TRUE(system("python antialias.py"));
}
