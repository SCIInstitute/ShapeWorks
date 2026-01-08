#include "Testing.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
void run_deepssm_test(const std::string& name) {
  setupenv(std::string(TEST_DATA_DIR) + "/../DeepSSMTests");

  std::string command = "bash " + name;
  ASSERT_FALSE(system(command.c_str()));
}

//---------------------------------------------------------------------------
TEST(DeepSSMTests, defaultTest) { run_deepssm_test("deepssm_default.sh"); }

TEST(DeepSSMTests, tlNetTest) { run_deepssm_test("deepssm_tl_net.sh"); }

TEST(DeepSSMTests, fineTuneTest) { run_deepssm_test("deepssm_fine_tune.sh"); }

TEST(DeepSSMTests, tlNetFineTuneTest) { run_deepssm_test("deepssm_tl_net_fine_tune.sh"); }
