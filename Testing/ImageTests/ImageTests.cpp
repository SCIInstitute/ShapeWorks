#include <gtest/gtest.h>

#include <Libs/Image/Image.h>
#include <Libs/Image/ImageUtils.h>

#include "TestConfiguration.h"

using namespace shapeworks;

TEST(ImageTests, isoresample_binary_isotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "binary-isotropic-input.nrrd");
  image.antialias();
  ImageUtils::isoresample(image);
  image.threshold().recenter();
  Image ground_truth(test_location + "binary-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresample_binary_anisotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "binary-anisotropic-input.nrrd");
  image.antialias();
  ImageUtils::isoresample(image);
  image.threshold().recenter();
  Image ground_truth(test_location + "binary-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresample_smooth_isotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "smooth-isotropic-input.nrrd");
  ImageUtils::isoresample(image);
  image.recenter();
  Image ground_truth(test_location + "smooth-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresample_smooth_anisotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "smooth-anisotropic-input.nrrd");
  ImageUtils::isoresample(image);
  image.recenter();
  Image ground_truth(test_location + "smooth-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresample_image_anisotropic_test) {

  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "image-anisotropic-input.nrrd");
  ImageUtils::isoresample(image);
  image.recenter();
  Image ground_truth(test_location + "image-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, pad_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/pad/");

  Image image(test_location + "1x2x2.nrrd");
  image.pad(30, 0.0);
  Image ground_truth(test_location + "pad_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, pad_identity_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/pad/");

  Image image(test_location + "1x2x2.nrrd");
  image.pad(0, 0.0);
  Image ground_truth(test_location + "1x2x2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, extractlabel_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/extract-label/");

  Image image(test_location + "1x2x2.nrrd");
  image.extractLabel(1.0);
  Image ground_truth(test_location + "extract-label_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, closeholes_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/close-holes/");

  Image image(test_location + "1x2x2.nrrd");
  image.closeHoles();
  Image ground_truth(test_location + "close-holes_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, threshold_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/threshold/");

  Image image(test_location + "1x2x2.nrrd");
  image.threshold();
  Image ground_truth(test_location + "threshold_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, computedt_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compute-dt/");

  Image image(test_location + "1x2x2.nrrd");
  image.computeDT();
  Image ground_truth(test_location + "compute-dt_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, dicom_read_test) {
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/dicom/");

  Image image(test_location + "dcm_files");
  Image ground_truth(test_location + "dicom.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, curvature_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/curvature/");

  Image image(test_location + "1x2x2.nrrd");
  image.applyCurvatureFilter();
  Image ground_truth(test_location + "curvature_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, gradient_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/gradient/");

  Image image(test_location + "1x2x2.nrrd");
  image.applyGradientFilter();
  Image ground_truth(test_location + "gradient_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, sigmoid_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/sigmoid/");

  Image image(test_location + "1x2x2.nrrd");
  image.applySigmoidFilter();
  Image ground_truth(test_location + "sigmoid_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, setlevel_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/set-level/");
  std::string other_location = std::string(TEST_DATA_DIR) + std::string("/curvature/curvature_baseline.nrrd");

  Image image(test_location + "1x2x2.nrrd");
  image.applyTPLevelSetFilter(other_location);
  Image ground_truth(test_location + "set-level_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, topopreservingsmooth_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/topo-preserving-smooth/");

  Image image(test_location + "1x2x2.nrrd");
  ImageUtils::topologyPreservingSmooth(image, 10, 10.5, 10, 1, true);
  Image ground_truth(test_location + "topo-preserving-smooth_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, blur_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/blur/");

  Image image(test_location + "1x2x2.nrrd");
  image.gaussianBlur(2.0);
  Image ground_truth(test_location + "blur_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, crop_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/crop/");
  std::string images_location = std::string(TEST_DATA_DIR) + std::string("/images/");
  std::vector<std::string> images = {
    images_location + "seg.ellipsoid_1.nrrd",
    images_location + "seg.ellipsoid_2.nrrd",
    images_location + "seg.ellipsoid_3.nrrd",
    images_location + "seg.ellipsoid_4.nrrd",
    images_location + "seg.ellipsoid_5.nrrd",
    images_location + "seg.ellipsoid_6.nrrd",
    images_location + "seg.ellipsoid_7.nrrd",
    images_location + "seg.ellipsoid_8.nrrd",
    images_location + "seg.ellipsoid_9.nrrd",
  };

  Image image(test_location + "seg.ellipsoid_1.nrrd");
  Image::Region region;
  region = ImageUtils::binaryBoundingBox(images);
  image.crop(region);
  Image ground_truth(test_location + "crop_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, com_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/com/");

  Image image(test_location + "1x2x2.nrrd");
  Transform xform = ImageUtils::createCenterOfMassTransform(image);
  image.applyTransform(xform);
  Image ground_truth(test_location + "com_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// TEST(ImageTests, translate_test)
// {
//   std::string test_location = std::string(TEST_DATA_DIR) + std::string("/translate/");

//   Image image(test_location + "1x2x2.nrrd");
//   Transform xform;
//   double v[3] = {10, 10, 10};
//   xform->Translate(Vector3(v));
//   image.applyTransform(xform);
//   Image ground_truth(test_location + "translate_baseline.nrrd");

//   ASSERT_TRUE(image == ground_truth);
// }

TEST(ImageTests, multicommand_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/multicommand/");

  Image image(test_location + "1x2x2.nrrd");
  image.applyCurvatureFilter().applyGradientFilter().applySigmoidFilter();
  Image ground_truth(test_location + "multicommand_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, reflect_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/reflect/");

  Image image(test_location + "1x2x2.nrrd");

  // reflect across XZ plane (looks like vertical direction facing "front" of volume, X-axis pointing right, Y-axis pointing up)
  image.reflect(makeVector3({0.0, 1.0, 0.0}));
  image.write("/Users/cam/data/sw/tmp/reflect_test.nrrd");

  Image ground_truth(test_location + "reflect_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}
