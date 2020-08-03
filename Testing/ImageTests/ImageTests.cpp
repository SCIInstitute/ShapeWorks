
#include "Testing.h"

#include "Image.h"
#include "ImageUtils.h"

using namespace shapeworks;

TEST(ImageTests, dicomReadTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/dicom/");

  Image image(test_location + "dcm_files");
  Image ground_truth(test_location + "dicom.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, readTestNoImage)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/non-existant-dir/");

  try {
    Image image(test_location + "foo.nrrd");
  } catch(...) {
    return;
  }

  ASSERT_TRUE(false);
}

TEST(ImageTests, fileFormatTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/info/");

  Image image_orig(test_location + "1x2x2.nrrd");
  image_orig.write(test_location + "1x2x2.tiff");
  Image image_tiff(test_location + "1x2x2.tiff");
  image_tiff.write(test_location + "1x2x2_back.nrrd");
  Image image_back(test_location + "1x2x2_back.nrrd");

  ASSERT_TRUE(image_orig.compare(image_tiff, false /* only compare pixels, not regions */) &&
              image_orig.compare(image_back, false /* only compare pixels, not regions */) &&
              image_tiff.compare(image_back));
}

TEST(ImageTests, fileFormatTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/info/");

  Image image_orig(test_location + "sample_001.dcm");
  image_orig.write(test_location + "sample_001.nrrd");
  Image image_nrrd(test_location + "sample_001.nrrd");
  image_nrrd.write(test_location + "sample_001_back.dcm");
  Image image_back(test_location + "sample_001_back.dcm");

  ASSERT_TRUE(image_orig == image_nrrd && image_orig == image_back);
}

