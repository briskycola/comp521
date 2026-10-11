#!/usr/bin/env bash
set -euo pipefail

OUTPUT="${1:-memory_trials.csv}"

gcc -O2 -Wall -Wextra -o alloc_probe alloc_probe.c

printf 'megabytes,trial,allocation_seconds,touch_seconds\n' > "$OUTPUT"

for megabytes in 16 64 128; do
    for trial in 1 2 3; do
        line=$(./alloc_probe "$megabytes")

        allocation=$(printf '%s\n' "$line" |
            sed -n 's/.*allocation=\([^ ]*\).*/\1/p')

        touch=$(printf '%s\n' "$line" |
            sed -n 's/.*touch=\([^ ]*\).*/\1/p')

        printf '%s,%s,%s,%s\n' \
            "$megabytes" \
            "$trial" \
            "$allocation" \
            "$touch" >> "$OUTPUT"
    done
done

printf 'Wrote %s\n' "$OUTPUT"
