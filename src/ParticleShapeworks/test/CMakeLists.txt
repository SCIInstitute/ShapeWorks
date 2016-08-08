#
# This file sets up testing for classes in the ParticleShapeModeling
# module.  To build these tests, enable testing in the CMake build.
# You can use ctest to run all of the tests.  The following command,
# for example, will run all of the ParticleShapeModeling tests,
# assuming they all retain the itkPSM prefix.  (Note the path to ctest
# may vary with the OS.)  
#
# /path/to/CMake/bin/ctest -R itkPSM
# 
# On Windows you will need to specify the build configuration with "-C Release"
# or "-C Debug", etc.
#
# Scrutinize the ouput by checking this file:
# 
# /path/to/ITK-build/Testing/Temporary/test_logger.txt 
#
# To add a new test, simply follow the examples below.  For the
# ParticleShapeModeling module tests, please use the Data directory to
# add any necessary new input test files.  Use the Data/Baseline
# directory to add any ground truth test results files.  Please only
# add new test data and ground truth results data if absolutely
# required. 
#

itk_module_test()
set(ITKParticleShapeModelingTests
itkPSMCleanAndCenterLabelMapImageFilterTest.cxx
itkPSMCommandLineClass2DTest.cxx
itkPSMContainerTest.cxx
itkPSMContainerArrayAttributeTest.cxx
itkPSMCostFunctionTest.cxx
itkPSMEntropyMixedEffectsModelFilterTest.cxx
itkPSMEntropyModelFilter2DMultiscaleTest.cxx
itkPSMEntropyModelFilter2DTest.cxx
itkPSMEntropyModelFilterTest.cxx
itkPSMEntropyModelFilterMultiscaleTest.cxx
itkPSMEntropyRegressionModelFilterTest.cxx
itkPSMEntropyRegressionModel2DFilterMultiscaleTest.cxx
itkPSMEntropyRegressionModel2DFilterTest.cxx
itkPSMGradientDescentOptimizerTest.cxx
itkPSMImplicitSurfaceImage2DFilterTest.cxx
itkPSMImplicitSurfaceImageFilterTest.cxx
itkPSMMeanCurvatureAttributeTest.cxx
itkPSMParticleSystemTest.cxx
itkPSMPointTreeTest.cxx
itkPSMProcrustesFunction2DTest.cxx
itkPSMProcrustesFunctionTest.cxx
itkPSMProcrustesRegistration2DTest.cxx
itkPSMProcrustesRegistrationTest.cxx
itkPSMProjectTest.cxx
itkPSMRBFCorrespondenceInterpolatorTest.cxx
itkPSMRegionNeighborhoodTest.cxx
itkPSMShapeEntropyFunctionTest.cxx
itkPSMSurfaceNeighborhoodTest.cxx
itkPSMTrimLabelMapImageFilterTest.cxx
itkPSMTwoCostFunctionTest.cxx
#itkPSMEntropyModelFilterTest2.cxx
)

# Create source code, compile and link a test driver
# Two variables must be defined before including this file.
#   KIT should define a unique name for the test driver.
#   KitTests should contain a list of test file names without a .cxx suffix.
# Arguments - Input
#   KIT - the name of the test directory
#   KIT_LIBS - a list of libraries needed to link the test driver
#   KitTests - a list of tests to be included in the test driver
#
#  (From ITK/Modules/Core/TestKernel/CreateTestDriver.cmake)
#

CreateTestDriver(ITKParticleShapeModeling "${ITKParticleShapeModeling-Test_LIBRARIES}" "${ITKParticleShapeModelingTests}")

set(TEST_DATA_ROOT "${ITKParticleShapeModeling_SOURCE_DIR}/Data")

itk_add_test(NAME itkPSMCommandLineClass2DTest 
COMMAND ITKParticleShapeModelingTestDriver
itkPSMCommandLineClass2DTest
 ${ITK_TEST_OUTPUT_DIR}/
)

itk_add_test(NAME itkPSMContainerTest COMMAND
ITKParticleShapeModelingTestDriver  itkPSMContainerTest )

