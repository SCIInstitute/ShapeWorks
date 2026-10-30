#include "Testing.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
void run_deepssm_test(const std::string& name) {
#if defined(__APPLE__) && defined(__x86_64__)
  // DeepSSM is unsupported on Intel Macs (see PythonWorker::deepssm_unsupported_reason)
  GTEST_SKIP() << "DeepSSM is not supported on Intel Macs";
#endif
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
