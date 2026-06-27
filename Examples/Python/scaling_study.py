#!/usr/bin/env python3
"""
ShapeWorks optimization scaling study.

Measures how `shapeworks optimize` runtime and CPU-core utilization scale with
the number of subjects (N) and the number of particles (P) on this machine.

Background / hypothesis (see CorrespondenceFunction.cpp:203 and
GradientDescentOptimizer.cpp:135):
  - Per-particle gradient work is parallelized over SUBJECTS via tbb::parallel_for.
  - The correspondence entropy step runs a SERIAL vnl_svd on the N x N Gram matrix,
    which is O(N^3) in the number of subjects.
  => As N grows, the serial cubic SVD dominates (Amdahl) and CPU collapses to ~1 core.

Cohort construction:
  A single canonical femur mesh (centered at the origin) is replicated N times,
  each copy perturbed by a small random rigid transform plus light per-vertex
  Gaussian noise. All copies share topology and are inherently aligned, giving a
  controlled, full-rank covariance. Shape *content* does not affect the O(N^3) SVD
  cost (it depends only on matrix size), so this is a valid and reproducible basis
  for a pure runtime/utilization study.

Phases: a run has an initialization phase (particle splitting; iters_per_split
iterations per level) and an optimization phase (optimization_iterations at the
full count). By default initialization runs in mean-energy mode with NO SVD and
parallelizes well; the O(N^3) SVD is in the optimization (entropy) phase. Drive
initialization with --iters-per-split / --init-schedule to study both phases.
(In multiscale runs, later scales' init switches to entropy and does run the SVD.)

Metrics per (N, P) cell -> scaling_results.csv:
  wall_s            total optimize wall-clock (s)
  init_ms           inclusive time in Optimize::Initialize (splitting phase)
  runopt_ms         inclusive time in Optimize::RunOptimize (optimization phase)
  svd_ms            inclusive time in correspondence::svd (from profile.txt)
  gram_ms           inclusive time in correspondence::gramMat
  sampling_ms       inclusive time in parallel_sampling
  before_iter_ms    inclusive time in gradient_before_iteration
  mean_cores        mean cores busy (proc cpu% / 100) sampled during the run
  peak_cores        peak cores busy
  peak_rss_mb       peak resident memory (MB)
  status            ok | timeout | error
"""
import argparse
import csv
import os
import subprocess
import sys
import threading
import time

import numpy as np
import psutil
import vtk
from vtk.util.numpy_support import numpy_to_vtk, vtk_to_numpy

import shapeworks as sw

SEED = 1234
DEFAULT_BASE = "Output/femur_cut/m03_L_femur.ply"
# Both correspondence paths build an N x N Gram matrix and run a SERIAL vnl_svd
# on it -> O(N^3) in subjects. The Legacy path (use_normals=0, no fields) lumps
# gram+svd+pinv into ComputeCovarianceMatrix::Entropy; the mesh-based path
# (normals/fields) exposes correspondence::svd / ::gramMat separately.
PROFILE_KEYS = {
    "cov_entropy_ms": "ComputeCovarianceMatrix::Entropy",  # legacy serial correspondence (incl SVD)
    "cov_mean_ms": "ComputeCovarianceMatrix::MeanEnergy",  # legacy init (mean energy)
    "svd_ms": "correspondence::svd",                       # mesh-based path SVD
    "gram_ms": "correspondence::gramMat",                  # mesh-based path Gram build
    "sampling_ms": "parallel_sampling",                    # parallel per-particle gradient
    "before_iter_ms": "gradient_before_iteration",
    "optimize_ms": "optimize",                             # total optimize scope
    "init_ms": "Optimize::Initialize",                     # initialization phase (splitting; mean-energy, no SVD by default)
    "runopt_ms": "Optimize::RunOptimize",                  # optimization phase (entropy, runs the SVD)
}


def log(msg):
    print(f"[scaling] {msg}", flush=True)


# --------------------------------------------------------------------------
# Cohort construction
# --------------------------------------------------------------------------
def read_polydata(path):
    if path.endswith(".ply"):
        reader = vtk.vtkPLYReader()
    else:
        reader = vtk.vtkPolyDataReader()
    reader.SetFileName(path)
    reader.Update()
    return reader.GetOutput()


