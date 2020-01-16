#include <gtest/gtest.h>

#include <Libs/Image/Image.h>

#include "TestConfiguration.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
TEST(PythonTests, tiny_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/resample/");

  Image image(test_location + "1x2x2.nrrd");
  image.resample(1.0f, true /*isbinary*/);
  image.binarize();
  Image ground_truth(test_location + "resample_baseline.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

//TEST(ImageTests, blah_test) {

// ...

// }
