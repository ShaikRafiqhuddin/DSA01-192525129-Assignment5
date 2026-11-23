## A5. Recovery and rethrow design
The parser catches invalid user text at the input boundary and offers correction. Domain validation throws typed BillingError subclasses. A claim-service boundary logs ClaimError once and uses throw; to preserve the current exception. An outer per-bill loop catches known recoverable errors, retains the unchanged bill and marks the claim rejected for correction, then continues. bad_alloc is handled at a small top-level boundary without allocating a large new error message. catch(...) is a final guard to report an unknown failure and release resources, not permission to hide all errors. Persistence must occur transactionally, with commit only after successful validation; this teaching program does not implement a durable database.

Algorithm ProcessBills
INPUT: Bills and SelectedPayer
OUTPUT: Accepted claims and a list of retained rejected bills
BEGIN
    FOR EACH CurrentBill IN Bills DO
        TRY
            Covered ← CALL Settle(CurrentBill, SelectedPayer)
            CALL RecordAccepted(CurrentBill, Covered)
        CATCH RecoverableError
            CALL RecordRejectedWithoutDeletingBill(CurrentBill)
        END TRY
    END FOR
END

Algorithm Settle
INPUT: CurrentBill and SelectedPayer
OUTPUT: Covered amount
BEGIN
    TRY
        RETURN CALL SelectedPayer.amountCovered(CurrentBill)
    CATCH ClaimError
        CALL SafeAudit(CurrentBill)
        RETHROW CURRENT EXCEPTION
    END TRY
END

The pre-run A5 prediction matched ~f ~main caught 42. Guard f is destroyed first, then main, before the handler runs. Execution abandons the try block before not printed. RAII vector/unique_ptr destructors release owned resources automatically on normal and exceptional exits. This demonstrates stack unwinding, not a claim that every conceivable resource failure is tested.

Alternative: manual delete in every catch needs cleanup on the normal return plus each exceptional/early-return path. For r resources and e error exits, as many as r(e+1) cleanup obligations arise instead of r ownership declarations; every newly added exit can omit up to r deletes. Both approaches release r objects in O(r) time, but RAII localizes ownership and exception safety.
