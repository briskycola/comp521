#!/bin/sh

for mode in mutex atomic compact padded
do
for threads in 1 2 4 8
do
/usr/bin/time -f \
"$mode,$threads,%e" \
./scalable_counters \
"$mode" "$threads" \
> /dev/null
done
done
