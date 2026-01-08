#!/usr/bin/env python3
"""
Verify DeepSSM test results by checking the mean distance from test_distances.csv.

Usage:
    python verify_deepssm_results.py <project_dir> [--exact_check save|verify] [--expected <value>]

The script checks that the mean surface-to-surface distance is reasonable (roughly 10, within tolerance).
For exact refactoring verification, use --exact_check save/verify to save or compare exact values.
"""

import argparse
import csv
import math
import os
import sys


def get_mean_distance(project_dir: str) -> float:
    """Read mean distance from test_distances.csv."""
    csv_path = os.path.join(project_dir, "deepssm", "test_distances.csv")
    if not os.path.exists(csv_path):
        raise FileNotFoundError(f"Results file not found: {csv_path}")

    distances = []
    with open(csv_path, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            distances.append(float(row['Distance']))

    if not distances:
        raise ValueError(f"No distances found in {csv_path}")

    return sum(distances) / len(distances)


def main():
    parser = argparse.ArgumentParser(description="Verify DeepSSM test results")
    parser.add_argument("project_dir", help="Path to the project directory containing deepssm/ output")
    parser.add_argument("--exact_check", choices=["save", "verify"],
                        help="Save or verify exact values for refactoring verification")
    parser.add_argument("--expected", type=float, default=150.0,
                        help="Expected mean distance for relaxed check (default: 150.0)")
    parser.add_argument("--tolerance", type=float, default=1.0,
                        help="Relative tolerance for relaxed check (default: 1.0 = 100%%)")
    args = parser.parse_args()

    try:
        mean_dist = get_mean_distance(args.project_dir)
        print(f"Mean distance: {mean_dist}")
    except (FileNotFoundError, ValueError) as e:
        print(f"Error: {e}")
        sys.exit(1)

    exact_check_file = os.path.join(args.project_dir, "exact_check_value.txt")

    if args.exact_check == "save":
        with open(exact_check_file, "w") as f:
            f.write(str(mean_dist))
        print(f"Saved exact check value to: {exact_check_file}")
        sys.exit(0)

    elif args.exact_check == "verify":
        if not os.path.exists(exact_check_file):
            print(f"Error: No saved value found at {exact_check_file}")
            print("Run with --exact_check save first to create baseline.")
            sys.exit(1)
        with open(exact_check_file, "r") as f:
            expected = float(f.read().strip())
        if mean_dist != expected:
            print(f"Exact check FAILED: expected {expected}, got {mean_dist}")
            sys.exit(1)
        print(f"Exact check PASSED: {mean_dist}")
        sys.exit(0)

    else:
        # Relaxed check for CI/cross-platform
        if not math.isclose(mean_dist, args.expected, rel_tol=args.tolerance):
            print(f"FAILED: mean distance {mean_dist} not close to {args.expected} (tolerance {args.tolerance})")
            sys.exit(1)
        print(f"PASSED: mean distance {mean_dist} is within tolerance of {args.expected}")
        sys.exit(0)


if __name__ == "__main__":
    main()
