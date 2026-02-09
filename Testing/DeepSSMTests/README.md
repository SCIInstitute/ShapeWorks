# DeepSSM Tests

Automated tests for DeepSSM using ShapeWorks project files (.swproj).

## Test Configurations

| Test | Description |
|------|-------------|
| `deepssm_default` | Standard DeepSSM (no TL-Net, no fine-tuning) |
| `deepssm_tl_net` | TL-DeepSSM network enabled |
| `deepssm_fine_tune` | Fine-tuning enabled |
| `deepssm_tl_net_fine_tune` | Both TL-DeepSSM and fine-tuning enabled |

## Running Tests

### Run all DeepSSM tests:
```bash
cd /path/to/build
ctest -R DeepSSMTests -V
```

### Run a specific test:
```bash
./bin/DeepSSMTests --gtest_filter="*default*"
./bin/DeepSSMTests --gtest_filter="*tl_net*"
```

### Run tests directly via shell scripts:
```bash
export DATA=/path/to/Testing/data
bash Testing/DeepSSMTests/deepssm_default.sh
```

## Test Data

Test data is stored in `Testing/data/deepssm_test_data.zip` and automatically extracted on first run. Contains:
- 5 femur meshes, CT images, and constraint files
- Pre-configured project files for each test configuration

## Result Verification

Tests verify that the mean surface-to-surface distance is within tolerance. The default tolerance is loose (0-300) for quick 1-epoch tests.

### Exact Check Mode (for refactoring verification)

When refactoring DeepSSM code, you can verify results are identical before and after changes.

**Run all configurations:**
```bash
# Save baselines (before refactoring)
bash Testing/DeepSSMTests/run_exact_check.sh save

# Verify after refactoring
bash Testing/DeepSSMTests/run_exact_check.sh verify
```

**Run a single configuration:**
```bash
cd Testing/data/deepssm/projects
rm -rf deepssm groomed *_particles
shapeworks deepssm --name default.swproj --all

# Save or verify
python Testing/DeepSSMTests/verify_deepssm_results.py . --exact_check save
python Testing/DeepSSMTests/verify_deepssm_results.py . --exact_check verify
```

Baseline values are saved to `exact_check_*.txt` in the project directory.

**Note:** Exact check is platform-specific due to floating-point differences. Only compare results from the same machine.

## Extended Tests (Manual)

Extended tests run on a directory of projects for meaningful accuracy checks. These are not part of automated CI.

### Directory Structure

```
/path/to/projects/
  project1/
    project1.swproj
    femur/...
  project2/
    project2.swproj
    data/...
```

Each subdirectory should contain a `.swproj` file and its associated data.

### Running Extended Tests

```bash
# Run all projects with relaxed tolerance
bash Testing/DeepSSMTests/run_extended_tests.sh /path/to/projects

# Save baselines for exact check
bash Testing/DeepSSMTests/run_extended_tests.sh /path/to/projects save

# Verify against baselines
bash Testing/DeepSSMTests/run_extended_tests.sh /path/to/projects verify

# Run specific project only
bash Testing/DeepSSMTests/run_extended_tests.sh /path/to/projects save femur
bash Testing/DeepSSMTests/run_extended_tests.sh /path/to/projects verify femur
```

Baseline values are saved to `exact_check_<project>.txt` in each project directory.
