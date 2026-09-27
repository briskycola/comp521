#!/usr/bin/env bash

# github.com/briskycola <----- check this out!
set -euo pipefail

WORK_UNITS="${1:-200000000}"
OUTPUT="${2:-lab5a.csv}"

gcc -O2 -Wall -Wextra -pthread -o perf_probe perf_probe.c
printf 'workers,trial,work_units,elapsed\n' > "$OUTPUT"

for workers in 1 2 4; do
    for trial in 1 2 3; do
        line=$(./perf_probe "$workers" "$WORK_UNITS")
        elapsed=$(printf '%s\n' "$line" | sed -n 's/.*elapsed=\([^ ]*\).*/\1/p')
        printf '%s,%s,%s,%s\n' \
            "$workers" "$trial" "$WORK_UNITS" "$elapsed" >> "$OUTPUT"
    done
done

printf 'Wrote %s\n' "$OUTPUT"