def write_polydata(poly, path):
    writer = vtk.vtkPolyDataWriter()
    writer.SetFileName(path)
    writer.SetInputData(poly)
    writer.SetFileTypeToBinary()
    writer.Write()


def get_points(poly):
    return vtk_to_numpy(poly.GetPoints().GetData()).copy()


def set_points(poly, pts):
    poly.GetPoints().SetData(numpy_to_vtk(np.ascontiguousarray(pts), deep=1))


def rotation_matrix(rx, ry, rz):
    cx, sx = np.cos(rx), np.sin(rx)
    cy, sy = np.cos(ry), np.sin(ry)
    cz, sz = np.cos(rz), np.sin(rz)
    Rx = np.array([[1, 0, 0], [0, cx, -sx], [0, sx, cx]])
    Ry = np.array([[cy, 0, sy], [0, 1, 0], [-sy, 0, cy]])
    Rz = np.array([[cz, -sz, 0], [sz, cz, 0], [0, 0, 1]])
    return Rz @ Ry @ Rx


def make_base(base_src, work_dir):
    """Load the source femur, center it at the origin, cache as base.vtk."""
    base_path = os.path.join(work_dir, "base.vtk")
    if os.path.exists(base_path):
        return base_path
    poly = read_polydata(base_src)
    pts = get_points(poly)
    pts = pts - pts.mean(axis=0)
    set_points(poly, pts)
    write_polydata(poly, base_path)
    log(f"base mesh: {pts.shape[0]} pts, centered -> {base_path}")
    return base_path


def make_cohort(base_path, n, cohort_dir, rot_deg=4.0, trans_mm=2.0, noise_mm=0.4):
    """Create N jittered copies of the base mesh. Cached & reproducible by seed.

    Copies are generated deterministically so cohort(N) is a prefix of cohort(M>N).
    """
    os.makedirs(cohort_dir, exist_ok=True)
    base_poly = read_polydata(base_path)
    base_pts = get_points(base_poly)
    rng = np.random.default_rng(SEED)
    files = []
    for i in range(n):
        # advance rng deterministically per-index so prefixes are stable
        r = np.random.default_rng([SEED, i])
        out = os.path.join(cohort_dir, f"femur_{i:05d}.vtk")
        files.append(out)
        if os.path.exists(out):
            continue
        ang = np.deg2rad(rot_deg) * r.standard_normal(3)
        R = rotation_matrix(*ang)
        t = trans_mm * r.standard_normal(3)
        noise = noise_mm * r.standard_normal(base_pts.shape)
        pts = base_pts @ R.T + t + noise
        poly = vtk.vtkPolyData()
        poly.DeepCopy(base_poly)
        set_points(poly, pts)
        write_polydata(poly, out)
    del rng
    return files


# --------------------------------------------------------------------------
# Project / optimize
# --------------------------------------------------------------------------
def build_project(cohort_files, particles, iterations, proj_path, use_normals=0,
                  iters_per_split=0, multiscale=0, multiscale_particles=32):
    subjects = []
    identity = np.eye(4).flatten()
    proj_dir = os.path.dirname(os.path.abspath(proj_path))
    for f in cohort_files:
        abs_f = os.path.abspath(f)
        rel = sw.utils.get_relative_paths([abs_f], proj_dir)
        s = sw.Subject()
        s.set_number_of_domains(1)
        s.set_original_filenames(rel)
        s.set_groomed_filenames(rel)
        s.set_groomed_transforms([identity])
        subjects.append(s)
    project = sw.Project()
    project.set_subjects(subjects)
    params = sw.Parameters()
    pdict = {
        "number_of_particles": particles,
        "use_normals": use_normals,
        "normals_strength": 10.0,
        "checkpointing_interval": 0,
        "keep_checkpoints": 0,
        "iterations_per_split": iters_per_split,
        "optimization_iterations": iterations,
        "starting_regularization": 1000,
        "ending_regularization": 10,
        "relative_weighting": 1,
        "initial_relative_weighting": 0.05,
        "procrustes": 0,
        "procrustes_interval": 0,
        "procrustes_scaling": 0,
        "save_init_splits": 0,
        "verbosity": 0,
        "multiscale": multiscale,
        "multiscale_particles": multiscale_particles,
    }
    for k, v in pdict.items():
        params.set(k, sw.Variant([v]))
    project.set_parameters("optimize", params)
    project.save(proj_path)
    return proj_path


