
#include "Testing.h"

#include "Image.h"
#include "ImageUtils.h"

using namespace shapeworks;

TEST(ImageTests, dicomReadTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/dcm_files");
  Image ground_truth(std::string(TEST_DATA_DIR) + "/dicom.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, readTestNoImage)
{
  try {
    Image image(std::string(TEST_DATA_DIR) + "foo.nrrd");
  } catch(...) {
    return;
  }

  ASSERT_TRUE(false);
}

TEST(ImageTests, fileFormatTest1)
{
  Image image_orig(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image_orig.write(std::string(TEST_DATA_DIR) + "/1x2x2.tiff");
  Image image_tiff(std::string(TEST_DATA_DIR) + "/1x2x2.tiff");
  image_tiff.write(std::string(TEST_DATA_DIR) + "/1x2x2_back.nrrd");
  Image image_back(std::string(TEST_DATA_DIR) + "/1x2x2_back.nrrd");

  ASSERT_TRUE(image_orig.compare(image_tiff, false /* only compare pixels, not regions */) &&
              image_orig.compare(image_back, false /* only compare pixels, not regions */) &&
              image_tiff.compare(image_back));
}

TEST(ImageTests, fileFormatTest2)
{
  Image image_orig(std::string(TEST_DATA_DIR) + "/sample_001.dcm");
  image_orig.write(std::string(TEST_DATA_DIR) + "/sample_001.nrrd");
  Image image_nrrd(std::string(TEST_DATA_DIR) + "/sample_001.nrrd");
  image_nrrd.write(std::string(TEST_DATA_DIR) + "/sample_001_back.dcm");
  Image image_back(std::string(TEST_DATA_DIR) + "/sample_001_back.dcm");

  ASSERT_TRUE(image_orig == image_nrrd && image_orig == image_back);
}

TEST(ImageTests, antialiasTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.antialias();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/antialias1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleBinaryIsotropicTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/binary-isotropic-input.nrrd");
  image.antialias();
  ImageUtils::isoresample(image);
  image.binarize().recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/binary-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleBinaryAnisotropicTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/binary-anisotropic-input.nrrd");
  image.antialias();
  ImageUtils::isoresample(image);
  image.binarize().recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/binary-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleSmoothIsotropicTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/smooth-isotropic-input.nrrd");
  ImageUtils::isoresample(image);
  image.recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/smooth-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleSmoothAnisotropicTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/smooth-anisotropic-input.nrrd");
  ImageUtils::isoresample(image);
  image.recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/smooth-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleImageAnisotropicTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/image-anisotropic-input.nrrd");
  ImageUtils::isoresample(image, 10.0);
  image.recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/image-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, recentertest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/recenter.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, recentertest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.setOrigin(image.center()); // should be identical to recenter
  Image ground_truth(std::string(TEST_DATA_DIR) + "/recenter.nrrd");

  ASSERT_FALSE(image == ground_truth);
}

TEST(ImageTests, padTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.pad(0, 0.0);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/pad1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, padTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.pad(100, 25, 1, 10.0);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/pad4.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, translateTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.translate(makeVector({10, 10, 10}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/translate1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, translateTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.translate(makeVector({-10, -10, -10}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/translate2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, translateTest3)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.translate(makeVector({0, 0, -10}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/translate3.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, comTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  TransformPtr xform = ImageUtils::createCenterOfMassTransform(image);
  image.applyTransform(xform);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/centerofmass1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, comTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Point com({79.2113, 117.8811, 46.4797});
  const double eps = 1E-4;

  ASSERT_TRUE(epsEqual(image.centerOfMass(), com, eps));
}

TEST(ImageTests, comTest3)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  TransformPtr xform = ImageUtils::createCenterOfMassTransform(image);
  image.applyTransform(xform, Image::NearestNeighbor);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/centerofmass3.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, scaleTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.scale(makeVector({2.0, 0.75, Pi})); // flying saucer filled with Pi
  Image ground_truth(std::string(TEST_DATA_DIR) + "/scale1_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, scaleTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin-centered.nrrd");
  image.scale(makeVector({-1.0, 1.5, 1.0})); // inverted in x
  Image ground_truth(std::string(TEST_DATA_DIR) + "/scale2_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest0)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.rotate(Pi / 2.0, makeVector({0,0,1})); // 90 degrees around the z axis
  Image ground_truth(std::string(TEST_DATA_DIR) + "/rotate0_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin-centered.nrrd");
  image.rotate(Pi / 2.0, makeVector({0,0,1})); // 90 degrees around the z axis
  Image ground_truth(std::string(TEST_DATA_DIR) + "/rotate1_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin-centered.nrrd");
  image.rotate(Pi / 4.0, makeVector({0.1,0.1,0.1})); // 45 degrees around (unnormalized) diagonal axis
  Image ground_truth(std::string(TEST_DATA_DIR) + "/rotate2_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest3)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin-centered.nrrd");
  image.rotate(Pi / 3.0, makeVector({-1,10,-3})); // Pi/3 radians around a weird axis
  Image ground_truth(std::string(TEST_DATA_DIR) + "/rotate3_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest4) 
{
  double angle = Pi / 2.0;
  shapeworks::Vector3 axis = makeVector({1, 0, 0});
  Image image(std::string(TEST_DATA_DIR) + "/la-bin-centered.nrrd");
  image.rotate(angle, axis);
  image.rotate(-angle, axis);

  Image image2(std::string(TEST_DATA_DIR) + "/la-bin-centered.nrrd");
  image2.rotate(-angle, axis);
  image2.rotate(angle, axis);

  ASSERT_FALSE(image == image2);
}

TEST(ImageTests, rotateTest5) 
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin-centered.nrrd");
  image.rotate(0, makeVector({0,0,1}));
  Image original(std::string(TEST_DATA_DIR) + "/la-bin-centered.nrrd");

  ASSERT_TRUE(image == original);
}

TEST(ImageTests, extractlabelTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.extractLabel(1.0);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/extractlabel1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, closeholesTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/image_with_holes.nrrd");
  image.closeHoles();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/closedholes.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, binarizeTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.binarize();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/binarize1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, binarizeTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/raw.nrrd");
  image.binarize(10.0, 100.0, 42);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/banded.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, computedtTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.computeDT();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/computedt1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, curvatureTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.applyCurvatureFilter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/curvature1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, gradientTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.applyGradientFilter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/gradient.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, sigmoidTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.applySigmoidFilter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/sigmoid1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, setlevelTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.applyTPLevelSetFilter(std::string(TEST_DATA_DIR) + "/curvature1.nrrd");
  Image ground_truth(std::string(TEST_DATA_DIR) + "/set-level_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, topopreservingsmoothTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  ImageUtils::topologyPreservingSmooth(image, 10, 10.5, 10);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/topo-preserving-smooth_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, blurTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.gaussianBlur(1.0);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/blur2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, cropTest)
{
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

  Image image(std::string(TEST_DATA_DIR) + "/seg.ellipsoid_1.nrrd");
  Image::Region region;
  region = ImageUtils::boundingBox(images);
  image.crop(region);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/crop_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, icpTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Image target(std::string(TEST_DATA_DIR) + "/target.nrrd");
  Image source(std::string(TEST_DATA_DIR) + "/source.nrrd");
  TransformPtr transform(ImageUtils::createRigidRegistrationTransform(target, source));
  image.applyTransform(transform, target.origin(), target.dims(), target.spacing(), target.coordsys());
  Image ground_truth(std::string(TEST_DATA_DIR) + "/icp_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1: good normal and point on plane, sets clipped values to pi
TEST(ImageTests, clip1Test)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.clip(makeVector({1,1,1}), Point({20,55,75}), 3.14);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip1_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1x: clips YZ plane at x=center+2.75
TEST(ImageTests, clip1xTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.clip(makeVector({1,0,0}), image.center() + Point({2.75,0,0}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip1x_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1y: clips XZ plane at y=center and uses return value of function
TEST(ImageTests, clip1yTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image = image.clip(makeVector({0,1,0}), image.center());
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip1y_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1z: clips XY plane using a far away p
TEST(ImageTests, clip1zTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.clip(makeVector({0,0,-1}), image.center() + Point({10000,100000,0}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip1z_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

/// clip2: clips using a zero length normal
TEST(ImageTests, clip2Test)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  try { image.clip(makeVector({0,0,0}), image.center()); }
  catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

/// clip3: clips using three non-colinear points for the plane, setting value to -pi
TEST(ImageTests, clip3Test)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.clip(image.origin(), Point({75,0,0}), Point({-10,0,0}), -3.14);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip3_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

/// clip4: clips using three colinear points for the plane
TEST(ImageTests, clip4Test)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  try { image.clip(Point({0,0,0}), Point({1,0,0}), Point({-10,0,0})); }
  catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

TEST(ImageTests, reflectTest1)
{
  // reflect across XZ plane (looks like vertical direction facing "front" of volume, X-axis pointing right, Y-axis pointing up)
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.reflect(Axis::X);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/reflect_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, reflectTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  image.reflect(Axis::X);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/reflect2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, setoriginTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.setOrigin();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/setorigin1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/input.nrrd");
  std::string src_filename(std::string(TEST_DATA_DIR) + "/source.particles");
  std::string dst_filename(std::string(TEST_DATA_DIR) + "/target.particles");

  TransformPtr transform(ImageUtils::createWarpTransform(src_filename, dst_filename));
  image.applyTransform(transform);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/warp1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/input.nrrd");
  std::string src_filename(std::string(TEST_DATA_DIR) + "/source.particles");
  std::string dst_filename(std::string(TEST_DATA_DIR) + "/target.particles");

  // only use every 3rd landmark point
  TransformPtr transform(ImageUtils::createWarpTransform(src_filename, dst_filename, 3));
  image.applyTransform(transform);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/warp2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest3)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  std::string src_filename(std::string(TEST_DATA_DIR) + "/bogus_src.pts");
  std::string dst_filename(std::string(TEST_DATA_DIR) + "/bogus_dst.pts");

  // empty files should just produce identity transform
  TransformPtr transform(ImageUtils::createWarpTransform(src_filename, dst_filename));
  image.applyTransform(transform);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest4)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  std::string src_filename(std::string(TEST_DATA_DIR) + "/source.particles");
  std::string dst_filename(std::string(TEST_DATA_DIR) + "/source.particles");

  // warping from A to A should produce A
  TransformPtr transform(ImageUtils::createWarpTransform(src_filename, dst_filename));
  image.applyTransform(transform);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest5)
{
  // warping from A to B and then back to A should produce A
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  std::string src_filename1(std::string(TEST_DATA_DIR) + "/source.particles");
  std::string dst_filename1(std::string(TEST_DATA_DIR) + "/target.particles");

  TransformPtr transform(ImageUtils::createWarpTransform(src_filename1, dst_filename1));
  image.applyTransform(transform);

  std::string src_filename2(std::string(TEST_DATA_DIR) + "/source.particles");
  std::string dst_filename2(std::string(TEST_DATA_DIR) + "/source.particles");

  TransformPtr transform_back(ImageUtils::createWarpTransform(src_filename2, dst_filename2));
  image.applyTransform(transform_back);

  Image ground_truth(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");

  ASSERT_TRUE(image.compare(ground_truth, true, 1));
}

TEST(ImageTests, compareTest1)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");

  ASSERT_TRUE(image1 == image2);
}

TEST(ImageTests, compareTest2)
{
  Image image1(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");

  ASSERT_FALSE(image1 == image2);
}

TEST(ImageTests, compareTest3a)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/same_image_diff_region_same_dims.nrrd");

  ASSERT_FALSE(image1.compare(image2));
}

TEST(ImageTests, compareTest3b)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/same_image_diff_region_same_dims.nrrd");

  ASSERT_TRUE(image1.compare(image2, false /* only compare pixels, not regions */));
}

TEST(ImageTests, compareTest4a)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/diff_image_diff_region_same_dims.nrrd");

  ASSERT_FALSE(image1.compare(image2));
}

TEST(ImageTests, compareTest4b)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/diff_image_diff_region_same_dims.nrrd");

  ASSERT_FALSE(image1.compare(image2, false /* only compare pixels, not regions */));
}

TEST(ImageTests, compareTest5a)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/diff_image_same_region_same_dims.nrrd");

  ASSERT_FALSE(image1.compare(image2));
}

TEST(ImageTests, compareTest5b)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/diff_image_same_region_same_dims.nrrd");

  ASSERT_TRUE(image1.compare(image2, true /* keep comparing regions */, 0.0 /* allow pixels to differ by 0% */, 1.0 /* allow pixels to differ by 1.0 */));
}

TEST(ImageTests, compareTest6a)
{
  Image image1(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/computedt1.nrrd");

  ASSERT_FALSE(image1.compare(image2, true /* keep comparing regions */, 0.0 /* allow pixels to differ by 0% */, 1.0 /* allow pixels to differ by 1.0 */));
}

TEST(ImageTests, compareTest6b)
{
  Image image1(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/computedt1.nrrd");

  ASSERT_TRUE(image1.compare(image2, true /* keep comparing regions */, 1 /* allow pixels to differ by 1% */, 1.0 /* allow pixels to differ by 1.0 */));
}

TEST(ImageTests, multicommandTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.applyCurvatureFilter().applyGradientFilter().applySigmoidFilter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/multicommand_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, dimsTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  //Dims dims({289, 329, 182}); // This was causing compile error on Windows
  Dims dims;
  dims[0] = 289; dims[1] = 329; dims[2] = 182;
  
  ASSERT_TRUE(image.dims() == dims);
}

TEST(ImageTests, sizeTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Point size({100,100,100});
  
  ASSERT_TRUE(image.size() == size);
}

TEST(ImageTests, spacingTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Vector spacing(makeVector({1,2,2}));
  
  ASSERT_TRUE(image.spacing() == spacing);
}

TEST(ImageTests, originTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Point origin({0, 0.5, 0.5});
  
  ASSERT_TRUE(image.origin() == origin);
}

TEST(ImageTests, centerTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Point ctr({50, 50.5, 50.5});
  
  ASSERT_TRUE(image.center() == ctr);
}

TEST(ImageTests, coordsysTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Image::ImageType::DirectionType coordsys;
  coordsys.SetIdentity();
  
  ASSERT_TRUE(image.coordsys() == coordsys);
}

TEST(ImageTests, negationTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  image = -image;
  Image baseline(std::string(TEST_DATA_DIR) + "/la-bin-negative.nrrd");
  
  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, negationTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  image = -(-image); // negation of negation
  Image baseline(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, additionTest1)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  try {
    image1 = image1 + image2; // they have different dims, so operator throws an exception
  } catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

TEST(ImageTests, additionTest2)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image3(image1 + image2);
  Image baseline(std::string(TEST_DATA_DIR) + "/la-bin-doubled.nrrd");

  ASSERT_TRUE(image3 == baseline);
  ASSERT_TRUE(image1 == image2);
}

TEST(ImageTests, additionTest3)
{
  Image image1(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  image1 += image2;
  Image baseline(std::string(TEST_DATA_DIR) + "/la-bin-doubled.nrrd");

  ASSERT_TRUE(image1 == baseline);
}

TEST(ImageTests, subtractionTest1)
{
  Image image1(std::string(TEST_DATA_DIR) + "/img1.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  try {
    image1 = image1 - image2; // the have different dims, so operator throws an exception
  } catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

TEST(ImageTests, subtractionTest2)
{
  Image image1(std::string(TEST_DATA_DIR) + "/img1.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/img2.nrrd");

  Image image3(image1 - image2);
  Image baseline(std::string(TEST_DATA_DIR) + "/img_diff.nrrd");
  ASSERT_TRUE(image3 == baseline);

  Image orig_image1(std::string(TEST_DATA_DIR) + "/img1.nrrd");
  ASSERT_TRUE(image1 == orig_image1);
}

TEST(ImageTests, subtractionTest3)
{
  Image image1(std::string(TEST_DATA_DIR) + "/img1.nrrd");
  Image image2(std::string(TEST_DATA_DIR) + "/img2.nrrd");
  image1 -= image2;
  Image baseline(std::string(TEST_DATA_DIR) + "/img_diff.nrrd");

  ASSERT_TRUE(image1 == baseline);
}

TEST(ImageTests, addTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image added(image + 3.14);
  Image baseline(std::string(TEST_DATA_DIR) + "/la-bin-plus-pi.nrrd");

  ASSERT_TRUE(added == baseline);

  Image orig_image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  ASSERT_TRUE(image == orig_image);
}

TEST(ImageTests, addTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  image += 3.14;
  Image baseline(std::string(TEST_DATA_DIR) + "/la-bin-plus-pi.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, subtractTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image subtracted(image - 3.14);
  Image baseline(std::string(TEST_DATA_DIR) + "/la-bin-minus-pi.nrrd");

  ASSERT_TRUE(subtracted == baseline);

  Image orig_image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  ASSERT_TRUE(image == orig_image);
}

TEST(ImageTests, subtractTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  image -= 3.14;
  Image baseline(std::string(TEST_DATA_DIR) + "/la-bin-minus-pi.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, multiplyTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image multiplied(image * 3.14);
  Image baseline(std::string(TEST_DATA_DIR) + "/baseline_multiply.nrrd");

  ASSERT_TRUE(multiplied == baseline);

  Image orig_image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  ASSERT_TRUE(image == orig_image);
}

TEST(ImageTests, multiplyTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  image *= 3.14;
  Image baseline(std::string(TEST_DATA_DIR) + "/baseline_multiply.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, divideTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Image divided(image / 3.14);
  Image baseline(std::string(TEST_DATA_DIR) + "/baseline_divide.nrrd");

  ASSERT_TRUE(divided == baseline);

  Image orig_image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  ASSERT_TRUE(image == orig_image);
}

TEST(ImageTests, divideTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  image /= 3.14;
  Image baseline(std::string(TEST_DATA_DIR) + "/baseline_divide.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, resample1)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.resample(makeVector({1, 1, 1}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resample1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resample2)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.resample(makeVector({1.5, 1.5, 1.5}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resample3.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resample3)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.resample(makeVector({0.98, 1.02, 3.14159}), Image::NearestNeighbor);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/baseline_resample3.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resample4)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");

  // sample center of image at same physical resolution
  image.resample(nullptr,
                 image.origin() + toPoint(image.dims() * 0.25) * toPoint(image.spacing()), // 1/4 into the image
                 image.dims() * 0.5, image.spacing(), image.coordsys(), Image::Linear);

  Image ground_truth(std::string(TEST_DATA_DIR) + "/baseline_resample4.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

/* fails to compile on linux due to weird template conflicts with itkeigen
TEST(ImageTests, resample5)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");

  // sample center of image at same physical resolution using a translation instead of changing origin
  AffineTransformPtr xform(AffineTransform::New());
  xform->Translate(toVector(toPoint(image.dims() * 0.25) * toPoint(image.spacing())));
  image.resample(xform, image.origin(),
                 image.dims() * 0.5, image.spacing(), image.coordsys(), Image::Linear);

  Image ground_truth(std::string(TEST_DATA_DIR) + "/baseline_resample5.nrrd");

  ASSERT_TRUE(image == ground_truth);
}
*/

TEST(ImageTests, resample6)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");

  // sample center of image at 2x physical resolution
  image.resample(nullptr,
                 image.origin() + toPoint(image.dims() * 0.25) * toPoint(image.spacing()), // 1/4 into the image
                 image.dims(), image.spacing() * 0.5, image.coordsys(), Image::Linear);

  Image ground_truth(std::string(TEST_DATA_DIR) + "/baseline_resample6.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resize1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.resize(Dims({2, 0, 0}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resize2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resize2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.resize(Dims({96, 96, 12}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resize3.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resize3)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.resize(Dims({12, 14, 80}), Image::NearestNeighbor);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resize5.nrrd");

  ASSERT_TRUE(image == ground_truth);
}
