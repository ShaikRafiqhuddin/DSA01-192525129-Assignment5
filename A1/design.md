## A1. Common interface and bill hierarchy
Every bill has an immutable patient identifier, total(), type(), packageCode() and print(out). A production record would add bill ID, creation time and a persisted line-item ledger. Bill is abstract; total() and type() are pure virtual, packageCode() is virtual, print() is nonvirtual and calls the virtual interface. All derived totals validate finite nonnegative amounts. A virtual noexcept destructor supports deletion through Bill pointers.

OutPatientBill = consultation + tests. InPatientBill = days × daily rate + procedures + medicines. DayCareBill = package price. EmergencyBill = base + hours × hourly rate. These formulas operate in currency units for this teaching experiment. Production software should use checked integer paise or a decimal money type, not binary double.

Algorithm DayTotal
INPUT: Bills, an array of n references to Bill
OUTPUT: Sum, the total of all valid bills
BEGIN
    Sum ← 0
    FOR Index ← 0 TO n − 1 DO
        Amount ← CALL Bills[Index].total()
        Sum ← Sum + Amount
    END FOR
    RETURN Sum
END

This is language-independent pseudocode, not C++ in place of the design. It uses explicit INPUT/OUTPUT, assignment arrows and closed block keywords. The separately named SIMATS pseudocode-standard handout was not present among the supplied material or located in the accessible class files, so exact institutional conformance needs faculty confirmation.

Alternative: one tagged BillRecord holds kind and the union of required fields. total(record) selects OP/IP/DC/ER using SWITCH kind. Both methods total n records in O(n), with O(1) arithmetic per record. For a variable k-type sequential switch the implementation can cost O(k), although compilers often use jump tables; here k=4 is fixed. Virtual objects typically carry an ABI-dependent vptr plus type-specific fields and allocation overhead; a tagged union stores the largest payload plus a tag. New virtual types add a derived class and creation/registration logic, while the record alternative also edits the central tag/union/switch. Client iteration does not change.
