# DSA01-192525129-Assignment5
Shaik Rafiqhuddin | Register number 192525129 | DSA0120 D

Hospital billing with runtime polymorphism and exceptions.

## Preparation and execution provenance
See PROVENANCE.md. These are assistant-prepared and assistant-executed reference experiments on 23 November 2026. Student individual rerun and review are required by the brief. No historical commits or personal student runs are claimed.

## Environment
Ubuntu Linux x86_64, g++ (Ubuntu 11.4.0-1ubuntu1~22.04.3) 11.4.0. Two logical CPUs visible. Teaching money values are doubles, not production financial types.

## Reproduce
`bash run.sh` compiles/runs all safe examples, memory-safety tests and timings. Commands run from the repository root. No stdin is required: deterministic inputs are in each source and input.txt explains the cases.

Compile ordinary programs: `g++ -std=c++17 -Wall -Wextra B1/main.cpp -o /tmp/bill && /tmp/bill` (substitute any task .cpp).
Compile timings: `g++ -std=c++17 -Wall -Wextra -O2 B3/benchmark.cpp -o /tmp/bench && /tmp/bench`.
Sanitizer: `g++ -std=c++17 -Wall -Wextra -g -fsanitize=address -fno-omit-frame-pointer -fno-pie -no-pie B4/main.cpp -o /tmp/safe && ASAN_OPTIONS=detect_leaks=1 /tmp/safe`.
Expected compiler-error experiment: `g++ -std=c++17 -Wall -Wextra B1/nonvirtual-invalid.cpp -o /tmp/invalid`.
Expected unsafe experiment: compile B1/nonvirtual-destructor.cpp with the sanitizer command; it must remain isolated from the passing test script. A2's tiny unsafe example happens not to trigger ASan here and remains undefined behavior.

## Task map and results
- A1: common bill interface, UML and pseudocode in design.md; B1 provides execution.
- A2: prediction matches; nonvirtual destructor unsafe, no ASan diagnostic for tiny example.
- A3: stateful function policy gives cover 1360; payer hierarchy in B1.
- A4: error hierarchy and noexcept design in design.md; B2 exercises it.
- A5: pre-run prediction matches destructor order and catch output.
- B1: completed supplied code gives 49200; extended 4-bill total 74700; all 12 payer checks pass; correct build clean under ASan.
- B2: all seven error cases release guards and continue; completed code and slicing experiment match explanations.
- B3: virtual/switch/variant checksums agree at every size; raw measured timings in timings.csv. Median of 3 timed passes after warm-up; allocations excluded; layout differs.
- B4: teleconsult + corporate extension preserves previous total, handles errors, and passes ASan.

Each task folder contains design/reasoning, input descriptions and applicable source/output files. Code/output evidence images are explicitly labeled captures of a local evidence viewer showing the actual source and recorded output, not terminal or student-device screenshots. Full raw outputs accompany them.
