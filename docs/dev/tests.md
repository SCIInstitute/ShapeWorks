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

+ For project related functions, use Testing/GroomTests/GroomTests.cpp  
+ For image related functions, use Testing/ImageTests/ImageTests.cpp  
+ For mesh related functions, use Testing/MeshTests/MeshTests.cpp  
+ For optimize related functions, use Testing/OptimizeTests/OptimizeTests.cpp  
+ For particles related functions, use Testing/ParticlesTests/ParticlesTests.cpp  

```
TEST(typeofoperationTests, nameoftestTest)
{
  // define test

  ASSERT_TRUE(); // compare result
}
```

For testing from Python interface, use Testing/PythonTests/PythonTests.cpp  
Python scripts should be added to Testing/PythonTests

```
TEST(pythonTests, nameoftestTest)
{
  run_use_case("script.py");
}
```

For testing from Command Line interface, use Testing/shapeworksTests/shapeworksTests.cpp  
Shell scripts should be added to Testing/shapeworksTests

```
TEST(shapeworksTests, nameoftestTest)
{
  run_use_case("script.sh");
}
```

Testing data should be placed in Testing/data.

## Measuring Test Coverage

Coverage is measured for the C++ code in `Libs/`, `Applications/` and `Studio/` (with gcovr) and for the Python packages in `Python/` (with coverage.py). C++ code reached from the Python tests through the Python bindings is counted too. Use a separate build directory, since the instrumented build is unoptimized.

```
pip install gcovr coverage
mkdir build-coverage && cd build-coverage
cmake -DSW_ENABLE_COVERAGE=ON -DCMAKE_BUILD_TYPE=Debug <usual options> ..
ninja                    # or make
source ../devenv.sh ./bin
ninja coverage
```

`ninja coverage` resets earlier results, runs `ctest`, and writes the reports. Reports are still written when some tests fail, but the target then exits with an error. Open `coverage/index.html` for a summary of both, with totals and coverage per library and Python package; it links to the detailed reports in `coverage/cpp/` and `coverage/python/`. The same summary is printed at the end of the run and saved as `coverage/summary.json`.

To run only some tests, pass `ctest` arguments in `SW_COVERAGE_CTEST_ARGS`, e.g. `SW_COVERAGE_CTEST_ARGS="-R Mesh" ninja coverage`. `ninja coverage_clean` and `ninja coverage_report` run the first and last steps on their own, if you prefer to run the tests yourself.

Python coverage works by placing a `sitecustomize.py` on `PYTHONPATH` during the test run, which starts coverage.py in every Python process the tests launch.

On macOS, Xcode's `llvm-cov gcov` is used to read the C++ data. Set `SW_GCOV_EXECUTABLE` to use a different `gcov` command.

### Weekly coverage runs

The [Linux Coverage](https://github.com/SCIInstitute/ShapeWorks/actions/workflows/coverage.yml) GitHub Action runs every week and can also be started by hand. Each run:

- shows the totals and the change since the previous run in the run summary,
- attaches the full HTML reports as the `coverage-report` artifact (kept for 90 days),
- on `master`, adds a row to `history.csv` on the [`coverage-history`](https://github.com/SCIInstitute/ShapeWorks/tree/coverage-history) branch, whose README shows trend charts and recent runs.
