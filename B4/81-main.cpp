#include "../B1/billing.hpp"
#include <cassert>
class TeleconsultBill : public Bill {
    double fee;
public:
    TeleconsultBill(std::string p, double f) : Bill(std::move(p)), fee(checked(f)) {}
    double total() const override { return fee; }
    std::string type() const override { return "TEL"; }
};
class CorporatePayer : public Payer {
public:
    double amountCovered(const Bill& b) const override { return b.total() * 0.9; }
    std::string name() const override { return "Corporate"; }
};
int main()
{
    auto bills = standardBills();
    double previousTotal = 0;
    for (const auto& b : bills) previousTotal += b->total();
    assert(previousTotal == 74700);
    std::cout << "Earlier bill total unchanged: " << previousTotal << '\n';
    bills.push_back(std::make_unique<TeleconsultBill>("NewPatient", 1000));
    CorporatePayer payer;
    const Bill& b = *bills.back();
    assert(b.total() == 1000);
    assert(payer.amountCovered(b) == 900);
    std::cout << b.type() << " total=" << b.total()
              << " corporate=" << payer.amountCovered(b) << '\n';
    try {
        TeleconsultBill invalid("Patient", -1);
    } catch (const BillingError& e) {
        std::cout << "Recovered: " << e.what() << '\n';
    }
    std::cout << "PASS: extension and exception recovery\n";
}
