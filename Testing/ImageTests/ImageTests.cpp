#include <gtest/gtest.h>

#include <Libs/Image/Image.h>

#include "TestConfiguration.h"

using namespace shapeworks;

TEST(ImageTests, isoresample_binary_isotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "binary-isotropic-input.nrrd");
  image.antialias();
  image.isoresample();
  image.binarize();
  image.recenter();
  Image ground_truth(test_location + "binary-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, isoresample_binary_anisotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "binary-anisotropic-input.nrrd");
  image.antialias();
  image.isoresample();
  image.binarize();
  image.recenter();
  Image ground_truth(test_location + "binary-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, isoresample_smooth_isotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "smooth-isotropic-input.nrrd");
  image.isoresample();
  image.recenter();
  Image ground_truth(test_location + "smooth-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, isoresample_smooth_anisotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "smooth-anisotropic-input.nrrd");
  image.isoresample();
  image.recenter();
  Image ground_truth(test_location + "smooth-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, isoresample_image_anisotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "image-anisotropic-input.nrrd");
  image.isoresample();
  image.recenter();
  Image ground_truth(test_location + "image-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, pad_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/pad/");

  Image image(test_location + "1x2x2.nrrd");
  image.pad(30, 0.0);
 
  Image ground_truth(test_location + "pad_baseline.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, pad_identity_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/pad/");

  Image image(test_location + "1x2x2.nrrd");
  image.pad(0, 0.0);
  Image ground_truth(test_location + "1x2x2.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

TEST(ImageTests, dicom_read_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/dicom/");

  // read dicom
  Image image;
  image.read(test_location + "dcm_files/");

  // read ground truth
  Image ground_truth(test_location + "dicom.nrrd");

  ASSERT_TRUE(image.compare_equal(ground_truth));
}

//TEST(ImageTests, blah_test) {

// ...

// }
