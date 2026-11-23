## B1. Implementation and validation
The complete supplied two-bill program produces OP Ravi: 1700, IP Meena: 47500, Total 49200. This is separate from the extended hospital implementation, which includes 1500 medicines in the inpatient bill and totals 49000 for that patient. The four totals are 1700, 49000, 20000 and 4000, summing to 74700. All 12 payer comparisons assert independently calculated expected values.

Code follows A1/A3. Deliberate refinements are smart-pointer ownership, constructor/overflow validation, a separate package code and a documented rejection rule. The teacher's complete-code program retains its raw-pointer setup, because its non-TODO structure is not changed; only comment-only lines are omitted under the user's no-comments rule. Program logic outside TODO regions is preserved.

Removing virtual from the pure total declaration while retaining =0 and override produces compiler errors: a nonvirtual function cannot be pure, and the derived declarations no longer override. A second, explicitly modified experiment replaces the base total with a nonvirtual return-0 implementation and removes those override specifiers. Calls through Bill* then print zero totals, although type() still dispatches. This distinction separates a compile failure from a runnable static-binding experiment.

The no-virtual-destructor complete-code variant produces a real ASan new-delete-type-mismatch diagnostic: the derived allocation is larger than the base deallocation type. The unmodified implementation passes ASan with exit 0 and no diagnostics.

Alternative: a template sum(container) calls each concrete bill's total() without virtual dispatch; each type lives in its own container. Total time is O(n) with compile-time type selection, and O(n+k) storage/containers for k types. Optimizers may inline the calls. A single heterogeneous ordered list and runtime-added types are no longer as simple; preserving those needs a variant, erased wrapper or base interface.
