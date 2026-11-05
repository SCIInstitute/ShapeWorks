#!/usr/bin/env python3
"""Combine the C++ (gcovr) and Python (coverage.py) results into one summary.

Writes <coverage_dir>/summary.json and <coverage_dir>/index.html, and prints a
text summary. Run by the coverage_report target; either input may be missing.
"""
import argparse
import datetime
import html
import json
import os
from collections import defaultdict


def load_json(path):
    if os.path.exists(path):
        with open(path) as f:
            return json.load(f)
    return None


def percent(covered, total):
    return 100.0 * covered / total if total else None


def cpp_area(filename):
    """Group C++ files by library / application (Studio as a whole)."""
    parts = filename.split("/")
    return parts[0] if parts[0] == "Studio" else "/".join(parts[:2])


def python_area(filename):
    """Group Python files by package (Python/<Name>Package/... -> Name)."""
    parts = filename.replace("\\", "/").split("/")
    if "Python" in parts:
        parts = parts[parts.index("Python") + 1:]
    return parts[0].removesuffix("Package")


def summarize_cpp(data):
    areas = defaultdict(lambda: defaultdict(int))
    for f in data["files"]:
        a = areas[cpp_area(f["filename"])]
        for key in ("line_covered", "line_total", "function_covered", "function_total"):
            a[key] += f[key]
    rows = [{
        "name": name,
        "lines": percent(a["line_covered"], a["line_total"]),
        "line_covered": a["line_covered"],
        "line_total": a["line_total"],
        "functions": percent(a["function_covered"], a["function_total"]),
    } for name, a in areas.items()]
    rows.sort(key=lambda r: -r["line_total"])
    studio = next((r for r in rows if r["name"] == "Studio"), None)
    totals = {
        "cpp_lines": data["line_percent"],
        "cpp_functions": data["function_percent"],
        "cpp_branches": data["branch_percent"],
        "cpp_line_covered": data["line_covered"],
        "cpp_line_total": data["line_total"],
        "studio_lines": studio["lines"] if studio else None,
    }
    return totals, rows


def summarize_python(data):
    areas = defaultdict(lambda: [0, 0])
    for filename, f in data["files"].items():
        a = areas[python_area(filename)]
        a[0] += f["summary"]["covered_lines"]
        a[1] += f["summary"]["num_statements"]
    rows = [{"name": name, "lines": percent(c, n), "line_covered": c, "line_total": n}
            for name, (c, n) in areas.items()]
    rows.sort(key=lambda r: -r["line_total"])
    t = data["totals"]
    totals = {
        "python_lines": t["percent_covered"],
        "python_line_covered": t["covered_lines"],
        "python_line_total": t["num_statements"],
    }
    return totals, rows


def pct(value):
    return "–" if value is None else f"{value:.1f}%"


def bar(value):
    width = 0 if value is None else max(0.0, min(100.0, value))
    return f'<div class="bar"><div style="width:{width:.1f}%"></div></div>'


def area_table(rows, with_functions):
    head = "<tr><th>Area</th><th>Lines</th><th></th><th class=num>Covered / total</th>"
    head += "<th>Functions</th></tr>" if with_functions else "</tr>"
    body = []
    for r in rows:
        cells = (f"<td>{html.escape(r['name'])}</td><td class=num>{pct(r['lines'])}</td>"
                 f"<td>{bar(r['lines'])}</td>"
                 f"<td class=num>{r['line_covered']:,} / {r['line_total']:,}</td>")
        if with_functions:
            cells += f"<td class=num>{pct(r['functions'])}</td>"
        body.append(f"<tr>{cells}</tr>")
    return f"<table>{head}{''.join(body)}</table>"


