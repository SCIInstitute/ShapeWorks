#!/usr/bin/env python3
"""Verify the dataset archives satisfy the paths the use cases actually glob for.

Parses each use case in Examples/Python with `ast`, walking the tiny_test and full
branches separately, to recover the glob patterns Step 1 will run. Each pattern is
then matched against the file list of the archive that use case downloads, so a
dataset that is missing a directory level, or was published with the wrong layout,
is caught before it reaches anyone.

Reads the archives' indexes only, so it is cheap even for the multi-gigabyte sets.

Usage:
    python3 Support/verify_dataset_archives.py [--staging DIR] [--examples DIR]
"""
import argparse
import ast
import fnmatch
import glob
import json
import os
import sys
import zipfile


def _const_str(node, env):
    """Best-effort evaluation of a string expression built from literals and names."""
    if isinstance(node, ast.Constant) and isinstance(node.value, str):
        return node.value
    if isinstance(node, ast.Name):
        return env.get(node.id)
    if isinstance(node, ast.BinOp) and isinstance(node.op, ast.Add):
        left = _const_str(node.left, env)
        right = _const_str(node.right, env)
        if left is not None and right is not None:
            return left + right
    if isinstance(node, ast.Subscript):
        return _const_str(node.value, env)
    if isinstance(node, ast.Call):
        if getattr(node.func, "id", None) == "sorted" and node.args:
            return _const_str(node.args[0], env)
        # Paths are sometimes anchored with os.getcwd(); the leading directory is
        # irrelevant here since patterns are matched relative to the archive root.
        if ast.unparse(node.func) == "os.getcwd":
            return ""
    return None


def _walk(body, env, found):
    """Walk statements, forking the environment at each If so branches stay independent."""
    for stmt in body:
        if isinstance(stmt, ast.Assign):
            for target in stmt.targets:
                if isinstance(target, ast.Name):
                    value = _const_str(stmt.value, env)
                    if value is not None:
                        env[target.id] = value
        elif isinstance(stmt, ast.If):
            _walk(stmt.body, dict(env), found)
            _walk(stmt.orelse, dict(env), found)
            continue
        elif isinstance(stmt, (ast.For, ast.While, ast.With, ast.Try)):
            _walk(stmt.body, env, found)
            continue

        for node in ast.walk(stmt):
            if not isinstance(node, ast.Call):
                continue
            name = ast.unparse(node.func)
            if name.endswith("download_dataset") and node.args:
                dataset = _const_str(node.args[0], env)
                if dataset:
                    env["__dataset__"] = dataset
                    env["__outdir__"] = _const_str(node.args[1], env) if len(node.args) > 1 else ""
            elif name == "glob.glob" and node.args:
                pattern = _const_str(node.args[0], env)
                if pattern and env.get("__dataset__"):
                    found.append((env["__dataset__"], pattern, env.get("__outdir__") or ""))


def runnable_use_cases(examples):
    """Use case names RunUseCase.py actually accepts, so scratch scripts are skipped."""
    runner = os.path.join(examples, "RunUseCase.py")
    tree = ast.parse(open(runner).read())
    for node in ast.walk(tree):
        if not isinstance(node, ast.Call):
            continue
        if not ast.unparse(node.func).endswith("add_argument"):
            continue
        for kw in node.keywords:
            if kw.arg == "choices" and isinstance(kw.value, (ast.List, ast.Tuple)):
                names = [e.value for e in kw.value.elts
                         if isinstance(e, ast.Constant) and isinstance(e.value, str)]
                if names:
                    return set(names)
    return set()


def collect_patterns(examples):
    """Return [(use case, dataset, pattern relative to the extraction root)]."""
    runnable = runnable_use_cases(examples)
    results = []
    for path in sorted(glob.glob(os.path.join(examples, "*.py"))):
        name = os.path.basename(path)
        if runnable and name[:-3] not in runnable:
            continue
        try:
            tree = ast.parse(open(path).read())
        except SyntaxError:
            continue
        for fn in [n for n in ast.walk(tree) if isinstance(n, ast.FunctionDef)]:
            if fn.name != "Run_Pipeline":
                continue
            found = []
            _walk(fn.body, {}, found)
            for dataset, pattern, outdir in found:
                rest = pattern[len(outdir):] if outdir and pattern.startswith(outdir) else pattern
                # The scripts concatenate path fragments, so doubled separators are
                # common; the filesystem collapses them but fnmatch would not.
                while "//" in rest:
                    rest = rest.replace("//", "/")
                results.append((name, dataset, rest.lstrip("/")))
    return results


def main():
    parser = argparse.ArgumentParser(description=__doc__,
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    here = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    parser.add_argument("--staging", default=os.path.expanduser("~/sci/datasets-web"),
                        help="directory holding the built archives")
    parser.add_argument("--examples", default=os.path.join(here, "Examples", "Python"),
                        help="directory holding the use case scripts")
    args = parser.parse_args()

    patterns = collect_patterns(args.examples)
    if not patterns:
        print("no glob patterns recovered from the use cases", file=sys.stderr)
        return 1

    manifest_path = os.path.join(args.staging, "manifest.json")
    if not os.path.exists(manifest_path):
        print(f"no manifest at {manifest_path}; run build_dataset_archives.py first",
              file=sys.stderr)
        return 1
    with open(manifest_path) as fh:
        published = json.load(fh).get("datasets", {})

    listings = {}
    rows, broken = [], 0
    for use_case, dataset, pattern in patterns:
        if dataset not in listings:
            entry = published.get(dataset)
            archive = os.path.join(args.staging, entry["file"]) if entry else None
            if not archive or not os.path.exists(archive):
                listings[dataset] = None
            else:
                with zipfile.ZipFile(archive) as zf:
                    listings[dataset] = [n for n in zf.namelist() if not n.endswith("/")]
        names = listings[dataset]
        if names is None:
            rows.append((use_case, dataset, pattern, 0, "NO ARCHIVE"))
            broken += 1
            continue
        hits = sum(1 for n in names if fnmatch.fnmatch(n, pattern))
        status = "ok" if hits else "NO MATCH"
        if not hits:
            broken += 1
        rows.append((use_case, dataset, pattern, hits, status))

    dw = max(len(r[1]) for r in rows) + 2
    pw = min(max(len(r[2]) for r in rows) + 2, 60)
    print(f"{'USE CASE':34} {'DATASET':{dw}} {'PATTERN':{pw}} {'N':>4}  STATUS")
    print("-" * (34 + dw + pw + 14))
    for use_case, dataset, pattern, hits, status in rows:
        print(f"{use_case:34} {dataset:{dw}} {pattern:{pw}} {hits:>4}  {status}")
    print("-" * (34 + dw + pw + 14))
    print(f"{len(rows)} patterns checked across {len(listings)} archives, {broken} broken")

    unused = sorted(set(published) - set(listings))
    if unused:
        print(f"\npublished but not referenced by any use case: {', '.join(unused)}")
    return 1 if broken else 0


if __name__ == "__main__":
    sys.exit(main())
