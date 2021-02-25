# How to Add and Run Unit Tests?

## Running ShapeWorks Automated Tests

To run the automated tests, after building, run:

$ ctest

or

$ make test

If using Unix Makefile, for example


## Adding New ShapeWorks Automated Tests

Depending on the type of operation to be tested, they should be defined in the associated Tests.cpp

Testing from C++ interface:
    For testing project related functions, use Testing/GroomTests/GroomTests.cpp
    For testing image related functions, use Testing/ImageTests/ImageTests.cpp
    For testing mesh related functions, use Testing/MeshTests/MeshTests.cpp
    For testing optimize related functions, use Testing/OptimizeTests/OptimizeTests.cpp
    For testing particles related functions, use Testing/ParticlesTests/ParticlesTests.cpp

``
TEST(typeofoperationTests, nameoftestTest)
{
  // define test

  ASSERT_TRUE(); // compare result
}
``

For testing from Python interface, use Testing/PythonTests/PythonTests.cpp
Python scripts should be added to Testing/PythonTests

``
TEST(pythonTests, nameoftestTest)
{
  ASSERT_FALSE(system("python script.py"));
}
``

For testing from Command Line interface, use Testing/shapeworksTests/shapeworksTests.cpp
Shell scripts should be added to Testing/shapeworksTests

``
TEST(shapeworksTests, nameoftestTest)
{
  ASSERT_FALSE(system("bash script.sh"));
}
``

Testing data should be placed in Testing/data.
