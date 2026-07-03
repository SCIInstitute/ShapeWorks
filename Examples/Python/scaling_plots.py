#!/usr/bin/env python3
"""Plot results from scaling_study.py (reads scaling_results.csv)."""
import argparse
import os

import numpy as np
import pandas as pd
import matplotlib
matplotlib.use("Agg")
import matplotlib.pyplot as plt
from matplotlib.ticker import ScalarFormatter, NullLocator


def _int_xaxis(ax, ticks):
    """Label a log2 x-axis with plain integers (8, 16, ...) instead of 2^n,
    removing only the x minor ticks so y-axis log minor ticks are kept."""
    ax.set_xticks(list(ticks))
    ax.xaxis.set_major_formatter(ScalarFormatter())
    ax.xaxis.set_minor_locator(NullLocator())


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--csv", default="Output/scaling_study/scaling_results.csv")
    ap.add_argument("--out-dir", default="Output/scaling_study/plots")
    args = ap.parse_args()
    os.makedirs(args.out_dir, exist_ok=True)

    df = pd.read_csv(args.csv)
    df = df[df["status"] == "ok"].copy()
    df["serial_corr_ms"] = df["cov_entropy_ms"] + df["svd_ms"] + df["gram_ms"]
    # Per-iteration metrics are the scale-invariant quantities to compare across
    # cells, because the iteration count varies with N (adaptive schedule).
    df["wall_iter_s"] = df["wall_s"] / df["iterations"]
    df["serial_iter_s"] = df["serial_corr_ms"] / df["iterations"] / 1000
    df["samp_iter_s"] = df["sampling_ms"] / df["iterations"] / 1000
    ps = sorted(df["p"].unique())
    ns = sorted(df["n"].unique())

    # (a) per-iteration runtime vs N per P, log-log, with N^3 reference
    fig, ax = plt.subplots(figsize=(7, 5))
    for p in ps:
        d = df[df["p"] == p].sort_values("n")
        ax.plot(d["n"], d["wall_iter_s"], "o-", label=f"P={p}")
    # N^3 reference anchored in the cubic regime (largest N, largest P)
    d0 = df[df["p"] == ps[-1]].sort_values("n")
    n_anchor, w_anchor = d0["n"].iloc[-1], d0["wall_iter_s"].iloc[-1]
    ref_n = np.array([n for n in ns if n >= 64], dtype=float)
    ax.plot(ref_n, w_anchor * (ref_n / n_anchor) ** 3, "k--", alpha=0.6, label="N³ reference")
    ax.set_xscale("log", base=2); ax.set_yscale("log"); _int_xaxis(ax, ns)
    ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("Wall-clock per iteration (s)")
    ax.set_title("OPTIMIZATION phase: runtime per iteration vs subjects\n(iterations_per_split=0, optimization only)")
    ax.legend(); ax.grid(True, which="both", alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/runtime_vs_N.png", dpi=130)

    # (b) per-iteration runtime vs P per N
    fig, ax = plt.subplots(figsize=(7, 5))
    for n in ns:
        d = df[df["n"] == n].sort_values("p")
        ax.plot(d["p"], d["wall_iter_s"], "s-", label=f"N={n}")
    ax.set_xscale("log", base=2); ax.set_yscale("log"); _int_xaxis(ax, ps)
    ax.set_xlabel("Number of particles (P)"); ax.set_ylabel("Wall-clock per iteration (s)")
    ax.set_title("OPTIMIZATION phase: runtime per iteration vs particles")
    ax.legend(fontsize=8, ncol=2); ax.grid(True, which="both", alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/runtime_vs_P.png", dpi=130)

    # (c) serial SVD vs parallel sampling, per iteration vs N (largest P)
    p = ps[-1]
    d = df[df["p"] == p].sort_values("n")
    fig, ax = plt.subplots(figsize=(7, 5))
    ax.plot(d["n"], d["serial_iter_s"], "o-", color="crimson", label="serial correspondence/SVD")
    ax.plot(d["n"], d["samp_iter_s"], "o-", color="steelblue",
            label="parallel sampling (high-N inflated by 1-time setup)")
    ax.set_xscale("log", base=2); ax.set_yscale("log"); _int_xaxis(ax, ns)
    ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("Time per iteration (s)")
    ax.set_title(f"OPTIMIZATION phase: serial SVD vs parallel sampling, per iteration (P={p})")
    ax.legend(fontsize=8); ax.grid(True, which="both", alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/phase_vs_N.png", dpi=130)

    # (d) mean cores busy vs N
    fig, ax = plt.subplots(figsize=(7, 5))
    for p in ps:
        d = df[df["p"] == p].sort_values("n")
        ax.plot(d["n"], d["mean_cores"], "o-", label=f"P={p}")
    ax.set_xscale("log", base=2); _int_xaxis(ax, ns)
    ax.set_xlabel("Number of subjects (N)"); ax.set_ylabel("Mean cores busy")
    ax.set_title("OPTIMIZATION phase: CPU utilization vs subjects\n(iterations_per_split=0, optimization only)")
    ax.legend(); ax.grid(True, alpha=0.3)
    fig.tight_layout(); fig.savefig(f"{args.out_dir}/cores_vs_N.png", dpi=130)

    print(f"wrote 4 plots to {args.out_dir}")
    # console summary
    print(df[["n", "p", "wall_s", "serial_corr_ms", "sampling_ms", "mean_cores", "peak_cores"]]
          .to_string(index=False))


if __name__ == "__main__":
    main()
