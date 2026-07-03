#!/usr/bin/env python3
"""Phase-comparison plots: initialization vs optimization (reads scaling_phases.csv)."""
import argparse
import os

import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter, NullLocator


def _int_xaxis(ax, ticks):
    """Plain-integer log2 x-axis (8, 16, ...) instead of 2^n; keeps y minor ticks."""
    ax.set_xticks(list(ticks))
    ax.xaxis.set_major_formatter(ScalarFormatter())
    ax.xaxis.set_minor_locator(NullLocator())


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", default="Output/scaling_study/scaling_phases.csv")
    ap.add_argument("--out-dir", default="Output/scaling_study/plots_phases")
    ap.add_argument("--init-only-csv", default="Output/scaling_study/init_only.csv",
                    help="init-phase isolation run (optimization_iterations=0)")
    ap.add_argument("--opt-only-csv", default="Output/scaling_study/scaling_overnight.csv",
                    help="optimization-phase run (iterations_per_split=0) for the per-phase cores comparison")
    ap.add_argument("--compare-p", type=int, default=256,
                    help="particle count at which to compare init vs opt phase cores")
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
    ax.set_xscale("log", base=2); ax.set_yscale("log"); _int_xaxis(ax, df["n"])
    ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("Phase wall-clock (s)")
    ax.set_title(f"INITIALIZATION vs OPTIMIZATION: phase wall time (P={p})")
    ax.legend(); ax.grid(True, which="both", alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/phase_walltime_vs_N.png", dpi=130)

    # (2) fraction of run spent in optimization vs N
    fig, ax = plt.subplots(figsize=(7, 5))
    ax.plot(df["n"], 100 * df["opt_frac"], "o-", color="darkorange")
    ax.axhline(50, ls="--", color="gray", alpha=0.6)
    ax.set_xscale("log", base=2); _int_xaxis(ax, df["n"])
    ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("% of run in optimization (SVD) phase")
    ax.set_title(f"INITIALIZATION vs OPTIMIZATION: share of run in optimization (P={p})")
    ax.set_ylim(0, 100); ax.grid(True, alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/opt_fraction_vs_N.png", dpi=130)

    # (3) blended CPU utilization vs N (realistic mixed run)
    fig, ax = plt.subplots(figsize=(7, 5))
    ax.plot(df["n"], df["mean_cores"], "o-", color="steelblue", label="mean cores busy (whole run)")
    ax.plot(df["n"], df["peak_cores"], "o--", color="lightsteelblue", alpha=0.7, label="peak cores")
    ax.set_xscale("log", base=2); _int_xaxis(ax, df["n"])
    ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("Cores busy")
    ax.set_title(f"INITIALIZATION + OPTIMIZATION: CPU utilization, realistic run (P={p})")
    ax.legend(); ax.grid(True, alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/cores_mixed_vs_N.png", dpi=130)

    # (4) per-phase CPU utilization: init-only vs opt-only, isolated runs
    n_plots = 3
    if os.path.exists(args.init_only_csv) and os.path.exists(args.opt_only_csv):
        io = pd.read_csv(args.init_only_csv)
        io = io[(io["status"] == "ok") & (io["p"] == args.compare_p)].sort_values("n")
        oo = pd.read_csv(args.opt_only_csv)
        oo = oo[(oo["status"] == "ok") & (oo["p"] == args.compare_p)].sort_values("n")
        if len(io) and len(oo):
            fig, ax = plt.subplots(figsize=(7, 5))
            ax.plot(io["n"], io["mean_cores"], "o-", color="seagreen",
                    label="INITIALIZATION phase (mean-energy, no SVD)")
            ax.plot(oo["n"], oo["mean_cores"], "o-", color="crimson",
                    label="OPTIMIZATION phase (entropy, O(N³) SVD)")
            ax.set_xscale("log", base=2); _int_xaxis(ax, sorted(oo["n"]))
            ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("Mean cores busy")
            ax.set_title(f"INITIALIZATION vs OPTIMIZATION: both collapse to ~1 core at scale (P={args.compare_p})")
            ax.legend(); ax.grid(True, alpha=0.3)
            fig.tight_layout(); fig.savefig(f"{args.out_dir}/cores_init_vs_opt.png", dpi=130)
            n_plots = 4

    print(f"wrote {n_plots} plots to {args.out_dir}")
    print(df[["n", "p", "iters_per_split", "iterations", "init_s", "opt_s",
              "opt_frac", "mean_cores"]].round(2).to_string(index=False))


if __name__ == "__main__":
    main()
