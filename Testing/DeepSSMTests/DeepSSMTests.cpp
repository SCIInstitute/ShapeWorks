#include "Testing.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
void run_deepssm_test(const std::string& name) {
  setupenv(std::string(TEST_DATA_DIR) + "/../DeepSSMTests");

  std::string command = "bash " + name;
  ASSERT_FALSE(system(command.c_str()));
}

//---------------------------------------------------------------------------
// Run 2 configurations that cover all code paths:
// - default: standard DeepSSM
// - tl_net_fine_tune: TL-DeepSSM with fine tuning (covers both tl_net and fine_tune paths)
TEST(DeepSSMTests, defaultTest) { run_deepssm_test("deepssm_default.sh"); }

TEST(DeepSSMTests, tlNetFineTuneTest) { run_deepssm_test("deepssm_tl_net_fine_tune.sh"); }
