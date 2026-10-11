# Project 1: Linux Scheduling and Multicore Performance

## Main Project Question
How does CPU affinity, eligible CPU count, thread count,
and oversubscription affect the placement and performance
of a multithreaded Linux workload?

## Extension Question
How does elapsed time change as the number of worker threads
increases from 1 to 8 when all threads are restricted to
the same two eligible CPUs? The extension compares 1, 2, 4,
and 8 worker threads using the two-CPU affinity set while
keeping the sample count and workload size constant.

## Predictions Before the Final Trials
Before running the final trials, I predicted that unrestricted
execution would have the lowest elapsed time because the workers
could simply use all CPUs available to the process. Additionally,
I predicted that restricting four workers to one CPU would produce
the highest elapsed time among the four-worker configurations
because all workers would have to share one logical CPU, which
would overload that specific logical CPU. Consequently,
restricting the workers to two CPUs would perform better than
the one-CPU configuration but worse than unrestricted execution.
Finally, I predicted that the eight-worker oversubscribed configuration
would take longer than the four-worker, two-CPU configuration because
more workers would compete for the same two CPUs.

## Independent Variables
1. CPU affinity configuration and number of eligible logical CPUs
1. Number of worker threads
1. For the extension, thread count: 1, 2, 4, and 8

## Dependent Variables
1. User CPU time
1. System CPU time
1. Elapsed time
1. Logical CPU placement from sched_getcpu() samples
1. Worker migration between sampled CPUs

## Controlled Variables
1. Operating environment and hardware
1. Sample count: 12 samples per worker
1. Workload size: 5,000,000 work units
1. Three trials per configuration
1. Same CPU affinity sets for corresponding configurations

## Selected CPU Sets
1. One-CPU affinity set: logical CPU `0`
1. Two-CPU affinity set: logical CPUs `0,1`
1. Unrestricted execution: the process's inherited CPU set

## Selected Workload Size
The selected workload size is 5,000,000 work units. Each
worker performs the deterministic CPU-bound workload between
CPU-placement observations.

## Reproduction Command
```bash
./run_affinity_trials.sh 0 0,1 5000000
```
