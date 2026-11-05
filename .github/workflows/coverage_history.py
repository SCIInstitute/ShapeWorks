#!/usr/bin/env python3
"""Record coverage totals over time.

Reads coverage/summary.json written by `make coverage`, prints a
markdown summary (for $GITHUB_STEP_SUMMARY) comparing against the previous run,
and with --append adds a row to history.csv and rewrites README.md in the
history directory (the coverage-history branch).
"""
import argparse
import csv
import datetime
import json
import os

FIELDS = ["date", "commit", "cpp_lines", "cpp_functions", "cpp_branches", "studio_lines", "python_lines", "run_url"]
METRICS = [
    ("cpp_lines", "C++ lines"),
    ("cpp_functions", "C++ functions"),
    ("cpp_branches", "C++ branches"),
    ("studio_lines", "Studio lines"),
    ("python_lines", "Python lines"),
]
CHARTS = [
    ("cpp_lines", "C++ line coverage (%)"),
    ("studio_lines", "Studio line coverage (%)"),
    ("python_lines", "Python line coverage (%)"),
]
CHART_RUNS = 52
TABLE_RUNS = 20


def load_json(path):
    if path and os.path.exists(path):
        with open(path) as f:
            return json.load(f)
    return None


def fmt(value):
    return "" if value is None else f"{value:.1f}"


def pct(value):
    return "–" if value is None else f"{value:.1f}%"


def current_totals(summary_path):
    totals = (load_json(summary_path) or {}).get("totals", {})
    return {key: totals.get(key) for key, _ in METRICS}


def read_history(path):
    if not os.path.exists(path):
        return []
    with open(path, newline="") as f:
        return list(csv.DictReader(f))


def to_float(text):
    try:
        return float(text)
    except (TypeError, ValueError):
        return None


def step_summary(totals, previous):
    lines = ["## Coverage", "", "| | This run | Previous | Change |", "|---|---|---|---|"]
    for key, label in METRICS:
        now = totals[key]
        before = to_float(previous.get(key)) if previous else None
        change = f"{round(now, 1) - round(before, 1):+.1f}" if now is not None and before is not None else ""
        lines.append(f"| {label} | {pct(now)} | {pct(before)} | {change} |")
    lines += ["", "The full HTML reports are in the `coverage-report` artifact."]
    return "\n".join(lines) + "\n"


def chart(rows, key, title):
    points = [(r["date"], to_float(r.get(key))) for r in rows[-CHART_RUNS:]]
    points = [(d, v) for d, v in points if v is not None]
    if not points:
        return []
    labels = ", ".join(f'"{d}"' for d, _ in points)
    values = ", ".join(f"{v:.1f}" for _, v in points)
    return [
        "```mermaid",
        "xychart-beta",
        f'    title "{title}"',
        f"    x-axis [{labels}]",
        '    y-axis "%" 0 --> 100',
        f"    bar [{values}]",
        "```",
        "",
    ]


def readme(rows, repo_url):
    latest = rows[-1]
    out = [
        "# ShapeWorks test coverage",
        "",
        f"Updated weekly by the [Linux Coverage]({repo_url}/actions/workflows/coverage.yml) workflow. "
        "Raw data is in [history.csv](history.csv); full HTML reports are attached to each workflow run.",
        "",
        f"Latest: {latest['date']}, commit [{latest['commit'][:10]}]({repo_url}/commit/{latest['commit']})",
        "",
        "| | Coverage |",
        "|---|---|",
    ]
    for key, label in METRICS:
        out.append(f"| {label} | {latest.get(key) or '–'}% |")
    out.append("")
    for key, title in CHARTS:
        out += chart(rows, key, title)
    out += [
        "## Recent runs",
        "",
        "| Date | Commit | " + " | ".join(label for _, label in METRICS) + " | Run |",
        "|---" * (len(METRICS) + 3) + "|",
    ]
    for r in reversed(rows[-TABLE_RUNS:]):
        commit = f"[{r['commit'][:10]}]({repo_url}/commit/{r['commit']})"
        values = " | ".join(r.get(key) or "–" for key, _ in METRICS)
        out.append(f"| {r['date']} | {commit} | {values} | [run]({r['run_url']}) |")
    return "\n".join(out) + "\n"


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary", required=True, help="coverage/summary.json")
    parser.add_argument("--history-dir", required=True)
    parser.add_argument("--commit", required=True)
    parser.add_argument("--run-url", default="")
    parser.add_argument("--repo-url", default="https://github.com/SCIInstitute/ShapeWorks")
    parser.add_argument("--summary-out", help="file to append the markdown summary to")
    parser.add_argument("--append", action="store_true", help="add this run to the history")
    args = parser.parse_args()

    totals = current_totals(args.summary)
    history_csv = os.path.join(args.history_dir, "history.csv")
    rows = read_history(history_csv)

    summary = step_summary(totals, rows[-1] if rows else None)
    print(summary)
    if args.summary_out:
        with open(args.summary_out, "a") as f:
            f.write(summary)

    if not args.append:
        return
    if all(value is None for value in totals.values()):
        print("No coverage results found, not updating history")
        return

    row = {
        "date": datetime.date.today().isoformat(),
        "commit": args.commit,
        "run_url": args.run_url,
        **{key: fmt(value) for key, value in totals.items()},
    }
    rows.append(row)
    os.makedirs(args.history_dir, exist_ok=True)
    with open(history_csv, "w", newline="") as f:
        writer = csv.DictWriter(f, fieldnames=FIELDS, lineterminator="\n")
        writer.writeheader()
        writer.writerows(rows)
    with open(os.path.join(args.history_dir, "README.md"), "w") as f:
        f.write(readme(rows, args.repo_url))


if __name__ == "__main__":
    main()