def parse_profile(profile_path):
    """Sum inclusive-ms across all threads for each profile key."""
    out = {k: 0.0 for k in PROFILE_KEYS}
    if not os.path.exists(profile_path):
        return out
    with open(profile_path) as fh:
        for line in fh:
            parts = line.split()
            if len(parts) < 8:
                continue
            name = parts[-1]
            for key, scope in PROFILE_KEYS.items():
                if name == scope:
                    try:
                        out[key] += float(parts[2])  # inclusive total ms column
                    except ValueError:
                        pass
    return out


def run_optimize(proj_path, time_cap_s, sample_dt=0.25):
    """Run shapeworks optimize, sampling CPU/RSS. Returns metrics dict."""
    proj_dir = os.path.dirname(os.path.abspath(proj_path))
    profile_path = os.path.join(proj_dir, "profile.txt")
    if os.path.exists(profile_path):
        os.remove(profile_path)
    env = dict(os.environ)
    env["SW_TIME_PROFILE"] = "1"
    cmd = ["shapeworks", "optimize", "--progress", "--name", os.path.basename(proj_path)]

    cores_samples = []
    peak_rss = [0.0]
    stop = threading.Event()

    t0 = time.time()
    proc = psutil.Popen(cmd, cwd=proj_dir, env=env,
                        stdout=subprocess.DEVNULL, stderr=subprocess.PIPE)

    def sampler():
        try:
            proc.cpu_percent(None)  # prime
        except Exception:
            return
        while not stop.is_set():
            try:
                cpu = proc.cpu_percent(None)
                rss = proc.memory_info().rss
                for c in proc.children(recursive=True):
                    try:
                        cpu += c.cpu_percent(None)
                        rss += c.memory_info().rss
                    except Exception:
                        pass
                cores_samples.append(cpu / 100.0)
                peak_rss[0] = max(peak_rss[0], rss)
            except (psutil.NoSuchProcess, psutil.AccessDenied):
                break
            time.sleep(sample_dt)

    th = threading.Thread(target=sampler, daemon=True)
    th.start()

    status = "ok"
    try:
        _, err = proc.communicate(timeout=time_cap_s)
    except subprocess.TimeoutExpired:
        proc.kill()
        proc.communicate()
        status = "timeout"
        err = b""
    finally:
        stop.set()
        th.join(timeout=2)
    wall = time.time() - t0

    if status == "ok" and proc.returncode != 0:
        status = "error"
        log(f"  optimize failed rc={proc.returncode}: {err.decode(errors='ignore')[:400]}")

    prof = parse_profile(profile_path)
    samples = [s for s in cores_samples if s > 0]
    metrics = {
        "wall_s": round(wall, 2),
        "mean_cores": round(float(np.mean(samples)), 2) if samples else 0.0,
        "peak_cores": round(float(np.max(cores_samples)), 2) if cores_samples else 0.0,
        "peak_rss_mb": round(peak_rss[0] / 1e6, 1),
        "n_samples": len(cores_samples),
        "status": status,
    }
    for k in PROFILE_KEYS:
        metrics[k] = round(prof[k], 1)
    return metrics


# --------------------------------------------------------------------------
# Driver
# --------------------------------------------------------------------------
def parse_schedule(spec):
    """'512:100,1024:20,...' -> sorted [(nmax, iters), ...]."""
    if not spec:
        return None
    sched = []
    for tok in spec.split(","):
        nmax, it = tok.split(":")
        sched.append((int(nmax), int(it)))
    return sorted(sched)


