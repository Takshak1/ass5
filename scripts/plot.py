"""Plot throughput vs. thread count from results/results.csv -> results/throughput.png"""
import csv
import os
from collections import defaultdict

import matplotlib.pyplot as plt
from matplotlib.ticker import FuncFormatter

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
CSV = os.path.join(ROOT, "results", "results.csv")
OUT = os.path.join(ROOT, "results", "throughput.png")

data = defaultdict(lambda: defaultdict(list))  # list -> threads -> [ops/s]
meta = None
with open(CSV) as f:
    for row in csv.DictReader(f):
        data[row["list"]][int(row["threads"])].append(float(row["ops_per_sec"]))
        meta = (row["key_range"], row["update_pct"])

style = {
    "global-lock": dict(color="#2a6fdb", marker="o", label="Global lock"),
    "hand-over-hand": dict(color="#e0620d", marker="s", label="Hand-over-hand"),
}

fig, ax = plt.subplots(figsize=(8, 5), dpi=150)
for name, by_t in data.items():
    ts = sorted(by_t)
    mean = [sum(by_t[t]) / len(by_t[t]) / 1e6 for t in ts]
    lo = [m - min(by_t[t]) / 1e6 for m, t in zip(mean, ts)]
    hi = [max(by_t[t]) / 1e6 - m for m, t in zip(mean, ts)]
    ax.errorbar(ts, mean, yerr=[lo, hi], linewidth=2, markersize=6, capsize=3, **style[name])

ax.set_yscale("log")
ax.set_xscale("log", base=2)
ax.set_xticks([1, 2, 4, 8, 16, 32, 64])
ax.xaxis.set_major_formatter(FuncFormatter(lambda v, _: f"{int(v)}"))
ax.yaxis.set_major_formatter(FuncFormatter(lambda v, _: f"{v:g}"))
ax.set_xlabel("Number of threads")
ax.set_ylabel("Throughput (million ops / sec, log scale)")
ax.set_title(
    f"Concurrent sorted linked list throughput\n"
    f"key range {meta[0]} (~{int(meta[0]) // 2} nodes), {meta[1]}% updates, {100 - int(meta[1])}% finds"
)
ax.grid(True, which="both", alpha=0.3)
ax.legend()
fig.tight_layout()
fig.savefig(OUT)
print("saved", OUT)