itk_add_test(NAME itkPSMContainerArrayAttributeTest COMMAND
ITKParticleShapeModelingTestDriver  itkPSMContainerArrayAttributeTest)

itk_add_test(NAME itkPSMCostFunctionTest COMMAND
ITKParticleShapeModelingTestDriver  itkPSMCostFunctionTest )

itk_add_test(NAME itkPSMEntropyMixedEffectsModelFilterTest 
COMMAND ITKParticleShapeModelingTestDriver
itkPSMEntropyMixedEffectsModelFilterTest
 ${TEST_DATA_ROOT}/Torus/PSMEntropyMixedEffectsModelFilterTest.xml
 ${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Torus/
)

itk_add_test(NAME itkPSMEntropyModelFilter2DMultiscaleTest 
 COMMAND ITKParticleShapeModelingTestDriver
 itkPSMEntropyModelFilter2DMultiscaleTest
 ${TEST_DATA_ROOT}/Torus/PSMEntropyModelFilterMultiscaleTest.xml
 ${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Torus/
)

itk_add_test(NAME itkPSMEntropyModelFilter2DTest 
 COMMAND ITKParticleShapeModelingTestDriver
 itkPSMEntropyModelFilter2DTest
 ${TEST_DATA_ROOT}/Cardiac/PSMEntropyModelFilter2DTest.xml
 ${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Cardiac/
)

itk_add_test(NAME itkPSMEntropyModelFilterMultiscaleTest 
 COMMAND ITKParticleShapeModelingTestDriver
 itkPSMEntropyModelFilterMultiscaleTest
 ${TEST_DATA_ROOT}/Torus/PSMEntropyModelFilterMultiscaleTest.xml
 ${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Torus/
)

itk_add_test(NAME itkPSMEntropyModelFilterTest 
 COMMAND ITKParticleShapeModelingTestDriver
 itkPSMEntropyModelFilterTest
 ${TEST_DATA_ROOT}/Torus/PSMEntropyModelFilterTest.xml
 ${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Torus/
)


itk_add_test(NAME itkPSMEntropyRegressionModel2DFilterMultiscaleTest 
COMMAND ITKParticleShapeModelingTestDriver
itkPSMEntropyRegressionModel2DFilterMultiscaleTest
${TEST_DATA_ROOT}/Cardiac/LACine/PSMEntropyRegressionModel2DFilterMultiscaleTest.xml
${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Cardiac/LACine/
)

itk_add_test(NAME itkPSMEntropyRegressionModel2DFilterTest 
COMMAND ITKParticleShapeModelingTestDriver
itkPSMEntropyRegressionModel2DFilterTest
${TEST_DATA_ROOT}/Cardiac/LACine/PSMEntropyRegressionModel2DFilterTest.xml
${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Cardiac/LACine/
)

itk_add_test(NAME itkPSMEntropyRegressionModelFilterTest 
COMMAND ITKParticleShapeModelingTestDriver
itkPSMEntropyRegressionModelFilterTest
${TEST_DATA_ROOT}/Torus/PSMEntropyModelFilterTest.xml
${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Torus/
)

itk_add_test(NAME itkPSMGradientDescentOptimizerTest COMMAND
ITKParticleShapeModelingTestDriver
itkPSMGradientDescentOptimizerTest)

itk_add_test(NAME itkPSMImplicitSurfaceImageFilterTest 
  COMMAND ITKParticleShapeModelingTestDriver  itkPSMImplicitSurfaceImageFilterTest
  ${TEST_DATA_ROOT}/Cardiac/blood_pool.nrrd  
  ${ITK_TEST_OUTPUT_DIR}/itkPSMImplicitSurfaceImageFilterTestOutput.nrrd 1.2 0.5
)

itk_add_test(NAME itkPSMImplicitSurfaceImage2DFilterTest 
  COMMAND ITKParticleShapeModelingTestDriver  itkPSMImplicitSurfaceImage2DFilterTest
  ${TEST_DATA_ROOT}/Cardiac/LACine/cinemorph1-00.dcm
  ${ITK_TEST_OUTPUT_DIR}/itkPSMImplicitSurfaceImage2DFilterTestOutput.nrrd 1.2 0.5
)

itk_add_test(NAME itkPSMMeanCurvatureAttributeTest 
  COMMAND ITKParticleShapeModelingTestDriver  itkPSMMeanCurvatureAttributeTest
  ${TEST_DATA_ROOT}/Torus/torus.00.nrrd
)

itk_add_test(NAME itkPSMParticleSystemTest COMMAND ITKParticleShapeModelingTestDriver itkPSMParticleSystemTest ) 

itk_add_test(NAME itkPSMPointTreeTest COMMAND ITKParticleShapeModelingTestDriver itkPSMPointTreeTest )

itk_add_test(NAME itkPSMProcrustesFunction2DTest 
 COMMAND ITKParticleShapeModelingTestDriver
 itkPSMProcrustesFunction2DTest
 ${TEST_DATA_ROOT}/Procrustes/PSMProcrustesFunction2DTest.xml
 ${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Procrustes/
)

itk_add_test(NAME itkPSMProcrustesFunctionTest 
 COMMAND ITKParticleShapeModelingTestDriver
 itkPSMProcrustesFunctionTest
 ${TEST_DATA_ROOT}/Procrustes/PSMProcrustesFunctionTest.xml
 ${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Procrustes/
)

itk_add_test(NAME itkPSMProcrustesRegistration2DTest 
 COMMAND ITKParticleShapeModelingTestDriver
 itkPSMProcrustesRegistration2DTest
 ${TEST_DATA_ROOT}/Procrustes/PSMProcrustesRegistration2DTest.xml
 ${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Procrustes/
)

itk_add_test(NAME itkPSMProcrustesRegistrationTest 
 COMMAND ITKParticleShapeModelingTestDriver
 itkPSMProcrustesRegistrationTest
 ${TEST_DATA_ROOT}/Procrustes/PSMProcrustesRegistrationTest.xml
 ${ITK_TEST_OUTPUT_DIR}/
 ${TEST_DATA_ROOT}/Procrustes/
)

itk_add_test(NAME itkPSMProjectTest COMMAND ITKParticleShapeModelingTestDriver itkPSMProjectTest
${TEST_DATA_ROOT}/PSMProjectExample.xml)

itk_add_test(NAME itkPSMRBFCorrespondenceInterpolatorTest
 COMMAND ITKParticleShapeModelingTestDriver
 itkPSMRBFCorrespondenceInterpolatorTest
 ${TEST_DATA_ROOT}/Torus/initialize.00.lpts
 ${TEST_DATA_ROOT}/Torus/initialize.01.lpts
 ${TEST_DATA_ROOT}/Torus/RBFTestInterpolatorPoints.lpts
)

itk_add_test(NAME itkPSMRegionNeighborhoodTest COMMAND ITKParticleShapeModelingTestDriver itkPSMRegionNeighborhoodTest )

itk_add_test(NAME itkPSMShapeEntropyFunctionTest COMMAND ITKParticleShapeModelingTestDriver itkPSMShapeEntropyFunctionTest)

itk_add_test(NAME itkPSMSurfaceNeighborhoodTest 
  COMMAND ITKParticleShapeModelingTestDriver itkPSMSurfaceNeighborhoodTest
  ${TEST_DATA_ROOT}/Torus/torus.00.nrrd
)

itk_add_test(NAME itkPSMTrimLabelMapImageFilterTest 
  COMMAND ITKParticleShapeModelingTestDriver  itkPSMTrimLabelMapImageFilterTest
  ${TEST_DATA_ROOT}/Torus/torus.00.nrrd  
  ${ITK_TEST_OUTPUT_DIR}/itkPSMTrimLabelMapImageFilterTestOutput.nrrd
)

itk_add_test(NAME itkPSMTwoCostFunctionTest COMMAND ITKParticleShapeModelingTestDriver  itkPSMTwoCostFunctionTest )

# Necessary on Linux build to include IOXML in proper order
target_link_libraries(ITKParticleShapeModelingTestDriver ITKIOXML)