def iters_for_n(n, schedule, default):
    if not schedule:
        return default
    for nmax, it in schedule:
        if n <= nmax:
            return it
    return schedule[-1][1]


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n-list", type=int, nargs="+", default=[8, 16, 32, 64, 128])
    ap.add_argument("--p-list", type=int, nargs="+", default=[128, 256, 512, 1024])
    ap.add_argument("--iterations", type=int, default=100,
                    help="fixed iteration count (used when --iter-schedule is empty)")
    ap.add_argument("--iter-schedule", default="",
                    help="adaptive iters as 'nmax:iters,...' e.g. '512:100,1024:20,2048:5,4096:3'. "
                         "Per-iteration metrics make cells comparable across different iter counts.")
    ap.add_argument("--use-normals", type=int, default=0,
                    help="0=Legacy correspondence (matches user config); 1=mesh-based (normals)")
    ap.add_argument("--iters-per-split", type=int, default=0,
                    help="initialization iterations per particle-split level (mean-energy phase, no SVD). "
                         "Default 0 measures the optimization phase only.")
    ap.add_argument("--init-schedule", default="",
                    help="adaptive iters-per-split as 'nmax:iters,...' (overrides --iters-per-split per N)")
    ap.add_argument("--multiscale", type=int, default=0,
                    help="1 enables multiscale; later scales run the SVD during init (use_shape_statistics_in_init)")
    ap.add_argument("--multiscale-particles", type=int, default=32,
                    help="coarsest-scale particle count when --multiscale 1")
    ap.add_argument("--time-cap", type=float, default=2700, help="per-cell wall-clock cap (s)")
    ap.add_argument("--base", default=DEFAULT_BASE)
    ap.add_argument("--work-dir", default="Output/scaling_study")
    ap.add_argument("--csv", default="Output/scaling_study/scaling_results.csv")
    ap.add_argument("--smoke", action="store_true", help="tiny smoke test (N=4,P=32,iters=10)")
    args = ap.parse_args()

    if args.smoke:
        args.n_list, args.p_list, args.iterations = [4], [32], 10
        args.time_cap = 300

    os.makedirs(args.work_dir, exist_ok=True)
    base_path = make_base(args.base, args.work_dir)

    fieldnames = ["n", "p", "iterations", "iters_per_split", "multiscale", "use_normals",
                  "wall_s", "init_ms", "runopt_ms", "mean_cores",
                  "peak_cores", "peak_rss_mb", "cov_entropy_ms", "cov_mean_ms",
                  "svd_ms", "gram_ms", "sampling_ms", "before_iter_ms",
                  "optimize_ms", "n_samples", "status"]
    write_header = not os.path.exists(args.csv)
    csv_fh = open(args.csv, "a", newline="")
    writer = csv.DictWriter(csv_fh, fieldnames=fieldnames)
    if write_header:
        writer.writeheader()
        csv_fh.flush()

    schedule = parse_schedule(args.iter_schedule)
    init_schedule = parse_schedule(args.init_schedule)
    log(f"grid: N={args.n_list} x P={args.p_list}, "
        f"iters={'schedule '+args.iter_schedule if schedule else args.iterations}, "
        f"iters_per_split={'schedule '+args.init_schedule if init_schedule else args.iters_per_split}, "
        f"multiscale={args.multiscale}, cap={args.time_cap}s")
    nmax = max(args.n_list)
    for n in args.n_list:
        cohort_dir = os.path.join(args.work_dir, f"cohort_{nmax:05d}")
        cohort = make_cohort(base_path, n, cohort_dir)[:n]
        iters = iters_for_n(n, schedule, args.iterations)
        ips = iters_for_n(n, init_schedule, args.iters_per_split)
        for p in args.p_list:
            cell_dir = os.path.join(args.work_dir, f"run_n{n:05d}_p{p:04d}")
            os.makedirs(cell_dir, exist_ok=True)
            proj = os.path.join(cell_dir, "scaling.swproj")
            build_project(cohort, p, iters, proj, use_normals=args.use_normals,
                          iters_per_split=ips, multiscale=args.multiscale,
                          multiscale_particles=args.multiscale_particles)
            log(f"running N={n} P={p} iters={iters} iters_per_split={ips} ...")
            m = run_optimize(proj, args.time_cap)
            row = {"n": n, "p": p, "iterations": iters, "iters_per_split": ips,
                   "multiscale": args.multiscale, "use_normals": args.use_normals, **m}
            writer.writerow(row)
            csv_fh.flush()
            serial_corr = m["cov_entropy_ms"] + m["svd_ms"] + m["gram_ms"]
            log(f"  -> {m['status']} wall={m['wall_s']}s "
                f"init={m['init_ms']/1000:.1f}s opt={m['runopt_ms']/1000:.1f}s "
                f"serial_corr={serial_corr:.0f}ms sampling={m['sampling_ms']}ms "
                f"mean_cores={m['mean_cores']} peak_cores={m['peak_cores']} rss={m['peak_rss_mb']}MB")
    csv_fh.close()
    log(f"done -> {args.csv}")


if __name__ == "__main__":
    main()
