"""Run bench.exe for both lists over a range of thread counts and save a CSV.

usage: python scripts/run_bench.py [--trials 3] [--duration 1000] [--range 1024] [--update 20]
"""
import argparse
import csv
import os
import subprocess
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
BENCH = os.path.join(ROOT, "bench.exe" if os.name == "nt" else "bench")
THREADS = [1, 2, 4, 8, 12, 16, 24, 32, 48, 64]

p = argparse.ArgumentParser()
p.add_argument("--trials", type=int, default=3)
p.add_argument("--duration", type=int, default=1000)
p.add_argument("--range", type=int, default=1024)
p.add_argument("--update", type=int, default=20)
p.add_argument("--out", default=os.path.join(ROOT, "results", "results.csv"))
a = p.parse_args()

os.makedirs(os.path.dirname(a.out), exist_ok=True)
with open(a.out, "w", newline="") as f:
    w = csv.writer(f)
    w.writerow(["list", "threads", "key_range", "update_pct", "trial", "duration_s", "total_ops", "ops_per_sec"])
    for t in THREADS:
        for lst in ["global", "hoh"]:
            for trial in range(a.trials):
                out = subprocess.check_output(
                    [BENCH, lst, str(t), str(a.duration), str(a.range), str(a.update)], text=True
                ).strip()
                name, thr, kr, up, secs, ops, tput = out.split(",")
                w.writerow([name, thr, kr, up, trial, secs, ops, tput])
                print(out, flush=True)
print("saved", a.out, file=sys.stderr)
