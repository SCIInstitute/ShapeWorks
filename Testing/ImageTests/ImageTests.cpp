#include "Testing.h"

#include "Image.h"
#include "VectorImage.h"
#include "ImageUtils.h"
#include "Mesh.h"

using namespace shapeworks;

TEST(ImageTests, dicomReadTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/dcm_files");
  Image ground_truth(std::string(TEST_DATA_DIR) + "/dicom.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, readTest)
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
  image.antialias().resample().binarize().recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/binary-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleBinaryAnisotropicTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/binary-anisotropic-input.nrrd");
  image.antialias().resample().binarize().recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/binary-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleSmoothIsotropicTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/smooth-isotropic-input.nrrd");
  image.resample().recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/smooth-isotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleSmoothAnisotropicTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/smooth-anisotropic-input.nrrd");
  image.resample().recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/smooth-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleImageAnisotropicTest) 
{
  Image image(std::string(TEST_DATA_DIR) + "/image-anisotropic-input.nrrd");
  image.resample(10.0).recenter();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/image-anisotropic-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleDistanceTransformIsotropicTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/binary-isotropic-dt.nrrd");
  image.resample();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/binary-isotropic-dt-isoresampled.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, isoresampleDistanceTransformAnisotropicTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/binary-anisotropic-dt.nrrd");
  image.resample();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/binary-anisotropic-dt-isoresampled.nrrd");

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
  TransformPtr xform = image.createCenterOfMassTransform();
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
  TransformPtr xform = image.createCenterOfMassTransform();
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

TEST(ImageTests, computedtTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.computeDT();
  Image ground_truth(std::string(TEST_DATA_DIR) + "/computedt1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, computedtTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.computeDT(1.0);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/computedt2.nrrd");

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

TEST(ImageTests, intensityTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/nonBinary.nrrd");
  image.applyIntensityFilter(0, 100);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/intensity.nrrd");

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
  image.topologyPreservingSmooth(10, 10.5, 10);
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

TEST(ImageTests, cropTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/seg.ellipsoid_1.nrrd");
  IndexRegion region = image.logicalBoundingBox().pad(-16);
  region.min[0] = 7;
  region.max[0] = 42;
  image.crop(image.logicalToPhysical(region));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/crop_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, cropTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/seg.ellipsoid_1.nrrd");
  auto region = image.physicalBoundingBox();
  image.crop(region);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/seg.ellipsoid_1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, boundingBoxSingleTest1)
{
  auto img = Image(std::string(TEST_DATA_DIR) + "/femurImage.nrrd");
  auto pbox = img.physicalBoundingBox();
  auto ground_truth = PhysicalRegion(Point({46.4821, -192.471, -737.593}),
                                     Point({134.482, -129.471, -609.593}));
  ASSERT_TRUE(epsEqualN(pbox.min, ground_truth.min) &&
              epsEqualN(pbox.max, ground_truth.max));
}

TEST(ImageTests, boundingBoxSingleTest2)
{
  auto img = Image(std::string(TEST_DATA_DIR) + "/femurImage.nrrd");
  auto lbox = img.logicalBoundingBox();
  auto ground_truth = IndexRegion(Coord({0, 0, 0}),
                                    Coord({88, 63, 128}));
  ASSERT_TRUE(lbox == ground_truth);
}

TEST(ImageTests, boundingBoxTest1)
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

  PhysicalRegion physicalregion = ImageUtils::boundingBox(images);
  PhysicalRegion ground_truth_p(Point({7, 16, 16}), Point({43, 34, 34}));

  ASSERT_TRUE(physicalregion == ground_truth_p);
}

TEST(ImageTests, boundingBoxTest2)
{
  std::string images_location = std::string(TEST_DATA_DIR) + std::string("/images/");
  Image img1(images_location + "seg.ellipsoid_1.nrrd");
  Image img2(images_location + "seg.ellipsoid_2.nrrd");
  Image img3(images_location + "seg.ellipsoid_3.nrrd");
  Image img4(images_location + "seg.ellipsoid_4.nrrd");
  Image img5(images_location + "seg.ellipsoid_5.nrrd");
  Image img6(images_location + "seg.ellipsoid_6.nrrd");
  Image img7(images_location + "seg.ellipsoid_7.nrrd");
  Image img8(images_location + "seg.ellipsoid_8.nrrd");
  Image img9(images_location + "seg.ellipsoid_9.nrrd");

  std::vector<std::reference_wrapper<const Image>> images
  {img1, img2, img3, img4, img5, img6, img7, img8, img9};

  PhysicalRegion physicalregion = ImageUtils::boundingBox(images);
  PhysicalRegion ground_truth_p(Point({7, 16, 16}), Point({43, 34, 34}));

  ASSERT_TRUE(physicalregion == ground_truth_p);
}

TEST(ImageTests, icpTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Image target(std::string(TEST_DATA_DIR) + "/target.nrrd");
  Image source(std::string(TEST_DATA_DIR) + "/source.nrrd");
  TransformPtr transform(target.createRigidRegistrationTransform(source));
  image.applyTransform(transform, target.origin(), target.dims(), target.spacing(), target.coordsys());
  Image ground_truth(std::string(TEST_DATA_DIR) + "/icp_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1: good normal and point on plane, sets clipped values to pi
TEST(ImageTests, clip1Test)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.clip(makePlane(Point({20,55,75}), makeVector({1,1,1})), 3.14);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip1_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1x: clips YZ plane at x=center+2.75
TEST(ImageTests, clip1xTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.clip(makePlane(image.center() + Point({2.75,0,0}), makeVector({1,0,0})));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip1x_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1y: clips XZ plane at y=center and uses return value of function
TEST(ImageTests, clip1yTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image = image.clip(makePlane(image.center(), makeVector({0,1,0})));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip1y_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

// clip1z: clips XY plane using a far away p
TEST(ImageTests, clip1zTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.clip(makePlane(image.center() + Point({10000,100000,0}), makeVector({0,0,-1})));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip1z_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

/// clip2: clips using a zero length normal
TEST(ImageTests, clip2Test)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  try { image.clip(makePlane(image.center(), makeVector({0,0,0}))); }
  catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
}

/// clip3: clips using three non-colinear points for the plane, setting value to -pi
TEST(ImageTests, clip3Test)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.clip(makePlane(image.origin(), Point({75,0,0}), Point({-10,0,0})), -3.14);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/clip3_baseline.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

/// clip4: clips using three colinear points for the plane
TEST(ImageTests, clip4Test)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  try { image.clip(makePlane(Point({0,0,0}), Point({1,0,0}), Point({-10,0,0}))); }
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
  std::string src_filename(std::string(TEST_DATA_DIR) + "/bogus_src.pts");
  std::string dst_filename(std::string(TEST_DATA_DIR) + "/bogus_dst.pts");

  try {
    // nonexistent files should throw exception
    ImageUtils::createWarpTransform(src_filename, dst_filename);
  } catch(std::invalid_argument) { return; }

  // fails if an exception is not thrown
  ASSERT_TRUE(false);
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

TEST(ImageTests, setSpacingTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Vector spacing(makeVector({1,2,2}));
  image.setSpacing(spacing);

  ASSERT_TRUE(image.spacing() == spacing);
}

TEST(ImageTests, chainedSpacingTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Vector spacing1(makeVector({1,2,2}));
  Vector spacing2(makeVector({0.2,0.2,1}));
  image.setSpacing(spacing1).setSpacing(spacing2);

  ASSERT_TRUE(image.spacing() == spacing2);
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

TEST(ImageTests, setCoordsysTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  Image::ImageType::DirectionType coordsys;
  coordsys.SetIdentity();
  coordsys[2][2] = -2;
  image.setCoordsys(coordsys);

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

TEST(ImageTests, resampleTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.resample(makeVector({1, 1, 1}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resample1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resampleTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  image.resample(makeVector({1.5, 1.5, 1.5}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resample3.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resampleTest3)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.resample(makeVector({0.98, 1.02, 3.14159}), Image::NearestNeighbor);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/baseline_resample3.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resampleTest4)
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
// related to https://github.com/SCIInstitute/ShapeWorks/issues/208
TEST(ImageTests, resampleTest5)
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

TEST(ImageTests, resampleTest6)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");

  // sample center of image at 2x physical resolution
  image.resample(nullptr,
                 image.origin() + toPoint(image.dims() * 0.25) * toPoint(image.spacing()), // 1/4 into the image
                 image.dims(), image.spacing() * 0.5, image.coordsys(), Image::Linear);

  Image ground_truth(std::string(TEST_DATA_DIR) + "/baseline_resample6.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resizeTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.resize(Dims({2, 0, 0}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resize2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resizeTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.resize(Dims({96, 96, 12}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resize3.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, resizeTest3)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.resize(Dims({12, 14, 80}), Image::NearestNeighbor);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/resize5.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, setSpacingTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.setSpacing(makeVector({2.0,2.0,2.0}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/spacing2.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, setSpacingTest2)
{
  Image image(std::string(TEST_DATA_DIR) + "/la1-small.nrrd");
  image.setSpacing(makeVector({1.0, 1.0, 1.0}));
  Image ground_truth(std::string(TEST_DATA_DIR) + "/spacing1.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, toMeshTest1)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  Mesh mesh = image.toMesh(1.0);
  Mesh ground_truth(std::string(TEST_DATA_DIR) + "/la-bin.vtk");

  ASSERT_TRUE(mesh == ground_truth);
}

TEST(ImageTests, getItk)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  auto itkImage(image.getITKImage());
  Image image2(itkImage);

  ASSERT_TRUE(image == image2);
}

TEST(ImageTests, getVtk)
{
  Image image(std::string(TEST_DATA_DIR) + "/la-bin.nrrd");
  auto vtkImage(image.getVTKImage());
  Image image2(vtkImage);

  ASSERT_TRUE(image == image2);
}

TEST(ImageTests, vectorImageTest1)
{
  Image dt(std::string(TEST_DATA_DIR) + "/computedt2.nrrd");
  auto interpolator = VectorImage(dt);
  Point pt1({37.0, 46.0, 50.0});  // outside close
  Point pt2({60.0, 58.2, 62.5});  // outside other side
  Point pt3({58.0, 44.0, 52.0});  // inside
  Point pt4({12.0, 18.0, 20.0});  // outside far
  Point pt5({-12.0, -18.0, 20.0});// outside original image's space!
  Vector v1 = interpolator.evaluate(pt1);
  Vector v2 = interpolator.evaluate(pt2);
  Vector v3 = interpolator.evaluate(pt3);
  Vector v4 = interpolator.evaluate(pt4); v4.Normalize();
  Vector v5 = interpolator.evaluate(pt5);

  ASSERT_TRUE(epsEqualN(v1, makeVector({0.966473, 0.309422, 0.0527124})) &&
              epsEqualN(v2, makeVector({-0.549147, -0.504978, -0.732781})) &&
              epsEqualN(v3, makeVector({-0.826156, 0.605647, -0.1373})) &&
              epsEqualN(v4, makeVector({0.644471, 0.556214, 0.524674})) &&
              epsEqualN(v5, makeVector({0.354269, -0.294689, 0.470524})));
}

TEST(ImageTests, statsTest)
{
  Image image(std::string(TEST_DATA_DIR) + "/1x2x2.nrrd");
  double min = image.min();
  double max = image.max();
  double mean = image.mean();
  double std = image.std();

  ASSERT_TRUE(equalNSigDigits(min, 0.0) &&
              equalNSigDigits(max, 1.0) &&
              equalNSigDigits(mean, 0.004166) &&
              equalNSigDigits(std, 0.062299));
}

TEST(ImageTests, orientationTest1)
{
  // ensure no change for correctly oriented images
  Image image(std::string(TEST_DATA_DIR) + "/orientation_rai.nrrd");
  Image ground_truth(std::string(TEST_DATA_DIR) + "/orientation_rai.nrrd");

  ASSERT_TRUE(image == ground_truth);
}

TEST(ImageTests, orientationTest2)
{
  // ensure orientation is updated when reading images with undesired orientation
  Image lpi(std::string(TEST_DATA_DIR) + "/orientation_lpi.nrrd");
  Image rpi(std::string(TEST_DATA_DIR) + "/orientation_rpi.nrrd");
  Image sal(std::string(TEST_DATA_DIR) + "/orientation_sal.nrrd");
  Image ground_truth(std::string(TEST_DATA_DIR) + "/orientation_rai.nrrd");

  ASSERT_TRUE(lpi == ground_truth &&
              rpi == ground_truth &&
              sal == ground_truth);
}

TEST(ImageTests, orientationTest3)
{
  // ensure com alignment works after reading images with undesired orientation
  Image image(std::string(TEST_DATA_DIR) + "/orientation_rpi.nrrd");
  auto com = image.centerOfMass();
  auto ctr = image.center();
  image.translate(ctr - com);
  Image ground_truth(std::string(TEST_DATA_DIR) + "/orientation_translated.nrrd");

  ASSERT_TRUE(image == ground_truth);
}
