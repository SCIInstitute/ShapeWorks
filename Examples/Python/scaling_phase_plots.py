#!/usr/bin/env python3
"""Phase-comparison plots: initialization vs optimization (reads scaling_phases.csv)."""
import argparse
import os

import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", default="Output/scaling_study/scaling_phases.csv")
    ap.add_argument("--out-dir", default="Output/scaling_study/plots_phases")
    args = ap.parse_args()
    os.makedirs(args.out_dir, exist_ok=True)

    df = pd.read_csv(args.csv)
    df = df[df["status"] == "ok"].sort_values("n").copy()
    df["init_s"] = df["init_ms"] / 1000
    df["opt_s"] = df["runopt_ms"] / 1000
    df["opt_frac"] = df["opt_s"] / (df["init_s"] + df["opt_s"])
    p = int(df["p"].iloc[0])

    # (1) init vs opt wall time vs N
    fig, ax = plt.subplots(figsize=(7, 5))
    ax.plot(df["n"], df["init_s"], "o-", color="seagreen", label="initialization (mean-energy, no SVD)")
    ax.plot(df["n"], df["opt_s"], "o-", color="crimson", label="optimization (entropy, O(N³) SVD)")
    ax.set_xscale("log", base=2); ax.set_yscale("log")
    ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("Phase wall-clock (s)")
    ax.set_title(f"INITIALIZATION vs OPTIMIZATION: phase wall time (P={p})")
    ax.legend(); ax.grid(True, which="both", alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/phase_walltime_vs_N.png", dpi=130)

    # (2) fraction of run spent in optimization vs N
    fig, ax = plt.subplots(figsize=(7, 5))
    ax.plot(df["n"], 100 * df["opt_frac"], "o-", color="darkorange")
    ax.axhline(50, ls="--", color="gray", alpha=0.6)
    ax.set_xscale("log", base=2)
    ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("% of run in optimization (SVD) phase")
    ax.set_title(f"INITIALIZATION vs OPTIMIZATION: share of run in optimization (P={p})")
    ax.set_ylim(0, 100); ax.grid(True, alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/opt_fraction_vs_N.png", dpi=130)

    # (3) blended CPU utilization vs N (realistic mixed run)
    fig, ax = plt.subplots(figsize=(7, 5))
    ax.plot(df["n"], df["mean_cores"], "o-", color="steelblue", label="mean cores busy (whole run)")
    ax.plot(df["n"], df["peak_cores"], "o--", color="lightsteelblue", alpha=0.7, label="peak cores")
    ax.set_xscale("log", base=2)
    ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("Cores busy")
    ax.set_title(f"INITIALIZATION + OPTIMIZATION: CPU utilization, realistic run (P={p})")
    ax.legend(); ax.grid(True, alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/cores_mixed_vs_N.png", dpi=130)

    print(f"wrote 3 plots to {args.out_dir}")
    print(df[["n", "p", "iters_per_split", "iterations", "init_s", "opt_s",
              "opt_frac", "mean_cores"]].round(2).to_string(index=False))


if __name__ == "__main__":
    main()
