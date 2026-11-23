#!/usr/bin/env bash
set -eu
cd "$(dirname "$0")"
mkdir -p /tmp/oop5-build
for source in A2/predict A5/predict A3/policy B1/main B1/complete B1/static-binding B2/predict B2/complete B2/errors B2/rethrow B4/main; do
    g++ -std=c++17 -Wall -Wextra "$source.cpp" -o /tmp/oop5-build/run
    /tmp/oop5-build/run
 done
for source in B1/main B4/main; do
    g++ -std=c++17 -Wall -Wextra -g -fsanitize=address -fno-omit-frame-pointer -fno-pie -no-pie "$source.cpp" -o /tmp/oop5-build/sanitized
    ASAN_OPTIONS=detect_leaks=1 /tmp/oop5-build/sanitized
 done
g++ -std=c++17 -Wall -Wextra -O2 B3/benchmark.cpp -o /tmp/oop5-build/benchmark
/tmp/oop5-build/benchmark