TEST(ImageTests, antialiasTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/antialias/");

  Image image(test_location + "1x2x2.nrrd");
  image.antialias();
  Image ground_truth(test_location + "antialias_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleBinaryIsotropicTest) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "binary-isotropic-input.nrrd");
  image.antialias();
  ImageUtils::isoresample(image);
  image.binarize().recenter();
  Image ground_truth(test_location + "binary-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleBinaryAnisotropicTest) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "binary-anisotropic-input.nrrd");
  image.antialias();
  ImageUtils::isoresample(image);
  image.binarize().recenter();
  Image ground_truth(test_location + "binary-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleSmoothIsotropicTest) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "smooth-isotropic-input.nrrd");
  ImageUtils::isoresample(image);
  image.recenter();
  Image ground_truth(test_location + "smooth-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleSmoothAnisotropicTest) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "smooth-anisotropic-input.nrrd");
  ImageUtils::isoresample(image);
  image.recenter();
  Image ground_truth(test_location + "smooth-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleImageAnisotropicTest) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/isoresample/");

  Image image(test_location + "image-anisotropic-input.nrrd");
  ImageUtils::isoresample(image, 10.0);
  image.recenter();
  Image ground_truth(test_location + "image-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, recentertest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/recenter/");

  Image image(test_location + "1x2x2.nrrd");
  image.recenter();
  Image ground_truth(test_location + "recenter_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, recentertest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/recenter/");

  Image image(test_location + "1x2x2.nrrd");
  image.setOrigin(image.center()); // should be identical to recenter
  Image ground_truth(test_location + "recenter_baseline.nrrd");

  ASSERT_FALSE(image == ground_truth);
}

TEST(ImageTests, padTest) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/pad/");

  Image image(test_location + "1x2x2.nrrd");
  image.pad(30, 0.0);
  Image ground_truth(test_location + "pad_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, padTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/pad/");

  Image image(test_location + "1x2x2.nrrd");
  image.pad(100, 25, 1, 10.0);
  Image ground_truth(test_location + "pad2_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, translateTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/translate/");

  Image image(test_location + "1x2x2.nrrd");
  image.translate(makeVector({10, 10, 10}));
  Image ground_truth(test_location + "translate1_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, translateTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/translate/");

  Image image(test_location + "1x2x2.nrrd");
  image.translate(makeVector({-10, -10, -10}));
  Image ground_truth(test_location + "translate2_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, translateTest3)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/translate/");

  Image image(test_location + "1x2x2.nrrd");
  image.translate(makeVector({0, 0, -10}));
  Image ground_truth(test_location + "translate3_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, comTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/com/");

  Image image(test_location + "1x2x2.nrrd");
  TransformPtr xform = ImageUtils::createCenterOfMassTransform(image);
  image.applyTransform(xform);
  Image ground_truth(test_location + "com_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, comTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/info/");

  Image image(test_location + "la-bin.nrrd");
  Point com({79.2113, 117.8811, 46.4797});
  const double eps = 1E-4;

  ASSERT_TRUE(epsEqual(image.centerOfMass(), com, eps));
}

TEST(ImageTests, scaleTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/scale/");

  Image image(test_location + "1x2x2.nrrd");
  image.scale(makeVector({2.0, 0.75, Pi})); // flying saucer filled with Pi
  Image ground_truth(test_location + "scale1_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, scaleTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/scale/");

  Image image(test_location + "la-bin-centered.nrrd");
  image.scale(makeVector({-1.0, 1.5, 1.0})); // inverted in x
  Image ground_truth(test_location + "scale2_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest0)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/rotate/");

  Image image(test_location + "la1-small.nrrd");
  image.rotate(Pi / 2.0, makeVector({0,0,1})); // 90 degrees around the z axis
  Image ground_truth(test_location + "rotate0_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/rotate/");

  Image image(test_location + "la-bin-centered.nrrd");
  image.rotate(Pi / 2.0, makeVector({0,0,1})); // 90 degrees around the z axis
  Image ground_truth(test_location + "rotate1_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/rotate/");

  Image image(test_location + "la-bin-centered.nrrd");
  image.rotate(Pi / 4.0, makeVector({0.1,0.1,0.1})); // 45 degrees around (unnormalized) diagonal axis
  Image ground_truth(test_location + "rotate2_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest3)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/rotate/");

  Image image(test_location + "la-bin-centered.nrrd");
  image.rotate(Pi / 3.0, makeVector({-1,10,-3})); // Pi/3 radians around a weird axis
  Image ground_truth(test_location + "rotate3_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, rotateTest4) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/rotate/");

  double angle = Pi / 2.0;
  shapeworks::Vector3 axis = makeVector({1, 0, 0});
  Image image(test_location + "la-bin-centered.nrrd");
  image.rotate(angle, axis);
  image.rotate(-angle, axis);

  Image image2(test_location + "la-bin-centered.nrrd");
  image2.rotate(-angle, axis);
  image2.rotate(angle, axis);

  ASSERT_FALSE(image == image2);
}

TEST(ImageTests, rotateTest5) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/rotate/");

  Image image(test_location + "la-bin-centered.nrrd");
  image.rotate(0, makeVector({0,0,1}));
  Image original(test_location + "la-bin-centered.nrrd");

  ASSERT_TRUE(image == original);
}

TEST(ImageTests, extractlabelTest) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/extract-label/");

  Image image(test_location + "1x2x2.nrrd");
  image.extractLabel(1.0);
  Image ground_truth(test_location + "extract-label_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, closeholesTest) 
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/close-holes/");

  Image image(test_location + "image_with_holes.nrrd");
  image.closeHoles();
  Image ground_truth(test_location + "closedholes.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, binarizeTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/threshold/");

  Image image(test_location + "1x2x2.nrrd");
  image.binarize();
  Image ground_truth(test_location + "threshold_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, binarizeTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/threshold/");

  Image image(test_location + "raw.nrrd");
  image.binarize(10.0, 100.0, 42);
  Image ground_truth(test_location + "banded.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, computedtTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compute-dt/");

  Image image(test_location + "1x2x2.nrrd");
  image.computeDT();
  Image ground_truth(test_location + "compute-dt_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, curvatureTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/curvature/");

  Image image(test_location + "1x2x2.nrrd");
  image.applyCurvatureFilter();
  Image ground_truth(test_location + "curvature_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, gradientTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/gradient/");

  Image image(test_location + "1x2x2.nrrd");
  image.applyGradientFilter();
  Image ground_truth(test_location + "gradient_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, sigmoidTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/sigmoid/");

  Image image(test_location + "1x2x2.nrrd");
  image.applySigmoidFilter();
  Image ground_truth(test_location + "sigmoid_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, setlevelTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/set-level/");
  std::string other_location = std::string(TEST_DATA_DIR) + std::string("/curvature/curvature_baseline.nrrd");

  Image image(test_location + "1x2x2.nrrd");
  image.applyTPLevelSetFilter(other_location);
  Image ground_truth(test_location + "set-level_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, topopreservingsmoothTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/topo-preserving-smooth/");

  Image image(test_location + "1x2x2.nrrd");
  ImageUtils::topologyPreservingSmooth(image, 10, 10.5, 10);
  Image ground_truth(test_location + "topo-preserving-smooth_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, blurTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/blur/");

  Image image(test_location + "1x2x2.nrrd");
  image.gaussianBlur(2.0);
  Image ground_truth(test_location + "blur_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, cropTest)
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

TEST(ImageTests, icpTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/icp/");

  Image image(test_location + "1x2x2.nrrd");
  Image target(test_location + "target.nrrd");
  Image source(test_location + "source.nrrd");
  TransformPtr transform(ImageUtils::createRigidRegistrationTransform(target, source));
  image.applyTransform(transform, target.dims(), target.origin(), target.spacing(), target.coordsys());
  Image ground_truth(test_location + "icp_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1: good normal and point on plane, sets clipped values to pi
TEST(ImageTests, clip1Test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image.clip(makeVector({1,1,1}), Point({20,55,75}), 3.14);
  Image ground_truth(test_location + "clip1_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1x: clips YZ plane at x=center+2.75
TEST(ImageTests, clip1xTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image.clip(makeVector({1,0,0}), image.center() + Point({2.75,0,0}));
  Image ground_truth(test_location + "clip1x_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1y: clips XZ plane at y=center and uses return value of function
TEST(ImageTests, clip1yTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image = image.clip(makeVector({0,1,0}), image.center());
  Image ground_truth(test_location + "clip1y_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1z: clips XY plane using a far away p
TEST(ImageTests, clip1zTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image.clip(makeVector({0,0,-1}), image.center() + Point({10000,100000,0}));
  Image ground_truth(test_location + "clip1z_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

/// clip2: clips using a zero length normal
TEST(ImageTests, clip2Test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  try { image.clip(makeVector({0,0,0}), image.center()); }
  catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

/// clip3: clips using three non-colinear points for the plane, setting value to -pi
TEST(ImageTests, clip3Test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  image.clip(image.origin(), Point({75,0,0}), Point({-10,0,0}), -3.14);
  Image ground_truth(test_location + "clip3_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

/// clip4: clips using three colinear points for the plane
TEST(ImageTests, clip4Test)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/clip/");

  Image image(test_location + "1x2x2.nrrd");
  try { image.clip(Point({0,0,0}), Point({1,0,0}), Point({-10,0,0})); }
  catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

TEST(ImageTests, reflectTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/reflect/");

  // reflect across XZ plane (looks like vertical direction facing "front" of volume, X-axis pointing right, Y-axis pointing up)
  Image image(test_location + "1x2x2.nrrd");
  image.reflect(Axis::X);
  Image ground_truth(test_location + "reflect_baseline1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, reflectTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/reflect/");

  Image image(test_location + "la-bin.nrrd");
  image.reflect(Axis::X);
  Image ground_truth(test_location + "reflect_baseline2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, setoriginTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/setorigin/");

  Image image(test_location + "1x2x2.nrrd");
  image.setOrigin();
  Image ground_truth(test_location + "setorigin_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/warp/");

  Image image(test_location + "input.nrrd");
  std::string src_filename(test_location + "source.particles");
  std::string dst_filename(test_location + "target.particles");

  TransformPtr transform(ImageUtils::createWarpTransform(src_filename, dst_filename));
  image.applyTransform(transform);
  Image ground_truth(test_location + "warp_baseline1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/warp/");

  Image image(test_location + "input.nrrd");
  std::string src_filename(test_location + "source.particles");
  std::string dst_filename(test_location + "target.particles");

  // only use every 3rd landmark point
  TransformPtr transform(ImageUtils::createWarpTransform(src_filename, dst_filename, 3));
  image.applyTransform(transform);
  Image ground_truth(test_location + "warp_baseline2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest3)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/warp/");

  Image image(test_location + "1x2x2.nrrd");
  std::string src_filename(test_location + "bogus_src.pts");
  std::string dst_filename(test_location + "bogus_dst.pts");

  // empty files should just produce identity transform
  TransformPtr transform(ImageUtils::createWarpTransform(src_filename, dst_filename));
  image.applyTransform(transform);
  Image ground_truth(test_location + "1x2x2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest4)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/warp/");

  Image image(test_location + "1x2x2.nrrd");
  std::string src_filename(test_location + "source.particles");
  std::string dst_filename(test_location + "source.particles");

  // warping from A to A should produce A
  TransformPtr transform(ImageUtils::createWarpTransform(src_filename, dst_filename));
  image.applyTransform(transform);
  Image ground_truth(test_location + "1x2x2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, warpTest5)
{
  // warping from A to B and then back to A should produce A
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/warp/");

  Image image(test_location + "1x2x2.nrrd");
  std::string src_filename1(test_location + "source.particles");
  std::string dst_filename1(test_location + "target.particles");

  TransformPtr transform(ImageUtils::createWarpTransform(src_filename1, dst_filename1));
  image.applyTransform(transform);

  std::string src_filename2(test_location + "source.particles");
  std::string dst_filename2(test_location + "source.particles");

  TransformPtr transform_back(ImageUtils::createWarpTransform(src_filename2, dst_filename2));
  image.applyTransform(transform_back);

  Image ground_truth(test_location + "1x2x2.nrrd");

  ASSERT_TRUE(image.compare(ground_truth, true, 1));
}

TEST(ImageTests, compareTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "la-bin.nrrd");

  ASSERT_TRUE(image1 == image2);
}

TEST(ImageTests, compareTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "1x2x2.nrrd");
  Image image2(test_location + "la-bin.nrrd");

  ASSERT_FALSE(image1 == image2);
}

TEST(ImageTests, compareTest3a)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "same_image_diff_region_same_dims.nrrd");

  ASSERT_FALSE(image1.compare(image2));
}

TEST(ImageTests, compareTest3b)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "same_image_diff_region_same_dims.nrrd");

  ASSERT_TRUE(image1.compare(image2, false /* only compare pixels, not regions */));
}

TEST(ImageTests, compareTest4a)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "diff_image_diff_region_same_dims.nrrd");

  ASSERT_FALSE(image1.compare(image2));
}

TEST(ImageTests, compareTest4b)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "diff_image_diff_region_same_dims.nrrd");

  ASSERT_FALSE(image1.compare(image2, false /* only compare pixels, not regions */));
}

TEST(ImageTests, compareTest5a)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "diff_image_same_region_same_dims.nrrd");

  ASSERT_FALSE(image1.compare(image2));
}

TEST(ImageTests, compareTest5b)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "diff_image_same_region_same_dims.nrrd");

  ASSERT_TRUE(image1.compare(image2, true /* keep comparing regions */, 0.0 /* allow pixels to differ by 0% */, 1.0 /* allow pixels to differ by 1.0 */));
}

TEST(ImageTests, compareTest6a)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "1x2x2.nrrd");
  Image image2(test_location + "compute-dt_baseline.nrrd");

  ASSERT_FALSE(image1.compare(image2, true /* keep comparing regions */, 0.0 /* allow pixels to differ by 0% */, 1.0 /* allow pixels to differ by 1.0 */));
}

TEST(ImageTests, compareTest6b)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/compare/");

  Image image1(test_location + "1x2x2.nrrd");
  Image image2(test_location + "compute-dt_baseline.nrrd");

  ASSERT_TRUE(image1.compare(image2, true /* keep comparing regions */, 1 /* allow pixels to differ by 1% */, 1.0 /* allow pixels to differ by 1.0 */));
}

TEST(ImageTests, multicommandTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/multicommand/");

  Image image(test_location + "1x2x2.nrrd");
  image.applyCurvatureFilter().applyGradientFilter().applySigmoidFilter();
  Image ground_truth(test_location + "multicommand_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, dimsTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/info/");

  Image image(test_location + "la-bin.nrrd");
  //Dims dims({289, 329, 182}); // This was causing compile error on Windows
  Dims dims;
  dims[0] = 289; dims[1] = 329; dims[2] = 182;
  
  ASSERT_TRUE(image.dims() == dims);
}

TEST(ImageTests, sizeTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/info/");

  Image image(test_location + "1x2x2.nrrd");
  Point size({100,100,100});
  
  ASSERT_TRUE(image.size() == size);
}

TEST(ImageTests, spacingTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/info/");

  Image image(test_location + "1x2x2.nrrd");
  Vector spacing(makeVector({1,2,2}));
  
  ASSERT_TRUE(image.spacing() == spacing);
}

TEST(ImageTests, originTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/info/");

  Image image(test_location + "1x2x2.nrrd");
  Point origin({0, 0.5, 0.5});
  
  ASSERT_TRUE(image.origin() == origin);
}

TEST(ImageTests, centerTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/info/");

  Image image(test_location + "1x2x2.nrrd");
  Point ctr({50, 50.5, 50.5});
  
  ASSERT_TRUE(image.center() == ctr);
}

TEST(ImageTests, coordsysTest)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/info/");

  Image image(test_location + "1x2x2.nrrd");
  Image::ImageType::DirectionType coordsys;
  coordsys.SetIdentity();
  
  ASSERT_TRUE(image.coordsys() == coordsys);
}

TEST(ImageTests, negationTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/negation/");

  Image image(test_location + "la-bin.nrrd");
  image = -image;
  Image baseline(test_location + "negation1_baseline.nrrd");
  
  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, negationTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/negation/");

  Image image(test_location + "la-bin.nrrd");
  image = -(-image); // negation of negation
  Image baseline(test_location + "la-bin.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, additionTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/addition/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "1x2x2.nrrd");
  try {
    image1 = image1 + image2; // they have different dims, so operator throws an exception
  } catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

TEST(ImageTests, additionTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/addition/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "la-bin.nrrd");
  Image image3(image1 + image2);
  Image baseline(test_location + "baseline_addition.nrrd");

  ASSERT_TRUE(image3 == baseline);
  ASSERT_TRUE(image1 == image2);
}

TEST(ImageTests, additionTest3)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/addition/");

  Image image1(test_location + "la-bin.nrrd");
  Image image2(test_location + "la-bin.nrrd");
  image1 += image2;
  Image baseline(test_location + "baseline_addition.nrrd");

  ASSERT_TRUE(image1 == baseline);
}

TEST(ImageTests, subtractionTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/subtraction/");

  Image image1(test_location + "img1.nrrd");
  Image image2(test_location + "1x2x2.nrrd");
  try {
    image1 = image1 - image2; // the have different dims, so operator throws an exception
  } catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

TEST(ImageTests, subtractionTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/subtraction/");

  Image image1(test_location + "img1.nrrd");
  Image image2(test_location + "img2.nrrd");

  Image image3(image1 - image2);
  Image baseline(test_location + "baseline_subtraction.nrrd");
  ASSERT_TRUE(image3 == baseline);

  Image orig_image1(test_location + "img1.nrrd");
  ASSERT_TRUE(image1 == orig_image1);
}

TEST(ImageTests, subtractionTest3)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/subtraction/");

  Image image1(test_location + "img1.nrrd");
  Image image2(test_location + "img2.nrrd");
  image1 -= image2;
  Image baseline(test_location + "baseline_subtraction.nrrd");

  ASSERT_TRUE(image1 == baseline);
}

TEST(ImageTests, addTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/add/");

  Image image(test_location + "la-bin.nrrd");
  Image added(image + 3.14);
  Image baseline(test_location + "baseline_add.nrrd");

  ASSERT_TRUE(added == baseline);

  Image orig_image(test_location + "la-bin.nrrd");
  ASSERT_TRUE(image == orig_image);
}

TEST(ImageTests, addTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/add/");

  Image image(test_location + "la-bin.nrrd");
  image += 3.14;
  Image baseline(test_location + "baseline_add.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, subtractTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/subtract/");

  Image image(test_location + "la-bin.nrrd");
  Image subtracted(image - 3.14);
  Image baseline(test_location + "baseline_subtract.nrrd");

  ASSERT_TRUE(subtracted == baseline);

  Image orig_image(test_location + "la-bin.nrrd");
  ASSERT_TRUE(image == orig_image);
}

TEST(ImageTests, subtractTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/subtract/");

  Image image(test_location + "la-bin.nrrd");
  image -= 3.14;
  Image baseline(test_location + "baseline_subtract.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, multiplyTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/multiply/");

  Image image(test_location + "la-bin.nrrd");
  Image multiplied(image * 3.14);
  Image baseline(test_location + "baseline_multiply.nrrd");

  ASSERT_TRUE(multiplied == baseline);

  Image orig_image(test_location + "la-bin.nrrd");
  ASSERT_TRUE(image == orig_image);
}

TEST(ImageTests, multiplyTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/multiply/");

  Image image(test_location + "la-bin.nrrd");
  image *= 3.14;
  Image baseline(test_location + "baseline_multiply.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, divideTest1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/divide/");

  Image image(test_location + "la-bin.nrrd");
  Image divided(image / 3.14);
  Image baseline(test_location + "baseline_divide.nrrd");

  ASSERT_TRUE(divided == baseline);

  Image orig_image(test_location + "la-bin.nrrd");
  ASSERT_TRUE(image == orig_image);
}

TEST(ImageTests, divideTest2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/divide/");

  Image image(test_location + "la-bin.nrrd");
  image /= 3.14;
  Image baseline(test_location + "baseline_divide.nrrd");

  ASSERT_TRUE(image == baseline);
}

TEST(ImageTests, resample1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/resample/");

  Image image(test_location + "la1-small.nrrd");
  image.resample(Point({0.78, 1.0, 10.0}));
  Image ground_truth(test_location + "baseline_resample1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resample2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/resample/");

  Image image(test_location + "la1-small.nrrd");
  image.resample(Point({0.98, 1.02, 3.14159}));
  Image ground_truth(test_location + "baseline_resample2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resize1)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/resize/");

  Image image(test_location + "la1-small.nrrd");
  image.resize(Dims({20, 40, 60}));
  Image ground_truth(test_location + "baseline_resize1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resize2)
{
  std::string test_location = std::string(TEST_DATA_DIR) + std::string("/resize/");

  Image image(test_location + "la1-small.nrrd");
  image.resize(Dims({12, 14, 80}));
  Image ground_truth(test_location + "baseline_resize2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

