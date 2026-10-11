#!/usr/bin/env bash

set -euo pipefail

PROGRAM="./sched_probe"
SAMPLES=12
TRIALS=3

if [[ $# -ne 3 ]]; then
    echo "Usage: $0 ONE_CPU TWO_CPU_LIST WORK_UNITS" >&2
    exit 2
fi

ONE_CPU="$1"
TWO_CPU_LIST="$2"
WORK_UNITS="$3"

if [[ ! -x "$PROGRAM" ]]; then
    gcc -O2 -Wall -Wextra -pthread sched_probe.c -o sched_probe
fi

if [[ ! "$ONE_CPU" =~ ^[0-9]+$ ]]; then
    echo "Error: ONE_CPU must be a logical CPU number" >&2
    exit 2
fi

if [[ ! "$TWO_CPU_LIST" =~ ^[0-9]+,[0-9]+$ ]]; then
    echo "Error: TWO_CPU_LIST must contain two CPUs, such as 2,3" >&2
    exit 2
fi

if [[ ! "$WORK_UNITS" =~ ^[1-9][0-9]*$ ]]; then
    echo "Error: WORK_UNITS must be a positive integer" >&2
    exit 2
fi

mkdir -p traces
find traces -maxdepth 1 -type f -name 'trace_*.txt' -delete

printf "configuration,threads,samples,work_units,trial,elapsed,user,system\n" \
    > timing.csv

for trial in $(seq 1 "$TRIALS")
do
    /usr/bin/time \
        -f "unrestricted,4,$SAMPLES,$WORK_UNITS,$trial,%e,%U,%S" \
        -o timing.csv \
        -a \
        "$PROGRAM" 4 "$SAMPLES" "$WORK_UNITS" \
        > "traces/trace_unrestricted_$trial.txt"
done

for trial in $(seq 1 "$TRIALS")
do
    /usr/bin/time \
        -f "one_cpu,4,$SAMPLES,$WORK_UNITS,$trial,%e,%U,%S" \
        -o timing.csv \
        -a \
        taskset -c "$ONE_CPU" \
        "$PROGRAM" 4 "$SAMPLES" "$WORK_UNITS" \
        > "traces/trace_one_cpu_$trial.txt"
done

for trial in $(seq 1 "$TRIALS")
do
    /usr/bin/time \
        -f "two_cpu,4,$SAMPLES,$WORK_UNITS,$trial,%e,%U,%S" \
        -o timing.csv \
        -a \
        taskset -c "$TWO_CPU_LIST" \
        "$PROGRAM" 4 "$SAMPLES" "$WORK_UNITS" \
        > "traces/trace_two_cpu_$trial.txt"
done

for trial in $(seq 1 "$TRIALS")
do
    /usr/bin/time \
        -f "oversubscribed,8,$SAMPLES,$WORK_UNITS,$trial,%e,%U,%S" \
        -o timing.csv \
        -a \
        taskset -c "$TWO_CPU_LIST" \
        "$PROGRAM" 8 "$SAMPLES" "$WORK_UNITS" \
        > "traces/trace_oversubscribed_$trial.txt"
done

for threads in 1 2 4 8
do
    for trial in $(seq 1 "$TRIALS")
    do
        /usr/bin/time \
            -f "extension_threads_$threads,$threads,$SAMPLES,$WORK_UNITS,$trial,%e,%U,%S" \
            -o timing.csv \
            -a \
            taskset -c "$TWO_CPU_LIST" \
            "$PROGRAM" "$threads" "$SAMPLES" "$WORK_UNITS" \
            > "traces/trace_extension_threads_${threads}_$trial.txt"
    done
done
