#!/usr/bin/env bash

set -u

PROGRAM="./sched_probe"
THREADS=4
SAMPLES=12
TRIALS=3

# TODO:
# Replace these CPU lists when necessary.
CPU_LISTS=("0" "0,1")

printf "configuration,trial,elapsed,user,system\n" \
    > timing.csv

# TODO:
# Add the repeated unrestricted trials.
for trial in $(seq 1 "$TRIALS")
do
    /usr/bin/time \
    -f "unrestricted, $trial, %e, %U, %S" \
    -o timing.csv \
    -a \
    "$PROGRAM" "$THREADS" "$SAMPLES" \
    > "trace_unrestricted_$trial.txt"
done


# TODO:
# Add repeated trials for each CPU list.
#for trial in $(seq 1 "$TRIALS")
#do
#    /usr/bin/time \
#    -f "one_cpu, $trial, %e, %U, %S" \
#    taskset -c 0 \
#    -o timing.csv \
#    -a \
#    "$PROGRAM" "$THREADS" "$SAMPLES" \
#    > "trace_one_cpu_$trial.txt"
#
#    /usr/bin/time \
#    -f "two_cpu, $trial, %e, %U, %S" \
#    taskset -c 0,1 \
#    -o timing.csv \
#    -a \
#    "$PROGRAM" "$THREADS" "$SAMPLES" \
#    > "trace_two_cpu_$trial.txt"
#
#    /usr/bin/time \
#    -f "two_cpu, $trial, %e, %U, %S" \
#    taskset -c 0,1 \
#    -o timing.csv \
#    -a \
#    "$PROGRAM" "$THREADS" "$SAMPLES" \
#    > "trace_two_cpu_$trial.txt"
#done
