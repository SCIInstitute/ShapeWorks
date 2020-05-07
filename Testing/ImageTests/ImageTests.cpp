#include <gtest/gtest.h>

#include <Libs/Image/Image.h>
#include <Libs/Image/ImageUtils.h>

#include "TestConfiguration.h"

using namespace shapeworks;

TEST(ImageTests, antialias_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/antialias/");

  Image image(test_location + "1x2x2.nrrd");
  image.antialias();
  Image ground_truth(test_location + "antialias_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, recenter_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/recenter/");

  Image image(test_location + "1x2x2.nrrd");
  image.recenter();
  Image ground_truth(test_location + "recenter_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

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

TEST(ImageTests, translate_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/translate/");

  Image image(test_location + "1x2x2.nrrd");
  image.translate(makeVector({10, 10, 10}));
  Image ground_truth(test_location + "translate_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, com_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/com/");

  Image image(test_location + "1x2x2.nrrd");
  TransformPtr xform = ImageUtils::createCenterOfMassTransform(image);
  image.applyTransform(xform);
  image.write("/Users/cam/data/sw/tmp/com_test.nrrd");
  Image ground_truth(test_location + "com_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, scale_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/scale/");

  Image image(test_location + "1x2x2.nrrd");
  image.scale(makeVector({2.0,0.75,3.14159}));
  image.write("/Users/cam/data/sw/tmp/scale_test.nrrd");
  Image ground_truth(test_location + "scale_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotate_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/rotate/");

  Image image(test_location + "1x2x2.nrrd");
  image.rotate(M_PI / 2.0, makeVector({1,1,1}));
  image.write("/Users/cam/data/sw/tmp/rotate_test.nrrd");
  Image ground_truth(test_location + "rotate_baseline.nrrd");

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
  region = ImageUtils::boundingBox(images);
  image.crop(region);
  Image ground_truth(test_location + "crop_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1: good normal and point on plane, sets clipped values to pi
TEST(ImageTests, clip1_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image.clip(makeVector({1,1,1}), Point({20,55,75}), 3.14);
  Image ground_truth(test_location + "clip1_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1x: clips YZ plane at x=center+2.75
TEST(ImageTests, clip1x_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image.clip(makeVector({1,0,0}), image.center() + Point({2.75,0,0}));
  Image ground_truth(test_location + "clip1x_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1y: clips XZ plane at y=center and uses return value of function
TEST(ImageTests, clip1y_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image = image.clip(makeVector({0,1,0}), image.center());
  Image ground_truth(test_location + "clip1y_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1z: clips XY plane using a far away p
TEST(ImageTests, clip1z_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image.clip(makeVector({0,0,-1}), image.center() + Point({10000,100000,0}));
  Image ground_truth(test_location + "clip1z_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

/// clip2: clips using a zero length normal
TEST(ImageTests, clip2_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  try { image.clip(makeVector({0,0,0}), image.center()); }
  catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

/// clip3: clips using three non-colinear points for the plane, setting value to -pi
TEST(ImageTests, clip3_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image.clip(image.origin(), Point({75,0,0}), Point({-10,0,0}), -3.14);
  Image ground_truth(test_location + "clip3_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

/// clip4: clips using three colinear points for the plane
TEST(ImageTests, clip4_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  try { image.clip(Point({0,0,0}), Point({1,0,0}), Point({-10,0,0})); }
  catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

TEST(ImageTests, reflect_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/reflect/");

  // reflect across XZ plane (looks like vertical direction facing "front" of volume, X-axis pointing right, Y-axis pointing up)
  Image image(test_location + "1x2x2.nrrd");
  image.reflect(makeVector({0.0, 1.0, 0.0}));
  image.write("/Users/cam/data/sw/tmp/reflect_test.nrrd"); //<ctc> write so once it's working we'll have a baseline (todo/fixme)
  Image ground_truth(test_location + "reflect_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, icp_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/icp/");

  Image image(test_location + "1x2x2.nrrd");
  Image target(test_location + "target.nrrd");
  Image source(test_location + "source.nrrd");
  TransformPtr xform(ImageUtils::rigidRegistration(target, source));
  image.applyTransform(xform);
  image.write("/Users/cam/data/sw/tmp/icp_test.nrrd");
  Image ground_truth(test_location + "icp_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, multicommand_test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/multicommand/");

  Image image(test_location + "1x2x2.nrrd");
  image.applyCurvatureFilter().applyGradientFilter().applySigmoidFilter();
  Image ground_truth(test_location + "multicommand_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}