def write_html(path, summary):
    t = summary["totals"]
    tiles = [
        ("C++ lines", t.get("cpp_lines")),
        ("C++ functions", t.get("cpp_functions")),
        ("C++ branches", t.get("cpp_branches")),
        ("Studio lines", t.get("studio_lines")),
        ("Python lines", t.get("python_lines")),
    ]
    tile_html = "".join(f'<div class="tile"><div class="label">{label}</div>'
                        f'<div class="value">{pct(v)}</div></div>' for label, v in tiles)
    sections = []
    if summary.get("cpp_areas"):
        sections.append('<h2>C++ <a href="cpp/index.html">detailed report</a></h2>'
                        + area_table(summary["cpp_areas"], True))
    if summary.get("python_areas"):
        sections.append('<h2>Python <a href="python/index.html">detailed report</a></h2>'
                        + area_table(summary["python_areas"], False))
    page = f"""<!doctype html>
<html lang="en"><head><meta charset="utf-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>ShapeWorks Coverage</title>
<style>
:root {{ --bg:#fff; --fg:#1d1d1f; --muted:#6e6e73; --line:#e5e5ea; --track:#ececf0; --fill:#2f6fde; --tile:#f6f6f8; }}
@media (prefers-color-scheme: dark) {{
  :root {{ --bg:#161618; --fg:#f2f2f4; --muted:#a1a1a6; --line:#2c2c30; --track:#2c2c30; --fill:#5b8ff0; --tile:#1f1f22; }}
}}
body {{ background:var(--bg); color:var(--fg); font:15px/1.45 -apple-system, system-ui, sans-serif; margin:0 auto; padding:24px 16px; max-width:960px; }}
h1 {{ font-size:22px; margin:0 0 4px; }}
h2 {{ font-size:17px; margin:32px 0 8px; }}
h2 a {{ font-size:13px; font-weight:normal; margin-left:8px; color:var(--fill); }}
.meta {{ color:var(--muted); font-size:13px; }}
.tiles {{ display:grid; grid-template-columns:repeat(auto-fit, minmax(150px, 1fr)); gap:12px; margin-top:20px; }}
.tile {{ background:var(--tile); border-radius:10px; padding:12px 14px; }}
.label {{ color:var(--muted); font-size:13px; }}
.value {{ font-size:24px; font-weight:600; font-variant-numeric:tabular-nums; }}
table {{ width:100%; border-collapse:collapse; font-variant-numeric:tabular-nums; }}
th, td {{ text-align:left; padding:6px 8px; border-bottom:1px solid var(--line); white-space:nowrap; }}
th {{ color:var(--muted); font-weight:500; font-size:13px; }}
.num {{ text-align:right; }}
td:nth-child(3) {{ width:40%; }}
.bar {{ background:var(--track); border-radius:3px; height:8px; min-width:60px; }}
.bar div {{ background:var(--fill); border-radius:3px; height:100%; }}
.wrap {{ overflow-x:auto; }}
</style></head><body>
<h1>ShapeWorks test coverage</h1>
<div class="meta">Generated {html.escape(summary["generated"])}</div>
<div class="tiles">{tile_html}</div>
<div class="wrap">{''.join(sections)}</div>
</body></html>
"""
    with open(path, "w") as f:
        f.write(page)


def print_text(summary):
    t = summary["totals"]
    print("Coverage summary")
    for label, key in (("C++ lines", "cpp_lines"), ("C++ functions", "cpp_functions"),
                       ("C++ branches", "cpp_branches"), ("Studio lines", "studio_lines"),
                       ("Python lines", "python_lines")):
        print(f"  {label:15s} {pct(t.get(key)):>7s}")
    for title, key in (("C++ by area", "cpp_areas"), ("Python by package", "python_areas")):
        if summary.get(key):
            print(title)
            for r in summary[key]:
                print(f"  {r['name']:30s} {pct(r['lines']):>7s}  ({r['line_covered']:,} / {r['line_total']:,} lines)")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("coverage_dir")
    args = parser.parse_args()

    summary = {"generated": datetime.datetime.now().strftime("%Y-%m-%d %H:%M"), "totals": {}}
    cpp = load_json(os.path.join(args.coverage_dir, "cpp", "summary.json"))
    if cpp:
        totals, summary["cpp_areas"] = summarize_cpp(cpp)
        summary["totals"].update(totals)
    python = load_json(os.path.join(args.coverage_dir, "python", "coverage.json"))
    if python:
        totals, summary["python_areas"] = summarize_python(python)
        summary["totals"].update(totals)

    with open(os.path.join(args.coverage_dir, "summary.json"), "w") as f:
        json.dump(summary, f, indent=2)
    write_html(os.path.join(args.coverage_dir, "index.html"), summary)
    print_text(summary)


if __name__ == "__main__":
    main()
