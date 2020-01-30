#include <gtest/gtest.h>

#include <Libs/Image/Image.h>

#include "TestConfiguration.h"

using namespace shapeworks;

//---------------------------------------------------------------------------
TEST(ImageTests, resample_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/resample/");

  Image image(test_location + "1x2x2.nrrd");
  image.resample(1.0f, true /*isbinary*/);
  image.binarize();
  Image ground_truth(test_location + "resample_baseline.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, padimage_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/padimage/");

  Image image(test_location + "1x2x2.nrrd");
  image.padimage(30, 0.0);
 
  Image ground_truth(test_location + "padimage_baseline.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, padimage_identity_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/padimage/");

  Image image(test_location + "1x2x2.nrrd");
  image.padimage(0, 0.0);
  Image ground_truth(test_location + "1x2x2.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, binarize_test) {

std::string test_location = std::string(TEST_DATA_DIR) + std::string("/binarize/");

Image image(test_location + "1x2x2.nrrd");
image.binarize();
Image ground_truth(test_location + "binarize_baseline.nrrd");

ASSERT_TRUE(image.compare_equal(ground_truth));

image.binarize();

ASSERT_TRUE(image.compare_equal(ground_truth));

}

//TEST(ImageTests, blah_test) {

// ...

// }
