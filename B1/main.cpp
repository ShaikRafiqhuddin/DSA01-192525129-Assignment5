#include "billing.hpp"
#include <cassert>
#include <iomanip>
int main()
{
    auto bills = standardBills();
    SelfPay self;
    Insurance insurance(100000, 0.2);
    GovernmentScheme government({{"OP", 1500}, {"IP", 45000}, {"DC", 18000}, {"ER", 3000}});
    const Payer* payers[] = {&self, &insurance, &government};
    double expected[][3] = {{0, 1360, 1500}, {0, 39200, 45000},
                            {0, 16000, 18000}, {0, 3200, 3000}};
    double total = 0;
    std::cout << std::fixed << std::setprecision(2);
    for (std::size_t i = 0; i < bills.size(); ++i) {
        bills[i]->print(std::cout);
        total += bills[i]->total();
        for (std::size_t j = 0; j < 3; ++j) {
            double covered = payers[j]->amountCovered(*bills[i]);
            assert(std::abs(covered - expected[i][j]) < 0.001);
            std::cout << "  " << payers[j]->name() << " covered=" << covered
                      << " patient=" << bills[i]->total() - covered << '\n';
        }
    }
    assert(total == 74700);
    std::cout << "PASS: 12 payer checks; day total=" << total << '\n';
}
